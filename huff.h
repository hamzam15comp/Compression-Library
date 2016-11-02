#define MAXNODES 511
#define CHARS 256

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "bitio.h"

typedef struct nodes {
	struct nodes *C1, *C2;
	int count;
	int symbol;
} huffnode;

typedef struct huffchar {
	int symbol;
	unsigned int count;
	unsigned int path;
	unsigned int bits;
} huffchar;

void storeSymbolCount(unsigned char *in, huffchar *s, unsigned int size);
void GenerateHuffTree(huffchar *s, bitstream *strm);
void StoreHuffTree(huffnode *node, huffchar *s, bitstream *strm, int path, int bits);
huffnode* ReadHuffTree(huffnode *node, bitstream *strm, int *num);
void HuffUncompress(unsigned char *in, unsigned char *out, unsigned int insize);
unsigned int HuffCompress(unsigned char *in, unsigned char *out, int insize);
void printhuffchar(huffchar *s);
