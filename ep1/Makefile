CC=gcc
CFLAGS=-Wall -pedantic -g
LIBS=-lm

all: pgrep

pgrep: dir.o pgrep.o
	${CC} ${CFLAGS} -o pgrep *.o ${LIBS}
clean:
	rm pgrep *.o

