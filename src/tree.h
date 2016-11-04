#ifndef TREE_H 
#define TREE_H 

#include "maths.h"
#include "model.h"

struct tree {
    mat4 model_mat;
    vec3 position, rotation, scale;
    bounding_box bbox;
};

struct model tree_model;

void tree_init_data(void);
void tree_draw(struct tree *tree);

#endif 
