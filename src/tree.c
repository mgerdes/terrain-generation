#include "tree.h"

void tree_init_data() {
    tree_model = model_create("resources/tree3.obj");
}

void tree_draw(struct tree *tree) {
    int i;

    if (!tree->is_visible) {
        return;
    }

    mat4 translation_mat = mat4_translation(&(vec3) {tree->position.x, tree->position.y, tree->position.z});
    mat4 scale_mat = mat4_scale(&tree->scale);
    mat4 rotation_mat = mat4_rotation_y(tree->rotation.y);
    mat4 temp_mat = mat4_multiply(&rotation_mat, &scale_mat);
    mat4 tree_model_mat = mat4_multiply(&translation_mat, &temp_mat);

    /*
     * Draw the tree normally, setting stencil buffer to 1's so we can draw 
     * just the border later.
     */
    glUseProgram(model_shader.program);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);

    glUniformMatrix4fv(model_shader.model_mat_location, 1, GL_TRUE, tree_model_mat.m);
    for (i = 0; i < tree_model.num_geometries; i++) {
        vec3 dc = tree_model.materials[tree_model.geometries[i].material_index].diffuse_color;
        glUniform3f(model_shader.diffuse_color_location, dc.x, dc.y, dc.z);
        if (tree->is_selected) {
            glUniform3f(model_shader.diffuse_color_location, dc.x + 0.3, dc.y + 0.3, dc.z + 0.3);
        }

        glBindVertexArray(tree_model.geometries[i].vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tree_model.geometries[i].eb);         
        glDrawElements(GL_TRIANGLES, 3 * tree_model.geometries[i].num_faces, GL_UNSIGNED_INT, 0);         
    }

    if (!tree->is_selected) {
        return;
    }

    /*
     * Draw with wireframe, only where the stencil buffer is 0, so
     * not to draw over the normal terrain
     */
    glUseProgram(single_color_shader.program);
    glStencilMask(0x00);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(3);

    glUniformMatrix4fv(single_color_shader.model_mat_location, 1, GL_TRUE, tree_model_mat.m);
    for (i = 0; i < tree_model.num_geometries; i++) {
        glBindVertexArray(tree_model.geometries[i].vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tree_model.geometries[i].eb);         
        glDrawElements(GL_TRIANGLES, 3 * tree_model.geometries[i].num_faces, GL_UNSIGNED_INT, 0);         
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void tree_draw_border(struct tree *tree) {
    int i;

    if (!tree->is_visible) {
        return;
    }

    mat4 translation_mat = mat4_translation(&(vec3) {tree->position.x, tree->position.y, tree->position.z});
    //vec3 scale_vec = vec3_add(&(vec3) {0.2, 0.2, 0.2}, &tree->scale); 
    mat4 scale_mat = mat4_scale(&tree->scale);
    mat4 rotation_mat = mat4_rotation_y(tree->rotation.y);
    mat4 temp_mat = mat4_multiply(&rotation_mat, &scale_mat);
    mat4 tree_model_mat = mat4_multiply(&translation_mat, &temp_mat);

    for (i = 0; i < tree_model.num_geometries; i++) {
        glUniformMatrix4fv(single_color_shader.model_mat_location, 1, GL_TRUE, tree_model_mat.m);

        /*
        vec3 dc = tree_model.materials[tree_model.geometries[i].material_index].diffuse_color;
        glUniform3f(model_shader.diffuse_color_location, dc.x, dc.y, dc.z);
        if (tree->is_selected) {
            glUniform3f(model_shader.diffuse_color_location, dc.x + 0.3, dc.y + 0.3, dc.z + 0.3);
        }
        */

        glBindVertexArray(tree_model.geometries[i].vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tree_model.geometries[i].eb);         
        glDrawElements(GL_TRIANGLES, 3 * tree_model.geometries[i].num_faces, GL_UNSIGNED_INT, 0);         
    }
}
