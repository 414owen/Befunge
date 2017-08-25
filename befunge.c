#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STACK_TYPE long
#define STACK_FMT "%ld"
#define DEF_STACK_SIZE 1000
#define WIDTH 80
#define HEIGHT 25

#define binary_op(__op_char__, __op__) \
case __op_char__: {\
	STACK_TYPE __a__ = pop();\
	STACK_TYPE __b__ = pop();\
	push(__b__ __op__ __a__);\
	break;\
}

#define direction_case(__case__, __dx__, __dy__) \
case __case__: {\
	dx = __dx__;\
	dy = __dy__;\
	break;\
}

#define if_case(__op__, __change__, __stop__)\
case __op__: {\
	STACK_TYPE a = pop();\
	if (a) {__change__ = -1;} \
	else {__change__ = 1;}\
	__stop__ = 0;\
	break;\
}

struct Program {
	char valid;
	size_t width;
	size_t height;
	char* lines;
};

struct Stack {
	STACK_TYPE* arr;
	STACK_TYPE* pointer;
	int size;
};

struct Stack stack;
struct Program program;

char empty_stack() {
	return stack.pointer < stack.arr;
}

STACK_TYPE head_with_info(bool empty) {
	if (empty) {return 0;}
	return *(stack.pointer);
}

// return head of list
STACK_TYPE head() {
	return head_with_info(empty_stack());
}

STACK_TYPE pop() {
	bool empty = empty_stack();
	STACK_TYPE res = head_with_info(empty);
	if (!empty) {stack.pointer--;}
	return res;
}

// Add element to head of list
void push(int t) {
	if (stack.pointer - stack.arr + 1 == stack.size) {
		stack.size *= 2;
		STACK_TYPE* old_p = stack.arr;
		stack.arr = realloc(old_p, sizeof(STACK_TYPE) * stack.size);
		stack.pointer = (stack.pointer - old_p) + stack.arr;
	}
	*(++(stack.pointer)) = t;
}

// "Classic Premature Optimisation" - Brian Whelan (BAmod pending)
// "Shut up I wanna" - me
struct Program* prog_from_file(char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Couldn't open file. Did you get the path right?\n");
		program.valid = 0;
	}
	size_t bflines = 0;
	size_t prog_size = WIDTH * HEIGHT;
	char* prog = malloc(prog_size);
	size_t max_len = 0;
	while (1) {
		char* linep = prog + bflines * WIDTH;
		int line_len = 0;
		while (1) {
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
	program.lines = prog;
	program.valid = 1;
	program.width = max_len;
	program.height = bflines;
	fclose(file);
}

void print_prog_with_pointer(int width, int height, int x, int y) {
	fprintf(stderr, "\x1b[2J");
	for (int j = 0; j < height; j++) {
		int linep = (WIDTH * j);
		for (int i = 0; i < width; i++) {
			if (y == j && x == i) {
				fprintf(stderr, "\x1b[44mX\x1b[49m");
			} else {
				fprintf(stderr, "%c", *(program.lines + (linep + i)));
			}
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n\x1b[34mStack: \x1b[39m");
	for (STACK_TYPE* p = stack.arr; p <= stack.pointer; p++) {
		fprintf(stderr, " " STACK_FMT, *p);
	}
	fprintf(stderr, "\n");
}

void out_of_bounds(int x, int y) {
	printf("Program limits are 80 x 25, tried to access %d x %d", x, y);
}

void run() {
	int width = program.width;
	int height = program.height;
	int x = 0;
	int y = 0;
	int dx = 1;
	int dy = 0;
	char string_mode = 0;
	while(1) {
		char curr = *(program.lines + (y * WIDTH + x));
#ifdef DEVEL
		print_prog_with_pointer(width, height, x, y);
		fprintf(stderr, "\n(press <return> to continue) ");
		while (getchar() != '\n') {}
#endif
		if (string_mode) {
			if (curr == '"') {
				string_mode = 0;
			} else {
				push(curr);
			}
		} else {
			if (curr >= '0' && curr <= '9') {
				push(curr - '0');
			} else {
				switch(curr) {
					binary_op('+', +)
					binary_op('-', -)
					binary_op('*', *)
					binary_op('/', /)
					binary_op('%', %)
					binary_op('`', >)
					case '!': {
						STACK_TYPE a = pop();
						push(a == 0);
						break;
					}
					direction_case('<', -1, 0)
					direction_case('>', 1, 0)
					direction_case('^', 0, -1)
					direction_case('v', 0, 1)
					case '?': {
						int dir = rand() % 4;
						switch(dir) {
							direction_case(0, 0, 1)
							direction_case(1, 0, -1)
							direction_case(2, 1, 0)
							direction_case(3, -1, 0)
						}
						break;
					}
					if_case('_', dx, dy)
					if_case('|', dy, dx)
					case '"': {
						string_mode = !string_mode;
						break;
					}
					case ':': {
						push(head());
						break;
					}
					case '\\': {
						 STACK_TYPE a = pop();
						 STACK_TYPE b = pop();
						 push(a);
						 push(b);
						 break;
					 }
					case '$': {
						pop();
						break;
					}
					case '.': {
						printf(STACK_FMT " ", pop());
						break;
					}
					case ',': {
						putchar(pop());
						break;
					}
					case '#': {
						x += dx;
						y += dy;
						break;
					}
					case 'p': {
						STACK_TYPE yy = pop();
						STACK_TYPE xx = pop();
						STACK_TYPE v = pop();
						if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
							out_of_bounds(xx, yy);
						if (width < xx) width = xx;
						if (height < yy) height = yy;
						*(program.lines +	(yy * WIDTH + xx)) = v;
						break;
					}
					case 'g': {
						STACK_TYPE yy = pop();
						STACK_TYPE xx = pop();
						if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
							out_of_bounds(xx, yy);
						push((unsigned char) *(program.lines + (yy * WIDTH + xx)));
						break;
					}
					case '&': {
						STACK_TYPE num;
						scanf(STACK_FMT, &num);
						push(num);
						break;
					}
					case '~': {
						int a = getchar();
						push(a);
						break;
					}
					case '@': {
						return;
					}
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
	} else {
		prog_from_file(argv[1]);
		if (program.valid) {
			stack.arr = malloc(sizeof(STACK_TYPE) * DEF_STACK_SIZE);
			stack.pointer = stack.arr - 1;
			stack.size = DEF_STACK_SIZE;
			run();
			free(stack.arr);
		}
		free(program.lines);
	}
	return 0;
}
