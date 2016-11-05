#ifndef TREE_H 
#define TREE_H 

#include "maths.h"
#include "model.h"

struct tree {
    mat4 model_mat;
    vec3 position, rotation, scale;
    bool is_visible, is_selected;
};

struct model tree_model;

void tree_init_data(void);
void tree_draw(struct tree *tree);
void tree_draw_border(struct tree *tree);

#endif 
