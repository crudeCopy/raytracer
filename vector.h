/* chang
 * implements a type for storing vectors */

#ifndef VECTOR_H
#define VECTOR_H

/* vector type */
typedef struct { float x, y, z; } vector_t;

/* vector length */
float vec_length (vector_t *v);

/* normalize vector 
 * returns 1 if success, 0 if fail (vec_length = 0) */
int vec_normalize (vector_t *v);

/* scale vector
 * always return 0 */
int vec_scale (vector_t *v, float scalar);

/* dot product of 2 vectors */
float vec_dot (vector_t *u, vector_t *v);

/* cross product of 2 vectors 
 * computes u x v and stores to dest */
int vec_cross (vector_t *dest, vector_t *u, vector_t *v);

/* returns angle between u and v in degrees */
float vec_angle_between (vector_t *u, vector_t *v);

#endif /* VECTOR_H */
