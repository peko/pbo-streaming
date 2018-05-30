#include <stdio.h>
#include <iostream>
#include <stdexcept>

#include <GL/gl3w.h>    
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Gui.h"
#include "PBO.h"
#include "Ffmpeg.h"

using namespace std;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

Engine::Engine(){

    printf("Engine\n");

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) throw std::runtime_error("Can't init glfw context!");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Ailove.Lab | PBO Streamer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " 
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    gui = new Gui(this);
    pbo = new PBO(this);
    ffmpeg = new Ffmpeg();

}


Engine::~Engine() {
    delete ffmpeg;
    delete gui;
    delete pbo;

    glfwDestroyWindow(window);
    glfwTerminate();
}


void Engine::loop() {
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        gui->draw();
        gui->background();
        
        pbo->fill(ffmpeg->buffer);
        pbo->draw();
        
        gui->render();

        glfwSwapBuffers(window);
    }
}
