/* ------------------------ /
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
#include <time.h>
// #define DEVEL
#define uint unsigned int
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define WIDTH 80
#define HEIGHT 25
#define binary_op(__op__) \
	int __a__ = car(stack);\
stack = cdr(stack);\
int __b__ = car(stack);\
int __new_val__ = __b__ __op__ __a__;\
if (!empty_stack(stack)) stack->val = __new_val__;\
else {\
	stack = cdr(stack);\
	stack = cons(stack, __new_val__);\
}

#ifdef DEVEL 
#define dprintf(...) fprintf (stderr, __VA_ARGS__)
#else
#define dprintf(...) 
#endif

/* ---------- */
/* |  TODO  | */
/* ---------- */

// * Determine whether file supplied is a folder before proceeding
// * Decompile switch-statement, see if we benefit from manual character
// binary-search (log(n)), or an array of funtion pointers (n, but has more
// overhead).


struct Program {
	bool valid;
	size_t width;
	size_t height;
	char* lines;
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

void invalid_file() {
	printf("Couldn't open file. Did you get the path right?\n");
}

// "Classic Premature Optimisation" - Brian Whelan (BAmod pending)
// "Shut up I wanna" - me
struct Program* prog_from_file(char* filename) {
	FILE* file = fopen(filename, "r");
	struct Program* program = malloc(sizeof(struct Program));
	if (file == NULL) {
		invalid_file();
		program->valid = false;
		return program;
	}
	size_t bflines = 0;
	size_t line_buf = 100;
	size_t prog_size = WIDTH * HEIGHT;
	char* prog = malloc(prog_size); // + 2 is for '\n' and '\0' from fgets
	size_t max_len = 0;
	while (true) {
		char* linep = prog + bflines * WIDTH;
		int line_len = 0;
		while (true) {
			int curr = fgetc(file);
			if (curr == '\n') {
				bflines++;
				max_len = max_len > line_len ? max_len : line_len;
				for (char* i = linep + line_len; i < WIDTH + linep; i++) {
					*i = ' ';
				}
				break;
			} else if (curr == EOF) {
				max_len = max_len > line_len ? max_len : line_len;
				goto fileEnd;
			} else {*(linep + line_len++) = (char) curr;}
		}
	}
fileEnd:	
	program->lines = prog;
	program->valid = true;
	program->width = max_len;
	program->height = bflines;
	return program;
}

void print_prog_with_pointer(char* program, int width, int height, int x, int y) {
	for (int j = 0; j < height; j++) {
		int linep = (WIDTH * j);
		for (int i = 0; i < width; i++) {
			if (y == j && x == i) {
				dprintf(ANSI_COLOR_GREEN "X" ANSI_COLOR_RESET);
			} else {
				dprintf("%c", *(program + (linep + i)));
			}
		}
		dprintf("\n");
	}
	dprintf("\n");
}

void out_of_bounds(int x, int y) {
	printf("Program limits are 80 x 25, tried to access %d x %d", x, y);
}

void run(struct Program* prog) {
	int width = prog->width;
	int height = prog->height;
	char* program = prog->lines;
	int x = 0;
	int y = 0;
	int dx = 1;
	int dy = 0;
	struct Stack* stack = new_stack();
	bool string_mode = false;
	while(true) {
		char curr = *(program +  (y * WIDTH + x));
#ifdef DEVEL
		print_prog_with_pointer(program, width, height, x, y);
#endif
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
					case '+': {binary_op(+); break;}
					case '-': {binary_op(-); break;}
					case '*': {binary_op(*); break;}
					case '/': {binary_op(/); break;}
					case '%': {binary_op(%); break;}
					case '`': {binary_op(>); break;}
					case '!': 
							  {
								  int a = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, a == 0);
								  break;
							  }
					case '<': 
							  {
								  dx = -1;
								  dy = 0;
								  break;
							  }
					case '>': 
							  {
								  dx = 1;
								  dy = 0;
								  break;
							  }
					case '^': 
							  {
								  dy = -1;
								  dx = 0;
								  break;
							  }
					case 'v': 
							  {
								  dx = 0;
								  dy = 1;
								  break;
							  }
					case '?': 
							  {
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
					case '_': 
							  {
								  int a = car(stack);
								  stack = cdr(stack);
								  if (a) {dx = -1;} 
								  else {dx = 1;}
								  dy = 0;
								  break;
							  }
					case '|': 
							  {
								  int a = car(stack);
								  stack = cdr(stack);
								  if (a) {dy = -1;} 
								  else {dy = 1;}
								  dx = 0;
								  break;
							  }
					case '"': 
							  {
								  string_mode = !string_mode;
								  break;
							  }
					case ':': 
							  {
								  stack = cons(stack, car(stack));
								  break;
							  }
					case '\\': 
							  {
								  int a = car(stack);
								  stack = cdr(stack);
								  int b = car(stack);
								  stack = cdr(stack);
								  stack = cons(stack, a);
								  stack = cons(stack, b);
								  break;
							  }
					case '$': 
							  {
								  stack = cdr(stack);
								  break;
							  }
					case '.': 
							  {
								  printf("%d ", car(stack));
								  stack = cdr(stack);
								  break;
							  }
					case ',': 
							  {
								  putchar(car(stack));
								  stack = cdr(stack);
								  break;
							  }
					case '#': 
							  {
								  x += dx;
								  y += dy;
								  break;
							  }
					case 'p': 
							  {
								  int yy = car(stack);
								  stack = cdr(stack);
								  int xx = car(stack);
								  stack = cdr(stack);
								  int v = car(stack);
								  stack = cdr(stack);
								  if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
									  out_of_bounds(xx, yy);
								  if (width < xx) width = xx;
								  if (height < yy) height = yy;
								  *(program +  (yy * WIDTH + xx)) = v;
								  break;
							  }
					case 'g': 
							  {
								  int yy = car(stack);
								  stack = cdr(stack);
								  int xx = car(stack);
								  stack = cdr(stack);
								  if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
									  out_of_bounds(xx, yy);
								  stack = cons(stack, *(program +  (yy * WIDTH + xx)));
								  break;
							  }
					case '&': 
							  {
								  int num;
								  scanf("%d", &num);
								  stack = cons(stack, num);
								  break;
							  }
					case '~': 
							  {
								  int a = getchar();
								  stack = cons(stack, a);
								  break;
							  }
					case '@': 
							  { return; }
				}
			}
		}
		x = (width  + x + dx) % width;
		y = (height + y + dy) % height;
	}
}

void usage(char* launch) {
	printf("Usage: %s [FILE]\nThis runs the befunge program in [FILE]\n", launch);
}

int main(int argc, char *argv[]) {
	srand(time(0));
	if (argc != 2) {
		usage(argv[0]); 
		return 0;
	}
	struct Program* program = prog_from_file(argv[1]);
	// printf("Program read\nwidth: %d, height: %d\n", program->width, program->height);
	// print_prog(program);
	if (program->valid) {
		run(program);
	} 
	return 0;
}

