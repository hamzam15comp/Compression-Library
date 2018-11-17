<html>
	<body>
	<H3>Compression Algorithm Library</H3>
	<h4>Introduction</h4>
		<pre>
The primary objective of this project is to demonstrate the working of compression algorithms.
This library implemented in ANSI C comprises of algorithms:
	-LZ77 
	-Huffman Coding
		</pre>
	<h4> Usage </h4>
		<pre>
Usage: ./project &ltALGORITHM&gt &ltCOMMANDS&gt &ltINPUT&gt &ltOUTPUT&gt
	Algorithms:
		-lz77   		LZ77 Compression
		-huff   		Huffman Compression;	
	Commands:
		-c			Compress/Encode input file to output file.
		-d 	 		Decompress/Decode input file to output file.
	Input/Output Files:
		-i <filename>		Name of input file.
		-o <filename>		Name of output file.
	Optional for LZ77 Compression:
		-l <value>   		Lookahead size (default 15)
		-s <value>   		Search-buffer size (default 4095)
		</pre>
	</body>
</html>
