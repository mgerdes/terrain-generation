#include "camera.h"

void camera_update_view_mat(struct camera *camera) {
    camera->view_mat = mat4_look_at(&camera->pos, &camera->targ, &camera->up);
}

void camera_update_to_follow_flying_thing(struct flying_thing *flying_thing, struct camera *camera) {
    camera->up = (vec3) {0, 1, 0};
    camera->pos = flying_thing->pos;
    camera->targ = vec3_add_scaled(&camera->pos, &flying_thing->dir, 1.0);
    camera_update_view_mat(camera);
}
