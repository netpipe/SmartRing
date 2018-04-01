/*

Rot13 (cifrario di Cesare)
by Luigi Auriemma
e-mail: aluigi@autistici.org
web:    aluigi.org

This program lets you to add or sub bytes to each char in a file
For example if you choose 4 the char 0x40 will be 0x44
Then for reconvert it you must simply specify -0f

LINUX & WIN32 VERSION

*/

#include <stdlib.h>
#include <stdio.h>



#define VER     "0.1.1"
#define BUFFSZ  16384



void std_err(void);



int main(int argc, char *argv[]) {
    FILE        *inz,
                *outz;
    int         chz,
                len,
                i;
    unsigned char   *buff,
                    *ptr;


    setbuf(stdout, NULL);

    fputs("\n"
        "Rot13 (cifrario di Cesare)\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 4) {
        printf("\nUsage: %s <file_in> <file_out> <[+|-]rot_num>\n", argv[0]);
        exit(1);
    }

    inz = fopen(argv[1], "rb");
    outz = fopen(argv[2], "wb");

    if(!inz || !outz) std_err();

    chz = atoi(argv[3]);
    buff = malloc(BUFFSZ);
    if(!buff) std_err();

    while(!feof(inz)) {
        len = fread(buff, 1, BUFFSZ, inz);

        ptr = buff;
        for(i = 0; i < len; i++, ptr++) *ptr += chz;

        if(fwrite(buff, len, 1, outz) != 1) {
            fputs("\nError: write error\n", stdout);
            exit(1);
        }
    }

    fclose(inz);
    fclose(outz);

    fputs("\nDone\n", stdout);

    return(0);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


