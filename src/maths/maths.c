#include <math.h>

#include "maths.h"

float rand_num() {
    return (rand() % 1000) / 1000.0;
}

/*
 * 3-D vectors
 */

float vec3_dot(vec3 *v1, vec3 *v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

vec3 vec3_sub(vec3 *v1, vec3 *v2) {
    vec3 v = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};
    return v;
}

void vec3_normalize_this(vec3 *v) {
    float l = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (l != 0) {
        v->x /= l;
        v->y /= l;
        v->z /= l;
    }
}

vec3 vec3_normalize(vec3 *v) {
    float l = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);

    if (l == 0) {
        return (vec3) {0, 0, 0};
    }

    return (vec3) {v->x / l, v->y / l, v->z / l};
}

vec3 vec3_cross(vec3 *v1, vec3 *v2) {
    float x = v1->y * v2->z - v1->z * v2->y;
    float y = v1->z * v2->x - v1->x * v2->z;
    float z = v1->x * v2->y - v1->y * v2->x;
    vec3 v = { x, y, z };
    return v;
}

vec3 vec3_clone(vec3 *v) {
    vec3 v1 = {v->x, v->y, v->z};
    return v1;
}

vec3 vec3_apply_mat(vec3 *v, mat4 *m) {
    float x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z + m->m[3];
    float y = m->m[4] * v->x + m->m[5] * v->y + m->m[6] * v->z + m->m[7];
    float z = m->m[8] * v->x + m->m[9] * v->y + m->m[10] * v->z + m->m[11];
    return (vec3) {x, y, z};
}

vec3 vec3_add_scaled(vec3 *v1, vec3 *v2, float s) {
    return (vec3) {
        v1->x + v2->x * s,
        v1->y + v2->y * s,
        v1->z + v2->z * s
    };
}

vec3 vec3_add(vec3 *v1, vec3 *v2) {
    return (vec3) {
        v1->x + v2->x,
        v1->y + v2->y,
        v1->z + v2->z
    };
}

vec3 vec3_scale(vec3 *v, float s) {
    return (vec3) {
        s * v->x,
        s * v->y,
        s * v->z
    };
}

void vec3_set(vec3 *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

float vec3_dist(vec3 *v1, vec3 *v2) {
    return sqrt((v1->x - v2->x) * (v1->x - v2->x) + (v1->y - v2->y) * (v1->y - v2->y) + (v1->z - v2->z) * (v1->z - v2->z));
}

float vec3_length(vec3 *v) {
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

bool vec3_equal(vec3 *v1, vec3 *v2) {
    return (fabs(v1->x - v2->x) < 0.001) && (fabs(v1->y - v2->y) < 0.001) && (fabs(v1->z - v2->z) < 0.001);
}

void vec3_print(vec3 *v) {
    printf("<%f, %f, %f>\n", v->x, v->y, v->z);
}

/*
 * 4-D vectors
 */

void vec4_set(vec4 *v, float x, float y, float z, float w) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
}

vec4 vec4_apply_mat(vec4 *v, mat4 *m) {
    float x = m->m[0] * v->x + m->m[1] * v->y + m->m[2] * v->z + m->m[3] * v->w;
    float y = m->m[4] * v->x + m->m[5] * v->y + m->m[6] * v->z + m->m[7] * v->w;
    float z = m->m[8] * v->x + m->m[9] * v->y + m->m[10] * v->z + m->m[11] * v->w;
    float w = m->m[12] * v->x + m->m[13] * v->y + m->m[14] * v->z + m->m[15] * v->w;
    return (vec4) {x, y, z, w};
}

void vec4_normalize_this(vec4 *v) {
    float l = sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
    if (l != 0) {
        v->x /= l;
        v->y /= l;
        v->z /= l;
        v->w /= l;
    }
}

void vec4_print(vec4 *v) {
    printf("<%f, %f, %f, %f>\n", v->x, v->y, v->z, v->w);
}

/*
 * 4 x 4 matrices
 */

mat4 mat4_identity() {
    mat4 m = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return m;
}

mat4 mat4_translation(vec3 *v) {
    mat4 m = {
        1, 0, 0, v->x,
        0, 1, 0, v->y,
        0, 0, 1, v->z,
        0, 0, 0, 1
    };
    return m;
}

mat4 mat4_scale(vec3 *v) {
    mat4 m = {
        v->x, 0, 0, 0,
        0, v->y, 0, 0,
        0, 0, v->z, 0,
        0, 0, 0, 1
    };
    return m;
}

/*
 * https://en.wikipedia.org/wiki/Rotation_matrix
 */
mat4 mat4_rotation_x(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    mat4 m = {
        1, 0, 0, 0,    
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    };
    return m;
}

mat4 mat4_rotation_y(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    mat4 m = {
        c, 0, s, 0,    
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1
    };
    return m;
}

mat4 mat4_rotation_z(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    mat4 m = {
        c, -s, 0, 0,    
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    return m;
}

void mat4_print(mat4 *m) {
    float *a = m->m;
    printf("[%f, %f, %f, %f]\n", a[0], a[1], a[2], a[3]);
    printf("[%f, %f, %f, %f]\n", a[4], a[5], a[6], a[7]);
    printf("[%f, %f, %f, %f]\n", a[8], a[9], a[10], a[11]);
    printf("[%f, %f, %f, %f]\n", a[12], a[13], a[14], a[15]);
}

mat4 mat4_multiply(mat4 *m1, mat4 *m2) {
    float *a = m1->m;
    float *b = m2->m;

    float c0 = a[0]*b[0] + a[1]*b[4] + a[2]*b[8] + a[3]*b[12];
    float c1 = a[0]*b[1] + a[1]*b[5] + a[2]*b[9] + a[3]*b[13];
    float c2 = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
    float c3 = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];

    float c4 = a[4]*b[0] + a[5]*b[4] + a[6]*b[8] + a[7]*b[12];
    float c5 = a[4]*b[1] + a[5]*b[5] + a[6]*b[9] + a[7]*b[13];
    float c6 = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
    float c7 = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];

    float c8 = a[8]*b[0] + a[9]*b[4] + a[10]*b[8] + a[11]*b[12];
    float c9 = a[8]*b[1] + a[9]*b[5] + a[10]*b[9] + a[11]*b[13];
    float c10 = a[8]*b[2] + a[9]*b[6] + a[10]*b[10] + a[11]*b[14];
    float c11 = a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11]*b[15];

    float c12 = a[12]*b[0] + a[13]*b[4] + a[14]*b[8] + a[15]*b[12];
    float c13 = a[12]*b[1] + a[13]*b[5] + a[14]*b[9] + a[15]*b[13];
    float c14 = a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
    float c15 = a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];

    mat4 c = {
        c0, c1, c2, c3,
        c4, c5, c6, c7,
        c8, c9, c10, c11,
        c12, c13, c14, c15
    };

    return c;
}

