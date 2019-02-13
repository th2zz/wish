CC = gcc
CFLAGS = -Wall -Werror -g
##all: wish str

## make
wish: wish.c commands.o str.o
##	gcc -Wall -Werror wish.c -o wish
	$(CC) $(CFLAGS) wish.c commands.o str.o -o wish
str.o: str.c
	$(CC) $(CFLAGS) -c str.c
##str: str.c
##	$(CC) $(CFLAGS) str.c -o str
commands.o: commands.c
	$(CC) $(CFLAGS) -c commands.c
	
## clean
clean:
	rm *.o wish str
