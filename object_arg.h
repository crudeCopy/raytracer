/* chang
 * a helper structure for the object struct, which holds a variable-length array of floats */

#ifndef OBJECT_ARG_H
#define OBJECT_ARG_H

/* object argument typedef
 * a full explanation of object types is in object.h
 * type 0 = sphere, 4 args; type 1 = cylinder, 8 args; */
typedef struct {
    unsigned count; /* number of arguments */
    float *a;       /* float argument array */
} object_arg_t;

/* initializes object_arg_t based on type t, stores to dest 
 * returns 1 if succeed, 0 if fail (if undefined t) */
int object_arg_init (object_arg_t *dest, unsigned t);

/* destructor */
void object_arg_destroy (object_arg_t *arg);

#endif /* OBJECT_ARG_H */
