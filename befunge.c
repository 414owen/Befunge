#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STACK_TYPE int
#define DEF_STACK_SIZE 1000
#define WIDTH 80
#define HEIGHT 25

#define binary_op(__op_char__, __op__, __stack__) \
case __op_char__: {\
	STACK_TYPE __a__ = pop(__stack__);\
	STACK_TYPE __b__ = pop(__stack__);\
	push(__stack__, __b__ __op__ __a__);\
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
	STACK_TYPE a = pop(stack);\
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

char empty_stack(struct Stack* s) {
	return s->pointer < s->arr;
}

STACK_TYPE head_with_info(bool empty, struct Stack *s) {
	if (empty) {return 0;}
	return *(s->pointer);
}

// return head of list
STACK_TYPE head(struct Stack *s) {
	return head_with_info(empty_stack(s), s);
}

STACK_TYPE pop(struct Stack* s) {
	bool empty = empty_stack(s);
	STACK_TYPE res = head_with_info(empty, s);
	if (!empty) {s->pointer--;}
	return res;
}

// Add element to head of list
void push(struct Stack *s, int t) {
	if (s->pointer - s->arr + 1 == s->size) {
		s->size *= 2;
		s->arr = realloc(s->arr, sizeof(STACK_TYPE) * s->size);
	}
	*(++(s->pointer)) = t;
}

struct Stack* new_stack() {
	struct Stack *result = malloc(sizeof(struct Stack));
	result->arr = malloc(sizeof(STACK_TYPE) * DEF_STACK_SIZE);
	result->pointer = result->arr - 1;
	result->size = DEF_STACK_SIZE;
	return result;
}

// "Classic Premature Optimisation" - Brian Whelan (BAmod pending)
// "Shut up I wanna" - me
struct Program* prog_from_file(char* filename) {
	FILE* file = fopen(filename, "r");
	struct Program* program = malloc(sizeof(struct Program));
	if (file == NULL) {
		fprintf(stderr, "Couldn't open file. Did you get the path right?\n");
		program->valid = 0;
		return program;
	}
	size_t bflines = 0;
	size_t line_buf = 100;
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
	program->lines = prog;
	program->valid = 1;
	program->width = max_len;
	program->height = bflines;
	fclose(file);
	return program;
}

void print_prog_with_pointer(char* program, int width, int height, int x, int y, struct Stack *s) {
	fprintf(stderr, "\x1b[2J");
	for (int j = 0; j < height; j++) {
		int linep = (WIDTH * j);
		for (int i = 0; i < width; i++) {
			if (y == j && x == i) {
				fprintf(stderr, "\x1b[44mX\x1b[49m");
			} else {
				fprintf(stderr, "%c", *(program + (linep + i)));
			}
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n\x1b[34mStack: \x1b[39m");
	for (STACK_TYPE* p = s->arr; p <= s->pointer; p++) {
		fprintf(stderr, " %d", *p);
	}
	fprintf(stderr, "\n");
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
	char string_mode = 0;
	while(1) {
		char curr = *(program + (y * WIDTH + x));
#ifdef DEVEL
		print_prog_with_pointer(program, width, height, x, y, stack);
		fprintf(stderr, "\n(press <return> to continue) ");
		while (getchar() != '\n') {}
#endif
		if (string_mode) {
			if (curr == '"') {
				string_mode = 0;
			} else {
				push(stack, curr);
			}
		} else {
			if (curr >= '0' && curr <= '9') {
				push(stack, curr - '0');
			} else {
				switch(curr) {
					binary_op('+', +, stack)
					binary_op('-', -, stack)
					binary_op('*', *, stack)
					binary_op('/', /, stack)
					binary_op('%', %, stack)
					binary_op('`', >, stack)
					case '!': {
						STACK_TYPE a = pop(stack);
						push(stack, a == 0);
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
						push(stack, head(stack));
						break;
					}
					case '\\': {
						 STACK_TYPE a = pop(stack);
						 STACK_TYPE b = pop(stack);
						 push(stack, a);
						 push(stack, b);
						 break;
					 }
					case '$': {
						pop(stack);
						break;
					}
					case '.': {
						printf("%d ", pop(stack));
						break;
					}
					case ',': {
						putchar(pop(stack));
						break;
					}
					case '#': {
						x += dx;
						y += dy;
						break;
					}
					case 'p': {
						STACK_TYPE yy = pop(stack);
						STACK_TYPE xx = pop(stack);
						STACK_TYPE v = pop(stack);
						if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
							out_of_bounds(xx, yy);
						if (width < xx) width = xx;
						if (height < yy) height = yy;
						*(program +	(yy * WIDTH + xx)) = v;
						break;
					}
					case 'g': {
						STACK_TYPE yy = pop(stack);
						STACK_TYPE xx = pop(stack);
						if (xx >= WIDTH || xx < 0 || yy >= HEIGHT || yy < 0)
							out_of_bounds(xx, yy);
						push(stack, (unsigned char) *(program + (yy * WIDTH + xx)));
						break;
					}
					case '&': {
						int num;
						scanf("%d", &num);
						push(stack, num);
						break;
					}
					case '~': {
						int a = getchar();
						push(stack, a);
						break;
					}
					case '@': {
						free(stack->arr);
						free(stack);
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
		struct Program* program = prog_from_file(argv[1]);
		if (program->valid) {
			run(program);
		}
		free(program->lines);
		free(program);
	}
	return 0;
}
