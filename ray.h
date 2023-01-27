/* chang
 * a struct for holding rays */

#ifndef RAY_H
#define RAY_H

/* ray struct 
 * i thought about using a point_t and a vector_t to make up a ray
 * but that sounds like way too much to type out regularly */
typedef struct {
    float x, y, z,
          dx, dy, dz;
} ray_t;

#endif /* RAY_H */
