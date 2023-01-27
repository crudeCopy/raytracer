/* chang
 * defines color conversion methods */

#include "color.h"

/* converts from normalized storage to three int pointers of value between 0 and 255 */
void color_to_ints (color_t *color, int *r, int *g, int *b) {
    *r = (int) (color->r * 255);
    *g = (int) (color->g * 255);
    *b = (int) (color->b * 255);
}

/* converts from three ints to normalized values between 0.0 and 1.0 and stores to dest */
void ints_to_color (color_t *dest, int r, int g, int b) {
    dest->r = ((float) r) / 255.0;
    dest->g = ((float) g) / 255.0;
    dest->b = ((float) b) / 255.0;
}
