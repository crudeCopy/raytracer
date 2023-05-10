/* chang
 * implements ray tracing algorithms */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "cray.h"

#define EPSILON 0.0005 /* a macro to define self-hit error */

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
color_t trace_ray (ray_t *ray, scene_t *scene, float trace_type, int src_obj, int src_tri, int iter_ct, int is_inside) {
    unsigned i;
    float b, c,
          t_add, t_sub, /* variables used in calculation of ray intersection */
          discriminant,
          numerator, denominator,
          closest_distance = FLT_MAX; /* arbitrarily large to start with */
    point_t point_add, point_sub,
            surface_point;
    object_t *curr_obj;
    color_t result_color = scene->bkg_color; /* bkg by default */

    ray_t shadow_ray;
    color_t shadow_flag;

    triangle_t *curr_tri;
    vector_t e_p;
    float d_00, d_01, d_11, d_0p, d_1p,
          determinant;
    point_t barycentrics; /* for barycentrics */
    float last_eta; /* for refraction */

    /* set last_eta to either the bkg_eta of the scene or src's eta */
    if (src_tri == -1 && src_obj == -1) last_eta = scene->bkg_eta;
    else if (src_tri == -1) last_eta = scene->objects[src_obj].color->eta;
    else if (src_obj == -1) last_eta = scene->tris[src_tri].color->eta; // possible issues TODO

    /* check for collision with each object */
    for (i = 0; i < scene->object_count; i++) {
        /* if an object is checking for shadow ray intersects, skips itself */
        if (trace_type > 0.0f && src_obj == i) continue;

        curr_obj = &(scene->objects[i]);

        /* calculate components of intersection model */

        /* A is already known to be 1 */
        b = 2.0 * (ray->dir.x * (ray->orig.x - curr_obj->c.x) +
                   ray->dir.y * (ray->orig.y - curr_obj->c.y) +
                   ray->dir.z * (ray->orig.z - curr_obj->c.z));
        c = pow (ray->orig.x - curr_obj->c.x, 2.0) + pow (ray->orig.y - curr_obj->c.y, 2.0) +
            pow (ray->orig.z - curr_obj->c.z, 2.0) - pow (curr_obj->radius, 2.0);


        /* calculate discriminant, determine if need to go on */
        discriminant = pow (b, 2.0) - 4.0 * c;


        if (discriminant > 0.0f) { /* 2 solutions */
            t_add = (-b + sqrt (discriminant)) / 2.0;
            t_sub = (-b - sqrt (discriminant)) / 2.0;

            point_at_t (&point_add, ray, t_add);
            point_at_t (&point_sub, ray, t_sub);

            /* disqualify if nan or not in front of view */
            if (t_add != t_add || t_add <= EPSILON) t_add = FLT_MAX;
            if (t_sub != t_sub || t_sub <= EPSILON) t_sub = FLT_MAX;

            /* cut things short if just trying to figure out shadow flag */
            if (trace_type > 0.0f && t_add < trace_type && t_add < closest_distance) {
                closest_distance = t_add;

                point_copy(&(shadow_ray.orig), &point_add);
                vec_copy(&(shadow_ray.dir), &(ray->dir));

                shadow_flag = trace_ray(&shadow_ray, scene, trace_type - t_add,
                    src_obj, src_tri, iter_ct + 1, is_inside);
                result_color = (color_t) { shadow_flag.r * (1 - curr_obj->color->alpha), 0, 0 };
                continue;
            }
            else if (trace_type > 0.0f && t_sub < trace_type && t_sub < closest_distance) {
                closest_distance = t_sub;

                point_copy(&(shadow_ray.orig), &point_sub);
                vec_copy(&(shadow_ray.dir), &(ray->dir));

                shadow_flag = trace_ray(&shadow_ray, scene, trace_type - t_sub,
                    src_obj, src_tri, iter_ct + 1, is_inside);
                result_color = (color_t) { shadow_flag.r * (1 - curr_obj->color->alpha), 0, 0 };
                continue;
            }
            else if (trace_type > 0.0f) /* not in shadow */
                continue;

            /* decide which is closest 
             * note: in a situation where previous closest_distance is the same as the closer of
             * these two new ones, the original closest_distance will remain */
            if (t_add <= t_sub && t_add < closest_distance) {
                closest_distance = t_add;
                surface_point = point_add;
            }
            else if (t_sub < t_add && t_sub < closest_distance) {
                closest_distance = t_sub;
                surface_point = point_sub;
            }
            else continue; /* skips assigning result_color */ 

            result_color = shade_ray (scene, &surface_point, ray, i, -1, NULL,
                iter_ct, last_eta, is_inside);
        }
        else if (discriminant == 0.0f) { /* 1 solution */
            t_add = -b / 2.0;

            point_at_t (&point_add, ray, t_add);

            /* disqualify if nan or not in front of view */
            if (t_add != t_add || t_add <= EPSILON) continue;
            
            /* for shadow flag */
            if (trace_type > 0.0f && t_add < trace_type && t_add < closest_distance) { /* in shadow */
                closest_distance = t_add;

                point_copy(&(shadow_ray.orig), &point_add);
                vec_copy(&(shadow_ray.dir), &(ray->dir));

                shadow_flag = trace_ray(&shadow_ray, scene, trace_type - t_add,
                    src_obj, src_tri, iter_ct + 1, is_inside);
                result_color = (color_t) { shadow_flag.r * (1 - curr_obj->color->alpha), 0, 0 };
                continue;
            }
            else if (trace_type > 0.0f) /* not in shadow */
                continue;

            /* if this point is the new least distance along this ray, then replace it
             * and make result color the color of this object, otherwise move on */
            if (t_add < closest_distance) {
                result_color = shade_ray (scene, &point_add, ray, i, -1, NULL,
                    iter_ct, last_eta, is_inside);
                closest_distance = t_add;
            }
        }
    }

    /* check for collision with each triangle */
    for(i = 0; i < scene->tri_count; i++) {
        /* skip tri if coming from a shade ray call on itself */
        if (trace_type > 0.0f && src_tri == i) continue;

        curr_tri = &(scene->tris[i]);

        /* check if the ray intersects the plane */
        denominator = curr_tri->n.x * ray->dir.x +
                      curr_tri->n.y * ray->dir.y +
                      curr_tri->n.z * ray->dir.z;

        if (denominator == 0) continue; /* if the denominator is 0, no finite valued t */

        numerator = -(curr_tri->n.x * ray->orig.x +
                      curr_tri->n.y * ray->orig.y +
                      curr_tri->n.z * ray->orig.z);

        /* if inside of a solid, reverse all n-controlled calculations */
        if (is_inside == 1) {
            denominator = -denominator;
            numerator = -numerator;
        }

        numerator -= curr_tri->d;

        t_add = numerator / denominator;

        point_at_t (&point_add, ray, t_add); /* point_add is point of intersection of ray/plane */

        /* disqualify if nan or not in front of view */
        if (t_add != t_add || t_add <= EPSILON) continue;


        /* if closest point thus far, check if point within triangle */
        if (t_add < closest_distance) {
            /* get dots of triangle vectors */
            d_00 = vec_dot (&(curr_tri->e0), &(curr_tri->e0));
            d_01 = vec_dot (&(curr_tri->e0), &(curr_tri->e1));
            d_11 = vec_dot (&(curr_tri->e1), &(curr_tri->e1));

            /* if determinant of system = 0, no solution */
            determinant = d_00 * d_11 - d_01 * d_01;
            if (determinant == 0) continue;

            /* get vector from p0 to point */
            vec_from_to (&e_p, curr_tri->p[0], &point_add);

            /* get barycenrics beta and gamma with cramer's rule */
            d_0p = vec_dot (&(curr_tri->e0), &e_p);
            d_1p = vec_dot (&(curr_tri->e1), &e_p);

            barycentrics = (point_t) {
                0.0,
                (d_11 * d_0p - d_01 * d_1p) / determinant,
                (d_00 * d_1p - d_01 * d_0p) / determinant
            };
            barycentrics.x = 1 - barycentrics.y - barycentrics.z;
                       
            /* in triangle */
            if ((barycentrics.y > 0 && barycentrics.y < 1) && 
                (barycentrics.z > 0 && barycentrics.z < 1) && (barycentrics.x > 0)) {   
                /* do calc for shadow flag */
                if (trace_type > 0.0f && t_add < trace_type && t_add < closest_distance) {
                    closest_distance = t_add;

                    point_copy(&(shadow_ray.orig), &point_add);
                    vec_copy(&(shadow_ray.dir), &(ray->dir));

                    shadow_flag = trace_ray(&shadow_ray, scene, trace_type - t_add,
                        src_obj, src_tri, iter_ct + 1, is_inside);
                    result_color = (color_t) { shadow_flag.r * (1 - curr_tri->color->alpha), 0, 0 };
                    continue;
                }
                else if (trace_type > 0.0f)
                    continue;

                result_color = shade_ray (scene, &point_add, ray, -1, i, &barycentrics, iter_ct, last_eta, is_inside); /*TODO: could lead to extraneous calls to shade, change this, instead store closest_point and closest_index vars */
                closest_distance = t_add;
            }
        }
    }

    /* if looking for shadow flag and no hits */
    if (trace_type > 0.0f && closest_distance == FLT_MAX)
        result_color = (color_t) {1, 1, 1};

    /* return result */
    return result_color;
}

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
color_t shade_ray (scene_t *scene, point_t *surface, ray_t *ray, int src_obj, 
                   int src_tri, point_t *bary_coords, int iter_ct, float eta_i,
                   int is_inside) {

    float n_dot_l, n_dot_h, /* intermediate calculations */
          dist_to_light,    /* for attenuation */
          shadow_flag;      /* for soft shadows */

    point_t light_orig,       /* origin point of the light (point lights only) */
            moved_light_orig; /* for soft shadows */

    vector_t n, l, v, h; /* vectors used in calculations */

    ray_t shadow_ray;
    color_t new_shadow_flag; /* used in shadow calcs */

    mtl_color_t *color; /* color of the surface */

    color_t precolor, lit_color, result; /* colors used in calculations */

    /* get origin, color, normal vector */
    if (src_obj >= 0 && src_obj < scene->object_count) {
        /* get pointer to source object */
        object_t *obj = &(scene->objects[src_obj]);

        /* mtl color */
        color = obj->color;/* TODO TODO: MAKE COLOR OF LIGHT AND DC A POINTER INSTEAD */

        /* surface normal */
        vec_from_to (&n, &(obj->c), surface);
        vec_normalize (&n);
    
        if (obj->texture != NULL) { /* reassign od if has a texture */
            float u_f, v_f;
            int u_i, v_i, index;

            /* get v (phi) and u (theta) */
            v_f = acos (n.z);
            u_f = atan2 (n.y, n.x);

            /* normalize v between 0 and 1 */
            v_f /= M_PI;

            /* normalize u between 0 and 1 correctly */
            if (u_f > 0)
                u_f /= 2 * M_PI;
            else if (u_f < 0)
                u_f = (u_f + 2 * M_PI) / ( 2 * M_PI);

            /* get color at point */
            u_i = (int) (u_f * obj->texture->w);
            v_i = (int) (v_f * obj->texture->h);

            /* use correct index to find color at pixel in texture */
            index = (v_i * obj->texture->w + (u_i % obj->texture->w));
            color_copy(&(color->od), &(obj->texture->px[index]));
        }
    }
    else if (src_tri >= 0 && src_tri < scene->tri_count) {
        /* get pointer to source tri */
        triangle_t *tri = &(scene->tris[src_tri]);

        /* mtl color */
        color = tri->color;

        /* if tri has texture, reassign Od */
        if (tri->texture != NULL) {
            float u_f, v_f;
            int u_i, v_i, index;

            /* get u and v */
            u_f = bary_coords->x * tri->coords[0]->u +
                bary_coords->y * tri->coords[1]->u +
                bary_coords->z * tri->coords[2]->u;
            v_f = bary_coords->x * tri->coords[0]->v +
                bary_coords->y * tri->coords[1]->v +
                bary_coords->z * tri->coords[2]->v;

            /* get index into texture */
            u_i = (int) (u_f * tri->texture->w);
            v_i = (int) (v_f * tri->texture->h);

            index = (v_i * tri->texture->w + (u_i % tri->texture->w));

            color_copy(&(color->od), &(tri->texture->px[index]));
        }

        /* surface normal */
        if (tri->norms[0] == NULL) /* no vert norms */
            vec_copy(&n, &(tri->n));
        else { /* with vert norms */
            vector_t tmp;

            /* n = alpha * norms[0] */
            vec_copy(&n, tri->norms[0]);
            vec_scale(&n, bary_coords->x);
            
            /* n += beta * norms[1] */
            vec_copy(&tmp, tri->norms[1]);
            vec_scale(&tmp, bary_coords->y);
            vec_add(&n, &n, &tmp);

            /* n += gamma * norms[2] */
            vec_copy(&tmp, tri->norms[2]);
            vec_scale(&tmp, bary_coords->z);
            vec_add(&n, &n, &tmp);
        }

        vec_normalize(&n);
    }
    else {
        fprintf(stderr, "shade_ray received an incorrect index\n");
        return (color_t) { 1, 0, 0 };
    }

    if (iter_ct > 10) {
        return color->od;
    }

    /* if inside of a solid object, reverse N */
    if (is_inside == 1) vec_scale(&n, -1);

    /* ambient term */
    color_copy(&result, &(color->od));
    color_scale(&result, color->ka);

    /* vector from surface point to ray origin */
    vec_from_to (&v, surface, &(ray->orig));
    vec_normalize (&v);

    for (int i = 0; i < scene->light_count; i++) {
        /* get pointer to current light */
        light_t *light = &(scene->lights[i]);

        /* from surface towards light */
        if (light->type == 0) { /* directional */
            l = (vector_t) { -light->x, -light->y, -light->z };
            dist_to_light = FLT_MAX; /* ALMOST infinitely far away */
        }
        else if (light->type > 0) { /* point */
            light_orig = (point_t) { light->x, light->y, light->z };
            vec_from_to(&l, surface, &light_orig);
            dist_to_light = vec_length(&l);
        }
        vec_normalize(&l);

        /* halfway between v and l */
        vec_add(&h, &l, &v);
        vec_normalize(&h);

        /* diffuse term */
        n_dot_l = color->kd * fmax(0.0, vec_dot (&n, &l));
        color_copy(&lit_color, &(color->od));
        color_scale(&lit_color, n_dot_l);

        /* specular term */
        n_dot_h = color->ks * pow(fmax(0.0, vec_dot(&n, &h)), color->n);
        color_copy(&precolor, &(color->os));
        color_scale(&precolor, n_dot_h);
        color_add(&lit_color, &lit_color, &precolor);

        /* factor in light hue/intensity */
        lit_color.r *= light->color.r;
        lit_color.g *= light->color.g;
        lit_color.b *= light->color.b;

        /* attenuated lights */
        if (light->type == 2) {
            float f_att = fmax(EPSILON, (light->c1 + light->c2 *
                dist_to_light + light->c3 * pow(dist_to_light, 2.0)));

            color_scale(&lit_color, f_att);
        }

        /* factor in shadow */
        shadow_ray.orig = (point_t) { surface->x, surface->y, surface->z };
        vec_copy(&(shadow_ray.dir), &l);

        /* if soft shade on and point light, get many values */
        if (light->type > 0 && scene->soft_shading == 1) { 
            float light_delta = scene->soft_shade_light_delta;
            int ray_count = scene->soft_shade_ray_count;

            shadow_flag = 0.0;

            for (int j = 0; j < ray_count; j++) {
                /* jitter the origin of the light source */
                moved_light_orig.x = light_orig.x + light_delta * 
                    ((float) rand () / (float) (RAND_MAX / 2.0) - 1); 
                moved_light_orig.y = light_orig.y + light_delta *
                    ((float) rand () / (float) (RAND_MAX / 2.0) - 1);
                moved_light_orig.z = light_orig.z + light_delta *
                    ((float) rand () / (float) (RAND_MAX / 2.0) - 1);

                vec_from_to(&(shadow_ray.dir), &(shadow_ray.orig), &moved_light_orig);

                dist_to_light = vec_length(&(shadow_ray.dir));
                vec_normalize(&(shadow_ray.dir));

                shadow_flag += trace_ray(&shadow_ray, scene, dist_to_light,
                    src_obj, src_tri, iter_ct, is_inside).r;
            }
            
            /* average results */
            shadow_flag /= (float) ray_count;
        }
        /* if soft shade off or directional, one simple ray will do */
        else {
            new_shadow_flag = trace_ray(&shadow_ray, scene, dist_to_light,
                src_obj, src_tri, iter_ct, is_inside);
            shadow_flag = new_shadow_flag.r;
        }
       
        /* incorporate the shadow flag into light-dependent color */
        color_scale(&lit_color, shadow_flag);

        /* add lit color to the result */
        color_add(&result, &result, &lit_color);
    }

    /* if enabled, do depth cueing */
    if (scene->has_depth_cue == 1) {
        /* calculate depth cue's alpha */
        float alpha_dc = depth_cue_alpha(&(scene->depth_cue), point_distance(&(scene->view_orig), surface));

        /* incorporate alpha into result */
        color_scale(&result, alpha_dc);
        color_copy(&precolor, &(scene->depth_cue.color));
        color_scale(&precolor, 1.0 - alpha_dc);
        color_add(&result, &result, &precolor);
    }

    /****** REFLECTANCE AND TRANSPARENCY ******/

    /* get fresnel reflectance coefficient */
    float n_dot_v = vec_dot(&n, &v);
    float fr = color->f0 + (1 - color->f0) * pow((1 - n_dot_v), 5);

    /* determine if reflection calculations are necessary */
    if (color->ks > 0) {
        /* scale N by 2*n_dot_v to get A */
        vector_t a;
        vec_copy(&a, &n);
        vec_scale(&a, 2 * n_dot_v);

        /* get R by subtracting V from A */
        vector_t _v, r;
        vec_copy(&_v, &v);
        vec_scale(&_v, -1);
        vec_add(&r, &a, &_v);

        /* put R into a ray from the surface, and trace */
        ray_t secondary_ray;
        color_t reflective_color;

        secondary_ray.orig = (point_t) { surface->x, surface->y, surface->z };
        vec_copy(&(secondary_ray.dir), &r);

        reflective_color = trace_ray(&secondary_ray, scene, 0.0, -1, -1, iter_ct + 1, is_inside);

        /* modify rgb */
        color_scale(&reflective_color, fr);
        color_add(&result, &result, &reflective_color);
    }

    /* determine if transparency calculations are necessary */
    if (color->alpha < 1) {
        float eta_t, eta_r, left_coef;
        vector_t refrac, a_vec, b_vec, _v;

        eta_t = color->eta;

        /* if inside, eta_i and eta_t will be the same, set eta_t to bkg eta */
        if (is_inside == 1) eta_t = scene->bkg_eta;

        eta_r = eta_i / eta_t;
        
        /* if no total internal reflection */
        left_coef = 1 - pow(eta_r, 2) * (1 - pow(n_dot_v, 2));

        if (left_coef >= 0) {
            left_coef = -sqrt(left_coef);

            vec_copy(&a_vec, &n);
            vec_copy(&b_vec, &n);
            vec_copy(&_v, &v);
            
            vec_scale(&a_vec, left_coef);
            vec_scale(&b_vec, n_dot_v);
            vec_scale(&_v, -1);
            vec_add(&b_vec, &b_vec, &_v);
            vec_scale(&b_vec, eta_r);
            vec_add(&refrac, &a_vec, &b_vec);
            vec_normalize(&refrac);

            /* make a ray starting at the surface and going on in the dir of refrac */
            ray_t tertiary_ray;
            tertiary_ray.orig = (point_t) { surface->x, surface->y, surface->z };
            vec_copy(&(tertiary_ray.dir), &refrac);
     
            /* get whether inside or not now */
            int new_inside = 1;
            if (is_inside == 1) new_inside = 0;
            color_t transparent_color = trace_ray(&tertiary_ray, scene, 0.0, src_obj, src_tri, iter_ct + 1, new_inside);

            /* modify rgb */
            float trans_coef = (1 - fr) * (1 - color->alpha);
            color_scale(&transparent_color, trans_coef);
            color_add(&result, &result, &transparent_color);
        }
    }

    /* check for overflow */
    result.r = fmin(1.0, result.r);
    result.g = fmin(1.0, result.g);
    result.b = fmin(1.0, result.b);

    return result;
}
