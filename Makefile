CC=gcc
CFLAGS=
TARGETS=
TESTS=fsm-test dstr-test hash-test
all: ${TARGETS}

debug: CFLAGS += -DDEBUG
debug: ${TARGETS}

test: ${TESTS}
debug-test: CFLAGS += -DDEBUG
debug-test:  ${TESTS}

fsm-test: fsm-test.o fsm.o dstr.o debug.o
dstr-test: dstr-test.o dstr.o debug.o
hash-test: hash-test.o hash.o


test1: test1.o input.o

%.o: %.c
	${CC} ${CFLAGS} -c $^ -o $@

clean:
	rm *.o

clean-all:
	rm ${TARGETS}
