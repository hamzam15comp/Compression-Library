#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef Bit

typedef struct bstrm {
	unsigned char *currbyte;
	unsigned int currbit;
} bitstream;

void initBitStream(bitstream *strm, unsigned char *in);
long GetFileSize(const char *f);
unsigned int readBit(bitstream *strm);
int readBits(bitstream *st, void *val, int valsize, unsigned int bits);
unsigned char readByte(bitstream *strm);
void writeBits(bitstream *strm, unsigned int i, unsigned int bits);
int bitof(int n);

#endif
