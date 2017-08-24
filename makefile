release: befunge.c
	clang -O3 -o befunge befunge.c

devel: befunge.c
	clang -g -o befunge befunge.c

catprog: befunge.c
	clang -g -o befunge -DDEVEL befunge.c
