CFLAGS= -g
CC=gcc
RM=rm -f
FILES=scanner.c string.c symtable.c
all: $(FILES)
	$(CC) $(CFLAGS) -o ifj22 $(FILES)

clean:
	$(RM) *.o ifj22
