/* chang
 * for testing really */

#include <stdio.h>
#include "scene.h"

int main () {
        scene_t scene;
        char *filename;

        filename = "input_test1";

        if (scene_from_file (&scene, filename) == 1) {
            printf ("FINISHED READING SCENE\n");    
        }

        scene_destroy (&scene);

	return 0;
}
