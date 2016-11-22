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

#ifdef DEVEL 
#define dprintf(format, ...) fprintf (stderr, format, __VA_ARGS__)
#else
#define dprintf(format, ...) 
#endif

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

bool empty_stack(struct Stack* f) {
	return !f->tail;
}

// return head of list
int car(struct Stack *s) {
	if (empty_stack(s)) {
		return 0;
	} else {
		return s->val;
	}
}

struct Stack* cdr(struct Stack* s) {
	if (empty_stack(s)) {return s;}
	else {
		struct Stack* ss = s->tail;
		free(s);
		return ss;
	}
}

void free_all(struct Stack *d) {
	while (!empty_stack(d)) {
		d = cdr(d);
	}
	cdr(d);
}

// Add element to head of list
struct Stack* cons(struct Stack *f, int t) {
	struct Stack *result = malloc(sizeof(struct Stack));
	result->val = t;
	result->tail = f;
	return result;
}

struct Stack* new_stack() {
	struct Stack *result = malloc(sizeof(struct Stack));
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
	int dx = 1;
	int dy = 0;
	struct Stack* stack = new_stack();
	stack->val = 0;
	bool string_mode = false;
	while(true) {
		char curr = lines[y][x];
		dprintf("%c %d ", curr, car(stack));
		if (string_mode) {
			if (curr == '"') {
				string_mode = false;
			} else {
				stack = cons(stack, curr);
			}
		} else {
			if (curr >= '0' && curr <= '9') {
				stack = cons(stack, curr - '0');
			} else {
				switch(curr) {
					case '+': {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, a + b);
								  break;
							  }
					case '-': {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, b - a);
								  break;
							  }
					case '*': {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, a * b);
								  break;
							  }
					case '/': {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, b / a);
								  break;
							  }
					case '%': {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, b % a);
								  break;
							  }
					case '!': {
								  int a = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, a == 0);
								  break;
							  }
					case '`': {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, b > a);
								  break;
							  }
					case '<': {
								  dx = -1;
								  dy = 0;
								  break;
							  }
					case '>': {
								  dx = 1;
								  dy = 0;
								  break;
							  }
					case '^': {
								  dy = -1;
								  dx = 0;
								  break;
							  }
					case 'v': {
								  dx = 0;
								  dy = 1;
								  break;
							  }
					case '?': {
								  int dir = rand() % 4;
								  switch(dir) {
									  case 0:
										  dx = 0;
										  dy = 1;
										  break;
									  case 1:
										  dx = 0;
										  dy = -1;
										  break;
									  case 2:
										  dx = 1;
										  dy = 0;
										  break;
									  case 3:
										  dx = -1;
										  dy = 0;
										  break;
								  }
								  break;
							  }
					case '_': {
								  int a = car(stack);
								  stack = cdr(stack);
								  if (a) {dx = -1;} 
								  else {dx = 1;}
								  dy = 0;
								  break;
							  }
					case '|': {
								  int a = car(stack);
								  stack = cdr(stack);
								  if (a) {dy = -1;} 
								  else {dy = 1;}
								  dx = 0;
								  break;
							  }
					case '"': {
								  string_mode = !string_mode;
								  break;
							  }
					case ':': {
								  stack = cons(stack, car(stack));
								  break;
							  }
					case '\\': {
								   int valt = car(stack);
								   stack = cdr(stack);
								   int valb = car(stack);
								   stack = cdr(stack);
								   stack = cons(stack, valt);
								   stack = cons(stack, valb);
								   break;
							   }
					case '$': {
								  stack = cdr(stack);
								  break;
							  }
					case '.': {
								  printf("%d ", car(stack));
								  stack = cdr(stack);
								  break;
							  }
					case ',': {
								  printf("%c", car(stack));
								  stack = cdr(stack);
								  break;
							  }
					case '#': {
								  x += dx;
								  y += dy;
								  break;
							  }
					case 'p': {
								  int y = car(stack);
								  stack = cdr(stack);
								  int x = car(stack);
								  stack = cdr(stack);
								  int v = car(stack);
								  stack = cdr(stack);
								  lines[y][x] = v;
								  break;
							  }
					case 'g': {
								  int y = car(stack);
								  stack = cdr(stack);
								  int x = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, lines[y][x]);
								  break;
							  }
					case '&': {
								  fflush(stdout);
								  int num;
								  scanf("%d", &num);
								  stack = cons(stack, num);
								  break;
							  }
					case '~': {
								  int a = getchar();
								  stack = cons(stack, a);
								  break;
							  }
					case '@': { return true; }
				}
			}
		}
		x = (x + dx);
		y = (y + dy);
		if (x < 0) {x = width + x;}
		if (y < 0) {y = height + y;}
		x = x % width;
		y = y % height;
	}
}

int main(int argc, char *argv[]) {
	struct Program* program = prog_from_stdin();
	printf("Program read\nwidth: %d, height: %d\n", program->width, program->height);
	// print_prog(program);
	run(program);
	return 0;
}
