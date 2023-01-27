/* chang
 * for testing really */

#include <stdio.h>
#include "scene.h"

int main () {
        scene_t scene;
        char *filename;

        filename = "input_test1";

        scene_from_file (&scene, filename);

        printf ("FINISHED READING SCENE");    

        scene_destroy (&scene);

	return 0;
}
