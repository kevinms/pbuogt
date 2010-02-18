CC = gcc
CCFLAGS = -g -Wall -shared
LIBS=
INCS=

all: webui

webui:
	${CC} ${CCFLAGS} -o webui.so webui.c ${LIBS} ${INCS}

clean:
	rm -fv *.so
