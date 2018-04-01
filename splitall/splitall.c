/*
    Copyright 2004,2005,2006 Luigi Auriemma

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



#define VER    "0.7.1"
#define BUFFSZ    102400
#define EXT1    ".txt"
#define EXT2    ".000"
#define EXT3    ".0"
#define EXT4    ".ssp"
#define EXT6    ".bat"



void std_err(void);
void len_err(void);
void check_err(FILE *err, char *filename);
void split01(char *filename, unsigned int size);
void split2(char *filename, unsigned int size);
void split3(char *filename, unsigned int size);
void split4(char *filename, unsigned int size);
void split5(char *filename, unsigned int size);
void split6(char *filename, unsigned int size);
void desplit01(char *filename);
void desplit2(char *filename);
void desplit3(char *filename);
void desplit4(char *filename);
void desplit5(char *filename);
void desplit6(char *filename);
char *path_remove(char *filename);
void next_file_name(void);
void help(void);
void esempi(void);



struct splitfile {
    unsigned char    head[7];
    unsigned char    current[3];
    unsigned char    total[3];
    unsigned char    ext[3];
} splitfile;



char    *outfile_end,
    *outfile_mid;
int    type = 0;



int main(int argc, char *argv[]) {
    int        i,
            split = 0,
            desplit = 0;
    unsigned int    splitsz = 0;


    setbuf(stdout, NULL);

    printf("\n"
        "SplitALL %s\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", VER);


    if(argc < 2) {
        printf("\nUsage: %s <file>\n"
            "\nOptions:\n"
            "-m NUM    metodo da utilizzare (-h e -e per conoscerli) [default 0]\n"
            "-s SIZE   split: frammentazione del file (specificare grandezza frammenti)\n"
            "-d        desplit: ricostruzione del file originale (default)\n"
            "-h        help\n"
            "-e        esempi di utilizzo\n"
            "\n", argv[0]);
            exit(1);
    }


    if(!memcmp(argv[1], "-h", 2)) help();
    if(!memcmp(argv[1], "-e", 2)) esempi();


    argc--;
    for(i = 1; i < argc; i++) {
        switch(argv[i][1]) {
            case 'm': if(argv[++i]) type = atoi(argv[i]); break;
            case 's': {
                split = 1;
                if(argv[++i]) splitsz = atoi(argv[i]);
                }; break;
            case 'd': desplit = 1; break;
            default: {
                printf("\nError: Argomento non valido (%s)\n", argv[i]);
                exit(1);
            }
        }
    }


    if((type > 6) || (type < 0)) {
        printf("\nError: Argomento errato. Usa 0, 1, 2, 3, 4, 5, 6\n");
        exit(1);
    }

    if(!split && !desplit) desplit = 1;

    /* SPLIT */
    if(split) {
        if(splitsz <= 0) {
            printf("\nError: la grandezza degli split deve essere maggiore di 0\n");
            exit(1);
        }
        switch(type) {
            case 0:
            case 1: split01(argv[argc], splitsz); break;
            case 2: split2(argv[argc], splitsz); break;
            case 3: split3(argv[argc], splitsz); break;
            case 4: split4(argv[argc], splitsz); break;
            case 5: split5(argv[argc], splitsz); break;
            case 6: split6(argv[argc], splitsz); break;
        }
    }

    /* DESPLIT */
    if(desplit) {
        switch(type) {
            case 0:
            case 1: desplit01(argv[argc]); break;
            case 2: desplit2(argv[argc]); break;
            case 3: desplit3(argv[argc]); break;
            case 4: desplit4(argv[argc]); break;
            case 5: desplit5(argv[argc]); break;
            case 6: desplit6(argv[argc]); break;
        }
    }

    printf("\n");

    return(0);
}



void std_err(void) {
    perror("\nError");
    exit(1);
}


void len_err(void) {
    printf("\nError: problemi nella lettura o nella scrittura del file\n");
    exit(1);
}



