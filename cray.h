/* chang
 * declares ray tracing algorithms */

#ifndef CRAY_H
#define CRAY_H

#include "scene.h"

/* trace ray, similar to specification
 *   ray: the ray being traced
 *   scene: the scene in which to trace the ray
 *   trace_type: variable that is either 0 to signify a first-level call or
 *               equal to the distance between the surface point and a light source
 *   src_obj: the index of object which a second-level call comes from, or -1
 *   src_tri: the index of triangle which a second-level call comes from, or -1
 *   iter_ct: used to keep track of how many iterations the trace_ray\shade_ray
 *            cycle has undergone
 *   is_inside: 1 if within a solid obj, 0 otherwise
 */
color_t trace_ray (ray_t *ray, scene_t *scene, float trace_type, int src_obj, int src_tri, int iter_ct, int is_inside);

/* shade ray, similar to spec
 *   scene: scene in which to shade
 *   surface: point on surface which was hit in trace_ray
 *   ray: ray which was used in trace_ray
 *   src_obj: index of object being shaded
 *   src_tri: index of triangle being shaded 
 *   bary_coords: point in triangle's barycentric coordinates
 *   iter_ct: used to keep track of iterations in cycle and terminate if too high
 *   eta_i: holds the index of refraction of the last object
 *   is_inside: 1 if within a solid obj, 0 otherwise
 */
color_t shade_ray (scene_t *scene, point_t *surface, ray_t *ray, int src_obj, int src_tri, point_t *bary_coords, int iter_ct, float eta_i, int is_inside);

#endif /* CRAY_H */
