CC = gcc
CFLAGS = -Wall -Werror -g
MATH = -lm
## make
wish: wish.c commands.o str.o
	$(CC) $(CFLAGS)  wish.c commands.o str.o -o wish $(MATH)

str.o: str.c
	$(CC) $(CFLAGS) -c str.c $(MATH)

commands.o: commands.c
	$(CC) $(CFLAGS) -c commands.c 
	
## clean
clean:
	rm *.o wish 