mat4 mat4_transpose(mat4 *m) {
    float * a = m->m;

    mat4 T = {
        a[0], a[4], a[8], a[12],
        a[1], a[5], a[9], a[13],
        a[2], a[6], a[10], a[14],
        a[3], a[7], a[11], a[15],
    };
    return T;
}

mat4 mat4_normal_transform(mat4 *model_transform) {
    mat4 m = mat4_inverse(model_transform);
    m = mat4_transpose(&m);
    return m;
}

/*
 * http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
 */
mat4 mat4_inverse(mat4 *m) {
    float *a = m->m;

    float det = 
        a[0]*a[5]*a[10]*a[15] +
        a[0]*a[6]*a[11]*a[13] +
        a[0]*a[7]*a[9]*a[14] +

        a[1]*a[4]*a[11]*a[14] +
        a[1]*a[6]*a[8]*a[15] +
        a[1]*a[7]*a[10]*a[12] +

        a[2]*a[4]*a[9]*a[15] +
        a[2]*a[5]*a[11]*a[12] +
        a[2]*a[7]*a[8]*a[13] +

        a[3]*a[4]*a[10]*a[13] +
        a[3]*a[5]*a[8]*a[14] +
        a[3]*a[6]*a[9]*a[12] -

        a[0]*a[5]*a[11]*a[14] -
        a[0]*a[6]*a[9]*a[15] -
        a[0]*a[7]*a[10]*a[13] -

        a[1]*a[4]*a[10]*a[15] -
        a[1]*a[6]*a[11]*a[12] -
        a[1]*a[7]*a[8]*a[14] -

        a[2]*a[4]*a[11]*a[13] -
        a[2]*a[5]*a[8]*a[15] -
        a[2]*a[7]*a[9]*a[12] -

        a[3]*a[4]*a[9]*a[14] -
        a[3]*a[5]*a[10]*a[12] -
        a[3]*a[6]*a[8]*a[13]; 
    
    float a0 = a[5]*a[10]*a[15] + a[6]*a[11]*a[13] + a[7]*a[9]*a[14]
        - a[5]*a[11]*a[14] - a[6]*a[9]*a[15] - a[7]*a[10]*a[13];

    float a1 = a[1]*a[11]*a[14] + a[2]*a[9]*a[15] + a[3]*a[10]*a[13]
        - a[1]*a[10]*a[15] - a[2]*a[11]*a[13] - a[3]*a[9]*a[14];

    float a2 = a[1]*a[6]*a[15] + a[2]*a[7]*a[13] + a[3]*a[5]*a[14]
        - a[1]*a[7]*a[14] - a[2]*a[5]*a[15] - a[3]*a[6]*a[13];

    float a3 = a[1]*a[7]*a[10] + a[2]*a[5]*a[11] + a[3]*a[6]*a[9]
        - a[1]*a[6]*a[11] - a[2]*a[7]*a[9] - a[3]*a[5]*a[10];

    float a4 = a[4]*a[11]*a[14] + a[6]*a[8]*a[15] + a[7]*a[10]*a[12]
        - a[4]*a[10]*a[15] - a[6]*a[11]*a[12] - a[7]*a[8]*a[14];

    float a5 = a[0]*a[10]*a[15] + a[2]*a[11]*a[12] + a[3]*a[8]*a[14]
        - a[0]*a[11]*a[14] - a[2]*a[8]*a[15] - a[3]*a[10]*a[12];

    float a6 = a[0]*a[7]*a[14] + a[2]*a[4]*a[15] + a[3]*a[6]*a[12]
        - a[0]*a[6]*a[15] - a[2]*a[7]*a[12] - a[3]*a[4]*a[14];

    float a7 = a[0]*a[6]*a[11] + a[2]*a[7]*a[8] + a[3]*a[4]*a[10]
        - a[0]*a[7]*a[10] - a[2]*a[4]*a[11] - a[3]*a[6]*a[8];

    float a8 = a[4]*a[9]*a[15] + a[5]*a[11]*a[12] + a[7]*a[8]*a[13]
        - a[4]*a[11]*a[13] - a[5]*a[8]*a[15] - a[7]*a[9]*a[12];

    float a9 = a[0]*a[11]*a[13] + a[1]*a[8]*a[15] + a[3]*a[9]*a[12]
        - a[0]*a[9]*a[15] - a[1]*a[11]*a[12] - a[3]*a[8]*a[13];

    float a10 = a[0]*a[5]*a[15] + a[1]*a[7]*a[12] + a[3]*a[4]*a[13]
        - a[0]*a[7]*a[13] - a[1]*a[4]*a[15] - a[3]*a[5]*a[12];

    float a11 = a[0]*a[7]*a[9] + a[1]*a[4]*a[11] + a[3]*a[5]*a[8]
        - a[0]*a[5]*a[11] - a[1]*a[7]*a[8] - a[3]*a[4]*a[9];

    float a12 = a[4]*a[10]*a[13] + a[5]*a[8]*a[14] + a[6]*a[9]*a[12]
        - a[4]*a[9]*a[14] - a[5]*a[10]*a[12] - a[6]*a[8]*a[13];

    float a13 = a[0]*a[9]*a[14] + a[1]*a[10]*a[12] + a[2]*a[8]*a[13]
        - a[0]*a[10]*a[13] - a[1]*a[8]*a[14] - a[2]*a[9]*a[12];

    float a14 = a[0]*a[6]*a[13] + a[1]*a[4]*a[14] + a[2]*a[5]*a[12]
        - a[0]*a[5]*a[14] - a[1]*a[6]*a[12] - a[2]*a[4]*a[13];

    float a15 = a[0]*a[5]*a[10] + a[1]*a[6]*a[8] + a[2]*a[4]*a[9]
        - a[0]*a[6]*a[9] - a[1]*a[4]*a[10] - a[2]*a[5]*a[8];

    mat4 I = {
        a0 / det, a1 / det, a2 / det, a3 / det,
        a4 / det, a5 / det, a6 / det, a7 / det,
        a8 / det, a9 / det, a10 / det, a11 / det, 
        a12 / det, a13 / det, a14 / det, a15 / det
    };

    return I;
}

