@echo off
c:\lcc\bin\lcc.exe -O -p6 -unused sha1.c
c:\lcc\bin\lcc.exe -O -p6 -unused md2_dgst.c
c:\lcc\bin\lcc.exe -O -p6 -unused md4_dgst.c
c:\lcc\bin\lcc.exe -O -p6 -unused md5.c
c:\lcc\bin\lcc.exe -O -p6 -unused mycrc.c
c:\lcc\bin\lcclnk.exe -s -subsystem:console mycrc.obj sha1.obj md2_dgst.obj md4_dgst.obj md5.obj
del mycrc.obj
del sha1.obj
del md2_dgst.obj
del md4_dgst.obj
del md5.obj
