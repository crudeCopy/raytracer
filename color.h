/* chang
 * implements typedefs and declares methods for handling RGB colors
 */

#ifndef COLOR_H
#define COLOR_H


/******************* TYPEDEFS ********************/

/* color_t : RGB color type
 *   - r, g, and b values
 */
typedef struct { float r, g, b; } color_t;

/* mtl_color_t : Phong material color type
 *  SIMPLE PHONG-BLINN
 *   - od : diffuse color
 *   - os : specular color
 *   - ka : ambient constant
 *   - kd : diffuse constant
 *   - ks : specular constant
 *   - n  : specular hightlight falloff
 *  EXTENDED FOR REFLECTIVITY AND TRANSPARENCY
 *   - alpha : transparency level
 *   - eta   : reflectivity constant
 *   - f0    : Fresnel reflectance coef. base
 */
typedef struct {
    color_t od, os;
    float ka, kd, ks, n,
          alpha, eta, f0;
} mtl_color_t;

/* depth_cue_t : Depth cue information type
 *   - color : color that depth cue goes to
 *   - alpha_max/_min : max and min alpha vals
 *   - d_near/_far : cue's idea of long and short distance
 */
typedef struct {
    color_t color;
    float alpha_max, alpha_min,
          d_far, d_near;
} depth_cue_t;

/* image_t : Image type
 *   - w : width
 *   - h : height
 *   - px : pixel data
 */
typedef struct {
    unsigned w, h;
    color_t *px;
} image_t;

/* img_coord_t : Image coordinate type
 *   - u and v values
 */
typedef struct {
    float u, v;
} img_coord_t;


/********************* METHODS *******************/

/* converts from normalized storage to three int pointers of value between 0 and 255 */
void color_to_ints (color_t *color, int *r, int *g, int *b);

/* converts from three ints to normalized values between 0.0 and 1.0 and stores to dest 
 * max_val is greatest integer rgb value*/ 
void ints_to_color (color_t *dest, int r, int g, int b, int max_val);

/* adds one color to another wavelength-wise */
void color_add (color_t *dest, color_t *a, color_t *b);

/* scales a color by the given scalar float */
void color_scale (color_t *color, float scalar);

/* copies data from src to dest */
void color_copy (color_t *dest, color_t *src);

/* get alpha_dc of the given depth cue at the given float distance */
float depth_cue_alpha (depth_cue_t *cue, float d_obj);

/* initialize an image_t from a ppm file */
int image_init (image_t *dest, char *name);

/* destroy image_t (free px) */
void image_destroy (image_t *img);

/* write image_t to an ascii ppm file */
int image_write (char *name, image_t *img);


#endif /* COLOR_H */
