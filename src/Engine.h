#pragma once

class Gui;
class PBO;
class Ffmpeg;
class Tracker;
class GLFWwindow;
typedef unsigned int GLuint;

using namespace std;

class Engine {
public:

    Engine();
    ~Engine();

    GLFWwindow* window;
    Gui* gui;
    PBO* pbo;
    Ffmpeg *ffmpeg;
    
    void loop();
};

