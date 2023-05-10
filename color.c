/* chang
 * defines methods for dealing with RGB colors
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"


/* converts from normalized storage to three int pointers of value between 0 and 255 */
void color_to_ints (color_t *color, int *r, int *g, int *b) {
    *r = (int) (color->r * 255);
    *g = (int) (color->g * 255);
    *b = (int) (color->b * 255);
}

/* converts from three ints to normalized values between 0.0 and 1.0 and stores to dest
 * max_val is the biggest value for each component */
void ints_to_color (color_t *dest, int r, int g, int b, int max_val) {
    dest->r = ((float) r) / (float) max_val;
    dest->g = ((float) g) / (float) max_val;
    dest->b = ((float) b) / (float) max_val;
}

/* adds one color to another wavelength-wise */
void color_add (color_t *dest, color_t *a, color_t *b) {
    *dest = (color_t) { a->r + b->r, a->g + b->g, a->b + b->b };
}

/* scales a color by the given scalar float */
void color_scale (color_t *color, float scalar) {
    *color = (color_t) { color->r * scalar, color->g * scalar, color->b * scalar };
}

/* copies data from src to dest */
void color_copy (color_t *dest, color_t *src) {
    *dest = (color_t) { src->r, src->g, src->b };
}

/* get alpha_dc of the given depth cue at the given float distance */
float depth_cue_alpha (depth_cue_t *cue, float d_obj) {
    float result;
    
    if (d_obj <= cue->d_near) return cue->alpha_max;
    else if (d_obj > cue->d_far) return cue->alpha_min;

    result = cue->alpha_min + (cue->alpha_max - cue->alpha_min) * ((cue->d_far - d_obj) / (cue->d_far - cue->d_near));

    return result;
}

/* initialize an image_t from a ppm file */
int image_init (image_t *dest, char *name) {
    FILE *file;
    int i, total_px,
        r, g, b, max_val;
    char buf[32];

    /* open file */
    file = fopen (name, "r");
    if (file == NULL) {
        fprintf (stderr, "image_init () unable to find file %s, read fail\n", name);
        return 0;
    } /* handle errors from fopen () */

    /* read ppm file header */
    fscanf (file, "%s", buf); 
    fscanf (file, "%d", &(dest->w));
    fscanf (file, "%d", &(dest->h));
    fscanf (file, "%d", &max_val);

    /* calculate total pixel count */
    total_px = dest->w * dest->h;

    /* start up color_t array */
    dest->px = malloc (total_px * sizeof (color_t));
    
    /* read color values */
    for (i = 0; i < total_px; i++) {
        /* get r */
        fscanf (file, "%d ", &r);
        /* get g */
        fscanf (file, "%d ", &g);
        /* get b */
        fscanf (file, "%d ", &b);
        
        /* normalize and store */
        ints_to_color (&(dest->px[i]), r, g, b, max_val);
    }

    /* close file */
    fclose (file);

    return 1;
}

/* destroys image_t (frees its px member) */
void image_destroy (image_t *img) {
    free (img->px);
}

/* write image_t to an ascii ppm file */
int image_write (char *name, image_t *img) {
    FILE *file;
    int r, g, b;
    unsigned i, row, total_px;

    /* get total number of pixels */
    total_px = img->w * img->h;

    /* start at first row */
    row = 0;

    /* strip suffix from filename */
    name = strtok (name, ".");

    /* add .ppm */
    strcat (name, ".ppm");

    /* open/create file */
    file = fopen (name, "w");

    if (file == NULL) { /* this should NOT happen, but just in case */
        fprintf (stderr, "image_write () unable to open file %s for some reason, write fail\n", name);
        return 0;
    }

    /* do header */
    fprintf (file, "P3\n%d %d\n255\n", img->w, img->h);

    /* do body */
    for (i = 0; i < total_px; i++) {
        /* convert current pixel color_t to ints */
        color_to_ints (&(img->px[i]), &r, &g, &b);

        /* print to file */
        fprintf (file, "%d %d %d ", r, g, b);
        row++;

        /* check if row too long */
        if (row >= 5) {
            fprintf (file, "\n");
            row = 0;
        }
    }

    /* close file */
    fclose (file);

    return 1;
}

