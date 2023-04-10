#include "SSTT.h"

SSTT::SSTT(std::string _language_code,int _samplerate, size_t _max_size){
  language_code = _language_code;
  samplerate = _samplerate;
  max_size = _max_size;

  buf = new short[max_size];

  request_running.store(false);
}

SSTT::~SSTT(){
  delete [] buf;
}

/*
https://github.com/GoogleCloudPlatform/cpp-samples/blob/main/speech/api/streaming_transcribe.cc
*/

void SSTT::Run(){



	std::thread t1(&SSTT::Request, this);
  t1.detach();

} 

void SSTT::Write(short*cur_buf, int cur_size) {
  while (!write_available.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  write_available.store(false);

  memset(buf, 0, sizeof(short) * max_size);
  n_size = cur_size;
  memcpy(buf, cur_buf, sizeof(short) * n_size);
  request_available.store(true);
  printf("Write()\n");
}

void SSTT::Request() 
  {
  speech::SpeechClient client = speech::SpeechClient(speech::MakeSpeechConnection());

  speech::v1::StreamingRecognizeRequest request;
  auto& streaming_config = *request.mutable_streaming_config();
  streaming_config.mutable_config()->set_sample_rate_hertz(samplerate);
  streaming_config.mutable_config()->set_language_code(language_code);
  streaming_config.mutable_config()->set_encoding(::google::cloud::speech::v1::RecognitionConfig::LINEAR16);
   streaming_config.set_interim_results(true);

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

      request.set_audio_content(buf, n_size*sizeof(short));
      if (!stream->Write(request, grpc::WriteOptions{}).get()) {
        printf("SSTT::Request()::Error\n");
        // Write().get() returns false if the stream is closed.
        throw stream->Finish().get();
      }
      write_available.store(true);
      printf("Request()\n");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
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
  auto status = stream->Finish().get();
  request_running.store(false);
  return 0;
}

int SSTT::Read() {

  while (!request_running.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  for (
    auto response = stream->Read().get();
    response.has_value();
    response = stream->Read().get()) {

    // Best only
    auto const& result = response->results()[0];
    auto const& best = result.alternatives()[0];

    if (!strcmp(language_code.c_str(), "ko-KR")) {
      std::string tmp = best.transcript();
    
      // UTF-8, 3-byte, Korean, Encoding
      for (int i = 0; i < tmp.length(); i += 3)
        std::cout << tmp.substr(i, 3);
      std::cout << std::endl;
    }
    else {
      std::cout << best.transcript() << "\n";
    }
  }
  Close();

  return 0;
}
