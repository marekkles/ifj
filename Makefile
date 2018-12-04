CC=gcc
TARGETS=main
all: ${TARGETS}

main: main.o debug.o dstr.o parser.o fsm.o symtable.o symbolstack.o codegen.o error.o

%.o: %.c
	${CC} -c $^ -o $@

clean:
	rm *.o

clean-all:
	rm ${TARGETS}

zip:
	zip -j xvasko16.zip *.c *.h Makefile rozdeleni rozsireni doc/dokumentace.pdf

dep:
	gcc -MM *.c >code.dep
