#include "lz77.h"

token match(node **tree, unsigned char *win, int la, int la_size) {
	token t;
	info i;

	/* find the longest match */
	i = find(tree, win, la, la_size);

	/* create the token */
	t.off = i.off;
	t.len = i.len;
	t.next = win[la + i.len];

	return t;
}

void writecode(token t, bitstream *strm, int la, int sb) {
	writeBits(strm, t.off, bitof(sb));
	writeBits(strm, t.len, bitof(la));
	writeBits(strm, t.next, 8);
}
token readcode(bitstream *strm, int la, int sb) {
	token t;
	int ret = 0, c = 0;

	ret += readBits(strm, &t.off, sizeof(t.off), bitof(sb));
	ret += readBits(strm, &t.len, sizeof(t.len), bitof(la));
	c += readBits(strm, &t.next, sizeof(t.next), sizeof(char));
	ret += c;

	/* check for ERR */
	if(ret < (bitof(sb) + bitof(la) + sizeof(char))) {
		if(c == sizeof(char)) {
			perror("Error reading bits.\n");
			exit(1);
		}
		else
			t.off = -1;
	}
	return t;
}

unsigned int LZCompress(unsigned char* in, unsigned char *out, unsigned int insize, int la, int sb) {
	int i;
	node *tree;
	creattree(&tree);
	token t;
	bitstream stream;
	initBitStream(&stream, out);
	unsigned char *win = in;
	int lasize, sbsize = 0;
	int size = insize;
	int sbindex = 0, laindex = 0;
	int LA_SIZE, SB_SIZE;

	/* Innitialize window sizes */
	LA_SIZE = (la == -1)? DEFAULT_LA_SIZE : la;
	SB_SIZE = (sb == -1)? DEFAULT_SB_SIZE : sb;

	/* Write default sizes to buffer */
	writeBits(&stream, SB_SIZE, 16);
	writeBits(&stream, LA_SIZE, 16);

	/* set lookahead size */
	lasize = (size > LA_SIZE) ? LA_SIZE : size;

	while(size > 0) {

		/* find the longest match in the tree*/
		t = match(&tree, win, laindex, lasize);

		/* write the token in the bitstream */
		writecode(t, &stream, LA_SIZE, SB_SIZE);

		/* read next characters as matched earlier */
		for(i = 0; i < t.len + 1; i++) {

			/* if search buffer's length is max, the oldest node is removed from the tree */
			if(sbsize == SB_SIZE) {
				delete(&tree, win, sbindex, LA_SIZE);
				sbindex++;
			}
			else
				sbsize++;

			/* insert a new node in the tree */
			insert(&tree, win, laindex, lasize, SB_SIZE);
			laindex++;

			size--;

			/* If we hit EOF before filling lookahead */
			lasize = (size > LA_SIZE) ? LA_SIZE : size;
		}
	}

	long outsize = (int) (stream.currbyte - out);
	outsize++;

	return outsize;
}

void LZUncompress(unsigned char* in, unsigned char *out, unsigned int insize) {
	token t;
	bitstream strm;
	initBitStream(&strm, in);
	int read = 0, off;
	unsigned char *buffer = out;
	int SB_SIZE, LA_SIZE;

	/* read header */
	readBits(&strm, &SB_SIZE, sizeof(SB_SIZE), 16);
	readBits(&strm, &LA_SIZE, sizeof(LA_SIZE), 16);

	while(read < insize) {

		/* read the code from bitstream*/
		t = readcode(&strm, LA_SIZE, SB_SIZE);

		/* reconstruct the original byte*/
		while(t.len > 0) {
			off = read - t.off;
			buffer[read] = buffer[off];
			read++;
			t.len--;
		}
		buffer[read] = t.next;
		read++;
	}
}
