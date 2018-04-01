/*
    Copyright 2004,2005 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define VER         "0.2"
#define BYTESLINE   16
#define CHEX        "\\x%02x"



void std_err(void);



int main(int argc, char *argv[]) {
    FILE            *fd    = stdin,
                    *fdo   = stdout;
    int             i,
                    len,
                    bs     = 0,
                    line   = BYTESLINE;
    unsigned char   *buff,
                    *p;
    char            *hexf  = CHEX,
                    *ifile,
                    *ofile = NULL;


    setbuf(stdout, NULL);

    fputs("\n"
        "BYTE2HEX "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stderr);

    if(argc < 2) {
        fprintf(stderr, "\n"
            "Usage: %s [options] <file>\n"
            "\n"
            "Options:\n"
            "-o FILE  output file (default stdout)\n"
            "-b NUM   number of bytes for each line (%d)\n"
            "-s       adds a backslash at the end of each line\n"
            "-f FMT   FMT is the format (C style) you want to use, like:\n"
            "         %s (default), %%o, %%d, 0x%%02x, \" %%02X\", \" myhex: %%02x \" ...\n"
            "\n"
            " Note: Use the filename - to select stdin\n"
            "\n", argv[0], line, hexf);
        exit(1);
    }

    argc--;
    for(i = 1; i < argc; i++) {
        switch(argv[i][1]) {
            case 'o': ofile = argv[++i]; break;
            case 'b': line = atoi(argv[++i]); break;
            case 's': bs = 1; break;
            case 'f': hexf = argv[++i]; break;
            default: {
                fprintf(stderr, "\nError: wrong command-line argument (%s)\n\n", argv[i]);
                exit(1);
                }
        }
    }

    ifile = argv[argc];
    if(strcmp(ifile, "-")) {
        fprintf(stderr, "- open input file  %s\n", ifile);
        fd = fopen(ifile, "rb");
        if(!fd) std_err();
    }

    if(ofile) {
        fprintf(stderr, "- open output file %s\n", ofile);
        fdo = fopen(ofile, "wb");
        if(!fdo) std_err();
    }

    buff = malloc(line);
    if(!buff) std_err();

    while((len = fread(buff, 1, line, fd))) {
        if(hexf == CHEX) fputc('\"', fdo);

        for(p = buff, i = 0; i < len; i++, p++) {
            fprintf(fdo, hexf, *p);
        }

        if(hexf == CHEX) fputc('\"', fdo);

        if(bs && (len == line)) fputs(" \\", fdo);
        fputc('\n', fdo);
    }

    if(fd != stdin) fclose(fd);
    if(fdo != stdout) fclose(fdo);
    free(buff);
    return(0);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


