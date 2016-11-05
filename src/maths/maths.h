#ifndef MATHS_H
#define MATHS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEGREES_PER_RADIAN 0.01745329251

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

float rand_num(void);
/*
 * 2-D vectors
 */

typedef struct vec2 {
    float x, y;
} vec2;

/*
 * 3-D vectors
 */

typedef struct vec3 {
    float x, y, z;
} vec3;

/*
 * 4-D vectors
 */

typedef struct vec4 {
    float x, y, z, w;
} vec4;

/*
 * 4 x 4 matrices
 * Stored in row-order form
 */
typedef struct mat4 {
    float m[16];
} mat4;

typedef struct ray {
    vec3 origin, direction; 
} ray;

typedef struct sphere {
    vec3 center;
    float radius;
} sphere;

float vec3_dot(vec3 *v1, vec3 *v2);
vec3 vec3_sub(vec3 *v1, vec3 *v2);
void vec3_normalize_this(vec3 *v);
vec3 vec3_normalize(vec3 *v);
vec3 vec3_cross(vec3 *v1, vec3 *v2);
vec3 vec3_clone(vec3 *v);
vec3 vec3_apply_mat(vec3 *v, mat4 *m);
void vec3_print(vec3 *v);
void vec3_set(vec3 *v, float x, float y, float z);
vec3 vec3_add_scaled(vec3 *v1, vec3 *v2, float s);
float vec3_dist(vec3 *v1, vec3 *v2);
float vec3_length(vec3 *v);
vec3 vec3_add(vec3 *v1, vec3 *v2);
vec3 vec3_scale(vec3 *v, float s);
bool vec3_equal(vec3 *v1, vec3 *v2);

void vec4_set(vec4 *v, float x, float y, float z, float w);
vec4 vec4_apply_mat(vec4 *v, mat4 *m);
void vec4_normalize_this(vec4 *v);
void vec4_print(vec4 *v);

mat4 mat4_identity();
mat4 mat4_translation(vec3 *v);
mat4 mat4_scale(vec3 *v);
mat4 mat4_rotation_x(float theta);
mat4 mat4_rotation_y(float theta);
mat4 mat4_rotation_z(float theta);
mat4 mat4_multiply(mat4 *m1, mat4 *m2);
mat4 mat4_inverse(mat4 *m);
mat4 mat4_transpose(mat4 *m);
mat4 mat4_normal_transform(mat4 *model_transform);
mat4 mat4_look_at(vec3 *position, vec3 *target, vec3 *up);
mat4 mat4_perspective_projection(float fov, float aspect, float near, float far);
mat4 mat4_orthographic_projection(float right, float left, float top, float bottom, float near, float far);
void mat4_print(mat4 *m);

float ray_dist_to_sphere(ray *r, sphere *s); 

#endif
