#include <chrono>
#include <thread>

#include <fstream>
#include <iostream>

#include "SSTT.h"
#include "WAV.h"


#ifdef _WIN32
#include <Windows.h>
#endif

int main() {

#ifdef _WIN32
	// To display Korean
	SetConsoleOutputCP(CP_UTF8);
#endif

	int max_size = 32 * 1024;
	SSTT recog("ko-KR",16000,max_size);

	std::string path_file = "kor_1.wav";

	WAV input;
	input.OpenFile(path_file);

	short* buf = new short[max_size];
	recog.Run();

	std::thread t1(&SSTT::Read, &recog);

	while (!input.IsEOF()) {
		int n_size = input.ReadUnit(buf, max_size);
    recog.Write(buf, n_size);
	}
	recog.Finish();

	t1.join();

	delete [] buf;
	return 0;
}