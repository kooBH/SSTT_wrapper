#include <google/cloud/speech/speech_client.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <memory>

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

    speech::v1::StreamingRecognizeRequest request;
    std::atomic<bool> request_available;
    std::atomic<bool> write_available;
    std::atomic<bool> request_running;
    std::atomic<bool> flag_clear;
    short* buf;
    size_t max_size, n_size;

    std::string transcript;

    std::unique_ptr<std::thread> thread_request;
    std::unique_ptr<std::thread> thread_read;

    

  public : 
    std::unique_ptr<RecognizeStream> stream;
    std::atomic<bool> is_final;
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
    void Clear();

    std::string GetTranscript();
    std::atomic<bool>updated;
    std::atomic<bool> alive_request;
    std::atomic<bool> alive_read;

    /*
    * 2024.01.12
    * Temporal solution for SSTT::Request()::Error
    When there is bottleneck in other process, SSTT::Request()::Error occurs.
    Then set dead to true, and delete SSTT and recreate it.
    */
    bool dead = false;


};
