CC=gcc
CFLAGS=-Wall -pedantic -g
LIBS=-lm -pthread

all: pgrep

pgrep: list.o pgrep.o grep.o
	${CC} ${CFLAGS} -o pgrep *.o ${LIBS}
clean:
	rm pgrep *.o

