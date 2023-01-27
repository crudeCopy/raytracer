/* chang
 * defines color_t */

#ifndef COLOR_H
#define COLOR_H

/* struct for colors, will be stored normalized between 0.0 and 1.0 */
typedef struct {
    float r, g, b;
} color_t;

/* converts from normalized storage to three int pointers of value between 0 and 255 */
void color_to_ints (color_t *color, int *r, int *g, int *b);

/* converts from three ints to normalized values between 0.0 and 1.0 and stores to dest */
void ints_to_color (color_t *dest, int r, int g, int b);

#endif /* COLOR_H */
