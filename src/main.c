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

    {
        model_init_data();
        tree_init_data();
        terrain_init_data();
        cursor_init_data();
        sky_background_init_data();
    }

    struct camera camera;
    camera.pos = (vec3) {100.0, 200.0, 100.0}; 
    camera.targ = (vec3) {0.0, 0.0, 0.0};
    camera.up = (vec3) {0.0, 1.0, 0.0};
    camera_update_view_mat(&camera);

    mat4 proj_mat = mat4_perspective_projection(67, 1.0, 1.0, 1000);
    mat4 ui_proj_mat = mat4_orthographic_projection(0.0, 1000.0, 1000.0, 0.0, 0.0, 1.0);

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
        int i;
        float current_time = glfwGetTime();
        float elapsed_time = current_time - previous_time;
        previous_time = current_time;

        glfwGetWindowSize(window, &state.window_width, &state.window_height);
        glViewport(0, 0, state.window_width, state.window_height);
        proj_mat = mat4_perspective_projection(60, state.window_width / (float) state.window_height, 1.0, 1000);
        ui_proj_mat = mat4_orthographic_projection(0.0, state.window_width, state.window_height, 0.0, 0.0, 1.0);

        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
        glStencilMask(0x00);

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

        glUseProgram(single_color_shader.program);
        glUniformMatrix4fv(single_color_shader.proj_mat_location, 1, GL_TRUE, proj_mat.m);
        glUniformMatrix4fv(single_color_shader.view_mat_location, 1, GL_TRUE, camera.view_mat.m);

        glUseProgram(cursor_shader.program);
        glUniformMatrix4fv(cursor_shader.proj_mat_location, 1, GL_TRUE, ui_proj_mat.m);

        {
            struct tree *closest_tree = NULL;
            float closest_tree_dist = 1000.0;

            for (i = 0; i < 100; i++) {
                struct tree *tree = &trees[i];
                if (!tree->is_visible) {
                    continue;
                }
                tree->is_selected = false;

                ray r = (ray) {flying_thing.pos, flying_thing.dir}; 
                sphere s = (sphere) {tree->position, 10.0};
                float tree_dist = ray_dist_to_sphere(&r, &s);

                if (tree_dist < closest_tree_dist && tree_dist > 0) {
                    closest_tree_dist = tree_dist;
                    closest_tree = tree;
                }
            }

            if (closest_tree) {
                state.selected_tree = closest_tree;
                closest_tree->is_selected = true;
            }
        }

        sky_background_draw();
        cursor_draw(&state);
        terrain_draw();

        static vec3 flying_thing_velocity = {0, 0, 0};

        vec3 force_on_flying_thing = {0, 0, 0};
        if (state.selected_tree && controls.left_mouse_down) {
            vec3 force = vec3_sub(&state.selected_tree->position, &flying_thing.pos);
            force_on_flying_thing = vec3_add(&force_on_flying_thing, &force);
        }
        else {
            flying_thing_velocity = (vec3) {0, 0, 0};
        }

        flying_thing_velocity = vec3_add_scaled(&flying_thing_velocity, &force_on_flying_thing, 0.016);
        
        float speed = vec3_length(&flying_thing_velocity);
        if (speed > 100) {
            flying_thing_velocity = vec3_normalize(&flying_thing_velocity);
            flying_thing_velocity = vec3_scale(&flying_thing_velocity, 100);
        }

        flying_thing.pos = vec3_add_scaled(&flying_thing.pos, &flying_thing_velocity, 0.016);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
};
