/* chang
 * implements methods for math in the 3D coordinate system 
 */

#include <math.h>

#include "r3math.h"


/* returns the vector from point a to point b and stores to dest */
void vec_from_to (vector_t *dest, point_t *a, point_t *b) {
    dest->x = b->x - a->x;
    dest->y = b->y - a->y;
    dest->z = b->z - a->z;
}

/* calculates the point given by point p + vector v and stores to dest */
void point_plus_vec (point_t *dest, point_t *p, vector_t *v) {
    dest->x = p->x + v->x;
    dest->y = p->y + v->y;
    dest->z = p->z + v->z;
}

/* calculates the distance between point a and point b */
float point_distance (point_t *a, point_t *b) {
    vector_t v;
    vec_from_to (&v, a, b);
    return vec_length (&v);
}

/* copies dest from src */
void point_copy (point_t *dest, point_t *src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

/* copies dest from src */
void vec_copy (vector_t *dest, vector_t *src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

/* vector length */
float vec_length (vector_t *v) {
    float result = (v->x * v->x) + (v->y * v->y) + (v->z * v->z);

    return sqrt(result); /* thanks math.h :) */
}

/* normalize vector
 * if returns 1, length is 0 */
int vec_normalize (vector_t *v) {
    float length = vec_length (v);

    /* error case */
    if (length <= 0) return 0;

    v->x = v->x / length;
    v->y = v->y / length;
    v->z = v->z / length;

    return 1;
}

/* scale vector */
void vec_scale (vector_t *v, float scalar) {
	v->x = v->x * scalar;
	v->y = v->y * scalar;
	v->z = v->z * scalar;
}

/* adds two vectors to get halfway vector, store in dest */
void vec_add (vector_t *dest, vector_t *a, vector_t *b) {
    *dest = (vector_t) { a->x + b->x, a->y + b->y, a->z + b->z };
}

/* dot product of 2 vectors */
float vec_dot (vector_t *u, vector_t *v) {
	return (u->x * v->x) + (u->y * v->y) + (u->z * v->z);
}

/* cross product of 2 vectors
 * computes u x v and stores to dest */
void vec_cross (vector_t *dest, vector_t *u, vector_t *v) {
	dest->x = (u->y * v->z) - (u->z * v->y);
	dest->y = (u->z * v->x) - (u->x * v->z);
	dest->z = (u->x * v->y) - (u->y * v->x);
}

/* returns angle between u and v in degrees */
float vec_angle_between (vector_t *u, vector_t *v) {
	float pi = 3.141593,
	      result = vec_dot (u, v);
	result /= vec_length (u) * vec_length (v);
	return acos (result) * (180.0 / pi);
}

/* calculates point at given t along given ray, stores to dest */
void point_at_t (point_t *dest, ray_t *ray, float t) {
    vector_t dir_vec = ray->dir; /* will this create copy or modify original?? */
    vec_scale (&dir_vec, t);

    point_plus_vec (dest, &(ray->orig), &dir_vec);
}
