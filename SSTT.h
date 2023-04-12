#include <google/cloud/speech/speech_client.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

//Audio recording parameters
// RATE = 16000
// CHUNK = int(RATE / 10)  # 100ms

// [START speech_streaming_recognize]
namespace speech = ::google::cloud::speech;
using RecognizeStream = ::google::cloud::AsyncStreamingReadWriteRpc<
    speech::v1::StreamingRecognizeRequest,
    speech::v1::StreamingRecognizeResponse>;

class SSTT{
  private : 
    std::string path_config;
    /*
    Korean : "ko-KR"
    English : "en-US"
    */
    std::string language_code;
    int samplerate;

    std::atomic<bool> request_available;
    std::atomic<bool> write_available;
    std::atomic<bool> request_running;
    short* buf;
    size_t max_size, n_size;

    std::string transcript;

  public : 
    std::unique_ptr<RecognizeStream> stream;
    SSTT(
      std::string language_code="en-US",
      int samplerate=16000,
      size_t max_size=32*1024
    );
    ~SSTT();
    void Run();
    void Write(short*, int size);
    void Request();
    int Read();
    void Finish();
    int Close();

    std::string GetTranscript();


};
