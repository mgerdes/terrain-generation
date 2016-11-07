#include "terrain.h"
#include "stb_image.h"

static const char *terrain_vertex_shader_string = 
    "#version 330\n"
    "in vec3 vertex_position;"
    "in vec3 vertex_normal;"
    "in vec3 vertex_color;"
    "out vec3 frag_normal;"
    "out vec3 frag_color;"
    "out float fog_factor;"
    "uniform mat4 model_mat;"
    "uniform mat4 view_mat;"
    "uniform mat4 proj_mat;"
    "void main () {"
    "   frag_normal = normalize((transpose(inverse(model_mat)) * vec4(vertex_normal, 1.0)).xyz);"
    "   vec3 position = (view_mat * model_mat * vec4(vertex_position, 1.0)).xyz;"
    "   fog_factor = pow(1.0 - max((1000.0 - length(position)) / 1000.0, 0.0), 2.0);"
    "   frag_color = vertex_color;"
    "   gl_Position = proj_mat * vec4(position, 1.0);"
    "}";

static const char *terrain_fragment_shader_string = 
    "#version 330\n"
    "in vec3 frag_normal;"
    "in vec3 frag_color;"
    "in float fog_factor;"
    "out vec4 color;"

    "void main () {"
    "   vec3 light_direction1 = normalize(vec3(1, -1, 1));"
    "   vec3 light_direction2 = normalize(vec3(-1, 1, -1));"
    "   vec3 light_direction3 = normalize(vec3(1, -1, -1));"
    "   vec3 light_direction4 = normalize(vec3(-1, 1, 1));"
    "   float kd = 0.5 * max(dot(light_direction1, frag_normal), 0.3);"
    "   kd += 0.5 * max(dot(light_direction2, frag_normal), 0.3);"
    "   kd += 0.5 * max(dot(light_direction3, frag_normal), 0.3);"
    "   kd += 0.5 * max(dot(light_direction4, frag_normal), 0.3);"
    "   vec3 cd = vec3(kd * frag_color);"
    "   color = vec4(pow(mix(cd, vec3(42 / 255.0, 78 / 255.0, 110 / 255.0), fog_factor), vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)) , 1.0);"
    "}";

static float *height_field;
static int num_points;
static float *points, *normals, *colors;

void terrain_init_data() {
    // Create the shader
    terrain_shader.program = util_create_shader_program(terrain_fragment_shader_string, terrain_vertex_shader_string);

    // Create each of the uniform locations
    terrain_shader.proj_mat_location = glGetUniformLocation(terrain_shader.program, "proj_mat");
    terrain_shader.view_mat_location = glGetUniformLocation(terrain_shader.program, "view_mat");
    terrain_shader.model_mat_location = glGetUniformLocation(terrain_shader.program, "model_mat");

    // Check for errors with uniform locations
    util_check_for_shader_location_errors(terrain_shader.proj_mat_location, "proj_mat");
    util_check_for_shader_location_errors(terrain_shader.view_mat_location, "view_mat");
    util_check_for_shader_location_errors(terrain_shader.model_mat_location, "model_mat");

    glGenVertexArrays(1, &terrain_geometry.vao);
    glGenBuffers(1, &terrain_geometry.points_vbo);
    glGenBuffers(1, &terrain_geometry.normals_vbo);
    glGenBuffers(1, &terrain_geometry.colors_vbo);

    terrain_update_geometry();
}

