/* chang
 * implements a simple typedef for triangles
 */

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdlib.h>

#include "r3math.h"
#include "color.h"


/* triangle_t:
 *   defines an in-scene triangle
 *   verts and vecs will be stored in correct order to maintain correct facing
 */
typedef struct {
    point_t *p[3]; /* array of 3 pointers to vertices */

    vector_t e0,   /* from p[0] to p[1] */
             e1,   /* from p[0] to p[2] */
             n;    /* e1 x e2 */

    vector_t *norms[3];

    mtl_color_t *color; /* mtl_color */

    image_t *texture; /* pointer to texture image */
    img_coord_t *coords[3]; /* pointers to texture img coords (in order) */

    float d;       /* plane equation "D" value TODO: need? */
} triangle_t;

/* initializes a triangle_t from 3 pointers to points (IN ORDER) */
void tri_init (triangle_t *dest, point_t *p0, point_t *p1, point_t *p2, mtl_color_t *color);

/* add vector normals in order */
void tri_add_norms (triangle_t *dest, vector_t *v0, vector_t *v1, vector_t *v2);

#endif

