CFLAGS= -g -Wall -Werror -pedantic -Wunreachable-code -Wextra  #-fsanitize=address #-O2
CC=gcc
RM=rm -f
FILES=scanner.c string.c symtable.c test.c
all: $(FILES)
	$(CC) $(CFLAGS) -o ifj22 $(FILES)

clean:
	$(RM) *.o ifj22
