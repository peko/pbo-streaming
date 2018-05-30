#pragma once

#include <string>

class Engine;

class Gui {
public:
    
    Gui(Engine* e);
    ~Gui();

    Engine* engine;

    void background();
    void draw();
    void render();

};
