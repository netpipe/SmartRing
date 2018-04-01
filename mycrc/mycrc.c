/*
    Copyright 2004-2005 Luigi Auriemma

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
#include "pck16.h"
#include "bsd16.h"
#include "crc16.h"
#include "crc16_2.h"
#include "fcs16.h"
#include "sum.h"
#include "crc32.h"
#include "crc32_2.h"
#include "sha1.h"
#include "md2.h"
#include "md4.h"
#include "md5.h"

#ifndef WIN32
    #define stricmp strcasecmp
#endif



#define VER     "0.3"
#define BUFFSZ  32768



typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;



void show_help(void);
unsigned int posix_crc(unsigned int crc, int size);
void show_hex(char *title, u_char *data, int len);
void std_err(void);



int main(int argc, char *argv[]) {
    FILE            *fd;
    md5_context     md5t;
    sha1_context    sha1t;
    MD2_CTX         md2t;
    MD4_CTX         md4t;
    u_char          md2h[16],
                    md4h[16],
                    md5h[16],
                    sha1h[20],
                    sum8       = 0,
                    xor8_f     = 0xff,
                    xor8_0     = 0,
                    *buff,
                    *p;
    u_short         pck_16     = 0,
                    bsd_16_f   = 0xffff,
                    bsd_16_0   = 0,
                    crc_16_f   = 0xffff,
                    crc_16_0   = 0,
                    arc_f      = 0xffff,
                    arc_0      = 0,
                    fcs_16_f   = 0xffff,
                    fcs_16_0   = 0,
                    sum_16     = 0;
    u_int          fcs_32_f   = 0xffffffff,
                    fcs_32_0   = 0,
                    alt_32_f   = 0xffffffff,
                    alt_32_0   = 0,
                    sum_32     = 0,
                    intotal    = 0;
    int             len,
                    i;
    u_char          in = 0,
                    crc8op     = 0,
                    crc16op    = 0,
                    crc32op    = 0,
                    md2op      = 0,
                    md4op      = 0,
                    md5op      = 0,
                    sha1op     = 0;


    setbuf(stdout, NULL);

    fputs("\n"
        "MyCRC "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s [options] [file]\n"
            "\n"
            "Options:\n"
            "-s       stdin input stream (you don't need to specify the file)\n"
            "-crc8    CRC8 checksums\n"
            "-crc16   CRC16 checksums\n"
            "-crc32   CRC32 checksums\n"
            "-md2     MD2 checksum\n"
            "-md4     MD4 checksum\n"
            "-md5     MD5 checksum\n"
            "-sha1    SHA1 checksum\n"
            "-h       shows a quick help\n"
            "default  all checksums activated and file input\n"
            "\n", argv[0]);
        exit(1);
    }

    /* OPTIONS */
    for(i = 1; i < argc; i++) {
        if(!stricmp(argv[i], "-s"))            in = 1;
          else if(!stricmp(argv[i], "-crc8"))  crc8op = 1;
          else if(!stricmp(argv[i], "-crc16")) crc16op = 1;
          else if(!stricmp(argv[i], "-crc32")) crc32op = 1;
          else if(!stricmp(argv[i], "-md2"))   md2op = 1;
          else if(!stricmp(argv[i], "-md4"))   md4op = 1;
          else if(!stricmp(argv[i], "-md5"))   md5op = 1;
          else if(!stricmp(argv[i], "-sha1"))  sha1op = 1;
          else if(!stricmp(argv[i], "-h"))     show_help();
    }
    argc--;

    /* default: all checksums activated */
    if(argc == 1) {
        crc8op  =
        crc16op =
        crc32op =
        md2op   =
        md4op   =
        md5op   =
        sha1op  = 1;
    }

    /* open file or use stdin */
    if(!in) {
        printf("- open file   %s\n", argv[argc]);
        fd = fopen(argv[argc], "rb");
        if(!fd) std_err();
    } else {
        fputs("- open stdin\n", stdout);
        fd = stdin;
    }

/* --- */

    /* HASH INIT */
    if(md2op) MD2_Init(&md2t);
    if(md4op) MD4_Init(&md4t);
    if(md5op) md5_starts(&md5t);
    if(sha1op) sha1_starts(&sha1t);

/* --- */

    buff = malloc(BUFFSZ);
    if(!buff) std_err();

    /* FILE READING AND CRC CALCULATION */
    while((len = fread(buff, 1, BUFFSZ, fd))) {
/* CRC8 */  if(crc8op) {
                for(p = buff, i = 0; i < len; i++, p++) {
                    sum8   += *p;
                    xor8_f ^= *p;
                    xor8_0 ^= *p;
                }
            }
/* CRC16 */ if(crc16op) {
                if(!intotal) pck_16 = pck16(buff, len);
                bsd_16_f = bsd16(bsd_16_f, buff, len);
                bsd_16_0 = bsd16(bsd_16_0, buff, len);
                crc_16_f = crc16(crc_16_f, buff, len);
                crc_16_0 = crc16(crc_16_0, buff, len);
                arc_f    = crc16_2(arc_f, buff, len);
                arc_0    = crc16_2(arc_0, buff, len);
                fcs_16_f = fcs16(fcs_16_f, buff, len);
                fcs_16_0 = fcs16(fcs_16_0, buff, len);
                sum_16   = sum16(sum_16, buff, len);
            }
/* CRC32 */ if(crc32op) {
                fcs_32_f = crc32(fcs_32_f, buff, len);
                fcs_32_0 = crc32(fcs_32_0, buff, len);
                alt_32_f = crc32_2(alt_32_f, buff, len);
                alt_32_0 = crc32_2(alt_32_0, buff, len);
                sum_32   = sum32(sum_32, buff, len);
            }
/* MD2 */   if(md2op)  MD2_Update(&md2t, buff, len);
/* MD4 */   if(md4op)  MD4_Update(&md4t, buff, len);
/* MD5 */   if(md5op)  md5_update(&md5t, buff, len);
/* SHA1 */  if(sha1op) sha1_update(&sha1t, buff, len);

        intotal += len;
    }
    fclose(fd);

