all: main

parse.o: parse.h parse.c
	gcc -c parse.c -o parse.o

main.o: main.c
	gcc -c main.c -o main.o

parse: parse.o
	gcc parse.o -o parse

main: parse.o main.o
	gcc main.o parse.o -lcurl -lxml2 -o main

.PHONY: clean

clean:
	rm -f *.o
