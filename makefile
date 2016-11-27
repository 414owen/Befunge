release: befunge.c
	clang -O3 -o befunge befunge.c

devel: befunge.c
	clang -g -o befunge befunge.c

catprog: befunge.c
	cat befunge.c | sed "s.// #define DEVEL.#define DEVEL." > /tmp/bef.c
	clang -g -o befunge /tmp/bef.c
