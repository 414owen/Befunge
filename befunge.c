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
// #define DEVEL

#ifdef DEVEL 
#define dprintf(...) fprintf (stderr, __VA_ARGS__)
#else
#define dprintf(...) 
#endif

struct Program {
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

struct Program* prog_from_file(char* filename) {
	size_t bflines = 0;
	size_t line_buf = 100;
	size_t prog_size = sizeof(char) * WIDTH * HEIGHT + 2;
	char* prog = malloc(prog_size); // + 2 is for '\n' and '\0' from fgets
	size_t max_len = 0;
	FILE* file = fopen(filename, "r");
	for (int j = 0; j < HEIGHT; j++) {
		char* linep = prog + j * sizeof(char) * WIDTH;
		fgets(linep, WIDTH + 2, file);
		if (feof(file)) {break;}
		int line_len = WIDTH + 1;
		while (true) {
			char prev = *(linep + line_len);
			*(linep + line_len) = ' ';
			if (prev == '\0') {break;}
		}
		if (line_len > max_len) {max_len = line_len;}
		bflines++;
	}
	struct Program* program = malloc(sizeof(struct Program));
	program->lines = prog;
	program->width = max_len;
	program->height = bflines;
	return program;
}

void print_prog(struct Program* program) {
	size_t width = program->width;
	size_t height = program->height;
	for (size_t j = 0; j < height; j++) {
		printf("%s\n", program->lines[j]);
	}
}

void print_prog_with_pointer(char* line, int width, int height, int x, int y) {
	for (size_t j = 0; j < y; j++) {
		for (size_t i = 0; i < width; i++) {
			dprintf("%c", lines[j][i]);
		}
		dprintf("\n");
	}
	for (size_t i = 0; i < x; i++) {
		dprintf("%c", lines[y][i]);
	}
	dprintf(ANSI_COLOR_RED "X" ANSI_COLOR_RESET);
	for (size_t i = x + 1; i < width; i++) {
		dprintf("%c", lines[y][i]);
	}
	dprintf("\n");
	for (size_t j = y + 1; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			dprintf("%c", lines[j][i]);
		}
		dprintf("\n");
	}

	dprintf("\n");
}

void out_of_bounds(int x, int y) {
	printf("Program limits are 80 x 25, tried to access %d x %d", x, y);
}

bool run(struct Program* prog) {
	size_t width = prog->width;
	size_t height = prog->height;
	char* program = prog->lines;
	int x = 0;
	int y = 0;
	int dx = 1;
	int dy = 0;
	struct Stack* stack = new_stack();
	bool string_mode = false;
	while(true) {
		char curr = program[sizeof(char) * (y * WIDTH + x)];
#ifdef DEVEL
		print_prog_with_pointer(lines, width, height, x, y);
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
					case '+': 
						{
							int a = car(stack);
							stack = cdr(stack);
							int b = car(stack);
							stack = cdr(stack);
							stack = cons(stack, a + b);
							break;
						}
					case '-': 
						{
							int a = car(stack);
							stack = cdr(stack);
							int b = car(stack);
							stack = cdr(stack);
							stack = cons(stack, b - a);
							break;
						}
					case '*': 
						{
							int a = car(stack);
							stack = cdr(stack);
							int b = car(stack);
							stack = cdr(stack);
							stack = cons(stack, a * b);
							break;
						}
					case '/': 
						{
							int a = car(stack);
							stack = cdr(stack);
							int b = car(stack);
							stack = cdr(stack);
							stack = cons(stack, b / a);
							break;
						}
					case '%': 
						{
							int a = car(stack);
							stack = cdr(stack);
							int b = car(stack);
							stack = cdr(stack);
							stack = cons(stack, b % a);
							break;
						}
					case '!': 
						{
							int a = car(stack);
							stack = cdr(stack);
							stack = cons(stack, a == 0);
							break;
						}
					case '`': 
						{
							int a = car(stack);
							stack = cdr(stack);
							int b = car(stack);
							stack = cdr(stack);
							stack = cons(stack, b > a);
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
							int valt = car(stack);
							stack = cdr(stack);
							int valb = car(stack);
							stack = cdr(stack);
							stack = cons(stack, valt);
							stack = cons(stack, valb);
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
							printf("%c", car(stack));
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
							if (xx >= WIDTH || x < 0 || y >= HEIGHT || y < 0) {
								out_of_bounds(x, y);
							}
							width = width > xx ? width : xx;
							height = height > yy ? height : yy;
							program[sizeof(char) * (y * WIDTH + x)] = v;
							break;
						}
					case 'g': 
						{
							int yy = car(stack);
							stack = cdr(stack);
							int xx = car(stack);
							stack = cdr(stack);
							if (xx >= WIDTH || x < 0 || y >= HEIGHT || y < 0) {
								out_of_bounds(x, y);
							}
							stack = cons(stack, program[sizeof(char) * (y * WIDTH + x)]);
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
						{ return true; }
				}
			}
		}
		x = (x + dx) % width;
		y = (y + dy) % height;
	}
}

int main(int argc, char *argv[]) {
	srand(time(0));
	struct Program* program = prog_from_file(argv[1]);
	printf("Program read\nwidth: %d, height: %d\n", program->width, program->height);
	// print_prog(program);
	run(program);
	return 0;
}
