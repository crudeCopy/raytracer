/* chang
 * main function for the raytracer
 */

#include <stdio.h>
#include <stdlib.h>

#include "r3math.h"
#include "cray.h"


int main (int argc, char** argv) {
    scene_t scene;
    image_t output;
    point_t curr_win_point;
    ray_t curr;
    unsigned i, j, base_px;

    /* check if there is a second argument */
    if (argc != 2) {
        fprintf (stderr, "correct usage: %s {path to input file}\n", argv[0]);
        return 0;
    }

    /* clean and stop if scene_from_file fails */
    if (scene_from_file (&scene, argv [1]) == 0) {
        scene_destroy (&scene);
        return 0;
    }

    /* initialize gigantosaurus color_t array */
    output.w = scene.img_width;
    output.h = scene.img_height;
    output.px = malloc (output.w * output.h * sizeof (color_t));
    
    /* determine the properties of the current ray */
    if (scene.projection_type == 0)
        curr = (ray_t) {scene.view_orig, (vector_t) {0, 0, 1}}; /* perspective, orig doesn't change */
    else if (scene.projection_type == 1)
        curr = (ray_t) {scene.ul, scene.view_dir};              /* parallel, dir doesn't change */

    curr_win_point = scene.ul; /* start at upper left */

    /* run through all pixels and cast at them */
    for (i = 0; i < output.h; i++) {
        base_px = i * output.w; /* useful in indexing pixels */

        for (j = 0; j < output.w; j++) {
            if (scene.projection_type == 0) {
                /* find vector from eye to point on window */
                vec_from_to (&(curr.dir), &scene.view_orig, &curr_win_point);
                vec_normalize (&(curr.dir));
            }

            /* trace curr to figure out color of this pixel */
            output.px[base_px + j] = trace_ray (&curr, &scene, 0.0, -1, -1, 0, 0); // TODO TODO its the last ones

            /* move window point over by one delta w */
            point_plus_vec (&curr_win_point, &curr_win_point, &(scene.dw));

            if (scene.projection_type == 1) /* update origin point of ray */
                curr.orig = curr_win_point;
        }
        /* move window point back all the way left and then down one delta h */
        point_plus_vec (&curr_win_point, &curr_win_point, &(scene.ret));
        point_plus_vec (&curr_win_point, &curr_win_point, &(scene.dh));

        if (scene.projection_type == 1) /* update origin point of ray */
            curr.orig = curr_win_point;
    }

    /* out to ppm */
    image_write (argv[1], &output);

    /* free malloc'd stuff */
    scene_destroy (&scene);    
    image_destroy (&output);

    return 1;
}
