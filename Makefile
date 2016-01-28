CC?=		gcc

CFLAGS+=	-Wall -Werror -pedantic -std=c11

PROG=		ssdaligntest

all: ${PROG}

${PROG}: ${PROG}.c
	${CC} ${CFLAGS} ${PROG}.c -o ${PROG} -lm

clean:
	rm -f ${PROG}
