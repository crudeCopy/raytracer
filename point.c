/* chang
 * implements math for points */

#include "point.h"
#include "vector.h"

/* returns the vector from point a to point b and stores to dest */
int vec_from_to (vector_t *dest, point_t *a, point_t *b) {
	dest->x = b->x - a->x;
	dest->y = b->y - a->y;
	dest->z = b->z - a->z;

	return 0;
}

