#makefile
CFLAGS = 

all: as_router

debug: CFLAGS =  -g
debug: CFLAGS += -DDEBUG
debug: as_router

as_router: main.o network.o list.o
	gcc -o as_router main.o network.o list.o

main.o: core.c defs.h network.h
	gcc -o main.o -c core.c $(CFLAGS) -W -Wall -ansi

network.o: network.c network.h defs.h list.h
	gcc -o network.o -c network.c $(CFLAGS) -W -Wall -ansi

list.o: list.c defs.h list.h
	gcc -o list.o -c list.c $(CFLAGS) -W -Wall -ansi

clean:
	rm -rf *.o;\
	rm -rf as_router
