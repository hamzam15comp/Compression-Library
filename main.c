#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>
#include"huff.h"


void Help(char *prog) {

	printf( "\nUsage: %s command algorithm infile outfile\n\n", prog );
	printf( " Commands:\n" );
	printf( "  -c      Compress\n" );
	printf( "  -d      Deompress\n\n" );
	printf( " Algorithms:\n" );
	printf( "  rle     RLE Compression\n" );
	printf( "  lz      LZ77 Compression\n" );
	printf( "  huff    Huffman Compression\n\n" );
	exit(1);
}


int main( int argc, char *argv[]) {

	FILE *f;
	unsigned char *in, *out, command;
	unsigned int  insize, outsize = 0;
	char *inname, *outname;
	int orig,huff = 0, lz = 0, rle = 0;
	if( argc < 5 ) Help(argv[0]);

	command = argv[1][1];
	if(command != 'c' && command != 'd') Help(argv[0]);
	if(strcmp("huff", argv[2]) == 0) huff = 1;
	else if(strcmp("lz", argv[2]) == 0) lz = 1;
	else if(strcmp("rle", argv[2]) == 0) rle = 1;
	else Help(argv[0]);
	inname  = argv[3];
	outname = argv[4];

	/*Open file to be compressed or decompressed*/	
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

	/*Compression*/	
	if(command == 'c') {
		
		fread( in, insize, 1, f );// Read file into IN buffer
		fclose(f);

		f = fopen( outname, "wb" );// Open Out file
		if(!f) {
			printf( "Unable to open output file \"%s\".\n", outname );
			free( in );
			return 0;
		}

		outsize = (insize * 2);
		out = malloc(outsize);// Allocate OUT buffer
		if(!out) {
			printf( "Not enough memory\n" );
			fclose( f );
			free( in );
			return 0;
		}
		if(huff)
		outsize = HuffCompress( in, out, insize);

		fwrite(&insize, sizeof(unsigned int), 1, f);// Write OUT buffer to file
		fwrite(out, outsize, 1, f);
		fclose(f);

		if(outsize > insize) {
			printf("Compression Failed");
			return 1;
		}
		
		double percent = (double)outsize/insize;
		printf("Compression Successful : %lf%% reduced\n", (1-percent)*100);
	
	}

	/*Decompression*/

	else if(command == 'd') {

		fread( &orig, sizeof(unsigned int), 1, f );// Read original size of file
		insize -= 4;
		//printf("%d\n", insize);
		fread( in, insize, 1, f );//Open Compressed file
		fclose(f);

		f = fopen( outname, "wb" );// Open outfile
		if(!f) {
			printf( "Unable to open output file \"%s\".\n", outname );
			free( in );
			return 0;
		}

		outsize = orig;
		out = malloc(outsize);// Allocate original size buffer
		if(!out) {
			printf( "Not enough memory\n" );
			fclose( f );
			free( in );
			return 0;
		}
 		if(huff)
		HuffUncompress( in, out, outsize);

		fwrite(out, outsize, 1, f);
		fclose(f);

		printf("\nDecompression Successful\n");
	}

	free( in );
	free( out );
	
	return 0;

}
