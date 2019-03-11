CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
LIBS=-lm -pthread

all: pgrep alt_pgrep

pgrep: list.o pgrep.o grep.o parallel_file.o
	${CC} ${CFLAGS} -o pgrep list.o pgrep.o grep.o parallel_file.o ${LIBS}
	
alt_pgrep: list.o alt_pgrep.o grep.o parallel_file.o
	${CC} ${CFLAGS} -o alt_pgrep list.o alt_pgrep.o grep.o parallel_file.o ${LIBS}
	
clean:
	rm pgrep *.o

