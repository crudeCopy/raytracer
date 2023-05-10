/* chang
 * provides a typedef for representing a light source */

#ifndef LIGHT_H
#define LIGHT_H


/* light_t:
 *   describes a point or directional light in coordinate space
 *   if directional, <x, y, z> defines the vector in which the light points
 *   if point, (x, y, z) defines the light's origin point
 */

typedef struct {
    int type;      /* 0 = directional, 1 = point,
                    * 2 = attenuated point
                    * (2 is internal only, invoke with attlight instead) */

    float x, y, z; /* can't be a point or a vector for sure, so just floats */

    float c1, c2, c3; /* constants for attenuation, NOT DEFINED IF NOT "attlight"
                       * yes, this is awful design */

    color_t color; /* color, controls hue and brightness of light */
} light_t;


#endif
