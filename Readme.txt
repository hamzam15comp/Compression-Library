
COMPRESSION LIBRARY

Name: Hamza Motiwalla
MIS: 111503025
SY BTech Computers

BRIEF DESCRIPTION
=================

Huffman Compression

The basic principle of Huffman Encoding is to find new binary representations for each symbol so that common symbols use few bits per symbol, while
uncommon symbols use more bits per symbol.

The solution to this problem is, in short, to make a histogram of the uncompressed data stream in order
to find how common each symbol is. A binary tree is then created by succesively joining the two least weighing nodes until there is only a single node left - the root node(i.e. bottom-up).

This tree serves two purposes:
1. The coder uses the tree to find the optimal representations for each symbol.
2. The decoder uses the tree to uniquely identify the start and stop of each code in the compressed
data stream: by traversing the tree from top to bottom while reading the compressed data bits,
selecting branches based on each individual bit in the data stream, the decoder knows that a
complete code has been read once a leaf node is reached.

The encoder is a 2 pass encoder. The first pass generates a huffman tree and the second pass encodes the data. The decoder is one pass and uses a huffman code table at the beginning of the compressed file to decode the data.




LZ77
Lempel, Ziv Encoding and Decoding

LZ77 is a lossless data compression algorithm published by Abraham Lempel 
and Jacob Ziv in 1977. It is a dictionary coder and maintains a sliding window 
during compression.

The sliding window is divided in two parts: 
- Search-Buffer (dictionary - encoded data)
- Lookahead (uncompressed data).

The idea behind the Lempel-Ziv compression algorithm replaces sequences of bytes with references to previous occurrences of the same sequences.
For simplicity, the algorithm can be thought of in terms of string matching. For instance, in written text
certain strings tend to occur quite often, and can be represented by pointers to earlier occurrences of the
string in the text. The idea is, of course, that pointers or references to strings are shorter than the strings
themselves.

LZ77 algorithms achieve compression by addressing byte sequences from former contents 
instead of the original data. All data will be coded in the same form (called token):
- Address to already coded contents; 
- Sequence length; 
- First deviating symbol.



USAGE
=====
Usage : ./project [OPTIONS]

Options :

  -c			Set compression mode
  -d			Set decompression mode
  -i <filename>		Filename input (source).
  -o <filename>		File name output (destination).
  -l <value>		Set Look Ahead length, must specify a positive value.
			Min value is 3 and the max value is 127
  -l <value>		Set Search Buffer length, must specify a positive value.
			Min value is 127 and the max value is 65535
  -h			Help Options and description.

The *lookahead* and *searchbuffer* sizes are optional. If the two options are not set, default values are used.


