release: befunge.c
	gcc -O3 -o befunge befunge.c

devel: befunge.c
	gcc -g -o befunge befunge.c

catprog: befunge.c
	cat befunge.c | sed "s.// #define DEVEL.#define DEVEL." > /tmp/bef.c
	gcc -g -o befunge /tmp/bef.c
