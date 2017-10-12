#makefile
FLAGS = -g
DEFINES = 
all: prefixer

debug: FLAGS +=  -g
debug: prefixer

prefixer: main.o common.o tree.o
	gcc -o prefixer main.o common.o tree.o

main.o: core.c defs.h common.h tree.h
	gcc -o main.o -c core.c $(FLAGS) -W -Wall -ansi

common.o: common.c common.h defs.h tree.h
	gcc -o common.o -c common.c $(FLAGS) -W -Wall -ansi

tree.o: tree.c tree.h
	gcc -o tree.o -c tree.c $(FLAGS) -W -Wall -ansi

clean:
	rm -rf *.o;\
	rm -rf prefixer
