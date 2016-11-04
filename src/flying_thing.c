#include "flying_thing.h"

void flying_thing_update_direction(struct flying_thing *flying_thing) {
    flying_thing->dir.x = cosf(flying_thing->inclination_angle) * cosf(flying_thing->azimuth_angle);
    flying_thing->dir.y = sinf(flying_thing->inclination_angle);
    flying_thing->dir.z = cosf(flying_thing->inclination_angle) * sinf(flying_thing->azimuth_angle);
}

void flying_thing_move_up(struct flying_thing *flying_thing, float delta) {
    flying_thing->pos.y += 60 * delta;
}

void flying_thing_move_down(struct flying_thing *flying_thing, float delta) {
    flying_thing->pos.y -= 60 * delta;
}

void flying_thing_move_forward(struct flying_thing *flying_thing, float delta) {
    flying_thing->pos.x += 60.0 * delta * cosf(flying_thing->azimuth_angle);
    flying_thing->pos.z += 60.0 * delta * sinf(flying_thing->azimuth_angle);
}

void flying_thing_move_backward(struct flying_thing *flying_thing, float delta) {
    flying_thing->pos.x += 60.0 * delta * cosf(flying_thing->azimuth_angle + M_PI);
    flying_thing->pos.z += 60.0 * delta * sinf(flying_thing->azimuth_angle + M_PI);
}

void flying_thing_move_left(struct flying_thing *flying_thing, float delta) {
    flying_thing->pos.x += 60.0 * delta * cosf(flying_thing->azimuth_angle - 0.5 * M_PI);
    flying_thing->pos.z += 60.0 * delta * sinf(flying_thing->azimuth_angle - 0.5 * M_PI);
}

void flying_thing_move_right(struct flying_thing *flying_thing, float delta) {
    flying_thing->pos.x += 60.0 * delta * cosf(flying_thing->azimuth_angle + 0.5 * M_PI);
    flying_thing->pos.z += 60.0 * delta * sinf(flying_thing->azimuth_angle + 0.5 * M_PI);
}

void flying_thing_increase_azimuth(struct flying_thing *flying_thing, float delta) {
    flying_thing->azimuth_angle += delta;
    flying_thing_update_direction(flying_thing);
}

void flying_thing_increase_inclination(struct flying_thing *flying_thing, float delta) {
    flying_thing->inclination_angle += delta;

    if (flying_thing->inclination_angle > M_PI * 0.5 - 0.01) {
        flying_thing->inclination_angle = M_PI * 0.5 - 0.01;
    }
    if (flying_thing->inclination_angle < -M_PI * 0.5 + 0.01) {
        flying_thing->inclination_angle = -M_PI * 0.5 + 0.01;
    }

    flying_thing_update_direction(flying_thing);
}
