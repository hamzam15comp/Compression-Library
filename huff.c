#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

#define MAXNODES 512
#define CHARS 256

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

void initBitStream(bitstream *strm, unsigned char *in) {
	strm->currbyte = in;
	strm->currbit = 0;
}

long GetFileSize( FILE *f ) {
	long pos, size;

	pos = ftell( f );
	fseek( f, 0, SEEK_END );
	size = ftell( f );
	fseek( f, pos, SEEK_SET );

	return size;
}


void printhuffchar(huffchar *s) {
	int i;
	for(i = 0; i < CHARS; i++) {
		if(s[i].count > 0)
			printf("%c  %d  %d  %d\n", s[i].symbol, s[i].count, s[i].path, s[i].bits);
	}
}

unsigned int readBit(bitstream *strm) {
	unsigned int mask = 0, pos, i;
	unsigned char *buffer, bit;
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
	unsigned int mask = 0, pos;
	unsigned char *buffer, bit, i;
	buffer = strm->currbyte;
	pos = strm->currbit;

	i = (buffer[0] << pos) | (buffer[1] >> (8-pos));
	buffer++;

	strm->currbyte = buffer;

	return i;
}

void writeBits(bitstream *strm, unsigned int i, unsigned int bits) {
	unsigned int pos, j = 0, mask = 0;
	unsigned char *buffer, bit = 0;
	buffer = strm->currbyte;
	pos = strm->currbit;
	
	mask = 1 << (bits-1);
	
	while(j < bits) {
		if (i & mask) {
			//printf("1");
			bit = 1 << (7-pos);
		}
		else {
			//printf("0"); 
			bit = 0;
		}
		*buffer = (*buffer & (~(1<<(7-pos))) | bit);
		i = i << 1;
		pos = (pos+1) & 7;
		if(!pos)
			++buffer;
		j++;
	}
	
	strm->currbyte = buffer;
	strm->currbit = pos;
}

void storeSymbolCount(unsigned char *in, huffchar *s, unsigned int size) {
	int i, ch;
	for(i = 0; i < CHARS; i++) {
		s[i].symbol = i;
		s[i].count = 0;
		s[i].path = 0;
		s[i].bits = 0;
	}
	for(i = 0; i < size; i++) {
		ch = *in;
		s[ch].count++;
		in++;
	}
}

huffnode* GenerateHuffTree(huffchar *s) {
	huffnode node[MAXNODES];
	int k, num = 0;
	for(k = 0; k < CHARS; k++) {
		if(s[k].count > 0) {
			node[num].symbol = s[k].symbol;
			node[num].count = s[k].count;
			node[num].C1 = node[num].C2 = NULL;
			num++;
		}
	}
	
	/* Look for the Lowest Count Nodes */
	huffnode *n1, *n2, *root;
	root = NULL;
	int total = num;
	huffnode *n;
	n = (huffnode*) malloc (sizeof(huffnode));
	n->count = INT_MAX;
	while(total > 1) {
		n1 = n2 = n;
		for(k = 0; k < num; k++) {
			if(node[k].count > 0) {
				if (node[k].count <= n1->count) {
					n2 = n1;
					n1 = &node[k];
				}
				else if (node[k].count <= n2->count) 
						n2 = &node[k];
			}	
		}
		/* Join the nodes */
		total--;
		root = &node[num];
		root->C1 = n1;
		root->C2 = n2;
		root->symbol = -1;
		root->count = n1->count + n2->count;
		n1->count = n2->count = 0;		
		num++;
	}		

	return root;
}
		

void StoreHuffTree(huffnode *node, huffchar *s, bitstream *strm, int path, int bits) {
	int k;
	/* If Leaf Node*/
	if(node->symbol >= 0) {
		writeBits(strm, 1, 1);
		writeBits(strm, node->symbol, 8);
		for(k = 0; k < CHARS; k++)
			if(node->symbol == s[k].symbol)
				break;
		printf("%c ", node->symbol);
		s[k].path = path;
		s[k].bits = bits;
		return;
	}
	else
		writeBits(strm, 0, 1);
	/* Folowing LNR order */
	StoreHuffTree(node->C1, s, strm, (path<<1) + 0, bits + 1);
	StoreHuffTree(node->C2, s, strm, (path<<1) + 1, bits + 1);
}

