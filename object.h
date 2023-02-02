/* chang
 * a common representation for any object in the scene */

#ifndef OBJECT_H
#define OBJECT_H

//#include "object_arg.h"TODO
#include "color.h"

/* object argument typedef
 * useful
 * TODO: update argument counts for each new object type *//*
typedef struct {
    unsigned count; 
    float *a;       
} object_arg_t;*/


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
    unsigned type, /* type of object, e.g. 0 = sphere, 1 = cylinder */
             argcount;

    /*object_arg_t arguments; TODO*//* arguments */
    float *args;

    color_t color; /* color */
} object_t;



/*TODO*/
/* initializes object_arg_t based on type t, stores to dest 
 * returns 1 if succeed, 0 if fail (if undefined t) */
/*int object_arg_init (object_arg_t *dest, unsigned t);*/

/* destructor */
/*void object_arg_destroy (object_arg_t *arg);
*/


/* initializes an object_t of type t with color input_color and
 * parameters from input_args; stores to dest; 1 = success, 0 = fail */
int object_init (object_t *dest, unsigned t, color_t *input_color, char *input_args);

/* destructor */
void object_destroy (object_t *obj);

#endif
