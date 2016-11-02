#include "huff.h"

void storeSymbolCount(unsigned char *in, huffchar *s, unsigned int size) {
	int i, ch;
	/* Initialize all Characters/Symbols in the array huffchar *s */
	for(i = 0; i < CHARS; i++) {
		s[i].symbol = i;
		s[i].count = s[i].path = s[i].bits = 0;
	}
	/* Calculating and storing the frequency of each symbol */
	for(i = 0; i < size; i++) {
		ch = *in;
		s[ch].count++;
		in++;
	}
}

void GenerateHuffTree(huffchar *s, bitstream *strm) {
	huffnode node[MAXNODES];
	int k, num = 0;

	/* Initialize all Symbol Nodes with at least one Count */
	for(k = 0; k < CHARS; k++) {
		if(s[k].count > 0) {
			node[num].symbol = s[k].symbol;
			node[num].count = s[k].count;
			node[num].C1 = node[num].C2 = (huffnode*) 0;
			num++;
		}
	}

	huffnode *n1, *n2, *root;
	root = (huffnode*) 0;
	int total = num;

	/* Creating a dummy node for comparisions */
	huffnode *n;
	n = (huffnode*) malloc (sizeof(huffnode));
	n->count = INT_MAX;

	/* Look for the TWO Lowest Count Nodes */
	while(total > 1) {
		n1 = n2 = (huffnode*) 0;

		for(k = 0; k < num; k++) {
			if(node[k].count > 0) {
				if (!n1 || node[k].count <= n1->count) {
					n2 = n1;
					n1 = &node[k];
				}
				else if (!n2 || node[k].count <= n2->count) 
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
	if(root)
		StoreHuffTree(root, s, strm, 0, 0);
	else
		StoreHuffTree(&node[0], s, strm, 0, 0);
	return;
}
		
void StoreHuffTree(huffnode *node, huffchar *s, bitstream *strm, int path, int bits) {
	int k;

	/* If Leaf Node && Symbol exists*/
	if(node->symbol >= 0) {
		/* Write 1 followed by character in the bitstream */
		writeBits(strm, 1, 1);
		writeBits(strm, node->symbol, 8);
		
		/* Store Path in the Symbol array huffchar *s for encoding*/
		for(k = 0; k < CHARS; k++)
			if(node->symbol == s[k].symbol)
				break;
		s[k].path = path;
		s[k].bits = bits;
		return;
	}
	else
		writeBits(strm, 0, 1);

	/* Recursive Storage */
	StoreHuffTree(node->C1, s, strm, (path<<1) + 0, bits + 1);
	StoreHuffTree(node->C2, s, strm, (path<<1) + 1, bits + 1);
}

huffnode* ReadHuffTree(huffnode *node, bitstream *strm, int *num) {	
	huffnode *currnode = &node[*num];
	(*num)++;
	currnode->symbol = -1;
	currnode->C1 = (huffnode*) 0;
	currnode->C2 = (huffnode*) 0;
	/* 1 indicates leaf node and that a symbol follows whereas 0 moves to next child */
	if(readBit(strm)) {
		currnode->symbol = readByte(strm);
		return currnode;
	}

	/* Recursive Reading */
	currnode->C1 = ReadHuffTree(node, strm, num);
	currnode->C2 = ReadHuffTree(node, strm, num);

	return currnode;
}

void HuffUncompress(unsigned char *in, unsigned char *out, unsigned int insize) {
	/* Is there anything to Decompress */
	if(insize < 1) return;

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
			if(readBit(&strm))
				n = n->C2;
			else
				n = n->C1;
		}
		*buf ++ = (unsigned char) n->symbol;
	}
}

unsigned int HuffCompress(unsigned char *in, unsigned char *out, int insize) {
	bitstream strm;
	int k = 0, symbol;
	huffchar sym[CHARS];

	initBitStream(&strm, out);
	storeSymbolCount(in, sym, insize);
	GenerateHuffTree(sym, &strm);

	/* Encode all characters of the in stream and write path and bits to the out stream */
	for(k = 0; k <= insize; k++) {
		symbol = in[k];
		writeBits(&strm, sym[symbol].path, sym[symbol].bits);
	}

	long outsize = (int) (strm.currbyte - out);
	outsize++;
	return outsize;
}

void printhuffchar(huffchar *s) {
	int i;
	for(i = 0; i < CHARS; i++) {
		if(s[i].count > 0)
			printf("%c  %d  %d  %d\n", s[i].symbol, s[i].count, s[i].path, s[i].bits);
	}
}

