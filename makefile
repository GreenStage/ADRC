#makefile
FLAGS = 
DEFINES = 
all: prefixer

debug: FLAGS +=  -g
debug: prefixer

prefixer: main.o common.o
	gcc -o prefixer main.o common.o

main.o: core.c defs.h common.h
	gcc -o main.o -c core.c $(FLAGS) -W -Wall -ansi

common.o: common.c common.h defs.h
	gcc -o common.o -c common.c $(FLAGS) -W -Wall -ansi

clean:
	rm -rf *.o;\
	rm -rf prefixer
