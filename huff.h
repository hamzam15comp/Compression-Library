#define MAXNODES 2047
#define CHARS 1024

typedef struct node {
	struct node *C1, *C2;
	int count;
	int symbol;
} huffnode;

typedef struct huffchar {
	int symbol;
	unsigned int count;
	unsigned int path;
	unsigned int bits;
} huffchar;

typedef struct bstrm {
	unsigned char *currbyte;
	unsigned int currbit;
} bitstream;

void initBitStream(bitstream *strm, unsigned char *in);
long GetFileSize( FILE *f );
void printhuffchar(huffchar *s);
unsigned int readBit(bitstream *strm);
unsigned char readByte(bitstream *strm);
void writeBits(bitstream *strm, unsigned int i, unsigned int bits);
void storeSymbolCount(unsigned char *in, huffchar *s, unsigned int size);
huffnode* GenerateHuffTree(huffchar *s);
void StoreHuffTree(huffnode *node, huffchar *s, bitstream *strm, int path, int bits);
huffnode* ReadHuffTree(huffnode *node, bitstream *strm, int *num);
void HuffUncompress(unsigned char *in, unsigned char *out, unsigned int insize);
unsigned int HuffCompress(unsigned char *in, unsigned char *out, int insize);
