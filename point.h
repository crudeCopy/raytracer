/* chang
 * implements a type for storing points */

#ifndef POINT_H
#define POINT_H

#include "vector.h"

/* a struct to hold xyz's for a point value */
typedef struct { float x, y, z; } point_t;

/* calculates the vector from point a to point b and stores to dest */
int vec_from_to (vector_t *dest, point_t *a, point_t *b);

#endif /* POINT_H */
