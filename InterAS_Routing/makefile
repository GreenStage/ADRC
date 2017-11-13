#makefile
CFLAGS = -D_BSD_SOURCE

all: as_router

debug: CFLAGS =  -g
debug: CFLAGS += -DDEBUG
debug: as_router

as_router: main.o network.o list.o
	gcc -o as_router main.o network.o list.o &&\
	rm -f *.o

main.o: src/core.c src/defs.h src/network.h
	gcc -o main.o -c src/core.c $(CFLAGS) -W -Wall -ansi

network.o: src/network.c src/network.h src/defs.h src/list.h
	gcc -o network.o -c src/network.c $(CFLAGS) -W -Wall -ansi

list.o: src/list.c src/defs.h src/list.h
	gcc -o list.o -c src/list.c $(CFLAGS) -W -Wall -ansi

clean:
	rm -rf *.o;\
	rm -rf as_router
