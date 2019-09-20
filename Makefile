SDLINC = `sdl2-config --cflags`
SDLLIB = `sdl2-config --libs`
INCS = -I. ${SDLINC}
LIBS = -lm ${SDLLIB}
CFLAGS = -std=c99 -pedantic -g -Wall ${INCS}
LDFLAGS = -s ${LIBS}

SRC = vec.c mat.c tri.c mesh.c sr.c
OBJ = ${SRC:.c=.o}

.PHONY: all options clean run

all: options clean sr run

options:
	@echo sr build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

run:
	@./sr

clean:
	@echo cleaning
	@rm -f srender ${OBJ}

.c.o:
	${CC} -c ${CFLAGS} $<

sr: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}
