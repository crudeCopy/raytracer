/* chang
 * for testing really */

#include <stdio.h>
#include "vector.h"
#include "point.h"

int main () {
	vector_t u = { 1.0, 2.0, 2.0 },
	 	 v = { -2.0, -1.0, 2.0 },
	       	 w;

	vec_cross (&w, &u, &v);

	printf("cross: (%f, %f, %f)\ndot: %f\nangle between: %f\n", w.x, w.y, w.z, vec_dot (&u, &v), vec_angle_between (&u, &v));

	return 0;
}
