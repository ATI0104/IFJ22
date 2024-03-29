CFLAGS= #-g -Wall -Werror -pedantic -Wunreachable-code -Wextra
CC=gcc
RM=rm -f
FILES=scanner.c string.c symtable.c parser.c stack.c ast.c semantics.c expression.c main.c generator.c
all: $(FILES)
	$(CC) $(CFLAGS) -o ifj22 $(FILES) -lm 
debug: #skips prolog check
	$(CC) $(CFLAGS) -D '_skip_prolog_check' -o ifj22 $(FILES) -lm 
clean:
	$(RM) *.o ifj22
