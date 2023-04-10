#include "SSTT.h"

SSTT::SSTT():client(speech::SpeechClient(speech::MakeSpeechConnection())){
}

SSTT::~SSTT(){

}

/*
https://github.com/GoogleCloudPlatform/cpp-samples/blob/main/speech/api/streaming_transcribe.cc
*/

void SSTT::run(){
 // client = speech::SpeechClient(speech::MakeSpeechConnection());

  //speech::v1::StreamingRecognizeRequest request;
  auto& streaming_config = *request.mutable_streaming_config();
  streaming_config.mutable_config()->set_sample_rate_hertz(16000);
  //streaming_config.mutable_config()->set_language_code("ko-KR");
  streaming_config.mutable_config()->set_encoding(speech::v1::RecognitionConfig_AudioEncoding_LINEAR16);
  streaming_config.set_interim_results(true);

  // Begin a stream.
  stream = client.AsyncStreamingRecognize();
  // The stream can fail to start, and `.get()` returns an error in this case.

  if (!stream->Start().get()) throw stream->Finish().get();
  // Write the first request, containing the config only.
  //if (!stream->Write(request, grpc::WriteOptions{}).get()) {
  int ret = stream->Write(request, grpc::WriteOptions{}).get();
  printf("SSTT::init ret : %d\n",ret);
  if(!ret){
    // Write().get() returns false if the stream is closed.
    throw stream->Finish().get();
  }
  printf("SSTT::run()\n");

} 

int SSTT::write(std::vector<char> buf, int size, std::wstring& result) {
  //Write to stream
  //speech::v1::StreamingRecognizeRequest request;
  request.set_audio_content(buf.data(), size);
  int ret = stream->Write(request, grpc::WriteOptions()).get();
  printf("SSTT::write()::ret : %d\n", ret);

  // Get Response
  for (
    auto response = stream->Read().get(); 
    response.has_value(); 
    response = stream->Read().get()) {
    // Dump the transcript of all the results.
    for (auto const& result : response->results()) {
      std::cout << "Result stability: " << result.stability() << "\n";
      for (auto const& alternative : result.alternatives()) {
        std::cout << alternative.confidence() << "\t"
          << alternative.transcript() << "\n";
      }
    }
  }
  return ret;
}


void SSTT::close(){
  printf("SSTT::close()\n");
  stream->WritesDone().get();
  auto status = stream->Finish().get();
}