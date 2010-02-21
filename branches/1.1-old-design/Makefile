CC = gcc
CCFLAGS = -g -Wall
LIBS=-lpthread
INCS=

all: pbuogt-com pbuogt-io

pbuogt-com: com.o list.o utils.o
	${CC} ${CCFLAGS} -o pbuogt-com com.o list.o utils.o ${LIBS} ${INCS}

pbuogt-io: io.o list.o utils.o
	${CC} ${CCFLAGS} -o pbuogt-io io.o list.o utils.o ${LIBS} ${INCS}

io.o: io.o
	${CC} ${CCFLAGS} -c io.c ${LIBS} ${INCS}

com.o: com.c
	${CC} ${CCFLAGS} -c com.c ${LIBS} ${INCS}

list.o: list.c
	${CC} ${CCFLAGS} -c list.c ${LIBS} ${INCS}

utils.o: utils.c
	${CC} ${CCFLAGS} -c utils.c ${LIBS} ${INCS}

clean:
	rm -fv *.o pbuogt-com pbuogt-io pbuogt-out pbuogt-event pbuogt-script

io-clean:
	rm -fv io.o pbuogt-io utils.o
