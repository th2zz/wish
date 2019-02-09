CC = gcc
CFLAGS = -Wall -Werror
##all: for multiple executable

## make
wish: wish.c
	$(CC) $(CFLAGS) wish.c -o wish


## clean
clean:
	rm *.o wish
