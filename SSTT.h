#include <google/cloud/speech/speech_client.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

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
    std::unique_ptr<RecognizeStream> stream;

    public : 
    SSTT(std::string path_config);
    ~SSTT();
    void run();
    int request(short* buf, int size,std::wstring &result);
    void close();
};