huffnode* ReadHuffTree(huffnode *node, bitstream *strm, int *num) {

	huffnode *currnode = &node[*num];
	(*num)++;
	currnode->symbol = -1;
	currnode->C1 = currnode->C2 = (huffnode*) 0;
	if(readBit(strm)) {
		currnode->symbol = (char) readByte(strm);
		printf("%c ", currnode->symbol);
		return currnode;
	}
	currnode->C1 = ReadHuffTree(node, strm, num);
	currnode->C2 = ReadHuffTree(node, strm, num);
	return currnode;
}

void HuffUncompress(unsigned char *in, unsigned char *out, unsigned int insize) {
	huffnode node[MAXNODES], *root, *n;
	bitstream strm;
	initBitStream(&strm, in);
	int num = 0, k;
	root = ReadHuffTree(node, &strm, &num);
	unsigned char *buf;
	buf = out;	
	for(k = 0; k < insize; k++) {
		n = root;
	
		while(n->symbol < 0) {
			printf("%d ", n->symbol);
			if(readBit(&strm) == 1) {
				n = n->C2;
			}
			else
				n = n->C1;
		}
		*buf ++ = (unsigned char) n->symbol;
		printf("%c ", n->symbol);
		
	}
}

unsigned int HuffCompress(unsigned char *in, unsigned char *out, int insize) {
	bitstream strm;
	huffchar sym[CHARS];
	initBitStream(&strm, out);
	storeSymbolCount(in, sym, insize);
	huffnode *root = GenerateHuffTree(sym);
	StoreHuffTree(root, sym, &strm, 0, 0);
	int k = 0, symbol;
	char ch = '#';
	printf("\n\n    %d    \n\n", sym[ch].path);
	for(k = 0; k <= insize; k++) {
		symbol = in[k];
		printf("%c%d ", symbol, sym[symbol].path);
		writeBits(&strm, sym[symbol].path, sym[symbol].bits);
	}
	long outsize = (int) (strm.currbyte - out);
	outsize++;
	printf("%d----->%ld\n", insize, outsize);
	return outsize;
}

int main( int argc, char *argv[]) {
	FILE *f;
	unsigned char *in, *out, command;
	unsigned int  insize, outsize = 0;
	char *inname, *outname;
	int orig;
	if( argc < 4 )
		return 1;

	command = argv[1][0];
	inname  = argv[2];
	outname = argv[3];
		
	f = fopen( inname, "rb" );
	if(!f) {
		printf( "Unable to open input file \"%s\".\n", inname);
		return 0;
	}
	insize = GetFileSize(f);
	in = (unsigned char *) malloc( insize );
	if( !in ) {
		printf( "Not enough memory\n");	
		fclose( f );
		return 0;
	}	
	if(command == 'c') {

		fread( in, insize, 1, f );
		fclose(f);

		f = fopen( outname, "wb" );
		if(!f) {
			printf( "Unable to open output file \"%s\".\n", outname );
			free( in );
			return 0;
		}
		outsize = (insize * 2);
		out = malloc(outsize);
		if(!out) {
			printf( "Not enough memory\n" );
			fclose( f );
			free( in );
			return 0;
		}
		outsize = HuffCompress( in, out, insize);
		fwrite(&insize, sizeof(unsigned int), 1, f);
		fwrite(out, outsize, 1, f);
		fclose(f);
		double percent = (double)outsize/insize;
		printf("Compression Successful : %lf%% reduced\n", (1-percent)*100);
	}
	else if(command == 'd') {

		fread( &orig, sizeof(unsigned int), 1, f );
		insize -= 4;
		printf("%d\n", insize);
		fread( in, insize, 1, f );
		fclose(f);

		f = fopen( outname, "wb" );
		if(!f) {
			printf( "Unable to open output file \"%s\".\n", outname );
			free( in );
			return 0;
		}
		outsize = orig;
		out = malloc(outsize);
		if(!out) {
			printf( "Not enough memory\n" );
			fclose( f );
			free( in );
			return 0;
		}
		printf("%d\n", orig); 
		HuffUncompress( in, out, outsize);
		fwrite(out, outsize, 1, f);
		fclose(f);
		printf("\nDecompression Successful\n");
	}

	free( in );
	free( out );
	
	return 0;

}



