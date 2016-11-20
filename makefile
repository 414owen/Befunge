release: befunge.c
	gcc -o target/befunge befunge.c

devel: befunge.c
	gcc -g -o target/befunge befunge.c
