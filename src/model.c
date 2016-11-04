#include "model.h"

static const char *model_vertex_shader_string = 
    "#version 330\n"
    "in vec3 vertex_position;"
    "in vec3 vertex_normal;"
    "out vec3 frag_normal;"
    "uniform mat4 model_mat;"
    "uniform mat4 view_mat;"
    "uniform mat4 proj_mat;"
    "void main () {"
    "   frag_normal = normalize((transpose(inverse(model_mat)) * vec4(vertex_normal, 1.0)).xyz);"
    "   gl_Position = proj_mat * view_mat * model_mat * vec4(vertex_position, 1.0);"
    "}";

static const char *model_fragment_shader_string = 
    "#version 330\n"
    "in vec3 frag_normal;"
    "out vec4 frag_color;"
    "uniform vec3 diffuse_color;"

    "void main () {"
    "   vec3 light_direction = normalize(vec3(1, 2, 3));"
    "   vec3 light_direction2 = normalize(vec3(-1, -2, -3));"
    "   float kd = max(dot(light_direction, frag_normal), 0.2);"
    "   kd += max(dot(light_direction2, frag_normal), 0.2);"
    "   frag_color = vec4(kd * pow(diffuse_color, vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), 1.0);"
    "}";

void model_init_data() {
    // Create the shader
    model_shader.program = util_create_shader_program(model_fragment_shader_string, model_vertex_shader_string);

    // Create each of the uniform locations
    model_shader.proj_mat_location = glGetUniformLocation(model_shader.program, "proj_mat");
    model_shader.view_mat_location = glGetUniformLocation(model_shader.program, "view_mat");
    model_shader.model_mat_location = glGetUniformLocation(model_shader.program, "model_mat");
    model_shader.diffuse_color_location = glGetUniformLocation(model_shader.program, "diffuse_color");

    // Check for errors with uniform locations
    util_check_for_shader_location_errors(model_shader.proj_mat_location, "proj_mat");
    util_check_for_shader_location_errors(model_shader.view_mat_location, "view_mat");
    util_check_for_shader_location_errors(model_shader.model_mat_location, "model_mat");
    util_check_for_shader_location_errors(model_shader.diffuse_color_location, "diffuse_color");
}

struct model model_create(const char *filename) {
    const struct aiScene *scene = aiImportFile(filename, aiProcess_OptimizeMeshes);
    if (!scene) {
        printf("Could not open the file\n");
    }

    struct model model;
    model.materials = malloc(sizeof(struct model_material) * scene->mNumMaterials);
    model.geometries = malloc(sizeof(struct model_geometry) * scene->mNumMeshes);
    model.num_geometries = scene->mNumMeshes;

    for (int i = 0; i < scene->mNumMaterials; i++) {
        struct aiColor4D diffuse_color, ambient_color;

        aiGetMaterialColor(scene->mMaterials[i], AI_MATKEY_COLOR_DIFFUSE, &diffuse_color);
        aiGetMaterialColor(scene->mMaterials[i], AI_MATKEY_COLOR_AMBIENT, &ambient_color);

        model.materials[i].diffuse_color = (vec3) {diffuse_color.r, diffuse_color.g, diffuse_color.b};

        if (aiGetMaterialTextureCount(scene->mMaterials[i], aiTextureType_DIFFUSE) > 0) {
            struct aiString texture_path;
            aiGetMaterialTexture(scene->mMaterials[i], aiTextureType_DIFFUSE, 0, &texture_path, 0, 0, 0, 0, 0, 0);
            //model.materials[i].diffuse_tex = util_create_texture(texture_path.data);
        }
        else {
            model.materials[i].diffuse_tex = 0;
        }

        float opacity = 1.0;
        float shininess = 1.0;

        aiGetMaterialFloatArray(scene->mMaterials[i], AI_MATKEY_OPACITY, &opacity, 0);
        aiGetMaterialFloatArray(scene->mMaterials[i], AI_MATKEY_SHININESS, &shininess, 0);

        //m->opacity = opacity;
        //m->shininess = shininess;
    }

    for (int i = 0; i < scene->mNumMeshes; i++) {
        const struct aiMesh *mesh = scene->mMeshes[i];

        const char *name = mesh->mName.data;
        printf("%d - %s\n", i, name);

        model.geometries[i].material_index = mesh->mMaterialIndex;

        // Create the meshes vao
        glGenVertexArrays(1, &model.geometries[i].vao);
        glBindVertexArray(model.geometries[i].vao);

        // Create an array of floats representing the points
        GLfloat *points = malloc(sizeof(GLfloat) * 3 * mesh->mNumVertices); 
        for (int j = 0; j < mesh->mNumVertices; j++) {
            points[3*j] = mesh->mVertices[j].x;
            points[3*j + 1] = mesh->mVertices[j].y;
            points[3*j + 2] = mesh->mVertices[j].z;
        }

        // Send points to opengl
        GLuint points_vbo;
        glGenBuffers(1, &points_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, points, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        // Free points
        free(points);

        // Create an array of floats representing the normals
        GLfloat *normals = malloc(sizeof(GLfloat) * 3 * mesh->mNumVertices); 
        if (mesh->mNormals) {
            for (int j = 0; j < mesh->mNumVertices; j++) {
                normals[3*j] = mesh->mNormals[j].x;
                normals[3*j + 1] = mesh->mNormals[j].y;
                normals[3*j + 2] = mesh->mNormals[j].z;
            }
        }

        // Send normals to opengl
        GLuint normals_vbo;
        glGenBuffers(1, &normals_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mesh->mNumVertices, normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        // Free normals
        free(normals);

        // Create an array of floats representing the texture coords
        GLfloat *texture_coords = malloc(sizeof(GLfloat) * 2 * mesh->mNumVertices); 
        if (mesh->mTextureCoords[0]) {
            for (int j = 0; j < mesh->mNumVertices; j++) {
                texture_coords[2*j] = mesh->mTextureCoords[0][j].x;
                texture_coords[2*j+1] = mesh->mTextureCoords[0][j].y;
            }
        }

        // Send texture coords to opengl
        GLuint texture_coords_vbo;
        glGenBuffers(1, &texture_coords_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texture_coords_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * mesh->mNumVertices, texture_coords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);

        // Free texture coords
        free(texture_coords);

        // Create an array representing the indices of each face
        GLuint *indices = malloc(sizeof(GLuint) * 3 * mesh->mNumFaces); 
        for (int j = 0; j < mesh->mNumFaces; j++) {
            indices[3*j] = mesh->mFaces[j].mIndices[0];
            indices[3*j+1] = mesh->mFaces[j].mIndices[1];
            indices[3*j+2] = mesh->mFaces[j].mIndices[2];
        }

        // Send indices to opengl
        glGenBuffers(1, &model.geometries[i].eb);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.geometries[i].eb);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * mesh->mNumFaces, indices, GL_STATIC_DRAW);

        // Free indices
        free(indices);
        
        model.geometries[i].num_faces = mesh->mNumFaces;
    }

    aiReleaseImport(scene);

    return model;
}
