install: main
		./main

main: main.o shell.o
		gcc -o main main.o shell.o

main.o: main.c
		gcc -c main.c

shell.o: shell.c
		gcc -c shell.c
