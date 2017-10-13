#makefile
FLAGS = 
DEFINES = 
all: prefixer

debug: FLAGS =  -g
debug: prefixer

prefixer: main.o tree.o
	gcc -o prefixer main.o tree.o

main.o: core.c defs.h tree.h
	gcc -o main.o -c core.c $(FLAGS) -W -Wall -ansi

tree.o: tree.c tree.h defs.h
	gcc -o tree.o -c tree.c $(FLAGS) -W -Wall -ansi

clean:
	rm -rf *.o;\
	rm -rf prefixer