void check_err(FILE *err, char *filename) {
    if(!err) {
        printf("\nFile error: %s", filename);
        perror("\nError");
        exit(1);
    }
}



void split01(char *filename, unsigned int size) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err;
    unsigned char    *buff;
    char        name[30];


    in = fopen(filename, "rb");
    check_err(in, filename);

    buff = malloc(size);
    if(!buff) std_err();

    name[0] = 'x';
    outfile_mid = name + 1;
    outfile_end = outfile_mid + 2;
    memset (outfile_mid, 0, 30);
    outfile_mid[0] = 'a';
    outfile_mid[1] = 'a' - 1;

    while(!feof(in)) {
        next_file_name();
        if(type == 1) {
            if((i % 650) == 0) memcpy(outfile_end, EXT1, 4);
        }

        out = fopen(name, "wb");
        check_err(out, name);

        len = fread(buff, 1, size, in);
        err = fwrite(buff, 1, len, out);
        if(err < len) len_err();
        fclose(out);
        i++;
        printf(".");
    }

    fclose(in);
    printf("\n%d files creati\n", i);
}



void split2(char *filename, unsigned int size) {
    FILE    *in,
        *out;
    int    i = 0,
        len,
        total,
        strfile = strlen(filename) - 4,
        err;
    unsigned char    *buff;
    char    name[BUFSIZ];


    in = fopen(filename, "rb");
    check_err(in, filename);

    err = fseek(in, 0, SEEK_END);
    if(err < 0) std_err();

    total = ftell(in);
    rewind(in);

    buff = malloc(size);
    if(!buff) std_err();

    size -= sizeof(splitfile);

    total /= size;
    if(total > 999) {
        printf("\nError: I frammenti sono superiori a 999\n");
        exit(1);
    }

    memcpy(name, filename, strfile);
    name[strfile] = 0;
    filename[strfile] = 0;

    while(!feof(in)) {
        sprintf(buff, "SPLITIT%03d%03d%s", i, total + 1, filename + strfile + 1);

        sprintf(name, "%s.%03d", path_remove(filename), i);

        out = fopen(name, "wb");
        check_err(out, name);

        len = fread(buff + sizeof(splitfile), 1, size, in);
        len += sizeof(splitfile);
        err = fwrite(buff, 1, len, out);
        if(err < len) len_err();

        fclose(out);
        i++;
        printf(".");
    }

    fclose(in);
    printf("\n%d files creati\n", i);
}



void split3(char *filename, unsigned int size) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err,
            strfile = strlen(filename);
    unsigned char    *buff;
    char        name[BUFSIZ];


    in = fopen(filename, "rb");
    check_err(in, filename);

    buff = malloc(size);
    if(!buff) std_err();

    filename[strfile - 4] = filename[strfile - 3];
    filename[strfile - 3] = filename[strfile - 2];
    filename[strfile - 2] = filename[strfile - 1];
    filename[strfile - 1] = 0;

    filename = path_remove(filename);

    while(!feof(in)) {
        sprintf(name, "%s.%d", filename, i);

        out = fopen(name, "wb");
        check_err(out, name);

        len = fread(buff, 1, size, in);
        err = fwrite(buff, 1, len, out);
        if(err < len) len_err();
        fclose(out);
        i++;
        printf(".");
    }

    fclose(in);
    printf("\n%d files creati\n", i);
}



void split4(char *filename, unsigned int size) {
    printf("\nError: Questo metodo non puo' essere utilizzato in quanto il\n"
        "riassemblatore degli split viene incorporato direttamente nel primo split\n"
        "(40 Kb in piu')\n");
}



