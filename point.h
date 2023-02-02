/* chang
 * implements a type for storing points */

#ifndef POINT_H
#define POINT_H

#include "vector.h"

/* a struct to hold xyz's for a point value */
typedef struct { float x, y, z; } point_t;

/* calculates the vector from point a to point b and stores to dest */
void vec_from_to (vector_t *dest, point_t *a, point_t *b); /* move to vec, or change name */

/* calculates the point given by point p + vector v and stores to dest */
void point_plus_vec (point_t *dest, point_t *p, vector_t *v);

/* calculates the distance from point a to point b */
float point_distance (point_t *a, point_t *b);

#endif /* POINT_H */
