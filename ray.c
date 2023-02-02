/* chang
 * one lonely function definition for ray_t */

#include "ray.h"

/* calculates point at given t along given ray, stores to dest */
void point_at_t (point_t *dest, ray_t *ray, float t) {
    vector_t dir_vec = ray->dir; /* will this create copy or modify original?? */
    vec_scale (&dir_vec, t);

    point_plus_vec (dest, &(ray->orig), &dir_vec);
}