void split5(char *filename, unsigned int size) {
    FILE        *in,
            *out;
    int        i = 1,
            j = 0,
            len,
            err;
    unsigned char    *buff;
    char        name[BUFSIZ];


    in = fopen(filename, "rb");
    check_err(in, filename);

    buff = malloc(size);
    if(!buff) std_err();

    filename = path_remove(filename);
    while(!feof(in)) {
        sprintf(name, "%s.%03d", filename, i);

        out = fopen(name, "wb");
        check_err(out, name);

        len = fread(buff, 1, size, in);
        err = fwrite(buff, 1, len, out);
        if(err < len) len_err();
        fclose(out);
        i++;
        j += len;
        printf(".");
    }

    fclose(in);
    printf("\n%d files creati\n", --i);

    /* creazione MXS file */
    sprintf(name, "%s.mxs", filename);
    in = fopen(name, "wb");
    check_err(in, name);

    /* orribile procedura per creazione file MXS, sono troppo pigro 8-) */
    len = sprintf(buff, "MXS%s\xff%d\xff%d\xffSplitall by Bugtest\xff\x61\xff\x4e\xff\x31\xff",
        filename, j, i);
    for(i = 0; i < len; i++) {
        if(buff[i] == 0xff) buff[i] = 0;
    }
    err = fwrite(buff, 1, len, in);
    if(err < len) len_err();
    fclose(in);

    printf("\nFile MXS creato\n");
}



void split6(char *filename, unsigned int size) {
    printf("\nError: Questo metodo al momento non e' implementato in quanto sarebbe\n"
        "scomodo creare un file .bat e poi dover modificare manualmente tutte le\n"
        "estensioni degli split per adattarli alle estensioni utilizzabili nel\n"
        "forum\n\n");
    exit(1);
}



void desplit01(char *filename) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err;
    char        name[30];
    unsigned char    *buff;


    out = fopen(filename, "wb");
    check_err(out, filename);

    buff = malloc(BUFFSZ);
    if(!buff) std_err();

    name[0] = 'x';
    outfile_mid = name + 1;
    outfile_end = outfile_mid + 2;
    memset(outfile_mid, 0, 30);
    outfile_mid[0] = 'a';
    outfile_mid[1] = 'a' - 1;

    while(1) {
        next_file_name();
        if(type == 1) if((i % 650) == 0) memcpy(outfile_end, EXT1, 4);

        in = fopen(name, "rb");
        if(!in) break;

        while(!feof(in)) {
            len = fread(buff, 1, BUFFSZ, in);
            err = fwrite(buff, 1, len, out);
            if(err < len) len_err();
        }
        fclose(in);
        i++;
        printf(".");
    }

    fclose(out);
    printf("\n%d files riassemblati\n", i);
}



void desplit2(char *filename) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err,
            strfile = strlen(filename),
            total;
    char        name[BUFSIZ],
            outname[BUFSIZ];
    unsigned char    *buff;


    in = fopen(filename, "rb");
    check_err(in, filename);

    err = fread(&splitfile, 1, sizeof(splitfile), in);

    memcpy(outname, filename, strfile - 4);
    outname[strfile - 4] = '.';
    memcpy(outname + strfile - 3, splitfile.ext, 3);
    outname[strfile] = 0;
    filename[strfile - 4] = 0;

    total = atoi(splitfile.total);

    out = fopen(path_remove(outname), "wb");
    if(!out) std_err();
    fclose(in);


    buff = malloc(BUFFSZ);
    if(!buff) std_err();

    /* nessun controllo sull'header per avere meno istruzioni */

    for(i = 0; i < total; i++) {
        sprintf(name, "%s.%03d", filename, i);

        in = fopen(name, "rb");
        if(!in) break;

        err = fseek(in, sizeof(splitfile), SEEK_SET);
        if(err < 0) std_err();

        while(!feof(in)) {
            len = fread(buff, 1, BUFFSZ, in);
            err = fwrite(buff, 1, len, out);
            if(err < len) len_err();
        }
        fclose(in);
        printf(".");
    }

    fclose(out);
    if(i < total) printf("\nError: Sono stati riassemblati solo %d files su %d\n", i, total);
        else printf("\n%d files riassemblati\n", i);
}



