/* chang
 * expands on the scene type, adding a method for reading */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "scene.h"

/* read scene info from input file located at file_path into scene */
int scene_from_file (scene_t *scene, char *file_path) {
    FILE *file;
    /* variables to hold scanned values */
    char key[20], args[128];
    float f1, f2, f3;
    unsigned i1, i2, 
             current_mtl_color = 0;

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

            printf ("read eye (%f, %f, %f)\n", f1, f2, f3);
        }
        else if (strcmp (key, "viewdir") == 0) { /* viewdir */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->view_dir = (vector_t) {f1, f2, f3};

            printf ("read viewdir (%f, %f, %f)\n", f1, f2, f3);
        }
        else if (strcmp (key, "updir") == 0) { /* updir */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->up_dir = (vector_t) {f1, f2, f3};

            printf ("read updir (%f, %f, %f)\n", f1, f2, f3);
        }
        else if (strcmp (key, "hfov") == 0) { /* hfov */
            fscanf (file, "%f", &f1);
            scene->fov_h = f1;

            printf ("read hfov %f degrees\n", f1);
        }
        else if (strcmp (key, "imsize") == 0) { /* imsize */
            fscanf (file, "%d %d", &i1, &i2);
            scene->img_width = i1;
            scene->img_height = i2;

            printf ("read imsize %dpx x %dpx\n", i1, i2);
        }
        else if (strcmp (key, "bkgcolor") == 0) { /* bkgcolor */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->bkg_color = (color_t) {f1, f2, f3};

            printf ("read bkgcolor (%f, %f, %f)\n", f1, f2, f3);
        }
        else if (strcmp (key, "mtlcolor") == 0) { /* mtlcolor */
            /* check if max number of colors are already loaded */
            printf ("\nMAX MTL COLORS : %d\nCURR MTL COLOR : %d\n\n", MAX_MTL_COLORS, current_mtl_color);
            if (current_mtl_color >= MAX_MTL_COLORS) {
                fprintf (stderr, "too many colors loaded, read fail\n");
                return 0;
            }

            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            current_mtl_color++;
            scene->mtl_colors [current_mtl_color] = (color_t) {f1, f2, f3};

            printf ("read mtlcolor (%f, %f, %f)\n", f1, f2, f3);
        }
        else if (strcmp (key, "sphere") == 0) { /* sphere */
            /* check if max number of objects are already loaded */
            if (scene->object_count >= MAX_OBJECTS) {
                fprintf (stderr, "too many objects loaded, read fail\n");
                return 0;
            }

            fgets (args, sizeof(args), file); /* read whole rest of line */

            object_init (&(scene->objects [scene->object_count]), 0, &(scene->mtl_colors [current_mtl_color - 1]), args); /* awfully long */
            scene->object_count++;

            printf ("read %s, object #%d, args: %s\n", key, scene->object_count, args);
        }
        else if (strcmp (key, "cylinder") == 0) { /* cylinder */
            /* check if max number of objects are already loaded */
            if (scene->object_count >= MAX_OBJECTS) {
                fprintf (stderr, "too many objects loaded, read fail\n");
                return 0;
            }

            fgets (args, sizeof(args), file); /* read whole rest of line */

            object_init (&(scene->objects [scene->object_count]), 1, &(scene->mtl_colors [current_mtl_color]), args); /* awfully long */
            scene->object_count++;

            printf ("read %s, object #%d, args: %s\n", key, scene->object_count, args);
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
        }
    }

    /* close file */
    fclose (file);

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
