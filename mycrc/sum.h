
unsigned short sum16(unsigned short crc, unsigned char *data, int len) {
    while(len--) {
        crc += *data;
        data++;
    }
    return(crc);
}



unsigned int sum32(unsigned int crc, unsigned char *data, int len) {
    while(len--) {
        crc += *data;
        data++;
    }
    return(crc);
}

