#include "sky_background.h"

static const char *sky_background_vertex_shader_string = 
    "#version 330\n"
    "in vec3 vertex_position;"
    "in vec3 vertex_color;"
    "out vec3 frag_color;"
    "void main () {"
    "   frag_color = vertex_color;" 
    "   gl_Position = vec4(vertex_position, 1.0);"
    "}";

static const char *sky_background_fragment_shader_string = 
    "#version 330\n"
    "in vec3 frag_color;"
    "out vec4 color;"
    "void main () {"
    "   color = vec4(frag_color, 1.0);"
    "}";

void sky_background_init_data() {
    sky_background_shader.program = util_create_shader_program(sky_background_fragment_shader_string, sky_background_vertex_shader_string);

    vec3 top_color = (vec3) { 173 / 255.0, 116 / 255.0, 168 / 255.0 };
    vec3 bot_color = (vec3) { 42 / 255.0, 78 / 255.0, 110 / 255.0 };

    float points[18] = {
        -1.0, 1.0, 0.5,
        1.0, 1.0, 0.5,
        1.0, -1.0, 0.5,

        -1.0, 1.0, 0.5,
        -1.0, -1.0, 0.5,
        1.0, -1.0, 0.5,
    };

    float colors[18] = {
        top_color.x, top_color.y, top_color.z,
        top_color.x, top_color.y, top_color.z,
        bot_color.x, bot_color.y, bot_color.z,

        top_color.x, top_color.y, top_color.z,
        bot_color.x, bot_color.y, bot_color.z,
        bot_color.x, bot_color.y, bot_color.z,
    };

    GLuint points_vbo, colors_vbo;
    sky_background_geometry.num_points = 6;
    glGenVertexArrays(1, &sky_background_geometry.vao);
    glGenBuffers(1, &points_vbo);
    glGenBuffers(1, &colors_vbo);

    glBindVertexArray(sky_background_geometry.vao);

    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * sky_background_geometry.num_points, points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * sky_background_geometry.num_points, colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
}

void sky_background_draw() {
    glDepthMask(GL_FALSE);
    glUseProgram(sky_background_shader.program);
    glBindVertexArray(sky_background_geometry.vao);
    glDrawArrays(GL_TRIANGLES, 0, sky_background_geometry.num_points);
    glDepthMask(GL_TRUE);
}
