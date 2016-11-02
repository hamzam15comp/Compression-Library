#include "bitio.h"

void initBitStream(bitstream *strm, unsigned char *in) {
	strm->currbyte = in;
	strm->currbit = 0;
}

long GetFileSize(const char *filename) {
	struct stat st;
	if(stat(filename, &st) != 0)
		return 0;
    return st.st_size; 
}

unsigned int readBit(bitstream *strm) {
	unsigned int mask = 0, pos, i;
	unsigned char *buffer;
	buffer = strm->currbyte;
	pos = strm->currbit;

	mask = 1 << (7-pos);
	
	if (*buffer & mask)
		i = 1;
	else
		i = 0;
	pos = (pos+1) & 7;
	if(!pos)
		++buffer;

	strm->currbyte = buffer;
	strm->currbit = pos;

	return i;
}

unsigned char readByte(bitstream *strm) {
	unsigned int pos;
	unsigned char *buffer, i = 0, j = 0, k = 0;
	buffer = strm->currbyte;
	pos = strm->currbit;

	j = buffer[0] << pos;
	k = buffer[1] >> (8 - pos);
	i = j | k;

	buffer++;

	strm->currbyte = buffer;

	return i;
}

int readBits(bitstream *st, void *val, int valsize, unsigned int bits) {
	unsigned int i, x = 0;
	unsigned char ch;
	int j = 0, r = 0;

	if(bits == sizeof(char)) {
		ch = (char) readByte(st);
		memset(val, 0, valsize);
		memcpy(val, &ch, valsize);
		return sizeof(char);
	}
	else {
		while(j < bits) {
			r++;
			j++;
			i = readBit(st);
			if(i) {
				x <<= 1;
				x = x | 1;
			}
			else
				x <<= 1;
		}
	}
	memset(val, 0, valsize);
	memcpy(val, &x, valsize);

	return r;
}

void writeBits(bitstream *strm, unsigned int i, unsigned int bits) {
	unsigned int pos, j = 0, mask = 0;
	unsigned char *buffer, bit = 0;
	buffer = strm->currbyte;
	pos = strm->currbit;

	mask = 1 << (bits-1);

	while(j < bits) {
		if (i & mask)
			bit = 1 << (7-pos);
		else
			bit = 0;

		*buffer = ((*buffer & (~(1<<(7-pos)))) | (bit));

		i = i << 1;
		pos = (pos+1) & 7;
		if(!pos)
			++buffer;
		j++;
	}

	strm->currbyte = buffer;
	strm->currbit = pos;
}

int bitof(int n){
	return (int)(ceil(log(n)/log(2)));
}
