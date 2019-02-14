CC = gcc
CFLAGS = -Wall -Werror -g
##all: wish str

## make
wish: wish.c commands.o str.o
##	gcc -Wall -Werror wish.c -o wish
	$(CC) $(CFLAGS)  wish.c commands.o str.o -o wish -lm -g
str.o: str.c
	$(CC) $(CFLAGS) -c str.c -lm -g
##str: str.c
##	$(CC) $(CFLAGS) str.c -o str
commands.o: commands.c
	$(CC) $(CFLAGS) -c commands.c -g
	
## clean
clean:
	rm *.o wish 
