@echo off
c:\mingw\bin\gcc -s -Wall -O3 -c sha1.c
c:\mingw\bin\gcc -s -Wall -O3 -c md2_dgst.c
c:\mingw\bin\gcc -s -Wall -O3 -c md4_dgst.c
c:\mingw\bin\gcc -s -Wall -O3 -c md5.c
c:\mingw\bin\gcc -s -Wall -O3 -c mycrc.c
c:\mingw\bin\gcc -s -Wall -O3 -o mycrc.exe mycrc.o sha1.o md2_dgst.o md4_dgst.o md5.o
del mycrc.o
del sha1.o
del md2_dgst.o
del md4_dgst.o
del md5.o
