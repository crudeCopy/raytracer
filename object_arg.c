/* chang
 * defines the initialization of an object_arg_t */

#include <stdlib.h>
#include <stdio.h>
#include "object_arg.h"

/* initializes object_arg_t based on type t, stores to dest
 * returns 1 if succeed, 0 if fail (if undefined t) */
int object_arg_init (object_arg_t *dest, unsigned t) {
    /* set the number of arguments */
    if (t == 0) dest->count = 4;      /* sphere */
    else if (t == 1) dest->count = 8; /* cylinder */
    else {                            /* undefined */
        fprintf (stderr, "object_arg_init () was passed an unknown object type %d, init fail\n", t);
        return 0;
    }

    /* initialize array of args to all 0.0s */
    dest->a = (float *) malloc(dest->count * sizeof(float));

    return 1;
}

/* destructor */
void object_arg_destroy (object_arg_t *arg) {
    free (arg->a);
}
