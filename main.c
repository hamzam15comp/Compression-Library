#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#define MIN_LA 3
#define MAX_LA 127
#define MIN_SB 127
#define MAX_SB 65535

void Help(char *prog) {

	printf( "\nUsage: %s <ALGORITHM> <COMMANDS> <INPUT> <OUTPUT>\n\n", prog );
	printf( "Algorithms:\n" );
	printf( "  -lz77   		LZ77 Compression\n" );
	printf( "  -huff   		Huffman Compression\n" );	
	printf( "Commands:\n" );
	printf( "  -c			Compress/Encode input file to output file.\n" );
	printf( "  -d 	 		Decompress/Decode input file to output file.\n" );
	printf( "Input/Output Files:\n");
	printf( "  -i <filename>		Name of input file.\n");
	printf( "  -o <filename>		Name of output file.\n");
	printf( "Optional for LZ77 Compression:\n");
	printf( "  -l <value>   		Lookahead size (default 15)\n");
	printf( "  -s <value>   		Search-buffer size (default 4095)\n\n");
	exit(1);
}

int main( int argc, char *argv[]) {

	FILE *f;
	unsigned char *in, *out, command;
	unsigned int  insize, outsize = 0;
	char *inname, *outname;
	int orig,huff = 0, lz = 0, la = -1, sb = -1;

	if( argc < 7 ) {
		if (argc > 1) {
			if(strcmp("-h", argv[1]) == 0)
				Help(argv[0]);
			else
				printf("Usage: ./project [OPTIONS]...\nTry './project -h' for more information.\n");
		}
		else
			printf("Usage: ./project [OPTIONS]...\nTry './project -h' for more information.\n");
		return 1;
	}

	/* Reading and Storing all Command Line Arguments */
	command = argv[2][1];
	if(command != 'c' && command != 'd') {
		printf("Usage: ./project [OPTIONS]...\nTry './project -h' for more information.\n");
		return 1;
	}

	if(strcmp("-huff", argv[1]) == 0) huff = 1;
	else if(strcmp("-lz77", argv[1]) == 0) lz = 1;
	else {
		printf("Usage: ./project [OPTIONS]...\nTry './project -h' for more information.\n");
		return 1;
	}

	inname  = argv[4];
	outname = argv[6];

	if(argc > 7) {
		if(argc < 10) {
			if((strcmp("-l", argv[7]) == 0)){
				la = atoi(argv[8]);
				if((la < MIN_LA) || (la > MAX_LA)) {
					printf("Invalid value for LookAhead Buffer\nReverting back to DEFAULT\n");
					la = -1;
				}
			}
			else if(strcmp("-s", argv[7]) == 0){
				sb = atoi(argv[8]);
				if((sb < MIN_SB) || (sb > MAX_SB)) {
					printf("Invalid value for Search Buffer\nReverting back to DEFAULT\n");
					sb = -1;
				}
			}
		}
		else {
			if((strcmp("-l", argv[7]) == 0) && (strcmp("-s", argv[9]) == 0)) {
				la = atoi(argv[8]);
				if((la < MIN_LA) || (la > MAX_LA)) {
					printf("Invalid value for LookAhead Buffer\nReverting back to DEFAULT\n");
					la = -1;
				}
				sb = atoi(argv[10]);
				if((sb < MIN_SB) || (sb > MAX_SB)) {
					printf("Invalid value for Search Buffer\nReverting back to DEFAULT\n");
					sb = -1;
				}
			}
		}
	}

	/*Open file to be compressed or decompressed*/	
	f = fopen(inname, "rb");
	if(!f) {
		printf( "Unable to open input file \"%s\".\n", inname);
		return 1;
	}

	insize = GetFileSize(inname);
	if(insize == 0){
		printf("File Empty\nCompression Terminated\n");
		return 1; 
	}

	in = (unsigned char *) malloc(insize);
	if(!in) {
		printf("Not enough memory\n");	
		fclose(f);
		return 1;
	}

	/*Compression*/	
	if(command == 'c') {
		
		fread(in, insize, 1, f);// Read file into IN buffer
		fclose(f);

		f = fopen(outname, "wb");// Open Out file
		if(!f) {
			printf("Unable to open output file \"%s\".\n", outname);
			free(in);
			return 1;
		}

		outsize = (insize * 2);
		out = malloc(outsize);// Allocate OUT buffer
		if(!out) {
			printf("Not enough memory\n");
			fclose(f);
			free(in);
			return 1;
		}

		if(huff)
		outsize = HuffCompress(in, out, insize);
		if(lz)
		outsize = LZCompress(in, out, insize, la, sb);

		double percent = (double)outsize/insize;
		if(outsize > insize || (percent*100 < 2.0)) {
			printf("Compression Failed \n");
			return 1;
		}
		else {
			//printf("%dbytes----->%ldbytes\n", insize, outsize);
			printf("Compression Successful : %lf%% reduced\n", (1-percent)*100);
			fwrite(&insize, sizeof(unsigned int), 1, f);// Write OUT buffer to file
			fwrite(out, outsize, 1, f);
			fclose(f);
		}
	}

	/*Decompression*/
	if(command == 'd') {

		fread( &orig, sizeof(unsigned int), 1, f );// Read original size of file
		insize -= 4;

		fread(in, insize, 1, f);//Open Compressed file
		fclose(f);

		f = fopen(outname, "wb");// Open outfile
		if(!f) {
			printf("Unable to open output file \"%s\".\n", outname);
			free(in);
			return 1;
		}

		outsize = orig;
		out = malloc(outsize);// Allocate original size buffer
		if(!out) {
			printf("Not enough memory\n");
			fclose(f);
			free(in);
			return 1;
		}

 		if(huff)
		HuffUncompress( in, out, outsize);
		if(lz)
		LZUncompress( in, out, outsize);

		fwrite(out, outsize, 1, f);
		fclose(f);

		printf("Decompression Successful\n");
	}

	free(in);
	free(out);
	
	return 0;

}
