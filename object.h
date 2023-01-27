/* chang
 * a common representation for any object in the scene */

#ifndef OBJECT_H
#define OBJECT_H

#include "object_arg.h"
#include "color.h"

/* a type for all possible objects to be stored in a scene
 *
 * GUIDE TO TYPES: (all args are of float type)
 *  0 -- SPHERE
 *      arguments = { x, y, z, radius }
 *
 *  1 -- CYLINDER
 *      arguments = { x, y, z, dx, dy, dz, radius, length }
 *
 *  MORE TO COME
 *
 * i typically prefer c to c++ personally, but this type has
 * enabled me to understand why i would choose c++ instead */
typedef struct {
    unsigned type; /* type of object, e.g. 0 = sphere, 1 = cylinder */

    object_arg_t arguments; /* arguments */

    color_t *color; /* pointer to color */
} object_t;

/* initializes an object_t of type t with color input_color and
 * parameters from input_args; stores to dest; 1 = success, 0 = fail */
int object_init (object_t *dest, unsigned t, color_t *input_color, char *input_args);

/* destructor */
void object_destroy (object_t *obj);

#endif
