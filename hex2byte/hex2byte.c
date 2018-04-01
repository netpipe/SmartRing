/*
    Copyright 2004,2005,2006,2007,2008 Luigi Auriemma

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

    http://www.gnu.org/licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef WIN32
    #define strnicmp    strncasecmp
#endif

typedef unsigned char   u8;



#define VER     "0.3"
#define INSZ    0xffff  // default, used only for stdin



int hex2byte(u8 *hex);
void std_err(void);



int main(int argc, char *argv[]) {
    struct stat xstat;
    FILE    *fd     = stdin,
            *fdo    = stdout;
    int     len     = INSZ,
            i,
            tmp,
            chr,
            verify_len,
            spaces,
            hexdump = 0;
    u8      *buff   = NULL,
            *in,
            *out,
            *verify = NULL,
            *format = NULL,
            *fmt    = NULL,
            *ifile,
            *ofile  = NULL,
            *limit,
            *oldout;

    setbuf(stdout, NULL);

    fputs("\n"
        "HEX2BYTE "VER"\n"
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
            "-f FMT   FMT is the format (C style) you want to use, like:\n"
            "         %%x, %%d, 0x%%02hhx, %%f, %%g, %%c, %%o ...\n"
            "         the default format is a special fast hexadecimal\n"
            "-x       specific option for parsing the hex dumps of max 16 chars per line\n"
            "         like the following example:\n"
            "   01C99BB0  61 62 63 64 65 66 67 68  69 6a 6b 6c 6d 6e 6f 70  abcdefghijklmnop\n"
            "\n"
            "it's possible to select stdin as input using - as file"
            "\n", argv[0]);
        exit(1);
    }

    argc--;
    for(i = 1; i < argc; i++) {
        if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
            fprintf(stderr, "\nError: wrong argument (%s)\n", argv[i]);
            exit(1);
        }
        switch(argv[i][1]) {
            case 'o': ofile     = argv[++i];    break;
            case 'f': fmt       = argv[++i];    break;
            case 'x': hexdump   = 1;            break;
            default: {
                fprintf(stderr, "\nError: wrong command-line argument (%s)\n", argv[i]);
                exit(1);
            }
        }
    }
    ifile = argv[argc];

    if(strcmp(ifile, "-")) {
        fprintf(stderr, "- open input file  %s\n", ifile);
        fd = fopen(ifile, "rb");
        if(!fd) std_err();
        fstat(fileno(fd), &xstat);
        len = xstat.st_size;
    }

    buff = malloc(len + 1);
    if(!buff) std_err();

    if(fmt) {
        tmp = snprintf(buff, len, fmt, 0xff);
        if((tmp <= 0) || (tmp > len)) tmp = 32; // not very important
        verify = malloc(tmp + 1);
        if(!verify) std_err();

        format = malloc(strlen(fmt) + 3);
        if(!format) std_err();
        sprintf(format, "%s%%n", fmt);
    }

    len = fread(buff, 1, len, fd);
    fprintf(stderr, "- input size:      %u   (0x%08x)\n", len, len);
    buff[len] = 0;  // mah, probably used for sscanf

    in     = buff;
    out    = buff;
    limit  = buff + len;
    oldout = NULL;
    spaces = 0;
    while(in < limit) {
        if(hexdump) {
            if(!oldout) {
                spaces = 0;
                while((in < limit) && ((in[0] == ' ') || (in[0] == '\t'))) in++;    // skip spaces before offset
                for(tmp = 0; (in < limit) && ((in[0] != ' ') && (in[0] != '\t')); in++, tmp++); // skip offset
                if(tmp == 2) {  // if it's 2 it's not considered an offset
                    in -= tmp;
                } else {
                    for(tmp = 0; (in < limit) && ((in[0] == ' ') || (in[0] == '\t')); in++, tmp++);
                    if(tmp >= 4) spaces = 1;
                }
                oldout = out;
            } else if((out - oldout) == 16) {
                while((in < limit) && ((in[0] != '\r') && (in[0] != '\n'))) in++;   // skip cr-lf
                while((in < limit) && ((in[0] == '\r') || (in[0] == '\n'))) in++;
                oldout = NULL;
                continue;
            } else {
                for(tmp = 0; (in < limit) && ((in[0] == ' ') || (in[0] == '\t')); in++, tmp++);
                if(tmp >= 4) {
                    oldout = out - 16;
                    continue;
                } else if(spaces && (tmp >= 2)) {
                    oldout = NULL;
                    continue;
                }
            }
        }

        if(!fmt) {
            chr = hex2byte(in);
            if(chr >= 0) {
                in += 2;
                *out++ = chr;
                continue;
            }
        } else if(sscanf(in, format, &chr, &tmp) == 1) {
            verify_len = sprintf(verify, fmt, chr & 0xff);
            if(!strnicmp(verify, in, verify_len)) {
                in  += tmp;
                *out++ = chr;
                continue;
            }
        }
        in++;
    }

    if(ofile) {
        fprintf(stderr, "- open output file %s\n", ofile);
        fdo = fopen(ofile, "wb");
        if(!fdo) std_err();
    }

    len = out - buff;
    fwrite(buff, len, 1, fdo);

    if(fd  != stdin)  fclose(fd);
    if(fdo != stdout) fclose(fdo);
    if(buff)   free(buff);
    if(format) free(format);
    if(verify) free(verify);
    fprintf(stderr, "- output size:     %u   (0x%08x)\n", len, len);
    return(0);
}



int hex2byte(u8 *hex) {
    static const signed char hextable[256] =
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\xff\xff\xff\xff\xff\xff"
        "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";

    if((hextable[hex[0]] < 0) || (hextable[hex[1]] < 0)) return(-1);
    return((hextable[hex[0]] << 4) | hextable[hex[1]]);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


