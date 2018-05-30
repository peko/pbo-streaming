#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "Ffmpeg.h"


// ffmpeg -f v4l2 -i /dev/video0 -f rawvideo -pix_fmt rgb24  pipe:1 > video
Ffmpeg::Ffmpeg() : Ffmpeg("/dev/video0", 640, 480){}
Ffmpeg::Ffmpeg(char* src, int w, int h):w(w), h(h) {
     
    char cmd[1024];
    char* tpl = 
        "ffmpeg"
        " -hide_banner"         // disable deubg
        " -f v4l2"              // camera
        " -framerate 24"        // framerate 
        " -video_size %2$dx%3$d"// Resolution 
        " -input_format yuyv422"  // format
        " -i %1$s"              // input file
        " -vf \"pad=width=640:height=640:x=0:y=80:color=black\""
        " -pix_fmt bgra"        // output format
        " -f rawvideo"          // container format
        " -"                    // pipe output
        " 2>>in.log";           // stderr to file
    //snprintf(cmd, 1024, tpl, file, w, h, seek, length);
    snprintf(cmd, 1024, tpl, src, w, h);
    stream =  popen(cmd, "r");
    if(stream == NULL) 
        fprintf(stderr, "ERR:");
    fprintf(stderr, "0x%x %s\n", stream, cmd);

    buffer = (unsigned char*) calloc(w*w*4,1);
    do_work = true;
    std::thread th(&Ffmpeg::read_fun, this); 
    swap(th, read_th);
}

Ffmpeg::~Ffmpeg() {
    do_work = false;
    read_th.join();
    if(pclose(stream)==-1) printf("Error stream close\n%s\n", strerror(errno)); 
    free(buffer);
}

void Ffmpeg::read_fun() { 
	while(stream!=NULL &&  do_work) {
        if(fread(buffer, w*w*4, 1, stream) == 0) break;
    }
}