/*
 * http://www.cs.virginia.edu/~gfx/Courses/1999/intro.fall99.html/lookat.html
 */
mat4 mat4_look_at(vec3* position, vec3* target, vec3* up) {
    vec3 up_p = vec3_clone(up);
    vec3_normalize_this(&up_p);

    vec3 f = vec3_sub(target, position);
    vec3_normalize_this(&f);

    vec3 s = vec3_cross(&f, &up_p);
    vec3_normalize_this(&s);

    vec3 u = vec3_cross(&s, &f);
    vec3_normalize_this(&u);
    
    mat4 M = {
        s.x, s.y, s.z, 0,
        u.x, u.y, u.z, 0,
        -f.x, -f.y, -f.z, 0,
        0, 0, 0, 1
    };

    mat4 T = mat4_translation(&(vec3) {-position->x, -position->y, -position->z});

    mat4 L = mat4_multiply(&M, &T);

    return L;
}

/*
 * http://www.cs.virginia.edu/~gfx/Courses/2000/intro.spring00.html/lectures/lecture09/sld017.htm
 */
mat4 mat4_perspective_projection(float fov, float aspect, float near, float far) {
    float f = 1.0 / tan(fov * DEGREES_PER_RADIAN / 2.0);
    float denominator = near - far;

    float a = f / aspect;
    float b = f;
    float c = (far + near) / denominator;
    float d = (2.0 * far * near) / denominator;

    mat4 P = {
        a, 0, 0, 0,
        0, b, 0, 0,
        0, 0, c, d,
        0, 0, -1, 0
    };

    return P;
}

/*
 * https://en.wikipedia.org/wiki/Orthographic_projection
 */
mat4 mat4_orthographic_projection(float left, float right, float bottom, float top, float near, float far) {
    float a = 2 / (right - left);
    float b = 2 / (top - bottom);
    float c = -2 / (far - near);
    float d = -(right + left) / (right - left);
    float e = -(top + bottom) / (top - bottom);
    float f = -(far + near) / (far - near);

    return (mat4) {
        a, 0, 0, d,
        0, b, 0, e,
        0, 0, c, f,
        0, 0, 0, 1
    };
}