void terrain_update_geometry() {
    int i, j, k, num_triangles, width;
    FILE *terrain_mesh_file = fopen("terrain_mesh.bin", "r");

    if (!terrain_mesh_file) {
        printf("Could not find the terrain mesh file. Make sure to run ./generate-terrain.py first.\n");
        exit(1);
    }

    fscanf(terrain_mesh_file, "%d", &width);

    height_field = malloc(sizeof(float) * width * width);
    for (i = 0; i < width; i++) {
        for (j = 0; j < width; j++) {
            fscanf(terrain_mesh_file, "%f", &height_field[width * i + j]);
        }
    }

    fscanf(terrain_mesh_file, "%d", &num_triangles);
    num_points = 3 * num_triangles;
    points = malloc(sizeof(float) * 3 * num_points);
    normals = malloc(sizeof(float) * 3 * num_points);
    colors = malloc(sizeof(float) * 3 * num_points);

    for (i = 0; i < num_triangles; i++) {
        vec3 p[3], c[3], t[2], n;

        for (j = 2; j >= 0; j--) {
            fscanf(terrain_mesh_file, "%f %f %f", &p[j].x, &p[j].y, &p[j].z);
            points[9 * i + 3 * j + 0] = p[j].x;
            points[9 * i + 3 * j + 1] = p[j].y;
            points[9 * i + 3 * j + 2] = p[j].z;

            fscanf(terrain_mesh_file, "%f %f %f", &c[j].x, &c[j].y, &c[j].z);
            colors[9 * i + 3 * j + 0] = c[j].x;
            colors[9 * i + 3 * j + 1] = c[j].y;
            colors[9 * i + 3 * j + 2] = c[j].z;
        }

        t[0] = vec3_sub(&p[0], &p[1]);
        t[1] = vec3_sub(&p[1], &p[2]);
        n = vec3_cross(&t[0], &t[1]);

        for (j = 0; j < 3; j++) {
            normals[9 * i + 3 * j + 0] = n.x;
            normals[9 * i + 3 * j + 1] = n.y;
            normals[9 * i + 3 * j + 2] = n.z;
        }
    }

    glBindVertexArray(terrain_geometry.vao);

    glBindBuffer(GL_ARRAY_BUFFER, terrain_geometry.points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_points, points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, terrain_geometry.normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_points, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, terrain_geometry.colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_points, colors, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    /*
    static float kinda_random[17] = { -0.9, 0.9, -0.4, 0.2, 0.6, -0.3, -0.8, -0.2, 0.5, 0.1, -0.6, -0.2, 0.8, -0.7, 0.4, -0.8, -0.1 };
    k = 0;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            float rx = 20 * kinda_random[k++ % 17] ;
            float rz = 20 * kinda_random[k++ % 17];

            float x = 40 + i * (600 / 10.0) + rx; 
            float z = 40 + j * (600 / 10.0) + rz;
            float y = height_field[(int) (600 * x) + (int) z];

            if (y >= 5 && y <= 10) {
                trees[i * 10 + j].is_visible = true;
            }
            else {
                trees[i * 10 + j].is_visible = false;
            }

            trees[i * 10 + j].is_selected = false;

            trees[i * 10 + j].position = (vec3) {x, y + 5.2, z};
            trees[i * 10 + j].scale = (vec3) {2.0, 2.0, 2.0};
            trees[i * 10 + j].rotation = (vec3) {0.0, x * z, 0.0};
        }
    }
    */
}

void terrain_draw() {
    int i, j, k = 0;
    mat4 scale_mat = mat4_scale(&(vec3) {1.0, 1.0, 1.0});
    mat4 translation_mat = mat4_translation(&(vec3) {0.0, 0.0, 0.0});
    mat4 model_mat = mat4_multiply(&translation_mat, &scale_mat);

    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT); 

    /*
     * Draw the terrain normally, setting stencil buffer to 1's so we can draw 
     * just the border later.
     */
    glUseProgram(terrain_shader.program);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    glUniformMatrix4fv(terrain_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
    glBindVertexArray(terrain_geometry.vao);
    glDrawArrays(GL_TRIANGLES, 0, num_points);

    /*
     * Draw with wireframe, only where the stencil buffer is 0, so
     * not to draw over the normal terrain
     */
    glUseProgram(single_color_shader.program);
    glStencilMask(0x00);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(4);

    glUniformMatrix4fv(single_color_shader.model_mat_location, 1, GL_TRUE, model_mat.m);
    glBindVertexArray(terrain_geometry.vao);
    //glDrawArrays(GL_TRIANGLES, 0, num_points);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT); 

    /*
     * Draw each of the trees.
     */
    glUseProgram(model_shader.program);
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            //tree_draw(&trees[i * 10 + j]);
        }
    }

    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT); 
}
