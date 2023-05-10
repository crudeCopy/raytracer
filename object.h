/* chang
 * a common representation for any object in the scene */

#ifndef OBJECT_H
#define OBJECT_H

#include "color.h"
#include "r3math.h"


/* sphere typedef */

typedef struct {
    point_t c; /* center point */

    float radius; /* radius */

    mtl_color_t *color; /* color */

    image_t *texture; /* pointer to texture image */
} object_t;



/* initializes an object_t of type t with color input_color  */
int object_init (object_t *dest, unsigned t, mtl_color_t *input_color, char *input_args);

/* destructor */
void object_destroy (object_t *obj);

#endif
