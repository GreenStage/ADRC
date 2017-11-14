#makefile
CFLAGS = -D_BSD_SOURCE -O3

all: as_router

debug: CFLAGS += 


debug: main.o network.o list.o heap.o
	gcc -o as_router main.o network.o list.o heap.o

as_router: main.o network.o list.o heap.o
	gcc -o as_router main.o network.o list.o heap.o &&\
	rm -f *.o

main.o: src/core.c src/defs.h src/network.h
	gcc -o main.o -c src/core.c $(CFLAGS) -W -Wall -ansi

network.o: src/network.c src/network.h src/defs.h src/list.h src/heap.h
	gcc -o network.o -c src/network.c $(CFLAGS) -W -Wall -ansi

list.o: src/list.c src/defs.h src/list.h
	gcc -o list.o -c src/list.c $(CFLAGS) -W -Wall -ansi

heap.o: src/heap.c src/heap.h src/defs.h
	gcc -o heap.o -c src/heap.c $(CFLAGS) -W -Wall -ansi
clean:
	rm -rf *.o;\
	rm -rf as_router
