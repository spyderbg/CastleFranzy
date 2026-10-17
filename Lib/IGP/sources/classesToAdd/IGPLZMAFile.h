#ifndef _LZMA_FILE_
#define _LZMA_FILE_

#import "IGPFile.h"

//#define MAX_BLOCK_SIZE	0x1000	//4k.

class IFileI;

class IGPLZMAFile
{
public:
	//Opens a LZMA encoded file
	static IGPLZMAFile* Open(TFileName fileName, int i_cache, int i_offset, bool bUseLZMA = false);
	static int GetSize(TFileName fileName, int i_cache, int i_offset, bool bUseLZMA = false);

	////Returns a LZMA file reader from a file pointer
	//static LZMAFile* open(IFileI* aFilePointer);

	//Closes a LZMA encoded file and frees all resources asociated with it
	static void Close(IGPLZMAFile*& f);

	int read(void* buffer, unsigned int size);	//Reads from the buffer

	s8		readChar();		//Reads a character from the buffer
	short	readShort();	//Reads a short from the buffer
	int		readInt();		//Reads an int from the buffer
	double  readDouble();

	int	skip(int size);	//Skips n bytes from the stream

private:
	//Constructor and destructor are private. Acces is done only thru open/close.
	IGPLZMAFile();
	~IGPLZMAFile();
	
#if defined USE_LZMA
	int readHeader();	//Reads the compressed file header and initializes object data
	int initDecoder();	//Initializes the LZMA decoder
	int	decode();		//Decodes a chunk of the input in the output buffer

	bool bLZMA;
#endif

private:
	IGPIResourceReadI*	pFile;				//File object
	unsigned int	length;				//Compressed file size
	unsigned int	outSize;			//Size of the decompressed file
	unsigned int	lzmaInternalSize;	//Internal size needed by the decompression algorithm
	unsigned int	dictionarySize;		//Size of the dictionary used in decompression
	unsigned char*	outStream;			//Buffer used for decoded data
	unsigned char*	lzmaInternalData;	//Buffer used internally for decompression
	unsigned char*	dictionary;			//Dictionary used in decompression
	int				lc, lp, pb;			//Decoder parameters
	unsigned int	validIndex;			//Valid index in the outStream
	unsigned int	index;				//File index
};

#endif
