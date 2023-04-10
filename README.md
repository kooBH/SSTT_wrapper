# SSTT_wrapper

wrapper for Streaming Speech-to-Text  of Google cloud services.  

## Dependecy

### vcpkg
+ linux  

+ windows
```
lib/vcpkg/bootstrap-vcpkg.bat
```

## Build

+ windows
```
cmake -S. -B.build "-DCMAKE_TOOLCHAIN_FILE=lib/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build .build
```

## Credential

```
export GOOGLE_APPLICATION_CREDENTIALS=/path/to/your/credentials-key.json
```

## REFERENCE
[how to call Google Cloud services from C++](https://github.com/GoogleCloudPlatform/cpp-samples)        
[vcpkg](https://github.com/microsoft/vcpkg)    
[Supported Languages](https://cloud.google.com/speech-to-text/docs/speech-to-text-supported-languages?hl=ko)     
[Cloud Text-to-Speech API C++ Client](https://googleapis.dev/cpp/google-cloud-texttospeech/HEAD/)     