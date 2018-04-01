CC = gcc

all:
	gcc -s ${CFLAGS} -c sha1.c
	gcc -s ${CFLAGS} -c md4_dgst.c
	gcc -s ${CFLAGS} -c md2_dgst.c
	gcc -s ${CFLAGS} -c md5.c
	gcc -s ${CFLAGS} -o mycrc mycrc.c sha1.o md4_dgst.o md2_dgst.o md5.o

clean:
	rm -f mycrc sha1.o md4_dgst.o md2_dgst.o md5.o
