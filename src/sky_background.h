#ifndef SKY_BACKGROUND_H
#define SKY_BACKGROUND_H

#include <GL/glew.h>

#include "maths.h"
#include "util.h"

struct sky_background_geometry {
    int num_points;
    GLuint vao;
} sky_background_geometry;

struct sky_background_shader {
    GLuint program;
} sky_background_shader;

void sky_background_init_data(void);
void sky_background_draw(void);

#endif
