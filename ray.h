/* chang
 * a struct for holding rays */

#ifndef RAY_H
#define RAY_H

#include "point.h"
#include "vector.h"

/* ray struct using a point_t and a vector_t to make up a ray */
typedef struct {
    point_t orig;
    vector_t dir;
} ray_t;

/* calculates point at given t along given ray, stores to dest */
void point_at_t (point_t *dest, ray_t *ray, float t);

#endif /* RAY_H */
