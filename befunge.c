#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** prog_from_file(char* name) {
	char** prog;
	while (true) {
	}
	return *prog;
}

struct bfline {
	char* line;
	int length;
}

char** prog_from_stdin() {
	int lines = 0;
	int line_buf = 100;
	bfline** prog = malloc(line_buf * sizeof(bfline));
	size_t ind = 0;
	char *line = NULL:
	size_t zerop = 0;
	while (true) {
		if (lines == line_buf) {
			line_buf = line_buf * 2;
			prog = realloc(prog, line_buf);
		}
		int size = getline(&line, &zerop, stdin);
		if (size < 0) break;
		bfline b;
		b.line = length = size;
		b.line = line;
		prog[lines++] = 
	}
	return *prog;
}

int main(int argc, char *argv[]) {
	char** program;
	if (argc > 1) {program = befunge::prog_from_file(argv[1]);}
	else {program = befunge::prog_from_stdin();}
}

