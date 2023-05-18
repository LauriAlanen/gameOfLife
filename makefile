CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11
LDFLAGS=-lSDL2 -lSDL2_image
OBJS = engine/BOARD_functions.c engine/main.c

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f main
