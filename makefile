CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11 -g
LDFLAGS=-lncurses

program: engine/main.c
	$(CC) $(CFLAGS) engine/main.c -o main $(LDFLAGS)

clean:
	rm -f main
