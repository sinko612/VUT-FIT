# Makefile
# autor: Simona Jánošíková, xjanos19

CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

proj2: proj2.o
	$(CC) $(CFLAGS) proj2.o -o proj2

proj2.o: proj2.c
	$(CC) $(CFLAGS) -c proj2.c -o proj2.o

clean:
	rm *.o