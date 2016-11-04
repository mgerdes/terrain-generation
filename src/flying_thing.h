#ifndef FLYING_THING
#define FLYING_THING

#include <math.h>

#include "maths.h"

struct flying_thing {
    float azimuth_angle, inclination_angle;
    vec3 pos, dir;
};

void flying_thing_update_direction(struct flying_thing *flying_thing);
void flying_thing_move_up(struct flying_thing *flying_thing, float delta);
void flying_thing_move_down(struct flying_thing *flying_thing, float delta);
void flying_thing_move_forward(struct flying_thing *flying_thing, float delta);
void flying_thing_move_backward(struct flying_thing *flying_thing, float delta);
void flying_thing_move_left(struct flying_thing *flying_thing, float delta);
void flying_thing_move_right(struct flying_thing *flying_thing, float delta);
void flying_thing_increase_azimuth(struct flying_thing *flying_thing, float delta);
void flying_thing_increase_inclination(struct flying_thing *flying_thing, float delta);

#endif
