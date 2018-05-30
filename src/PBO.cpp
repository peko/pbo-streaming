#include <GL/gl3w.h>    
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>
#include "Engine.h"
#include "linmath.h"
#include "PBO.h"

#ifdef USE_DOUBLE
typedef double real_t;
const GLenum GL_REAL_T = GL_DOUBLE;
#else
typedef float real_t;
const GLenum GL_REAL_T = GL_FLOAT;
#endif

#define SIZE 640

//geometry: textured quad
const float quad[] = {
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f};

const float  texcoord[] = {
     0.0f, 1.0f,
     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     0.0f, 1.0f};   

const char* vertex_shader_text =
    "#version 330 core\n"
    "layout(location = 0) in vec4 pos;\n"
    "layout(location = 1) in vec2 tex;\n"
    "smooth out vec2 UV;\n"
    "uniform mat4 MVP;\n"
    "void main() {\n"
    "  gl_Position = MVP * pos;\n"
    "  UV = tex;\n"
    "}";
    
const char* fragment_shader_text =
    "#version 330 core\n"
    "smooth in vec2 UV;\n"
    "out vec4 outColor;\n"
    "uniform sampler2D cltexture;\n"
    "void main() {\n"
    "  outColor = vec4(texture(cltexture, UV).rgb, 1.0);\n"
    "}";



GLuint create_program(const char* vertexSrc,
                      const char* fragmentSrc);
PBO::PBO(Engine* e) : engine(e) {   

//GEOMETRY
    //buffers                      
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id); 

    //geometry buffer
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float),
                 &quad[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //texture coordinate buffer
    GLuint uv_id;  
    glGenBuffers(1, &uv_id);

    glBindBuffer(GL_ARRAY_BUFFER, uv_id);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(real_t),
                 &texcoord[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 

    buffers_data = (unsigned char*)calloc(2,SIZE*SIZE*4);
    buffers[0] = &buffers_data[0];
    buffers[1] = &buffers_data[SIZE*SIZE*4];
    
    // create texture 
    std::vector< char > tc(SIZE*SIZE*4, 128);
    for (size_t i=0;i<SIZE*SIZE;i++) {
        tc[i*4+0] = (i*2)%256; 
        tc[i*4+1] = (i+i/SIZE)%256;
        tc[i*4+2] = (i/2)%256;
        tc[i*4+3] = (i/2)%256;
    }
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);  
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 SIZE,
                 SIZE,
                 0,
                 GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 &tc[0]);

    //optional
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //required
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glBindTexture(GL_TEXTURE_2D, 0);


//OPENGL RENDERING SHADERS
   
    program = create_program(vertex_shader_text, fragment_shader_text);

    //enable gl program
    glUseProgram(program);

    //extract ids of shader variables
    mvp_loc = glGetUniformLocation(program, "MVP");

    tex_loc = glGetUniformLocation(program, "cltexture");

    //only need texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glUniform1i(tex_loc, 0);


    // generate pbos
    glGenBuffers(2, pbo_ids);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_ids[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, SIZE*SIZE*4, nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_ids[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, SIZE*SIZE*4, nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    pbo_i = 0;
}

PBO::~PBO() {
    free(buffers_data);
    glDeleteBuffers(2, pbo_ids);
    glDeleteBuffers(1, &vbo_id);
    glDeleteBuffers(1, &uv_id);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteTextures(1, &tex_id);
}


// 
void PBO::draw() {

    float ratio;
    int width, height;
    mat4x4 m, p, mvp;

    glfwGetFramebufferSize(engine->window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);

    glUseProgram(program);
    {
        // SET VIEW
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, 3.141);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (const GLfloat*) mvp);

        // select geometry
        glBindVertexArray(vao_id); 
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindTexture(GL_TEXTURE_2D, tex_id);
 	    //draw
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0); 
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
	}
    glUseProgram(0);
}


// fill
void PBO::fill(unsigned char* src) {
    /*
    int t = (int)(glfwGetTime()*100);
    for(unsigned int i=0; i<SIZE*SIZE; i++) {
        int r = rand()%128;
        int v = (i/SIZE+r+t*4);
        buffers[0][i*4+0] = v; 
        buffers[0][i*4+1] = v;
        buffers[0][i*4+2] = v;
        buffers[0][i*4+3] = 255;
    }
    glBufferData(pbo_ids[pbo_i], SIZE*SIZE*4, buffers[0], GL_STREAM_DRAW);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_ids[pbo_i]);
    glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SIZE, SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    */

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_ids[pbo_i]);
    {
        // send to texture
        glBindTexture(GL_TEXTURE_2D, tex_id);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SIZE, SIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    pbo_i = (pbo_i+1)%2;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_ids[pbo_i]); 
    {
        unsigned char* buffer = (unsigned char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
        { 
            if(src == NULL) {
                int t = (int)(glfwGetTime()*100);
                for(unsigned int i=0; i<SIZE*SIZE; i++) {
                    int r = rand()%128;
                    int v = (i/SIZE+r+t*4);
                    buffer[i*4+0] = v; 
                    buffer[i*4+1] = v;
                    buffer[i*4+2] = v;
                    buffer[i*4+3] = 255;
                }
            } else {
                std::memcpy(buffer, src, SIZE*SIZE*4);
            }
    	}
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

GLuint create_program(const char* vertexSrc,
                      const char* fragmentSrc) {
    // Create the shaders
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    GLint res = GL_FALSE;
    int logsize = 0;
    // Compile Vertex Shader
    glShaderSource(vs, 1, &vertexSrc, 0);
    glCompileShader(vs);

    // Check Vertex Shader
    glGetShaderiv(vs, GL_COMPILE_STATUS, &res);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logsize);
 
    if(logsize > 1){
        std::vector<char> errmsg(logsize + 1, 0);
        glGetShaderInfoLog(vs, logsize, 0, &errmsg[0]);
        std::cout << &errmsg[0] << std::endl;
    }
    // Compile Fragment Shader
    glShaderSource(fs, 1, &fragmentSrc, 0);
    glCompileShader(fs);

    // Check Fragment Shader
    glGetShaderiv(fs, GL_COMPILE_STATUS, &res);
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logsize);
    if(logsize > 1){
        std::vector<char> errmsg(logsize + 1, 0);
        glGetShaderInfoLog(fs, logsize, 0, &errmsg[0]);
        std::cout << &errmsg[0] << std::endl;
    }

    // Link the program
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check the program
    glGetProgramiv(program, GL_LINK_STATUS, &res);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);
    if(logsize > 1) {
        std::vector<char> errmsg(logsize + 1, 0);
        glGetShaderInfoLog(program, logsize, 0, &errmsg[0]);
        std::cout << &errmsg[0] << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
