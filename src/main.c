#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "maths.h"
#include "model.h"
#include "tree.h"
#include "camera.h"
#include "terrain.h"
#include "flying_thing.h"
#include "controls.h"

int main() {
    int width, height;
    GLFWwindow *window = util_init_gl_window("opengl", 1000, 1000);

    {
        model_init_data();
        tree_init_data();
        terrain_init_data();
    }

    struct camera camera;
    camera.pos = (vec3) {100.0, 200.0, 100.0}; 
    camera.targ = (vec3) {0.0, 0.0, 0.0};
    camera.up = (vec3) {0.0, 1.0, 0.0};
    camera_update_view_mat(&camera);

    mat4 proj_mat = mat4_perspective_projection(60, 1.0, 1.0, 1000);

    struct flying_thing flying_thing;
    flying_thing.pos = (vec3) {0, 0, 0};
    flying_thing.dir = (vec3) {1, 0, 0};
    flying_thing.azimuth_angle = 0.0;
    flying_thing.inclination_angle = 0.0;
        
    glUseProgram(model_shader.program);
    glUniformMatrix4fv(model_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
    glUniformMatrix4fv(model_shader.view_mat_location, 1, GL_TRUE, camera.view_mat.m);

    struct tree tree;

    float sensitivity = 0.05;

    float previous_time = glfwGetTime();
    float time_since_fps_update = 0.0;

    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float elapsed_time = current_time - previous_time;
        previous_time = current_time;

        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        proj_mat = mat4_perspective_projection(60, width / (float) height, 1.0, 1000);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        controls_update(window);

        if (controls.key_down[GLFW_KEY_UP]) {
            flying_thing_increase_inclination(&flying_thing, 0.03);
        }
        if (controls.key_down[GLFW_KEY_DOWN]) {
            flying_thing_increase_inclination(&flying_thing, -0.03);
        }
        if (controls.key_down[GLFW_KEY_LEFT]) {
            flying_thing_increase_azimuth(&flying_thing, -0.03);
        }
        if (controls.key_down[GLFW_KEY_RIGHT]) {
            flying_thing_increase_azimuth(&flying_thing, 0.03);
        }
        if (controls.key_down[GLFW_KEY_W]) {
            flying_thing_move_forward(&flying_thing, sensitivity);
        }
        if (controls.key_down[GLFW_KEY_S]) {
            flying_thing_move_backward(&flying_thing, sensitivity);
        }
        if (controls.key_down[GLFW_KEY_A]) {
            flying_thing_move_left(&flying_thing, sensitivity);
        }
        if (controls.key_down[GLFW_KEY_D]) {
            flying_thing_move_right(&flying_thing, sensitivity);
        }
        if (controls.key_down[GLFW_KEY_Q]) {
            flying_thing_move_down(&flying_thing, sensitivity);
        }
        if (controls.key_down[GLFW_KEY_E]) {
            flying_thing_move_up(&flying_thing, sensitivity);
        }

        flying_thing_update_direction(&flying_thing);
        camera_update_to_follow_flying_thing(&flying_thing, &camera);
        camera_update_view_mat(&camera);

        glUseProgram(model_shader.program);
        glUniformMatrix4fv(model_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
        glUniformMatrix4fv(model_shader.view_mat_location, 1, GL_TRUE, camera.view_mat.m);

        glUseProgram(terrain_shader.program);
        glUniformMatrix4fv(terrain_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
        glUniformMatrix4fv(terrain_shader.view_mat_location, 1, GL_TRUE, camera.view_mat.m);

        terrain_draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
};
