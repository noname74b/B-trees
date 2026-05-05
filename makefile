run: prog
	./prog
prog: main.c trees.o
	gcc main.c trees.o -o prog
trees.o: trees.c trees.h
	gcc -c trees.c -o trees.o
