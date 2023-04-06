

#include "SSTT.h"
#include "WAV.h"

int main(){
	SSTT recog;

	std::string path_file = "kor_1.wav";
	std::wstring result;

	WAV input;
	input.OpenFile(path_file);

	short * buf = new short[64*1024];

	recog.run();

	while (!input.IsEOF()) {

		input.ReadUnit(buf,64*1024);
		recog.request(buf, 64 * 1024, result);
	
	}
	recog.close();
	

    return 0;
}