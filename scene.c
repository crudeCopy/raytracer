/* chang
 * expands on the scene type, adding a method for reading 
 */

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
    float f1, f2, f3,
          length;
    unsigned i1, i2, i3,
             i4, i5, i6,
             i7, i8, i9,
             current_mtl_color = 0;
    light_t *current_light;

    /* variables to check for important variable initialization TODO */
    unsigned has_eye = 0,
             has_viewdir = 0,
             has_updir = 0,
             has_hfov = 0,
             has_imsize = 0,
             has_bkgcolor = 0,
             has_mtlcolors = 0,  /* these two will be true (=1) if any    */
             has_projection = 0,
             got_texture_last = 0;

    /* automatically no soft shading */
    scene->soft_shading = 0;

    /* variables for math after reading */
    point_t win_center,
            new_end;
    vector_t view_orig_to_win,
             half_width_u,
             half_height_v;

    /* set counts to 0 */
    scene->object_count = 0;
    scene->light_count = 0;
    scene->vert_count = 0;
    scene->tri_count = 0;
    scene->norm_count = 0;
    scene->texture_count = 0;
    scene->img_coord_count = 0;

    /* initialize objects and mtl_colors */
    scene->objects = malloc (MAX_OBJECTS * sizeof (object_t));
    scene->mtl_colors = malloc (MAX_MTL_COLORS * sizeof (mtl_color_t));
    scene->lights = malloc (MAX_LIGHTS * sizeof (light_t));
    scene->verts = malloc (MAX_VERTS * sizeof (point_t));
    scene->tris = malloc (MAX_TRIS * sizeof (triangle_t));
    scene->norms = malloc (MAX_VERTS * sizeof (vector_t));
    scene->textures = malloc (MAX_TEXTURES * sizeof (image_t));
    scene->img_coords = malloc (MAX_VERTS * sizeof (img_coord_t));

    /* no depth cue by default */
    scene->has_depth_cue = 0;

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
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            scene->bkg_color = (color_t) {f1, f2, f3};

            /* get index of refraction for bg */
            fscanf (file, "%f", &f1);
            scene->bkg_eta = f1;

            has_bkgcolor = 1;
        }
        else if (strcmp (key, "mtlcolor") == 0) { /* mtlcolor */
            /* check if max number of colors are already loaded */
            if (current_mtl_color >= MAX_MTL_COLORS - 1) {
                fprintf (stderr, "too many colors loaded, read fail\n");
                return 0;
            }
            
            /* get diffuse color */
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            scene->mtl_colors[current_mtl_color].od = (color_t) {f1, f2, f3};

            /* get specular color */
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            scene->mtl_colors[current_mtl_color].os = (color_t) {f1, f2, f3};

            /* get constants */
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            scene->mtl_colors[current_mtl_color].ka = f1;
            scene->mtl_colors[current_mtl_color].kd = f2;
            scene->mtl_colors[current_mtl_color].ks = f3;

            /* get falloff control, alpha, and eta */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->mtl_colors[current_mtl_color].n = f1;
            scene->mtl_colors[current_mtl_color].alpha = f2;
            scene->mtl_colors[current_mtl_color].eta = f3;

            /* calculate fresnel base */
            scene->mtl_colors[current_mtl_color].f0 = pow ((f3 - 1)/(f3 + 1), 2);

            current_mtl_color++;

            got_texture_last = 0;
            has_mtlcolors = 1;
        }
        else if (strcmp (key, "sphere") == 0) { /* object */
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

            /* get sphere center */
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            scene->objects[scene->object_count].c = (point_t) { f1, f2, f3 };

            /* get sphere radius */
            fscanf (file, "%f", &f1);
            scene->objects[scene->object_count].radius = f1;

            /* set sphere color */
            scene->objects[scene->object_count].color = &(scene->mtl_colors[current_mtl_color - 1]);

            /* if texture was gotten more recently than mtlcolor, set texture. */
            if (got_texture_last == 1)
                scene->objects[scene->object_count].texture = &(scene->textures[scene->texture_count - 1]);

            scene->object_count++;
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
        else if (strcmp (key, "light") == 0 || strcmp (key, "attlight") == 0) { /* light */
            /* check if going over max number of lights */
            if (scene->light_count >= MAX_LIGHTS) {
                fprintf (stderr, "too many light sources loaded, read fail\n");
                return 0;
            }

            current_light = &(scene->lights[scene->light_count]);
            
            /* get pos/vec */
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            current_light->x = f1;
            current_light->y = f2;
            current_light->z = f3;

            /* get light type */
            fscanf (file, "%d ", &i1);
            if (i1 != 0 && i1 != 1) {
                fprintf (stderr, "unknown light source type %d read, read fail\n", i1);
                return 0;
            }
            current_light->type = i1;

            /* if vec, normalize */
            if (i1 == 0) {
                length = sqrt (f1 * f1 + f2 * f2 + f3 * f3);
                
                current_light->x /= length;
                current_light->y /= length;
                current_light->z /= length;
            }

            /* get light color */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            current_light->color = (color_t) {f1, f2, f3};

            /* account for attlight */
            if (strcmp (key, "attlight") == 0) {
                current_light->type += 2;

                /* get attenuation constants */
                fscanf (file, " %f %f %f", &f1, &f2, &f3);
                current_light->c1 = f1;
                current_light->c2 = f2;
                current_light->c3 = f3;
            }

            /* increment light count by 1 */
            scene->light_count++;
        }
        else if (strcmp (key, "depthcueing") == 0) { /* depth cue */
            /* get cue color */
            fscanf (file, "%f %f %f ", &f1, &f2, &f3);
            scene->depth_cue.color = (color_t) {f1, f2, f3};

            /* get cue alpha max/min */
            fscanf (file, "%f %f ", &f1, &f2);
            scene->depth_cue.alpha_max = f1;
            scene->depth_cue.alpha_min = f2;

            /* get cue distance far/near */
            fscanf (file, "%f %f", &f1, &f2);
            scene->depth_cue.d_far = f1;
            scene->depth_cue.d_near = f2;

            /* update the scene to show it has a depth cue */
            scene->has_depth_cue = 1;
        }
        else if (strcmp (key, "softshading") == 0) { /* soft shading enable */
            /* get light delta and ray count */
            fscanf (file, "%f %d", &f1, &i1);
            scene->soft_shading = 1;
            scene->soft_shade_light_delta = f1;
            scene->soft_shade_ray_count = i1;
        }
        else if (strcmp (key, "#") == 0) { /* comment */
            fgets (args, sizeof (args), file); /* basically seek to end of line */
        }
        else if (strcmp (key, "v") == 0) { /* vertex */
            /* check if going over vertex limit */
            if (scene->vert_count >= MAX_VERTS) {
                fprintf (stderr, "too many vertices, read fail\n");
                return 0;
            }

            /* get point */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->verts[scene->vert_count] = (point_t) {f1, f2, f3};

            /* increment count */
            scene->vert_count++;
        }
        else if (strcmp (key, "vn") == 0) { /* vertex normal */
            /* check if going over vertex (normal) limit */
            if (scene->norm_count >= MAX_VERTS) {
                fprintf (stderr, "too many vertex normals, read fail\n");
                return 0;
            }

            /* get normal vector */
            fscanf (file, "%f %f %f", &f1, &f2, &f3);
            scene->norms[scene->norm_count] = (vector_t) {f1, f2, f3};

            /* increment count */
            scene->norm_count++;
        }
        else if (strcmp (key, "f") == 0) { /* triangle */
            /* check if max tris */
            if (scene->tri_count >= MAX_TRIS) {
                fprintf (stderr, "too many triangles, read fail\n");
                return 0;
            }

            /* check if there is a color */
            if (has_mtlcolors != 1) {
                fprintf (stderr, "scene_from_file () read a face before a mtlcolor, read fail\n");
                return 0;
            }

            /* get vert index/texture/vector norm */
            fgets (args, sizeof (args), file);
            
            if (sscanf (args, "%d/%d/%d %d/%d/%d %d/%d/%d", &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9) == 9) { /* param supreme; all 3 */
                /* check if scene has textures */
                if (scene->texture_count <= 0) {
                    fprintf (stderr, "tried assigning a triangle with texture coords before passing a texture, read fail\n");
                    return 0;
                }

                /* assign each pointer, checking if all within range */
                if ((i1 <= 0 || i1 > scene->vert_count) ||
                    (i4 <= 0 || i4 > scene->vert_count) ||
                    (i7 <= 0 || i7 > scene->vert_count)) {
                    fprintf (stderr, "vertex index out of range, read fail\n");
                    return 0;
                }
            
                /* check if all textures within range */
                if ((i2 <= 0 || i2 > scene->img_coord_count) ||
                    (i5 <= 0 || i5 > scene->img_coord_count) ||
                    (i8 <= 0 || i8 > scene->img_coord_count)) {
                    fprintf (stderr, "texture coordinate index out of range, read fail\n");
                    return 0;
                }

                /* check if all normal vectors within range */
                if ((i3 <= 0 || i3 > scene->norm_count) ||
                    (i6 <= 0 || i6 > scene->norm_count) ||
                    (i9 <= 0 || i9 > scene->norm_count)) {
                    fprintf (stderr, "vertex normal index out of range, read fail\n");
                    return 0;
                }

                /* init the triangle based on these three points */
                tri_init (&(scene->tris[scene->tri_count]), &(scene->verts[i1 - 1]),
                          &(scene->verts[i4 - 1]), &(scene->verts[i7 - 1]), &(scene->mtl_colors [current_mtl_color - 1]));

                /* give pointer to recent texture */
                scene->tris[scene->tri_count].texture = &(scene->textures[scene->texture_count - 1]);

                /* give pointers to requested texture coords */
                scene->tris[scene->tri_count].coords[0] = &(scene->img_coords[i2 - 1]);
                scene->tris[scene->tri_count].coords[1] = &(scene->img_coords[i5 - 1]);
                scene->tris[scene->tri_count].coords[2] = &(scene->img_coords[i8 - 1]);

                /* add vertex norms to the triangle */
                tri_add_norms (&(scene->tris[scene->tri_count]), &(scene->norms[i3 - 1]),
                               &(scene->norms[i6 - 1]), &(scene->norms[i9 - 1]));
            }
            else if (sscanf (args, "%d//%d %d//%d %d//%d", &i1, &i2, &i3, &i4, &i5, &i6) == 6) { /* verts and norms */
                /* assign each pointer, checking if all within range */
                if ((i1 <= 0 || i1 > scene->vert_count) ||
                    (i3 <= 0 || i3 > scene->vert_count) ||
                    (i5 <= 0 || i5 > scene->vert_count)) {
                    fprintf (stderr, "vertex index out of range, read fail\n");
                    return 0;
                }
                if ((i2 <= 0 || i2 > scene->norm_count) ||
                    (i4 <= 0 || i4 > scene->norm_count) ||
                    (i6 <= 0 || i6 > scene->norm_count)) {
                    fprintf (stderr, "vertex normal index out of range, read fail\n");
                    return 0;
                }

                /* init the triangle based on these three points */
                tri_init (&(scene->tris[scene->tri_count]), &(scene->verts[i1 - 1]),
                          &(scene->verts[i3 - 1]), &(scene->verts[i5 - 1]), &(scene->mtl_colors [current_mtl_color - 1]));

                /* add vertex norms to the triangle */
                tri_add_norms (&(scene->tris[scene->tri_count]), &(scene->norms[i2 - 1]),
                               &(scene->norms[i4 - 1]), &(scene->norms[i6 - 1]));
            }
            else if (sscanf (args, "%d/%d %d/%d %d/%d", &i1, &i2, &i3, &i4, &i5, &i6) == 6) { /* verts and textures */
                /* check if scene has textures */
                if (scene->texture_count <= 0) {
                    fprintf (stderr, "tried assigning a triangle with texture coords before passing a texture, read fail\n");
                    return 0;
                }

                /* check if all vertices within range */
                if ((i1 <= 0 || i1 > scene->vert_count) ||
                    (i3 <= 0 || i3 > scene->vert_count) ||
                    (i5 <= 0 || i5 > scene->vert_count)) {
                    fprintf (stderr, "vertex index out of range, read fail\n");
                    return 0;
                }

                /* check if all textures within range */
                if ((i2 <= 0 || i2 > scene->img_coord_count) ||
                    (i4 <= 0 || i4 > scene->img_coord_count) ||
                    (i6 <= 0 || i6 > scene->img_coord_count)) {
                    fprintf (stderr, "texture coordinate index out of range, read fail\n");
                    return 0;
                }

                /* init the triangle based on these three points */
                tri_init (&(scene->tris[scene->tri_count]), &(scene->verts[i1 - 1]),
                          &(scene->verts[i3 - 1]), &(scene->verts[i5 - 1]), &(scene->mtl_colors [current_mtl_color - 1]));

                /* give pointer to recent texture */
                scene->tris[scene->tri_count].texture = &(scene->textures[scene->texture_count - 1]);

                /* give pointers to texture coords requested */
                scene->tris[scene->tri_count].coords[0] = &(scene->img_coords[i2 - 1]);
                scene->tris[scene->tri_count].coords[1] = &(scene->img_coords[i4 - 1]);
                scene->tris[scene->tri_count].coords[2] = &(scene->img_coords[i6 - 1]);
            }
            else if (sscanf (args, "%d %d %d", &i1, &i2, &i3) == 3) { /* verts only */
                /* assign each pointer, checking if all within range */
                if ((i1 <= 0 || i1 > scene->vert_count) ||
                    (i2 <= 0 || i2 > scene->vert_count) ||
                    (i3 <= 0 || i3 > scene->vert_count)) {
                    fprintf (stderr, "vertex index out of range, read fail\n");
                    return 0;
                }

                /* init the triangle based on these three points */
                tri_init (&(scene->tris[scene->tri_count]), &(scene->verts[i1 - 1]),
                          &(scene->verts[i2 - 1]), &(scene->verts[i3 - 1]), &(scene->mtl_colors [current_mtl_color - 1]));
            }
            else { /* error */
                fprintf (stderr, "scene_from_file () found strange arcane face format, read fail\n");
                return 0;
            }

            /* increment count */
            scene->tri_count++;
        }
        else if (strcmp (key, "texture") == 0) { /* texture file */
            /* get filename */
            fscanf (file, "%s", args);

            if (image_init (&(scene->textures[scene->texture_count]), args) != 1) { /* read error */
                fprintf (stderr, "scene_from_file () had a tough time getting the texture at %s, read fail\n", args);
                return 0;
            }

            got_texture_last = 1;
            scene->texture_count++;
        }
        else if (strcmp (key, "vt") == 0) { /* texture image coords */
            /* get coords */
            fscanf (file, "%f %f", &f1, &f2);

            /* check if not in range */
            if ((f1 < 0 || f1 > 1) || (f2 < 0 || f2 > 1)) {
                fprintf (stderr, "scene_from_file () found an image coordinate outside of range [0, 1], read fail\n");
                return 0;
            }

            /* update latest image coord */
            scene->img_coords[scene->img_coord_count] = (img_coord_t) {f1, f2};

            /* increment image coord count */
            scene->img_coord_count++;
        }
        else {
            fprintf (stderr, "scene_from_file () read unknown key '%s', read fail\n", key);
            return 0;
        }
    }

    /* close file */
    fclose (file);


    /* now that input has been gotten, check if all necessary values are present */
    if ((has_eye + has_viewdir + has_hfov + has_imsize) != 4) {
        fprintf (stderr, "scene_from_file () read from an input file with one or more missing essential value\n");
        return 0;
    }


    /* TODO TODO TODO UPDATE has_* variables */


    /* setup default values if necessary */
    if (has_updir == 0) scene->up_dir = (vector_t) {0.0, 1.0, 0.0};      /* updir automatically "straight-on" or "no-roll" */
    if (has_bkgcolor == 0) scene->bkg_color = (color_t) {0.0, 0.0, 0.0}; /* bkgcolor automatically black */
    if (has_projection == 0) scene->projection_type = 0;                 /* projection automatically perspective */    

    
    /* extrapolate unknowns from input known values */

    scene->d = 1.0;
    scene->aspect = ((float) scene->img_width) / ((float) scene->img_height);
    
    scene->win_width = 2.0 * scene->d * tan (0.5 * (scene->fov_h * (M_PI/180.0)));         /* win_width */
    scene->win_height = scene->win_width / scene->aspect; /* win_height */

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
    int i;

    /* destroy objects, mtl_colors, and lights */
    free (scene->objects);
    free (scene->mtl_colors);
    free (scene->lights);
    free (scene->verts);
    free (scene->tris);
    free (scene->norms);

    /* free individual textures */
    for (i = 0; i < scene->texture_count; i++) {
        image_destroy (&(scene->textures[i]));
    }
    free (scene->textures);

    free (scene->img_coords);
}
