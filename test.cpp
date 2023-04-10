

#include "SSTT.h"
#include "WAV.h"

#include <chrono>
#include <thread>

#include <fstream>
#include <iostream>

int main(){
	SSTT recog;

	std::string path_file = "kor_1.wav";
	std::wstring result;

	WAV input;
	input.OpenFile(path_file);

	int n_size = 32 * 1024;

	short * buf = new short[n_size];

	recog.run();


	int tick = 0;
	/*
	while (!input.IsEOF()) {

		input.ReadUnit(buf,n_size);
		recog.write(buf, sizeof(short)*n_size, result);

		printf("tick : %d\n",tick++);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	
	}
	*/
	std::ifstream file_stream("audio2.raw", std::ios::binary);
	auto constexpr kChunkSize = 8*1024;
	std::vector<char> chunk(kChunkSize);

	while (true) {
		// Read another chunk from the file.
		file_stream.read(chunk.data(), chunk.size());
		int bytes_read = file_stream.gcount();
		printf("bytes read : %d\n",bytes_read);
		// And write the chunk to the stream.
		if (bytes_read > 0) {
			recog.write(chunk.data(), bytes_read, result);
		  //printf("tick : %d\n",tick++);
		}
		if (!file_stream) {
			// Done reading everything from the file, so done writing to the stream.
			recog.close();
			break;
		}
		// Wait a second before writing the next chunk.
		//std::this_thread::sleep_for(std::chrono::seconds(1));
	}

    return 0;
}