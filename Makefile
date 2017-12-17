install: main
		rm *.o
		./main

main: main.o shell.o cache.o
		gcc -o main main.o shell.o cache.o

main.o: main.c
		gcc -c main.c

shell.o: shell.c
		gcc -c shell.c

cache.o: cache.c
		gcc -c cache.c
