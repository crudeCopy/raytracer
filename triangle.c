/* chang
 * implements the methods listed in triangle.h */

#include "triangle.h"

/* initializes a triangle_t from 3 pointers to points (IN ORDER) */
void tri_init (triangle_t *dest, point_t *p0, point_t *p1, point_t *p2, mtl_color_t *color) {
    dest->p[0] = p0;
    dest->p[1] = p1;
    dest->p[2] = p2;

    /* first vector normal and texture coord NULL by default, others undef */
    dest->norms[0] = NULL;
    dest->coords[0] = NULL;

    vec_from_to (&(dest->e0), p0, p1);
    vec_from_to (&(dest->e1), p0, p2);
    vec_cross (&(dest->n), &(dest->e0), &(dest->e1));
    
    /* set color */
    dest->color = color;

    /* set texture automatically to NULL */
    dest->texture = NULL;

    dest->d = -(dest->n.x * p2->x + dest->n.y * p2->y + dest->n.z * p2->z);
}

/* adds pointers to vector normals in order */
void tri_add_norms (triangle_t *dest, vector_t *v0, vector_t *v1, vector_t *v2) {
    dest->norms[0] = v0;
    dest->norms[1] = v1;
    dest->norms[2] = v2;
}

