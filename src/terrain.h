#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb_image_write.h"
#include "maths.h"
#include "model.h"
#include "tree.h"

struct terrain_shader {
    GLuint program;
    GLuint proj_mat_location, view_mat_location, model_mat_location;
} terrain_shader;

struct terrain_geometry {
    int num_faces;
    GLuint vao, points_vbo, normals_vbo, colors_vbo;
} terrain_geometry;

void terrain_init_data(void);
void terrain_update_geometry(void);
void terrain_draw(void);

#endif
