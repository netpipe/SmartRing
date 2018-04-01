
unsigned short pck16(unsigned char *data, int len) {
    unsigned int   sum    = 0;
    int             i      = len >> 1,
                    endian = 1; // big endian
    unsigned short  crc,
                    *p     = (unsigned short *)data;

    if(*(char *)&endian) endian = 0;
    while(i--) sum += *p++;
    if(len & 1) sum += (*p & (0xff << (endian << 3)));
    crc = sum = (sum >> 16) + (sum & 0xffff);
    if(sum >>= 16) crc += sum;
    if(!endian) crc = (crc >> 8) | (crc << 8);
    return(~crc);
}

