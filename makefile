CC?=gcc
DEBUG=-g -O0
OUTPUT=-o befunge
INPUT=befunge.c

release: $(INPUT)
	$(CC) -O3 $(OUTPUT) $(INPUT)

devel: $(INPUT)
	$(CC) $(DEBUG) $(OUTPUT) $(INPUT)

catprog: $(INPUT)
	$(CC) $(DEBUG) $(OUTPUT) -DDEVEL $(INPUT)
