/* chang
 * implements typedefs and declares methods for math
 * in the 3D coord system 
 */

#ifndef R3MATH_H
#define R3MATH_H


/******************** TYPEDEFS *********************/

/* point_t : Point type
 *   - x, y, and z coords
 *   - (special use case for barycentrics)
 *      - x --> alpha, y --> beta, z --> gamma
 */
typedef struct { float x, y, z; } point_t;

/* vector_t : Vector type
 *   - x, y, and z component directions
 */
typedef struct { float x, y, z; } vector_t;

/* ray_t : Ray type
 *   - orig : origin point
 *   - dir  : direction vector
 */
typedef struct {
    point_t orig;
    vector_t dir;
} ray_t;


/********************* METHODS *********************/

/* calculates the vector from point a to point b and stores to dest */
void vec_from_to (vector_t *dest, point_t *a, point_t *b); /* move to vec, or change name */

/* calculates the point given by point p + vector v and stores to dest */
void point_plus_vec (point_t *dest, point_t *p, vector_t *v);

/* calculates the distance from point a to point b */
float point_distance (point_t *a, point_t *b);

/* copies dest from src */
void point_copy (point_t *dest, point_t *src);

/* copies dest from src */
void vec_copy (vector_t *dest, vector_t *src);

/* vector length */
float vec_length (vector_t *v);

/* normalize vector */
int vec_normalize (vector_t *v);

/* scale vector */
void vec_scale (vector_t *v, float scalar);

/* adds two vectors to get "halfway" vector, stores to dest */
void vec_add (vector_t *dest, vector_t *a, vector_t *b);

/* dot product of 2 vectors */
float vec_dot (vector_t *u, vector_t *v);

/* cross product of 2 vectors 
 * computes u x v and stores to dest */
void vec_cross (vector_t *dest, vector_t *u, vector_t *v);

/* returns angle between u and v in degrees */
float vec_angle_between (vector_t *u, vector_t *v);

/* calculates point at given t along given ray, stores to dest */
void point_at_t (point_t *dest, ray_t *ray, float t);


#endif /* R3MATH_H */
