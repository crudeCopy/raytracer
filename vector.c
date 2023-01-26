/* chang
 * implements math for vectors */

#include <math.h>
#include "vector.h"

/* vector length */
float vec_length (vector_t *v) {
	float result = (v->x * v->x) + (v->y * v->y) + (v->z * v->z);
	return sqrt(result); /* thanks math.h :) */
}

/* normalize vector 
 * returns 1 if success, 0 if fail (vec_length = 0) */
int vec_normalize (vector_t *v) {
	float length = vec_length (v);

	/* if this happens we have a problem */
	if (length <= 0) return 0;

	v->x = v->x / length;
	v->y = v->y / length;
	v->z = v->z / length;

	return 1;
}

/* scale vector
 * always return 0 */
int vec_scale (vector_t *v, float scalar) {
	v->x = v->x * scalar;
	v->y = v->y * scalar;
	v->z = v->z * scalar;
	
	return 0;
}

/* dot product of 2 vectors */
float vec_dot (vector_t *u, vector_t *v) {
	return (u->x * v->x) + (u->y * v->y) + (u->z * v->z);
}

/* cross product of 2 vectors
 * computes u x v and stores to dest */
int vec_cross (vector_t *dest, vector_t *u, vector_t *v) {
	dest->x = (u->y * v->z) - (u->z * v->y);
	dest->y = (u->z * v->x) - (u->x * v->z);
	dest->z = (u->x * v->y) - (u->y * v->x);

	return 0;
}

/* returns angle between u and v in degrees */
float vec_angle_between (vector_t *u, vector_t *v) {
	float pi = 3.141593,
	      result = vec_dot (u, v);
	result /= vec_length (u) * vec_length (v);
	return acos (result) * (180.0 / pi);
}
