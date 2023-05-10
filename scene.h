/* chang
 * implements a scene type, which holds info from the input file
 */

#ifndef SCENE_H
#define SCENE_H

#include "r3math.h"
#include "object.h"
#include "color.h"
#include "light.h"
#include "triangle.h"


/* constants for maximum values of objects and colors */
const static unsigned MAX_OBJECTS = 10,
                      MAX_MTL_COLORS = 10,
                      MAX_LIGHTS = 5,
                      MAX_VERTS = 5000,
                      MAX_TRIS = 7000,
                      MAX_TEXTURES = 5;

/* scene type; one image will have one scene_t associated
 * three groups of members:
 *    known before input -> d
 *
 *    from input file -> view_orig, view_dir, up_dir, fov_h, objects, object_count, 
 *                       img_width, img_height, projection_type, bkg_color, mtl_colors
 *
 *    calculated -> win_width, win_height, aspect, fov_v, ul, ur, ll, lr, dw, dh, u, v, n TODO TODO NOT ACCURATE
*/
typedef struct {
    /* SCENE POSITIONAL INFO */

    point_t view_orig,      /* center of projection */
            ul, ur, ll, lr; /* corners of viewing window */

    vector_t view_dir, /* viewing direction */
             up_dir,   /* global "up" direction */
             u, v,     /* vecs orthogonal to viewing window plane */
             n,        /* vec normal to viewing window plane; maybe unnecessary? just -1*view_dir */
             dw, dh,   /* translations from one window location to next */
             ret;      /* translations from end of window row to beginning */

    unsigned img_width,       /* width of output in pixels */
             img_height,      /* height of output in pixels */ 
             projection_type; /* type of projection; 0 = perspective, 1 = parallel; 0 by default */

    float win_width,  /* width of the viewing window */
          win_height, /* height of the viewing window*/
          aspect,     /* aspect ratio */
          fov_h,      /* horizontal field of view; to be supplied */
          d;          /* distance along view_dir between view_orig and center point of viewing window; arbitrary */

    /* COLOR NONSENSERY */

    color_t bkg_color;       /* background color */
    float bkg_eta;           /* background color index of refraction */
    mtl_color_t *mtl_colors; /* material color array, up to MAX_MTL_COLORS */
          
    /* OBJECT MISCELLANIA */

    unsigned object_count; /* how many object in scene */
    object_t *objects;     /* set of given objects to include, up to MAX_OBJECTS */

    /* LIGHT THINGS */

    unsigned light_count; /* how many lights in scene */
    light_t *lights;      /* array of light sources included in the scene */

    unsigned soft_shading,         /* whether to soft shade from point light sources; 0 = no, 1 = yes (default 0) */
             soft_shade_ray_count; /* how many rays to "bundle" and shoot using soft shading */
    float soft_shade_light_delta;  /* how far from the light origin a soft-shade ray can be at most */
    
    unsigned has_depth_cue; /* whether to use depth cue in scene; 0 = no, 1 = yes (default 0) */
    depth_cue_t depth_cue; /* holds user-given values about depth cue */

    /* POLYGON STUFF */

    unsigned vert_count; /* number of vertices stored, up to MAX_VERTS */
    point_t *verts;      /* array of all vertices */

    unsigned tri_count; /* number of triangles stored, up to MAX_TRIS */
    triangle_t *tris;   /* array of all triangles */

    unsigned norm_count;
    vector_t *norms;    /* vector normal array */

    unsigned texture_count;
    image_t *textures; /* texture image array */

    unsigned img_coord_count;
    img_coord_t *img_coords; /* array of image coordinates */
} scene_t;

/* read scene info from input file located at file_path into scene 
 * returns 1 if success, 0 if fail */
int scene_from_file (scene_t *scene, char *file_path);

/* destroy scene object */
void scene_destroy (scene_t *scene);

#endif /* SCENE_H */
