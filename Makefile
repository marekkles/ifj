CC=gcc
CFLAGS=-ggdb3
TARGETS=main
TESTS=fsm-test dstr-test hash-test parser-test symtable-test error-test symbolstack-test
all: ${TARGETS}

debug: CFLAGS += -DDEBUG -ggdb
debug: ${TARGETS}

test: ${TESTS}
debug-test: CFLAGS += -DDEBUG -ggdb
debug-test:  ${TESTS}

fsm-test: fsm-test.o fsm.o dstr.o debug.o symtable.o
dstr-test: dstr-test.o dstr.o debug.o symtable.o
hash-test: hash-test.o hash.o
parser-test: parser-test.o debug.o dstr.o parser.o fsm.o symtable.o
symtable-test: symtable-test.o debug.o dstr.o symtable.o
error-test: error-test.o error.o
symbolstack-test: symbolstack-test.o debug.o symbolstack.o symtable.o dstr.o

main: main.o fsm.o dstr.o debug.o

%.o: %.c
	${CC} ${CFLAGS} -c $^ -o $@

#codegen.o: codegen.c codegen.h
#debug.o: debug.c debug.h token.h dstr.h
#dstr.o: dstr.c dstr.h debug.h token.h
#dstr-test.o: dstr-test.c dstr.h debug.h token.h
#fsm.o: fsm.c fsm.h return.h dstr.h token.h debug.h
#fsm-test.o: fsm-test.c fsm.h return.h dstr.h token.h debug.h
#hash.o: hash.c hash.h
#hash-test.o: hash-test.c hash.h
#main.o: main.c token.h debug.h dstr.h fsm.h return.h
#parser.o: parser.c parser.h return.h debug.h token.h dstr.h fsm.h
#parser-test.o: parser-test.c debug.h token.h dstr.h parser.h return.h fsm.h
#symtable.o: symtable.c symtable.h hash.h

clean:
	rm *.o

clean-all:
	rm ${TARGETS}

zip:
	zip -j xvasko16.zip *.c *.h Makefile rozdeleni rozsireni doc/dokumentace.pdf

dep:
	gcc -MM *.c