GCC = gcc -Wall -Werror

all: main.c point.c vector.c color.c object.c scene.c
	$(GCC) -lm main.c scene.c object.c object_arg.c color.c vector.c point.c -o cray


clean:
	rm -f *.o cray
