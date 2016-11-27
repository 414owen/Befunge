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
#include <time.h>
// #define DEVEL
#define STACK_TYPE int
#define DEF_STACK_SIZE 1000
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
#define binary_op(__op__, __stack__) \
	STACK_TYPE __a__ = car(__stack__);\
cdr(__stack__);\
STACK_TYPE __b__ = car(__stack__);\
cdr(__stack__);\
cons(__stack__, __b__ __op__ __a__);\

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
	STACK_TYPE* arr;
	int pointer;
	int size;
};

bool empty_stack(struct Stack* s) {
	return s->pointer < 0;
}

// return head of list
STACK_TYPE car(struct Stack *s) {
	if (empty_stack(s)) {
		return 0;
	} else {
		return s->arr[s->pointer];
	}
}

void cdr(struct Stack* s) {
	if (!empty_stack(s)) s->pointer -= 1;
}

void free_stack(struct Stack *s) {
	free(s->arr);
	free(s);
}

// Add element to head of list
void cons(struct Stack *s, int t) {
	s->arr[++(s->pointer)] = t;
}

struct Stack* new_stack() {
	struct Stack *result = malloc(sizeof(struct Stack));
	result->pointer = -1;
	result->size = DEF_STACK_SIZE;
	result->arr = malloc(sizeof(STACK_TYPE) * DEF_STACK_SIZE);
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
				cons(stack, curr);
			}
		} else {
			if (curr >= '0' && curr <= '9') {
				cons(stack, curr - '0');
			} else {
				switch(curr) {
					case '+': {binary_op(+, stack); break;}
					case '-': {binary_op(-, stack); break;}
					case '*': {binary_op(*, stack); break;}
					case '/': {binary_op(/, stack); break;}
					case '%': {binary_op(%, stack); break;}
					case '`': {binary_op(>, stack); break;}
					case '!': 
							  {
								  STACK_TYPE a = car(stack);
								  cdr(stack);
								  cons(stack, a == 0);
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
								  STACK_TYPE a = car(stack);
								  cdr(stack);
								  if (a) {dx = -1;} 
								  else {dx = 1;}
								  dy = 0;
								  break;
							  }
					case '|': 
							  {
								  STACK_TYPE a = car(stack);
								  cdr(stack);
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
								  cons(stack, car(stack));
								  break;
							  }
					case '\\': 
							  {
								  STACK_TYPE a = car(stack);
								  cdr(stack);
								  STACK_TYPE b = car(stack);
								  cdr(stack);
								  cons(stack, a);
								  cons(stack, b);
								  break;
							  }
					case '$': 
							  {
								  cdr(stack);
								  break;
							  }
					case '.': 
							  {
								  printf("%d ", car(stack));
								  cdr(stack);
								  break;
							  }
					case ',': 
							  {
								  putchar(car(stack));
								  cdr(stack);
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
								  STACK_TYPE yy = car(stack);
								  cdr(stack);
								  STACK_TYPE xx = car(stack);
								  cdr(stack);
								  STACK_TYPE v = car(stack);
								  cdr(stack);
								  if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
									  out_of_bounds(xx, yy);
								  if (width < xx) width = xx;
								  if (height < yy) height = yy;
								  *(program +  (yy * WIDTH + xx)) = v;
								  break;
							  }
					case 'g': 
							  {
								  STACK_TYPE yy = car(stack);
								  cdr(stack);
								  STACK_TYPE xx = car(stack);
								  cdr(stack);
								  if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
									  out_of_bounds(xx, yy);
								  cons(stack, (unsigned char) *(program +  (yy * WIDTH + xx)));
								  break;
							  }
					case '&': 
							  {
								  int num;
								  scanf("%d", &num);
								  cons(stack, num);
								  break;
							  }
					case '~': 
							  {
								  int a = getchar();
								  cons(stack, a);
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
	if (program->valid) {
		run(program);
	} 
	return 0;
}

