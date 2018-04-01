/*
    Copyright 2008 Luigi Auriemma

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
#include <stdint.h>
#include <ctype.h>
#include "show_dump.h"

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;



#define VER         "0.1"



void shownum(u8 *data, int len, int bits, FILE *fdo, int hex);
void doit(u8 *in, int len, FILE *fdo, int hexdump, int numbits);
int check_convers(u8 *in, int insz);
int bin2byte(u8 *in, int insz, u8 *out, int outbits);
int num2bin(u8 *in, int insz, u8 *out, int numbits, int hexdump);
int byte2bin(u8 *in, int insz, u8 *out, int inbits);
FILE *fopen_input(u8 *fname);
FILE *fopen_output(u8 *fname, int append);
void std_err(void);



int     buffsz  = 8192,
        convers = 0,
        endian  = 1;



int main(int argc, char *argv[]) {
    FILE    *fd     = NULL,
            *fdo    = stdout;
    int     i,
            len,
            hexdump = 0,
            inputf  = 0,
            append  = 0,
            numbits = 0;
    u8      *input  = NULL,
            *output = NULL,
            *in     = NULL;

    setbuf(stdin,  NULL);
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    fputs("\n"
        "Bynaryo "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stderr);

    if(argc < 2) {
        fprintf(stderr, "\n"
            "Usage: %s [options] <binary/ascii_string>\n"
            "\n"
            "Options:\n"
            "-f    <string> will be considered an input file, use - for stdin\n"
            "-a    force the conversion from binary to ascii (automatically recognized)\n"
            "-b    force the conversion from ascii to binary (automatically recognized)\n"
            "-x    hex dump of the output or alternative hex input/output if -n/-N\n"
            "-o F  write the converted string to the file F, overwriting it if exists\n"
            "-O F  as above but the data will be appended if the file already exists\n"
            "-n B  with -a the output will be a sequence of big endian numbers of B bits\n"
            "      with -b the input will be considered a sequence of numbers of B bits\n"
            "      the numbers are considered unsigned decimals, use -x for hexadecimals\n"
            "      B can be 8, 16, 32 or 64\n"
            "-N B  as above but the numbers will be in little endian format (only -a)\n"
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
            case '-':
            case '?':
            case 'h': exit(1);                      break;
            case 'f': inputf  = 1;                  break;
            case 'a': convers = 1;                  break;
            case 'b': convers = -1;                 break;
            case 'x': hexdump = 1;                  break;
            case 'o': output  = argv[++i];          break;
            case 'O': {
                output  = argv[++i];
                append  = 1;
                } break;
            case 'n': numbits = atoi(argv[++i]);    break;
            case 'N': {
                numbits = atoi(argv[++i]);
                endian  = 0;
                } break;
            default: {
                fprintf(stderr, "\nError: wrong argument (%s)\n", argv[i]);
                exit(1);
            }
        }
    }
    input = argv[argc];

    if(numbits && (numbits != 8) && (numbits != 16) && (numbits != 32) && (numbits != 64)) {
        fprintf(stderr, "\nError: you must choose a number of bits like 8, 16, 32 or 64\n");
        exit(1);
    }

    if(inputf) fd  = fopen_input(input);
    if(output) fdo = fopen_output(output, append);

    if(fd) {
        in = malloc(buffsz);
        if(!in) std_err();
        while((len = fread(in, 1, buffsz, fd))) {
            doit(in, len, fdo, hexdump, numbits);
        }
    } else {
        buffsz = strlen(input);
        doit(input, buffsz, fdo, hexdump, numbits);
    }

    if(fdo != stdout) fclose(fdo);
    if(fd)  fclose(fd);
    if(in)  free(in);
    fprintf(stderr, "\n");
    return(0);
}



void shownum(u8 *data, int len, int bits, FILE *fdo, int hex) {
    static const u8 hexstr[] = "0123456789abcdef";
    u64     num;
    int     i,
            bytes;
    u8      tmp[32],
            *limit;

    bytes = bits >> 3;
    for(limit = data + len - bytes; data <= limit; data += bytes) {
        num = 0;
        for(i = 0; i < bytes; i++) {
            if(endian) {
                num |= (data[i] << ((bytes - 1 - i) << 3));
            } else {
                num |= (data[i] << (i << 3));
            }
        }
        tmp[sizeof(tmp) - 1] = 0;
        i = sizeof(tmp) - 2;
        if(hex) {
            do {
                tmp[i] = hexstr[num % 16];
                num /= 16;
            } while(num && (--i >= 0));
            fprintf(fdo, "0x%s ", tmp + i);
        } else {
            do {
                tmp[i] = '0' + (num % 10);
                num /= 10;
            } while(num && (--i >= 0));
            fprintf(fdo, "%s ", tmp + i);
        }
    }
}



void doit(u8 *in, int len, FILE *fdo, int hexdump, int numbits) {
    static int  first   = 1;
    static int  outsz   = 0;
    static u8   *out    = NULL;

    if(first) {
        if(!convers) convers = check_convers(in, len);
        if(convers > 0) {   // -a
            if(numbits) {
                fprintf(stderr, "- conversion: binary -> %d bits\n", numbits);
            } else {
                fprintf(stderr, "- conversion: binary -> ascii\n");
            }
            outsz = buffsz >> 3;
        } else {            // -b
            if(numbits) {
                fprintf(stderr, "- conversion: %d bits --> binary\n", numbits);
                outsz = buffsz * numbits;
            } else {
                fprintf(stderr, "- conversion: ascii -> binary\n");
                outsz = buffsz * 8;
            }
        }
        out = malloc(outsz);
        if(!out) std_err();
        first = 0;
    }

    if(convers > 0) {               // INPUT
        len = bin2byte(in, len, out, 8);
    } else {
        if(numbits) {
            len = num2bin(in, len, out, numbits, hexdump);
        } else {
            len = byte2bin(in, len, out, 8);
        }
    }
    if(len > outsz) {
        fprintf(stderr, "\nError: the output string is bigger (%d) than the allocated memory (%d)\n", len, outsz);
        exit(1);
    }

    if(numbits && (convers > 0)) {  // OUTPUT
        shownum(out, len, numbits, fdo, hexdump);
    } else if(hexdump && (convers > 0)) {
        show_dump(out, len, fdo);
    } else {
        fwrite(out, 1, len, fdo);
    }
}



int check_convers(u8 *in, int insz) {
    int     i;

    for(i = 0; i < insz; i++) {
        if((in[i] != '1') && (in[i] != '0') && (in[i] != '\t') && (in[i] != ' ') && (in[i] != '\r') && (in[i] != '\n')) return(-1);
    }
    return(1);
}



int bin2byte(u8 *in, int insz, u8 *out, int outbits) {
    static int  i   = 0;    // static is needed for handling various inputs longer than BUFFSZ
    static u8   c   = 0;
    u8          *o;

    for(o = out; insz; o++) {
        while(i < outbits) {
            if(!insz) return(o - out);
            if(*in == '1') {
                c = (c << 1) | 1;
                i++;
            } else if(*in == '0') {
                c <<= 1;
                i++;
            }
            in++;
            insz--;
        }
        *o = c;
        c = 0;
        i = 0;
    }
    return(o - out);
}



int num2bin(u8 *in, int insz, u8 *out, int numbits, int hexdump) {
    static const u8 hexstr[] = "0123456789abcdef";
    static u64      num = 0;    // valid also for files over BUFFSZ, but I have not implemented it because is impossible to know it
    int     i,                  // for example I could have an input of exactly BUFFSZ bytes and I can't know if there are other numbers after it
            dohex;
    u8      c,
            *o,
            *limit;

    limit = in + insz;
    for(o = out; in < limit; o += numbits) {
        dohex = hexdump;
        for(i = 0; in < limit;) {
            c = tolower(*in++);
            if(!(((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')) || (c == 'x'))) {
                if(!i) continue;
                break;
            }
            if(c == 'x') {
                dohex = 1; // 0x1234 the first 0 doesn't matter
                continue;
            }
            if(dohex) {
                num = (num * 16) + ((u8 *)strchr(hexstr, c) - (u8 *)hexstr);
            } else {
                if((c >= 'a') && (c <= 'f')) continue;
                num = (num * 10) + (c - '0');
            }
            i++;
        }
        if(!i) break;

        for(i = numbits - 1; i >= 0; i--) {
            o[i] = '0' + (num & 1);
            num >>= 1;
        }
        num = 0;
    }
    return(o - out);
}



int byte2bin(u8 *in, int insz, u8 *out, int inbits) {
    int     i;
    u8      c,
            *o,
            *limit;

    limit = in + insz;
    for(o = out; in < limit; o += inbits) {
        c = *in++;
        for(i = inbits - 1; i >= 0; i--) {
            o[i] = '0' + (c & 1);
            c >>= 1;
        }
    }
    return(o - out);
}



FILE *fopen_input(u8 *fname) {
    FILE    *fd;

    if(!strcmp(fname, "-")) return(stdin);
    fprintf(stderr, "- open %s for reading\n", fname);
    fd = fopen(fname, "rb");
    if(!fd) std_err();
    return(fd);
}



FILE *fopen_output(u8 *fname, int append) {
    FILE    *fd;

    if(!strcmp(fname, "-")) return(stdout);
    fprintf(stderr, "- open %s for writing\n", fname);
    fd = fopen(fname, append ? "ab" : "wb");
    if(!fd) std_err();
    return(fd);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


