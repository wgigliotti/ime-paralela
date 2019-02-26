CC=gcc
CFLAGS=-Wall -pedantic -g
LIBS=-lm

all: pgrep

pgrep: pgrep.o 
	${CC} ${CFLAGS} -o pgrep pgrep.o ${LIBS}
clean:
	rm pgrep *.o

