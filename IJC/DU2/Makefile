# Makefile
# Řešení IJC-DU2, příklad a), 19.4.2022
# Autor: Simona Jánošíková, FIT
# Přeloženo: gcc (server merlin) 19.4.

tail: tail.o
	gcc -std=c11 -pedantic -Wall -Wextra tail.o -o tail

tail.o: tail.c
	gcc -std=c11 -pedantic -Wall -Wextra -c tail.c -o tail.o

clean:
	rm -r *.o	