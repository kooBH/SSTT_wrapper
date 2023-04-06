#include <google/cloud/speech/speech_client.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>


#include "SSTT.h"

SSTT::SSTT(){
}

SSTT::~SSTT(){

}

/*
https://github.com/GoogleCloudPlatform/cpp-samples/blob/main/speech/api/streaming_transcribe.cc
*/

void SSTT::run(){
auto client = speech::SpeechClient(speech::MakeSpeechConnection());

  speech::v1::StreamingRecognizeRequest request;  
  auto& streaming_config = *request.mutable_streaming_config();
  streaming_config.mutable_config()->set_sample_rate_hertz(16000);
  streaming_config.mutable_config()->set_language_code("ko-KR");
  streaming_config.mutable_config()->

  // Begin a stream.
  stream = client.AsyncStreamingRecognize();
  // The stream can fail to start, and `.get()` returns an error in this case.
  if (!stream->Start().get()) throw stream->Finish().get();
  // Write the first request, containing the config only.
  if (!stream->Write(request, grpc::WriteOptions{}).get()) {
    // Write().get() returns false if the stream is closed.
    throw stream->Finish().get();
  }

  // Read responses.
  auto read = [this] { return stream->Read().get(); };
  for (auto response = read(); response.has_value(); response = read()) {
    // Dump the transcript of all the results.
    for (auto const& result : response->results()) {
      std::cout << "Result stability: " << result.stability() << "\n";
      for (auto const& alternative : result.alternatives()) {
        std::cout << alternative.confidence() << "\t"
                  << alternative.transcript() << "\n";
      }
    }
  }

  auto status = stream->Finish().get();
} 

void SSTT::request(short*buf, int size,std::wstring &result){
  speech::v1::StreamingRecognizeRequest request;
  request.set_audio_content(buf,size);
  stream->Write(request, grpc::WriteOptions()).get();
  auto response = stream->Read().get();
  for (auto const& result : response->results()) {
      std::cout << "Result stability: " << result.stability() << "\n";
      for (auto const& alternative : result.alternatives()) {
        std::cout << alternative.confidence() << "\t"
                  << alternative.transcript() << "\n";
      }
  }


}

void SSTT::close(){
  stream->WritesDone().get();
}