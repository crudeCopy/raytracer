/* chang
 * expands on the scene type, adding a method for reading */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
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
    point_t win_center;
    vector_t view_orig_to_win,
             center_to_corner;
    float half_height, half_width;

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
            if (current_mtl_color >= MAX_MTL_COLORS) {
                fprintf (stderr, "too many colors loaded, read fail\n");
                return 0;
            }

            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            current_mtl_color++;
            scene->mtl_colors [current_mtl_color] = (color_t) {f1, f2, f3};

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
        fprintf (stderr, "scene_from_file () read from an input file with one or more missing essential value");
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
    if (has_updir == 0) scene->up_dir = (vector_t) {0, 1, 0};            /* updir automatically "straight-on" or "no-roll" */
    if (has_bkgcolor == 0) scene->bkg_color = (color_t) {0.0, 0.0, 0.0}; /* bkgcolor automatically black */
    if (has_projection == 0) scene->projection_type = 0;                 /* projection automatically perspective */    

    /* extrapolate unknowns from input known values */
    scene->d = 1;
    scene->aspect = scene->img_width / scene->img_height;

    scene->win_width = 2 * scene->d * tan (0.5 * scene->fov_h);         /* win_width */
    scene->win_height = scene->win_width / scene->aspect; /* win_height */
    scene->fov_v = 2 * atan (scene->win_height / (2 * scene->d));       /* fov_v */

    vec_scale (&view_orig_to_win, scene->d);
    point_plus_vec (&win_center, &(scene->view_orig), &view_orig_to_win); /* win_center will help for corners */

    half_width = scene->win_width / 2;
    half_height = scene->win_height / 2; /* for corners */

    center_to_corner = (vector_t) {-half_width, half_height, 0.0};
    point_plus_vec (&(scene->ul), &win_center, &center_to_corner); /* ul */

    center_to_corner = (vector_t) {half_width, half_height, 0.0};
    point_plus_vec (&(scene->ur), &win_center, &center_to_corner); /* ur */

    center_to_corner = (vector_t) {-half_width, -half_height, 0.0};
    point_plus_vec (&(scene->ll), &win_center, &center_to_corner); /* ll */

    center_to_corner = (vector_t) {half_width, -half_height, 0.0};
    point_plus_vec (&(scene->lr), &win_center, &center_to_corner); /* lr */

    scene->dw = scene->win_width / (scene->img_width - 1); /* dw */
    scene->dh = scene->dw / scene->aspect;                 /* dh */

    vec_cross (&(scene->u), &(scene->view_dir), &(scene->up_dir));
    vec_normalize (&(scene->u)); /* u, normalized */

    vec_cross (&(scene->v), &(scene->n), &(scene->u)); /*v, cross of two orthogonal unit vecs, so already normal */

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
