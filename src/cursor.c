#include "cursor.h"

static const char *cursor_vertex_shader_string = 
    "#version 330\n"
    "in vec3 vertex_position;"
    "uniform mat4 model_mat;"
    "uniform mat4 proj_mat;"
    "void main () {"
    "   gl_Position = proj_mat * model_mat * vec4(vertex_position, 1.0);"
    "}";

static const char *cursor_fragment_shader_string = 
    "#version 330\n"
    "out vec4 color;"
    "void main () {"
    "   color = vec4(0.0, 0.0, 0.0, 1.0);"
    "}";

void cursor_init_data() {
    cursor_shader.program = util_create_shader_program(cursor_fragment_shader_string, cursor_vertex_shader_string);

    cursor_shader.proj_mat_location = glGetUniformLocation(cursor_shader.program, "proj_mat");
    cursor_shader.model_mat_location = glGetUniformLocation(cursor_shader.program, "model_mat");

    util_check_for_shader_location_errors(cursor_shader.proj_mat_location, "proj_mat");
    util_check_for_shader_location_errors(cursor_shader.model_mat_location, "model_mat");

    static float points[18] = {
        -1.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
        1.0, -1.0, 0.0,

        -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
    };

    GLuint points_vbo;
    cursor_geometry.num_points = 6;
    glGenVertexArrays(1, &cursor_geometry.vao);
    glGenBuffers(1, &points_vbo);

    glBindVertexArray(cursor_geometry.vao);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * cursor_geometry.num_points, points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
}

void cursor_draw(struct state *state) {
    glUseProgram(cursor_shader.program);

    mat4 scale_mat = mat4_scale(&(vec3) {2, 2, 1});
    mat4 translation_mat = mat4_translation(&(vec3) {(state->window_width + 2.0) * 0.5, (state->window_height + 2.0) * 0.5, 0});
    mat4 model_mat = mat4_multiply(&translation_mat, &scale_mat);
    glUniformMatrix4fv(cursor_shader.model_mat_location, 1, GL_TRUE, model_mat.m);

    glBindVertexArray(cursor_geometry.vao);
    glDrawArrays(GL_TRIANGLES, 0, cursor_geometry.num_points);
}
