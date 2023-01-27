/* chang
 * implements math for points */

#include "point.h"
#include "vector.h"

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
