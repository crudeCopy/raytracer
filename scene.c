/* chang
 * expands on the scene type, adding a method for reading */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "scene.h"

/* read scene info from input file located at file_path into scene */
int scene_from_file (scene_t *scene, char *file_path) {
    FILE *file;
    /* variables to hold scanned values */
    char key[20], args[128];
    float f1, f2, f3;
    unsigned i1, i2, 
             current_mtl_color = 0;

    /* variables to check for important variable initialization */
    unsigned has_eye = 0,
             has_viewdir = 0,
             has_updir = 0,
             has_hfov = 0,
             has_imsize = 0,
             has_bkgcolor = 0,
             has_mtlcolors = 0,  /* these two will be true (=1) if any    */
             has_objects = 0,    /* number of objects/mtlcolors are added */
             has_projection = 0;

    /* variables for math after reading */
    point_t win_center,
            new_end;
    vector_t view_orig_to_win,
             half_width_u,
             half_height_v;

    /* set object count to 0 */
    scene->object_count = 0;

    /* initialize objects and mtl_colors */
    scene->objects = malloc (MAX_OBJECTS * sizeof (object_t));
    scene->mtl_colors = malloc (MAX_MTL_COLORS * sizeof (color_t));

    /* open file, test if exists/is open */
    file = fopen (file_path, "r");

    if (file == NULL) {
        fprintf (stderr, "scene_from_file () was passed a path to a file %s that either doesn't exist or cannot be opened, read scene fail\n", file_path);
        return 0;
    }

    /* read through keywords until EOF */    
    while (fscanf (file, "%s ", key) != EOF) {
        if (strcmp (key, "eye") == 0) { /* eye */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->view_orig = (point_t) {f1, f2, f3};

            has_eye = 1;
        }
        else if (strcmp (key, "viewdir") == 0) { /* viewdir */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->view_dir = (vector_t) {f1, f2, f3};
            scene->n = (vector_t) {-f1, -f2, -f3}; /* n is defined as -view_dir */

            /* normalize */
            has_viewdir = vec_normalize (&(scene->view_dir)); /* will be 0 if vector is {0, 0, 0}, 1 otherwise */
            vec_normalize (&(scene->n));
        }
        else if (strcmp (key, "updir") == 0) { /* updir */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->up_dir = (vector_t) {f1, f2, f3};

            /* normalize */
            has_updir = vec_normalize (&(scene->up_dir));
        }
        else if (strcmp (key, "hfov") == 0) { /* hfov */
            fscanf (file, "%f", &f1);
            scene->fov_h = f1;

            has_hfov = 1;
        }
        else if (strcmp (key, "imsize") == 0) { /* imsize */
            fscanf (file, "%d %d", &i1, &i2);
            scene->img_width = i1;
            scene->img_height = i2;

            has_imsize = 1;
        }
        else if (strcmp (key, "bkgcolor") == 0) { /* bkgcolor */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->bkg_color = (color_t) {f1, f2, f3};

            has_bkgcolor = 1;
        }
        else if (strcmp (key, "mtlcolor") == 0) { /* mtlcolor */
            /* check if max number of colors are already loaded */
            if (current_mtl_color >= MAX_MTL_COLORS - 1) {
                fprintf (stderr, "too many colors loaded, read fail\n");
                return 0;
            }

            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->mtl_colors [current_mtl_color] = (color_t) {f1, f2, f3};
            current_mtl_color++;

            has_mtlcolors = 1;
        }
        else if (strcmp (key, "sphere") == 0 || strcmp (key, "cylinder") == 0) { /* object */
            unsigned type = 0;

            /* check if max number of objects are already loaded */
            if (scene->object_count >= MAX_OBJECTS) {
                fprintf (stderr, "too many objects loaded, read fail\n");
                return 0;
            }

            /* check if there is a color for this thing */
            if (has_mtlcolors != 1) {
                fprintf (stderr, "scene_from_file () read an object before a mtlcolor, read fail\n");
                return 0;
            }

            /* check for material type, WILL get more complex */
            if (strcmp (key, "cylinder") == 0) type = 1;

            fgets (args, sizeof(args), file); /* read whole rest of line */

            object_init (&(scene->objects [scene->object_count]), type, &(scene->mtl_colors [current_mtl_color - 1]), args); /* awfully long */
            scene->object_count++;

            has_objects = 1;
        }
        else if (strcmp (key, "projection") == 0) { /* projection */
            fscanf (file, "%s", args);
            if (strcmp (args, "perspective") == 0) {
                scene->projection_type = 0;
            } else if (strcmp (args, "parallel") == 0) {
                scene->projection_type = 1;
            } else {
                fprintf (stderr, "scene_from_file () read keyword projection but found undefined projection type afterwards, read fail\n");
                return 0;
            }
            has_projection = 1;
        }
    }

    /* close file */
    fclose (file);


    /* now that input has been gotten, check if all necessary values are present */
    if ((has_eye + has_viewdir + has_hfov + has_imsize) != 4) {
        fprintf (stderr, "scene_from_file () read from an input file with one or more missing essential value\n");
        return 0;
    }


    /* prompt the user if they REALLY want to render an image with no objects */
    if (has_objects == 0) {
        char go_on = 'n';

        printf ("scene_from_file () retrieved no objects from the input file. still render? (y/n)  ");
        scanf ("%c", &go_on);

        if (go_on != 'y') return 0;
    }


    /* setup default values if necessary */
    if (has_updir == 0) scene->up_dir = (vector_t) {0.0, 1.0, 0.0};      /* updir automatically "straight-on" or "no-roll" */
    if (has_bkgcolor == 0) scene->bkg_color = (color_t) {0.0, 0.0, 0.0}; /* bkgcolor automatically black */
    if (has_projection == 0) scene->projection_type = 0;                 /* projection automatically perspective */    

    
    /* extrapolate unknowns from input known values */

    scene->d = 2.0;
    scene->aspect = ((float) scene->img_width) / ((float) scene->img_height);
    
    scene->win_width = 2.0 * scene->d * tan (0.5 * (scene->fov_h * (M_PI/180.0)));         /* win_width */
    scene->win_height = scene->win_width / scene->aspect; /* win_height */
    /* TODO is fov_v needed? scene->fov_v = 2.0 * atan (scene->win_height / (2.0 * scene->d)) * (180.0/M_PI);*/ /* fov_v */


    /* VECTOR TIME */

    vec_cross (&(scene->u), &(scene->view_dir), &(scene->up_dir));
    vec_normalize (&(scene->u)); /* u, normalized */

    vec_cross (&(scene->v), &(scene->u), &(scene->view_dir)); /* v, cross of two orthog. unit vecs, already normal */


    /* CORNER TIME */

    view_orig_to_win = scene->view_dir;
    vec_scale (&view_orig_to_win, scene->d); 
    point_plus_vec (&win_center, &(scene->view_orig), &view_orig_to_win); /* win_center = view_origin + d*view_dir */
  
    half_width_u = scene->u;
    vec_scale (&half_width_u, -(scene->win_width / 2.0)); /* -(w/2)*u */
    half_height_v = scene->v;
    vec_scale (&half_height_v, scene->win_height / 2.0);  /* +(h/2)*v */
    
    point_plus_vec (&(scene->ul), &win_center, &half_width_u);
    point_plus_vec (&(scene->ul), &(scene->ul), &half_height_v); /* ul */

    vec_scale (&half_width_u, -1.0); /* +(w/2)*u */
    point_plus_vec (&(scene->ur), &win_center, &half_width_u);
    point_plus_vec (&(scene->ur), &(scene->ur), &half_height_v); /* ur */

    vec_scale (&half_width_u, -1.0); /* -(w/2)*u */
    vec_scale (&half_height_v, -1.0); /* -(h/2)*v */
    point_plus_vec (&(scene->ll), &win_center, &half_width_u);
    point_plus_vec (&(scene->ll), &(scene->ll), &half_height_v); /* ll */

    vec_scale (&half_width_u, -1.0); /* +(w/2)*u */
    point_plus_vec (&(scene->lr), &win_center, &half_width_u);
    point_plus_vec (&(scene->lr), &(scene->lr), &half_height_v); /* lr */


    /* DELTAS */

    vec_from_to (&(scene->dw), &(scene->ul), &(scene->ur));
    vec_from_to (&(scene->dh), &(scene->ul), &(scene->ll));
    vec_scale (&(scene->dw), 1.0 / ((float) scene->img_width - 1.0));
    vec_scale (&(scene->dh), 1.0 / ((float) scene->img_height - 1.0));
    point_plus_vec (&new_end, &(scene->ur), &(scene->dw));
    vec_from_to (&(scene->ret), &new_end, &(scene->ul));


    return 1;
}

