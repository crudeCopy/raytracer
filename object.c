/* chang
 * defines initialization for object_t */

#include <string.h>
#include <stdlib.h>
#include "object.h"

/* initializes an object_t of type t with color input_color and
 * parameters from input_args; stores to dest; 1 = success, 0 = fail */
int object_init (object_t *dest, unsigned t, color_t *input_color, char *input_args) {
    unsigned i;
    char *arg_tokenized;
    /* set type and color */
    dest->type = t;
    dest->color = input_color;

    /* initialize args to 0.0, check for error in object_arg_init */
    if (object_arg_init (&(dest->arguments), t) == 0) return 0;

    /* perform surgery on the string */
    arg_tokenized = strtok (input_args, " ");

    i = 0;
    while (arg_tokenized != NULL && i < dest->arguments.count) {
        dest->arguments.a [i] = atof (arg_tokenized);
        i++;
        arg_tokenized = strtok (NULL, " ");
    }

    return 0;
}

/* destructor */
void object_destroy (object_t *obj) {
    object_arg_destroy (&(obj->arguments));
}
