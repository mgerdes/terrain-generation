#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>

struct controls {
    /* Mouse Controls */
    float mouse_sensitivity;
    int left_mouse_down, left_mouse_clicked;
    int right_mouse_down, right_mouse_clicked;
    int middle_mouse_down, middle_mouse_clicked;
    float mouse_pos_x, mouse_delta_x, mouse_pos_y, mouse_delta_y;

    /* Keyboard Controls */
    int key_down[GLFW_KEY_LAST+1];
    int key_clicked[GLFW_KEY_LAST+1];
} controls;

void controls_update(GLFWwindow *window);

#endif
