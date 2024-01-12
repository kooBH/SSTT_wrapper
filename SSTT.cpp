#include "SSTT.h"

SSTT::SSTT(std::string _language_code,int _samplerate, size_t _max_size){
  language_code = _language_code;
  samplerate = _samplerate;
  max_size = _max_size;

  buf = new short[max_size];
  request_running.store(false);
  updated.store(false);
  is_final.store(false);
  flag_clear.store(false);

  alive_read.store(false);
  alive_request.store(false);
}

SSTT::~SSTT(){
  request_running.store(false);

  delete [] buf;
}

/*
https://github.com/GoogleCloudPlatform/cpp-samples/blob/main/speech/api/streaming_transcribe.cc
*/

void SSTT::Run(){
  alive_request.store(true);
  alive_read.store(true);

  thread_request = std::make_unique<std::thread>(&SSTT::Request, this);
  thread_read = std::make_unique<std::thread>(&SSTT::Read, this);

  thread_request->detach();
  thread_read->detach();
} 

void SSTT::Write(short*cur_buf, int cur_size) {

  if (!write_available.load())return;

  int cnt = 0;
  while (!write_available.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  write_available.store(false);

  memset(buf, 0, sizeof(short) * max_size);
  n_size = cur_size;
  memcpy(buf, cur_buf, sizeof(short) * n_size);
  request_available.store(true);
  //printf("SSTT::Write()\n");
}

void SSTT::Request(){
  speech::SpeechClient client = speech::SpeechClient(speech::MakeSpeechConnection());

  auto& streaming_config = *request.mutable_streaming_config();
  streaming_config.mutable_config()->set_sample_rate_hertz(samplerate);
  streaming_config.mutable_config()->set_language_code(language_code);
  streaming_config.mutable_config()->set_encoding(::google::cloud::speech::v1::RecognitionConfig::LINEAR16);
   streaming_config.set_interim_results(true);
   streaming_config.enable_voice_activity_events();

  // Begin a stream.
  stream = client.AsyncStreamingRecognize();
  //  The stream can fail to start, and `.get()` returns an error in this case.
  if (!stream->Start().get()) throw stream->Finish().get();
  if (!stream->Write(request, grpc::WriteOptions{}).get()) {
     // Write().get() returns false if the stream is closed.
     throw stream->Finish().get();
  }
  write_available.store(true);
  request_available.store(false);
  request_running.store(true);

  while (request_running.load()) {
    if (request_available.load()) {
      request_available.store(false);

      if (flag_clear.load()) {
        printf("SSTT::Clear()\n");
        request.clear_audio_content();
        //request.clear_streaming_request();
        flag_clear.store(false);
      }
      else 
        request.set_audio_content(buf, n_size*sizeof(short));
      if (!stream->Write(request, grpc::WriteOptions{}).get()) {
        printf("SSTT::Request()::Error\n");
        request_running.store(false);
        // Write().get() returns false if the stream is closed.
        //throw stream->Finish().get();

        while(alive_read.load())
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        dead = true;
      }
      write_available.store(true);
     // printf("SST::Request()\n");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  alive_request.store(false);
}

void SSTT::Finish() {
  while (!write_available.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  printf("SSTT::Finish()\n");
  stream->WritesDone().get();
}


int SSTT::Close(){
  printf("SSTT::Close()\n");
  if (dead)return -1;
  return 0;
  auto status = stream->Finish().get();
  request_running.store(false);
  return 0;
}

int SSTT::Read() {
  while (!request_running.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  printf("SSTT::Read()::Start\n");

  /*
    for (
    auto response = stream->Read().get();
    response.has_value();
    response = stream->Read().get()) {
  */
  while(request_running.load()){
    if(!stream)
    stream->Read().wait();
    //stream->Read().wait_for(std::chrono::milliseconds(100));
    auto response = stream->Read().get();

    if (!response.has_value()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      //printf("SSTT::Read::has_no_value\n");
      continue;
    }
    //printf("SSTT::Read()\n");
    // Best only
    if (response->results().empty()) {
      printf("SSTT::Read::result.empty\n");
      continue;
    }
    auto const& result = response->results()[0];
    auto const& best = result.alternatives()[0];

    if (result.is_final()) { 
      printf("SSTT::Read::final\n");
      is_final.store(true); 
    }

    if (!strcmp(language_code.c_str(), "ko-KR")) {
      std::string tmp = best.transcript();
      transcript = tmp;
    
      // UTF-8, 3-byte, Korean, Encoding
      for (int i = 0; i < tmp.length(); i += 3)
        std::cout << tmp.substr(i, 3);
      std::cout << std::endl;
    }
    else {
      transcript = best.transcript();
      std::cout << best.transcript() << "\n";
    }
    updated.store(true);

  }
  printf("Close Read\n");
  Close();

  alive_read.store(false);
  return 0;
}


std::string SSTT::GetTranscript() {
  updated.store(false);
  return transcript;
}

void SSTT::Clear() {
  flag_clear.store(true);
}