void desplit3(char *filename) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err,
            strfile = strlen(filename);
    char        name[BUFSIZ],
            outname[BUFSIZ];
    unsigned char    *buff;


    memcpy(outname, filename, strfile - 5);
    outname[strfile - 5] = '.';
    memcpy(outname + strfile - 4, filename + strfile - 5, 3);
    outname[strfile - 1] = 0;
    filename[strfile - 2] = 0;

    out = fopen(path_remove(outname), "wb");
    check_err(out, path_remove(outname));

    buff = malloc(BUFFSZ);
    if(!buff) std_err();

    while(1) {
        sprintf(name, "%s.%d", filename, i);

        in = fopen(name, "rb");
        if(!in) break;

        while(!feof(in)) {
            len = fread(buff, 1, BUFFSZ, in);
            err = fwrite(buff, 1, len, out);
            if(err < len) len_err();
        }
        fclose(in);
        i++;
        printf(".");
    }

    fclose(out);
    printf("\n%d files riassemblati\n", i);
}



void desplit4(char *filename) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err,
            strfile = strlen(filename);
    char        name[BUFSIZ],
            outname[BUFSIZ],
            *stri;
    unsigned char    *buff;


    in = fopen(filename, "rb");
    check_err(in, filename);

    buff = malloc(272);
    if(!buff) std_err();

    err = fseek(in, -272, SEEK_END);
    if(err < 0) std_err();

    err = fread(buff, 1, 272, in);
    buff[err] = 0;
    stri = strrchr(buff, 0x5c) + 1;
    printf("Originale: %s\nFilename:  %s\n", buff, stri);

    memcpy(outname, stri, BUFSIZ);

    out = fopen(outname, "wb");
    check_err(out, outname);
    fclose(in);

    filename[strfile - 7] = 0;

    free(buff);
    buff = malloc(BUFFSZ);
    if(!buff) std_err();


    while(1) {
        if(i == 0) sprintf(name, "%s_%02d.exe", filename, i);
            else sprintf(name, "%s_%02d%s", filename, i, EXT4);

        in = fopen(name, "rb");
        if(!in) break;

        if(i == 0) {
            err = fseek(in, 0xA000, SEEK_SET);
            if(err < 0) std_err();
        }

        while(!feof(in)) {
            len = fread(buff, 1, BUFFSZ, in);
            err = fwrite(buff, 1, len, out);
            if(err < len) len_err();
        }
        fclose(in);

        if(i == 0) {
            err = fseek(out, -272, SEEK_END);
            if(err < 0) std_err();
        }

        i++;
        printf(".");
    }

    fclose(out);
    printf("\n%d files riassemblati\n", i);
}



void desplit5(char *filename) {
    FILE        *in,
            *out;
    int        i = 1,
            len,
            err,
            strfile = strlen(filename);
    char        name[BUFSIZ],
            outname[BUFSIZ];
    unsigned char    *buff;


    memcpy(outname, filename, strfile - 4);
    outname[strfile - 4] = 0;
    out = fopen(path_remove(outname), "wb");
    check_err(out, path_remove(outname));

    buff = malloc(BUFFSZ);
    if(!buff) std_err();


    while(1) {
        sprintf(name, "%s.%03d", outname, i);

        in = fopen(name, "rb");
        if(!in) break;

        while(!feof(in)) {
            len = fread(buff, 1, BUFFSZ, in);
            err = fwrite(buff, 1, len, out);
            if(err < len) std_err();
        }
        fclose(in);
        i++;
        printf(".");
    }

    fclose(out);
    printf("\n%d files riassemblati\n", i - 1);
}



