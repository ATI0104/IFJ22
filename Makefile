CFLAGS= -g -Wall -Werror -pedantic -Wunreachable-code -Wextra   #-fsanitize=address #-O2
CC=gcc
RM=rm -f
FILES=scanner.c string.c symtable.c parser.c test.c stack.c ast.c semantics.c #expression.c
all: $(FILES)
	$(CC) $(CFLAGS) -o ifj22 $(FILES) -lm 
debug: #skips prolog check
	$(CC) $(CFLAGS) -D '_skip_prolog_check' -o ifj22 $(FILES) -lm 
clean:
	$(RM) *.o ifj22
