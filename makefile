release: befunge.c
	gcc -o target/befunge befunge.c

devel: befunge.c
	echo "#define DEVEL" > /tmp/bef.c
	cat befunge.c >> /tmp/bef.c
	gcc -g -o target/befunge /tmp/bef.c
