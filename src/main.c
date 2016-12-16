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
#include "cursor.h"
#include "state.h"
#include "sky_background.h"

int main() {
    struct state state;
    GLFWwindow *window = util_init_gl_window("opengl", 1000, 1000);

    model_init_data();
    tree_init_data();
    terrain_init_data();
    cursor_init_data();
    sky_background_init_data();

    struct camera camera;
    struct flying_thing flying_thing;
    flying_thing.pos = (vec3) {200.0, 200.0, 200.0};

    mat4 proj_mat,ui_proj_mat; 
        
    glUseProgram(model_shader.program);
    glUniformMatrix4fv(model_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
    glUniformMatrix4fv(model_shader.view_mat_location, 1, GL_TRUE, camera.view_mat.m);

    struct tree tree;

    float previous_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        int i;
        float current_time = glfwGetTime();
        float elapsed_time = current_time - previous_time;
        previous_time = current_time;

        glfwGetWindowSize(window, &state.window_width, &state.window_height);
        glViewport(0, 0, state.window_width, state.window_height);
        proj_mat = mat4_perspective_projection(60, state.window_width / (float) state.window_height, 1.0, 1000.0);
        ui_proj_mat = mat4_orthographic_projection(0.0, state.window_width, state.window_height, 0.0, 0.0, 1.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

        controls_update(window);

        flying_thing_increase_azimuth(&flying_thing, 0.001 * controls.mouse_delta_x);
        flying_thing_increase_inclination(&flying_thing, -0.001 * controls.mouse_delta_y);

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
            flying_thing_move_forward(&flying_thing, 0.05);
        }
        if (controls.key_down[GLFW_KEY_S]) {
            flying_thing_move_backward(&flying_thing, 0.05);
        }
        if (controls.key_down[GLFW_KEY_A]) {
            flying_thing_move_left(&flying_thing, 0.05);
        }
        if (controls.key_down[GLFW_KEY_D]) {
            flying_thing_move_right(&flying_thing, 0.05);
        }
        if (controls.key_down[GLFW_KEY_Q]) {
            flying_thing_move_down(&flying_thing, 0.05);
        }
        if (controls.key_down[GLFW_KEY_E]) {
            flying_thing_move_up(&flying_thing, 0.05);
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

        glUseProgram(single_color_shader.program);
        glUniformMatrix4fv(single_color_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
        glUniformMatrix4fv(single_color_shader.view_mat_location, 1, GL_TRUE, camera.view_mat.m);

        glUseProgram(cursor_shader.program);
        glUniformMatrix4fv(cursor_shader.proj_mat_location, 1, GL_TRUE, ui_proj_mat.m);

        sky_background_draw();
        cursor_draw(&state);
        terrain_draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
};
