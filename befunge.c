/* ------------------------ */
/* |                      | */
/* |                      | */
/* |      Befunge-93      | */
/* |                      | */
/* |                      | */
/* ------------------------ */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define uint unsigned int

struct Bfline {
	char* line;
	size_t length;
};

struct Program {
	char** lines;
	size_t width;
	size_t height;
};

struct Stack {
	int val;
	struct Stack* tail;
};

bool empty_stack(Stack* f) {
	return !f->tail;
}

// return head of list
double car(Stack *f) {
	return f->val;
}

Stack* free_head(Stack *f) {
	Stack* new = f->tail;
	free(f);
	return new;
}

void free_all(Stack *d) {
	while (!empty_stack_d(d)) {
		d = free_head_d(d);
	}
	free_head_d(d);
}

// Add element to head of list
Stack* cons(Stack *f, double t) {
	Stack *result = malloc(sizeof(Stack));
	result->val = t;
	result->tail = f;
	return result;
}

Stack* new_stack() {
	Stack *result = malloc(sizeof(Stack));
	result->tail = NULL;
	return result;
}

struct Program* prog_from_stdin() {
	size_t bflines = 0;
	size_t line_buf = 100;
	struct Bfline** prog = malloc(line_buf * sizeof(struct Bfline));
	size_t ind = 0;
	char *line = NULL;
	size_t zerop = 0;
	size_t max_len = 0;
	while (true) {
		if (bflines == line_buf) {
			line_buf = line_buf * 2;
			prog = realloc(prog, line_buf);
		}
		int size = getline(&line, &zerop, stdin);
		if (size < 0) {break;}
		else if (max_len < size) {max_len = size;}
		struct Bfline *b = malloc(sizeof(struct Bfline));
		b->length = size;
		b->line = line;
		line = NULL;
		prog[bflines++] = b;
	}
	struct Program* program = malloc(sizeof(struct Program));
	program->lines = malloc(sizeof(char*) * bflines);
	program->width = max_len;
	program->height = bflines;
	for (int i = 0; i < bflines; i++) {
		struct Bfline* bfl = prog[i];
		char* new_line = realloc(bfl->line, max_len);
		program->lines[i] = new_line;
		for (int j = bfl->length - 1; j < max_len - 1; j++) {
			new_line[j] = ' ';
		}
		new_line[max_len - 1] = '\0';
		free(bfl);
	}
	free(prog);
	return program;
}

void print_prog(struct Program* program) {
	size_t width = program->width;
	size_t height = program->height;
	for (size_t j = 0; j < height; j++) {
		printf("%s\n", program->lines[j]);
	}
}

bool run(struct Program* prog) {
	size_t width = prog->width;
	size_t height = prog->height;
	char** lines = prog->lines;
	int x = 0;
	int y = 0;
	struct Stack* stack = new_stack();
	while(true) {
		char curr = lines[y][x];
		if (curr >= '0' && curr <= '9') {
			cons(stack, curr - '0');
		} else {
			switch(curr) {
				case '@':
					return true;
					break;
				case 
			}
		}
	}
}

int main(int argc, char *argv[]) {
	struct Program* program = prog_from_stdin();
	printf("Program read\nwidth: %d, height: %d\n", program->width, program->height);
	
	print_prog(program);
	return 0;
}
