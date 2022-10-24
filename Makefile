CFLAGS= -lm -g -Wall -Werror -pedantic -Wunreachable-code -Wextra   #-fsanitize=address #-O2
CC=gcc
RM=rm -f
FILES=scanner.c string.c symtable.c parser.c test.c stack.c
all: $(FILES)
	$(CC) $(CFLAGS) -o ifj22 $(FILES)
debug: #skips prolog check
	$(CC) $(CFLAGS) -D '_skip_prolog_check' -o ifj22 $(FILES)
clean:
	$(RM) *.o ifj22
