#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "maths.h"
#include "util.h"

struct model_shader {
    GLuint program;
    GLuint proj_mat_location, view_mat_location, model_mat_location;
    GLuint diffuse_color_location;
} model_shader;

struct model_geometry {
    int num_faces, material_index; 
    GLuint vao, eb;
};

struct model_material {
    GLuint diffuse_tex;
    vec3 diffuse_color;
};

struct model {
    int num_geometries;
    struct model_geometry *geometries;
    struct model_material *materials;
};

struct model model_create(const char *filename);
void model_init_data(void);

#endif
