#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tree_lz.h"
#include "bitio.h"

typedef struct token{
	int off, len;
	char next;
}token;

/* Optimised Look Ahead and Search buffer sizes */
#define DEFAULT_LA_SIZE 15
#define DEFAULT_SB_SIZE 4095

token match(node **tree, unsigned char *win, int la, int la_size);
void writecode(token t, bitstream *strm, int la, int sb);
token readcode(bitstream *strm, int la, int sb);
void LZUncompress(unsigned char* in, unsigned char *out, unsigned int insize);
unsigned int LZCompress(unsigned char* in, unsigned char *out, unsigned int insize, int la, int sb);