void desplit6(char *filename) {
    FILE        *in,
            *out;
    int        i = 0,
            len,
            err,
            strfile = strlen(filename);
    char        name[BUFSIZ],
            outname[BUFSIZ],
            *ptr;
    unsigned char    *buff;


    memcpy(outname, filename, strfile - 3);
    memcpy(outname + strfile - 3, EXT6, 4);
    outname[strfile + 1] = 0;

    in = fopen(outname, "rb");
    if(!in) {
        printf("\nFile %s non trovato, inserire il nome del file da creare: ", outname);
        err = read(fileno(stdin), outname, BUFSIZ);
        outname[err - 1] = 0;
        ptr = outname;
    } else {
        for(i = 0; i < 4; i++) fgets(outname, sizeof(outname), in);
        ptr = strrchr(outname, '\"');
        if(ptr)    *ptr = 0;
        ptr = strrchr(outname, '\"') + 1;
        printf("Filename: %s\n", ptr);
        fclose(in);
    }

    out = fopen(ptr, "wb");
    check_err(out, ptr);

    buff = malloc(BUFFSZ);
    if(!buff) std_err();

    memcpy(name, filename, strfile - 2);
    name[strfile - 2] = 0;
    outfile_mid = name + strfile - 2;
    outfile_end = outfile_mid + 2;
    memset(outfile_mid, 0, 30);
    outfile_mid[0] = 'a';
    outfile_mid[1] = 'a' - 1;

    i = 0;
    while(1) {
        next_file_name();
        if(type == 1) if((i % 650) == 0) memcpy(outfile_end, EXT1, 4);

        in = fopen(name, "rb");
        if(!in) break;

        while(!feof(in)) {
            len = fread(buff, 1, BUFFSZ, in);
            err = fwrite(buff, 1, len, out);
            if(err < len) len_err();
        }
        fclose(in);
        i++;
        printf(".");
    }

    fclose(out);
    printf("\n%d files riassemblati\n", i);
}



char *path_remove(char *filename) {
    char    *stri;

    stri = strrchr(filename, '\\');            //Win32 path
    if(!stri) stri = strrchr(filename, '/');    //Unix path
    if(!stri) stri = filename - 1;
    return(stri + 1);
}



/* copiato spudoratamente da split.c per Linux */
void next_file_name(void) {
    static unsigned n_digits = 2;
    char *p;

    for(p = outfile_end - 1; *p == 'z'; p--) *p = 'a';
    ++(*p);
    if(*p == 'z' && p == outfile_mid) {
        ++n_digits;
        ++outfile_mid;
        *outfile_end++ = 'a';
        *outfile_end++ = 'a';
    }
}



void help(void) {
    printf("\n---\nHelp:\n\n"
           "-s = split (frammentazione del file)\n"
           "-d = desplit (ricostruzione del file originale)\n"
           "\n"
           "0 = Linux split esempio: xaa\n"
           "1 = SplitALL    esempio: xaa.txt\n"
           "2 = Splitfile   esempio: filename.000 [+ header]\n"
           "3 = Splitit!    esempio: filenameext.0\n"
           "4 = Splitter    esempio: filename_00.exe [.ssp i successivi]\n"
           "5 = MaxSplitter esempio: filename.ext.001 [+ MXS file]\n"
           "6 = Spezzatutto esempio: filename.aa\n"
           "---\n\n");
    exit(1);
}



void esempi(void) {
    printf("\n---\nEsempi:\n\n"
        "splitall -s 102400 file.zip (0 e' il metodo di default)\n"
        "splitall -s 102400 -m 0 file.zip\n"
        "splitall -s 10000 -m 1 \"my file.zip\"\n"
        "splitall -s 99999 -m 2 file_1.zip\n"
        "splitall -s 256000 -m 3 \"file 1 2 3.zip\"\n"
        "splitall -d -m 0 destinazione.zip (SPECIFICARE IL NOME FILE DA OTTENERE!)\n"
        "splitall -d -m 1 destinazione.zip (SPECIFICARE IL NOME FILE DA OTTENERE!)\n"
        "splitall -d -m 2 frammento.000\n"
        "splitall -d -m 3 frammentozip.0\n"
        "splitall -d -m 4 frammento_00.exe\n"
        "splitall -d -m 5 frammento.exe.001\n"
        "splitall -d -m 6 frammento.aa\n"
        "\n"
        "NOTA: In modalita' desplit va' specificato il file da ottenere per i metodi\n"
        "0 ed 1, invece per gli altri metodi bisogna solo specificare il nome del\n"
        "PRIMO frammento disponibile\n"
        "---\n\n");
    exit(1);
}


