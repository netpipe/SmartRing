
unsigned short bsd16(unsigned short crc, unsigned char *data, int len) {
    while(len--) {
        crc = ((crc >> 1) + ((crc & 1) << 15)) + *data;
        data++;
    }
    return(crc);
}

