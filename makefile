run: main
	./main
trees.o: trees.c trees.h
	gcc -c trees.c trees.h
main: trees.o main.c
	gcc main.c trees.o -o main