#ifndef STATE_H
#define STATE_H

#include "tree.h"

struct state {
    int window_width, window_height;

    struct tree *selected_tree;
};

#endif
