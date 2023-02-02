/* chang
 * defines color conversion methods */

#include <stdio.h>
#include <string.h>
#include "color.h"

/* converts from normalized storage to three int pointers of value between 0 and 255 */
void color_to_ints (color_t *color, int *r, int *g, int *b) {
    *r = (int) (color->r * 255);
    *g = (int) (color->g * 255);
    *b = (int) (color->b * 255);
}

/* converts from three ints to normalized values between 0.0 and 1.0 and stores to dest 
void ints_to_color (color_t *dest, int r, int g, int b) {
    dest->r = ((float) r) / 255.0;
    dest->g = ((float) g) / 255.0;
    dest->b = ((float) b) / 255.0;
} unnecessary unless reading FROM ppm */

/* takes color_t array and creates {name}.ppm
 * returns 1 if success, 0 if fail */
int write_colors_to_ppm (char *name, color_t *pixels, unsigned w, unsigned h) {
    FILE *file;
    int r, g, b;
    unsigned i, row = 0,
             total_px = w * h;

    /* strip suffix from filename */
    name = strtok (name, ".");

    /* add .ppm */
    strcat (name, ".ppm");

    /* open/create file */
    file = fopen (name, "w");

    if (file == NULL) { /* this should NOT happen, but just in case */
        fprintf (stderr, "write_colors_to_ppm () unable to open file %s for some reason, write fail", name);
        return 0;
    }

    /* do header */
    fprintf (file, "# chang, cray\nP3\n%d %d\n255\n", w, h);

    /* do body */
    for (i = 0; i < total_px; i++) {
        /* convert current pixel color_t to ints */
        color_to_ints (&(pixels[i]), &r, &g, &b);

        /* print to file */
        fprintf (file, "%d %d %d ", r, g, b);
        row++;

        /* check if row too long */
        if (row >= 5) {
            fprintf (file, "\n");
            row = 0;
        }
    }

    fclose (file);

    return 1;
}
