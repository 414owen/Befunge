release: befunge.c
	gcc -o befunge befunge.c

devel: befunge.c
	cat befunge.c | sed "s.// #define DEVEL.#define DEVEL." > /tmp/bef.c
	gcc -g -o befunge /tmp/bef.c
