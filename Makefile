NAME = raytracer1d
GCC = gcc -Wall -Werror -g

all: main.c r3math.c color.c object.c triangle.c scene.c cray.c
	$(GCC) -lm -o $(NAME) main.c r3math.c color.c object.c triangle.c scene.c cray.c

clean:
	rm -f *.o $(NAME) input/*.ppm
