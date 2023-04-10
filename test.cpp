

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
//	input.OpenFile(path_file);

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
	if (!file_stream) {
		printf("Failed to open 'audio2.raw'\n");
		exit(-1);
	}
	auto constexpr kChunkSize = 64*1024;
	std::vector<char> chunk(kChunkSize);

	while (true) {
		// Read another chunk from the file.
		file_stream.read(chunk.data(), chunk.size());
		int bytes_read = file_stream.gcount();
		// And write the chunk to the stream.
		if (bytes_read > 0) {
			printf("sending : %d k bytes\n",bytes_read/1024);
			int ret = recog.write(chunk, bytes_read, result);
			if(!ret)break;
		}else {
			printf("0 bytes read\n");	
			recog.close();
			break;
		}
		if (!file_stream) {
			printf("closing file stream\n");
			// Done reading everything from the file, so done writing to the stream.
			recog.close();
			break;
		}
		// Wait a second before writing the next chunk.
		//std::this_thread::sleep_for(std::chrono::seconds(1));
	}

    return 0;
}