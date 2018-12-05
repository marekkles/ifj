CC=gcc
CFLAGS=
TARGETS=main

all: ${TARGETS}
debug: CFLAGS+=-DDEBUG -ggdb
debug:${TARGETS}


main: main.o debug.o dstr.o parser.o fsm.o symtable.o symbolstack.o codegen.o error.o

%.o: %.c
	${CC} ${CFLAGS} -c $^ -o $@

clean:
	rm *.o

clean-all:
	rm ${TARGETS}

zip:
	zip -j xvasko16.zip *.c *.h Makefile rozdeleni rozsireni doc/dokumentace.pdf

dep:
	gcc -MM *.c >code.dep
