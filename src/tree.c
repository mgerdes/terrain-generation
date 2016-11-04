#include "tree.h"

void tree_init_data() {
    tree_model = model_create("resources/tree3.obj");
}

void tree_draw(struct tree *tree) {
    int i;

    mat4 translation_mat = mat4_translation(&(vec3) {tree->position.x, tree->position.y, tree->position.z});
    mat4 scale_mat = mat4_scale(&(vec3) {2.0, 2.0, 2.0});
    mat4 rotation_mat = mat4_rotation_y(tree->rotation.y);
    mat4 temp_mat = mat4_multiply(&scale_mat, &rotation_mat);
    mat4 tree_model_mat = mat4_multiply(&translation_mat, &temp_mat);

    for (i = 0; i < tree_model.num_geometries; i++) {
        glUniformMatrix4fv(model_shader.model_mat_location, 1, GL_TRUE, tree_model_mat.m);

        vec3 dc = tree_model.materials[tree_model.geometries[i].material_index].diffuse_color;
        glUniform3f(model_shader.diffuse_color_location, dc.x, dc.y, dc.z);

        glBindVertexArray(tree_model.geometries[i].vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tree_model.geometries[i].eb);         
        glDrawElements(GL_TRIANGLES, 3 * tree_model.geometries[i].num_faces, GL_UNSIGNED_INT, 0);         
    }
}
