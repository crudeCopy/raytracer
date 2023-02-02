/* chang
 * main function */

#include <stdio.h>
#include <stdlib.h>
#include "ray.h"
#include "scene.h"

int main (int argc, char** argv) {
    scene_t scene;
    color_t *pixels;
    point_t curr_win_point;
    ray_t curr;
    unsigned i, j, 
             total_pixels,
             base_pixels;


    /* check if there is a second argument */
    if (argc != 2) {
        fprintf (stderr, "correct usage: ./cray {path to input file}\n");
        return 0;
    }


    /* clean and stop if scene_from_file fails */
    if (scene_from_file (&scene, argv [1]) == 0) {
        scene_destroy (&scene);
        return 0;
    }


    /* initialize gigantosaurus color_t array */
    total_pixels = scene.img_width * scene.img_height;
    pixels = malloc (total_pixels * sizeof (color_t));

    curr = (ray_t) {scene.view_orig, (vector_t) {0, 0, 1}};
    curr_win_point = scene.ul; /* start at upper left */


    for (i = 0; i < scene.img_height; i++) {
        base_pixels = scene.img_width * i; /* useful in indexing pixels */

        for (j = 0; j < scene.img_width; j++) {
            /* find vector from eye to point on window */
            vec_from_to (&(curr.dir), &scene.view_orig, &curr_win_point);
            vec_normalize (&(curr.dir));


            /* trace curr to figure out color of this pixel */
            pixels [base_pixels + j] = trace_ray (&curr, &scene);


            /* move window point over by one delta w */
            point_plus_vec (&curr_win_point, &curr_win_point, &(scene.dw));
        }
        /* move window point back all the way left and then down one delta h */
        point_plus_vec (&curr_win_point, &curr_win_point, &(scene.ret));
        point_plus_vec (&curr_win_point, &curr_win_point, &(scene.dh));
    }


    /* out to ppm */
    write_colors_to_ppm (argv [1], pixels, scene.img_width, scene.img_height);


    /* free malloc'd stuff */
    scene_destroy (&scene);    
    free (pixels);


    return 1;
}
