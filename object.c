/* chang
 * defines initialization for object_t */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#if 0
/* initializes an object_t of type t with color input_color and
 * parameters from input_args; stores to dest; 1 = success, 0 = fail */
int object_init (object_t *dest, unsigned t, mtl_color_t *input_color, char *input_args) {
    unsigned i;
    char *arg_tokenized;


    /* set type and color */
    dest->type = t;
    dest->color = (mtl_color_t) {
        (color_t) {input_color->od.r, input_color->od.g, input_color->od.b},  
        (color_t) {input_color->os.r, input_color->os.g, input_color->os.b},
        input_color->ka, input_color->kd, input_color->ks, input_color->n
    };


    /* check object type is valid TODO: update */
    switch (t) {
        case 0:
            dest->argcount = 4;
            break;
        case 1:
            dest->argcount = 8;
            break;
        default:
            fprintf (stderr, "object_init () was passed an unknown object type %d, init fail\n", t);
            return 0;
    }


    /* allocate right amount of space for args */
    dest->args = (float *) malloc(dest->argcount * sizeof(float));


    /* perform surgery on the string */
    arg_tokenized = strtok (input_args, " ");


    /* read in the right amount of arguments from file */
    i = 0;
    while (arg_tokenized != NULL && i < dest->argcount) {
        dest->args [i] = atof (arg_tokenized);
        i++;
        arg_tokenized = strtok (NULL, " ");
    }


    /* set texture to null by default */
    dest->texture = NULL;

    return 0;
}


/* destructor */
void object_destroy (object_t *obj) {
    free (obj->args);
}
#endif
