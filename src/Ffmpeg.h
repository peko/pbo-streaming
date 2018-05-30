#pragma once

#include <thread>

class Ffmpeg {
public:
    Ffmpeg();
    Ffmpeg(char* src, int w, int h);
    ~Ffmpeg();
    unsigned char* buffer;

private:
    int w, h;
	FILE* stream;
	std::thread read_th;
    bool do_work;
	void read_fun();
};

