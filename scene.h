/* chang
 * implements a scene type, which holds info from the input file */

#ifndef SCENE_H
#define SCENE_H

#include "point.h"
#include "vector.h"
#include "object.h"
#include "color.h"

/* constants for maximum values of objects and colors */
const static unsigned MAX_OBJECTS = 10,
               MAX_MTL_COLORS = 10;

/* scene type; one image will have one scene_t associated
 * three groups of members:
 *    known before input -> d
 *
 *    from input file -> view_orig, view_dir, up_dir, fov_h, objects, object_count, 
 *                       img_width, img_height, projection_type, bkg_color, mtl_colors
 *
 *    calculated -> ul, ur, ll, lr, u, v, n, fov_v
*/
typedef struct {
    point_t view_orig,      /* center of projection */
            ul, ur, ll, lr; /* corners of viewing window */

    vector_t view_dir, /* viewing direction */
             up_dir,   /* global "up" direction */
             u, v,     /* vecs orthogonal to viewing window plane */
             n;        /* vec normal to viewing window plane; maybe unnecessary? just -1*view_dir */

    float fov_h, /* horizontal field of view; to be supplied */
          fov_v, /* vertical field of view; to be calculated */
          d;     /* distance along view_dir between view_orig and center point of viewing window; arbitrary */

    object_t *objects; /* set of given objects to include, holds up to 10 */

    unsigned object_count,    /* holds how many items are initialized in objects */
             img_width,       /* width of output in pixels */
             img_height,      /* height of output in pixels */ 
             projection_type; /* type of projection; 0 = perspective, 1 = parallel; 0 by default */

    color_t bkg_color,       /* background color */
            *mtl_colors; /* material color */
} scene_t;

/* read scene info from input file located at file_path into scene 
 * returns 1 if success, 0 if fail */
int scene_from_file (scene_t *scene, char *file_path);

/* destroy scene object */
void scene_destroy (scene_t *scene);

#endif /* SCENE_H */
