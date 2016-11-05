#ifndef CURSOR_H
#define CURSOR_H

#include <GL/glew.h>

#include "maths.h"
#include "util.h"
#include "state.h" 

struct cursor_geometry {
    int num_points; 
    GLuint vao;
} cursor_geometry;

struct cursor_shader {
    GLuint program;
    GLuint proj_mat_location, model_mat_location;
} cursor_shader;

void cursor_init_data(void);
void cursor_draw(struct state *state);

#endif