/* --- */

    /* FINAL STAGE */
    if(md2op)  MD2_Final(md2h, &md2t);
    if(md4op)  MD4_Final(md4h, &md4t);
    if(md5op)  md5_finish(&md5t, md5h);
    if(sha1op) sha1_finish(&sha1t, sha1h);

/* --- */

    /* VISUALIZATION */
    printf("\n"
        "Filesize:   %u   (0x%08x)\n"
        "\n", intotal, intotal);

    if(crc8op || crc16op || crc32op) {
//                     0xf,0        0xf,~      0,0          0,~
        fputs("        1          2          3          4          Comment\n", stdout);
    }

    if(crc8op) {
        printf("CRC8    %02x         %02x        .%02x         %02x         XOR8\n",
            xor8_f,
            xor8_f ^ 0xff,
            xor8_0,
            xor8_0 ^ 0xff);
        printf("        %02x                                          sum8\n", sum8);
    }
    if(crc16op) {
        printf("CRC16                         %04x      .%04x       network headers < %d\n",
            pck_16 ^ 0xffff,    // because returns with ~
            pck_16,
            BUFFSZ);
        printf("        %04x       %04x      .%04x       %04x       BSD\n",
            bsd_16_f,
            bsd_16_f ^ 0xffff,
            bsd_16_0,
            bsd_16_0 ^ 0xffff);
        printf("        %04x       %04x      .%04x       %04x       the most diffused (0xC0C1)\n",
            crc_16_f,
            crc_16_f ^ 0xffff,
            crc_16_0,
            crc_16_0 ^ 0xffff);
        printf("        %04x       %04x      .%04x       %04x       rare, AC3, ARC (0x8005)\n",
            arc_f,
            arc_f ^ 0xffff,
            arc_0,
            arc_0 ^ 0xffff);
        printf("        %04x      .%04x       %04x       %04x       FCS16 (0x1189)\n",
            fcs_16_f,
            fcs_16_f ^ 0xffff,
            fcs_16_0,
            fcs_16_0 ^ 0xffff);
        printf("        %04x                                        sum16\n", sum_16);
    }
    if(crc32op) {
        printf("CRC32   %08x  .%08x   %08x   %08x   FCS32, classic (0x77073096)\n",
            fcs_32_f,
            ~fcs_32_f,
            fcs_32_0,
            ~fcs_32_0);
        printf("        %08x   %08x   %08x   %08x   alternative (0x04c11db7)\n",
            alt_32_f,
            ~alt_32_f,
            alt_32_0,
            ~alt_32_0);
        printf("        %08x                                    POSIX (0x04c11db7 + length)\n",
            posix_crc(alt_32_0, intotal));
        printf("        %08x                                    sum32\n", sum_32);
    }
    if(md2op)  show_hex("MD2     ", md2h, sizeof(md2h));
    if(md4op)  show_hex("MD4     ", md4h, sizeof(md4h));
    if(md5op)  show_hex("MD5     ", md5h, sizeof(md5h));
    if(sha1op) show_hex("SHA1    ", sha1h, sizeof(sha1h));

    fputc('\n', stdout);
    return(0);
}



void show_help(void) {
    fputs("\n"
        "The main purpose of this tool is to give a help to understand what type of CRC\n"
        "algorithm has been used in a program or in a file.\n"
        "\n"
        "Usually it generates 4 checksums:\n"
        " 1 = initialized with 0xffffffff, finished with 0\n"
        " 2 = initialized with 0xffffffff, finished with 0xffffffff (~)\n"
        " 3 = initialized with 0,          finished with 0\n"
        " 4 = initialized with 0,          finished with 0xffffffff (~)\n"
        "\n"
        "A dot is located at the left of the standard initialized and finished checksum.\n"
        "\n"
        "Let me know if you have suggestions about adding other checksums to control.\n"
        "\n", stdout);
    exit(0);
}



unsigned int posix_crc(unsigned int crc, int size) {
  const static unsigned int  crctable[] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
        0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
        0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
        0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
        0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
        0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
        0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
        0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
        0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
        0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
        0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
        0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
        0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
        0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
        0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
        0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
        0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
        0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
        0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
        0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
        0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
        0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4 };

    for(; size; size >>= 8) {
        crc = crctable[(size ^ (crc >> 24)) & 0xff] ^ (crc << 8);
    }
    return(~crc);
}



void show_hex(char *title, u_char *data, int len) {
    const static char   *hex = "0123456789abcdef";

    fputs(title, stdout);
    while(len--) {
        fputc(hex[*data >> 4], stdout);
        fputc(hex[*data & 15], stdout);
        data++;
    }
    fputc('\n', stdout);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}

