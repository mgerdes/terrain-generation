#include "controls.h"

void controls_update(GLFWwindow *window) {
    double new_mouse_pos_x, new_mouse_pos_y;
    glfwGetCursorPos(window, &new_mouse_pos_x, &new_mouse_pos_y);

    controls.mouse_delta_x = new_mouse_pos_x - controls.mouse_pos_x;
    controls.mouse_delta_y = new_mouse_pos_y - controls.mouse_pos_y;

    controls.mouse_pos_x = new_mouse_pos_x;
    controls.mouse_pos_y = new_mouse_pos_y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        controls.left_mouse_down = 1;
    }
    else {
        controls.left_mouse_clicked = controls.left_mouse_down;
        controls.left_mouse_down = 0;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        controls.right_mouse_down = 1;
    }
    else {
        controls.right_mouse_clicked = controls.right_mouse_down;
        controls.right_mouse_down = 0;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        controls.middle_mouse_down = 1;
    }
    else {
        controls.middle_mouse_clicked = controls.middle_mouse_down;
        controls.middle_mouse_down = 0;
    }

    int i;

    for (i = 48; i < 58; i++) {
        controls.key_clicked[i] = !glfwGetKey(window, i) && controls.key_down[i];
    }
    for (i = 65; i < 91; i++) {
        controls.key_clicked[i] = !glfwGetKey(window, i) && controls.key_down[i];
    }
    for (i = 262; i < 266; i++) {
        controls.key_clicked[i] = !glfwGetKey(window, i) && controls.key_down[i];
    }
    controls.key_clicked[GLFW_KEY_LEFT_SHIFT] = !glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && controls.key_down[GLFW_KEY_LEFT_SHIFT];
    controls.key_clicked[GLFW_KEY_BACKSPACE] = !glfwGetKey(window, GLFW_KEY_BACKSPACE) && controls.key_down[GLFW_KEY_BACKSPACE];
    controls.key_clicked[GLFW_KEY_GRAVE_ACCENT] = !glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) && controls.key_down[GLFW_KEY_GRAVE_ACCENT];

    for (i = 48; i < 58; i++) {
        controls.key_down[i] = glfwGetKey(window, i);
    }
    for (i = 65; i < 91; i++) {
        controls.key_down[i] = glfwGetKey(window, i);
    }
    for (i = 262; i < 266; i++) {
        controls.key_down[i] = glfwGetKey(window, i);
    }
    controls.key_down[GLFW_KEY_LEFT_SHIFT] = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    controls.key_down[GLFW_KEY_BACKSPACE] = glfwGetKey(window, GLFW_KEY_BACKSPACE);
    controls.key_down[GLFW_KEY_GRAVE_ACCENT] = glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT);
}
