#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>

#include "maths.h"
#include "flying_thing.h"

struct camera {
    vec3 pos, targ, up;
    mat4 view_mat;
};

void camera_update_view_mat(struct camera *camera);
void camera_update_to_follow_flying_thing(struct flying_thing *flying_thing, struct camera *camera);

#endif
