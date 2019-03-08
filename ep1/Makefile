CC=gcc
CFLAGS=-Wall -Wmisleading-indentation -pedantic -g
LIBS=-lm -pthread

all: pgrep

pgrep: list.o pgrep.o grep.o sleep.o
	${CC} ${CFLAGS} -o pgrep *.o ${LIBS}

clean:
	rm pgrep *.o

