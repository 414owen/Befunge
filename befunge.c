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
#define lambda(return_type, function_body) \
	({ \
	 return_type __fn__ function_body \
	 __fn__; \
	 })

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
int car(struct Stack *f) {
	return f->val;
}

struct Stack* cdr(struct Stack* s) {
	if (s->tail) {return s->tail;}
	else {return s;}
}

struct Stack* free_head(struct Stack *f) {
	if (f->tail) {
		struct Stack* new = f->tail;
		free(f);
		return new;
	} else {
		return f;
	}
}

void free_all(struct Stack *d) {
	while (!empty_stack(d)) {
		d = free_head(d);
	}
	free_head(d);
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

void replace(struct Stack* s, int val) {
	s->val = val;
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

void arithmetic(struct Stack* stack, int (*func)(int, int)) {
	if (empty_stack(stack)) {
		stack = cons(stack, 0);
	} else {
		int a = car(stack);
		stack = free_head(stack);
		if (empty_stack(stack)) {
			stack = cons(stack, 0);
		} else {
			replace(stack, func(a, car(stack)));
		}
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
		dprintf("%c", curr);
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
					case '@':
						return true;
					case '+': {
								  arithmetic(stack, lambda(int, (int a, int b) {return a+b;}));
								  break;

							  }
					case '-': {
								  int a = car(stack);
								  stack = free_head(stack);
								  replace(stack, a - car(stack));
								  break;
							  }
					case '*': {
								  int a = car(stack);
								  stack = free_head(stack);
								  replace(stack, a * car(stack));
								  break;
							  }
					case '/': {
								  int a = car(stack);
								  stack = free_head(stack);
								  replace(stack, a / car(stack));
								  break;
							  }
					case '%': {
								  int a = car(stack);
								  stack = free_head(stack);
								  replace(stack, a % car(stack));
								  break;
							  }
					case '!': {
								  int a = car(stack);
								  replace(stack, !a == 0);
								  break;
							  }
					case '`': {
								  int a = car(stack);
								  stack = free_head(stack);
								  replace(stack, a < car(stack));
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
								  dprintf("%d", a);
								  stack = free_head(stack);
								  if (a) {dx = -1;} 
								  else {dx = 1;}
								  dy = 0;
								  break;
							  }
					case '|': {
								  int a = car(stack);
								  stack = free_head(stack);
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
								  if (!empty_stack(stack)) {
									  stack = cons(stack, car(stack));
								  }
								  break;
							  }
					case '\\': {
								   struct Stack* stack_two = cdr(stack);
								   int tmp = car(stack);
								   replace(stack, car(stack_two));
								   replace(stack_two, tmp);
								   break;
							   }
					case '.': {
								  printf("%d ", car(stack));
								  stack = free_head(stack);
								  break;
							  }
					case ',': {
								  printf("%c", car(stack));
								  stack = free_head(stack);
								  break;
							  }
					case '#': {
								  x += dx;
								  y += dy;
								  break;
							  }
					case 'p': {
								  int y = car(stack);
								  stack = free_head(stack);
								  int x = car(stack);
								  stack = free_head(stack);
								  int v = car(stack);
								  stack = free_head(stack);
								  lines[y][x] = v;
								  break;
							  }
					case 'g': {
								  int y = car(stack);
								  stack = free_head(stack);
								  int x = car(stack);
								  stack = free_head(stack);
								  stack = cons(stack, lines[y][x]);
								  break;
							  }
					case '&': {
								  printf("\nPlease input a number: ");
								  fflush(stdout);
								  int num;
								  scanf("%d", &num);
								  stack = cons(stack, num);
								  break;
							  }
					case '~': {
								  printf("\nPlease input a character: ");
								  int a = getchar();
								  stack = cons(stack, a);
								  break;
							  }
				}
			}
		}
		x = (x + dx) % width;
		y = (y + dy) % height;
	}
}

int main(int argc, char *argv[]) {
	struct Program* program = prog_from_stdin();
	printf("Program read\nwidth: %d, height: %d\n", program->width, program->height);
	print_prog(program);
	run(program);
	return 0;
}
