CC = gcc
CFLAGS = -Wall -Wextra


build: minesweeper.c
	$(CC) $(CFLAGS) -o minesweeper minesweeper.c

clean:
	rm -f minesweeper