/* destructor for initialized scenes */
void scene_destroy (scene_t *scene) {
    /* destroy each object */
    for (int i = 0; i < scene->object_count; i++) {
        object_destroy (&(scene->objects [i]));
    }

    /* destroy objects and mtl_colors */
    free (scene->objects);
    free (scene->mtl_colors);
}

/* trace ray, similar to specification */
color_t trace_ray (ray_t *ray, scene_t *scene) {
    unsigned i; //obj_type;
    float b, c,
          t_add, t_sub,
          t_add_dist, t_sub_dist, /* variables used in calculation of ray intersection */
          discriminant,
          closest_distance = FLT_MAX; /* arbitrarily large to start with */
    point_t point_add, point_sub;
    object_t curr_obj;
    color_t result_color = scene->bkg_color; /* bkg by default */

    /* check for collision with each object UPDATE FOR CYL */
    for (i = 0; i < scene->object_count; i++) {
        curr_obj = scene->objects[i];

        /* calculate B and C of the intersection model; A is already known to be 1 */

        b = 2.0 * (ray->dir.x * (ray->orig.x - curr_obj.args[0]) +
                 ray->dir.y * (ray->orig.y - curr_obj.args[1]) +
                 ray->dir.z * (ray->orig.z - curr_obj.args[2]));
        c = pow (ray->orig.x - curr_obj.args[0], 2.0) + pow (ray->orig.y - curr_obj.args[1], 2.0) +
            pow (ray->orig.z - curr_obj.args[2], 2.0) - pow (curr_obj.args[3], 2.0);

        /* calculate discriminant, determine if need to go on */
        discriminant = pow (b, 2.0) - 4.0 * c;

        if (discriminant > 0) { /* 2 solutions */
            t_add = (-b + sqrt (discriminant)) / 2.0;
            t_sub = (-b - sqrt (discriminant)) / 2.0;

            point_at_t (&point_add, ray, t_add);
            point_at_t (&point_add, ray, t_sub);

            t_add_dist = point_distance (&(ray->orig), &point_add);
            t_sub_dist = point_distance (&(ray->orig), &point_sub);

            /* disqualify if not in front of view */
            if (t_add_dist <= 0) t_add_dist = FLT_MAX;
            if (t_add_dist <= 0) t_add_dist = FLT_MAX;

            /* decide which is closest 
             * note: in a situation where previous closest_distance is the same as the closer of
             * these two new ones, the original closest_distance will remain */
            if (t_add_dist <= t_sub_dist && t_add_dist < closest_distance)
                closest_distance = t_add_dist;
            else if (t_sub_dist < t_add_dist && t_sub_dist < closest_distance)
                closest_distance = t_sub_dist;
            else continue; /* skips assigning result_color */

            result_color = shade_ray (&curr_obj);
        }
        else if (discriminant == 0) { /* 1 solution */
            t_add = -b / 2.0;

            point_at_t (&point_add, ray, t_add);

            t_add_dist = point_distance (&(ray->orig), &point_add);

            /* if this point is the new least distance along this ray, then replace it
             * and make result color the color of this object, otherwise move on */
            if (t_add_dist < closest_distance) {
                result_color = shade_ray (&curr_obj);
                closest_distance = t_add_dist;
            }
        }
    }

    /* return result */
    return result_color;
}

/* shade ray, similar to spec */
color_t shade_ray (object_t *obj) {
    /* for now, just return its color */
    return obj->color;
}
