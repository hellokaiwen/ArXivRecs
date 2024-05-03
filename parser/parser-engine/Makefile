all: main

parser.o: parser.h parser.c
	gcc -c parser.c -o parser.o

main.o: main.c
	gcc -c main.c -o main.o

parser: parser.o
	gcc parser.o -o parser

main: parser.o main.o
	gcc main.o parser.o -lcurl -o main

.PHONY: clean

clean:
	rm -f *.o
