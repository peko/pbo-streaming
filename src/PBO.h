#pragma once

typedef unsigned int GLuint;
class Engine;

class PBO {

public:
	PBO(Engine* e);
	~PBO();

	void fill(unsigned char* src);
	void draw();

	GLuint tex_id;

private:
    Engine* engine;
    
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    
    GLuint vbo_id;
    GLuint vao_id;
    GLuint uv_id;

	GLuint mvp_loc;
	GLuint tex_loc;

	GLuint pbo_ids[2];
	int pbo_i;
	unsigned char* buffers_data;
	unsigned char* buffers[2];
};

