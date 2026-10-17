
#include "IGPLZMAFile.h"
//#include "LZMADecode.h"
//#include "BIA\Lib3D\GX_Device.h"
//#include "BIA\Game.h"

IGPLZMAFile::IGPLZMAFile()
{
	pFile				= NULL;
	outStream			= NULL;
	lzmaInternalData	= NULL;
	dictionary			= NULL;
	dictionarySize		= 0;
	validIndex			= 0;
	index				= 0;
}

IGPLZMAFile::~IGPLZMAFile()
{
	//Close the file
	SAFE_DELETE( pFile );

	//Delete the allocated memory
	outStream = NULL;
	lzmaInternalData = NULL;
	dictionary = NULL;
}

//Opens a LZMA encoded file
IGPLZMAFile * IGPLZMAFile::Open(TFileName fileName, int i_cache, int i_offset, bool bUseLZMA)
{
	//Create the file
	IGPLZMAFile* f = NEW IGPLZMAFile();
	if (!f)
		return NULL;

	//Open the file
	f->pFile = IGPIFileReadI::Open((void*)fileName, i_cache, i_offset);
	if (!f->pFile)
	{
		Close(f);
		return NULL;
	}

#if (USE_LZMA)

	f->bLZMA = bUseLZMA;

	if(f->bLZMA)
	{
		//Read file header
		if (f->readHeader() < 0)
			Close(f);

		//Initialize decoder
		if (f->initDecoder() < 0)
			Close(f);

		//Decode the first chunk
		if (f->decode() < 0)
			Close(f);
	}
#endif

	//Return the created file.
	return f;
}
int IGPLZMAFile::GetSize(TFileName fileName, int i_cache, int i_offset, bool bUseLZMA)
{
	//Create the file
	IGPLZMAFile* f = NEW IGPLZMAFile();
	if (!f)
		return NULL;

	//Open the file
	f->pFile = IGPIFileReadI::Open((void*)fileName, i_cache, i_offset);
	if (!f->pFile)
	{
		Close(f);
		return 0;
	}

	//Return the created file.
	return IGPIFileReadI::GetSize((void*)fileName, i_cache, i_offset);
}
//Closes a LZMA encoded file and frees all resources asociated with it
void IGPLZMAFile::Close(IGPLZMAFile*& f)
{
	SAFE_DELETE(f);
}

#if (USE_LZMA)
//Decodes a chunk of the input in the output buffer
int	IGPLZMAFile::decode()
{
	//Compute decompressed block size
	u32 blockSize = outSize;
	if (blockSize > MAX_OUTSTREAM_SIZE)
		blockSize = MAX_OUTSTREAM_SIZE;

	//Call LZMA decoder
	if (LzmaDecode(lzmaInternalData, outStream, blockSize, &validIndex))
		return -1;

	//Decompressed outSizeProcessed bytes. Update the sizes.
	outSize = (outSize < validIndex) ? 0 : outSize	- validIndex;

	//No error
	return 0;
}

//Initializes the LZMA decoder
int IGPLZMAFile::initDecoder()
{
	u32 *memory_zone = cGame::getGame()->getGlobalZone();

	// skip the file area
	memory_zone += cGame::getGame()->getMaxFileBufferSize();

	if ((lzmaInternalSize > (MAX_INTERNAL_SIZE * 4)) || (dictionarySize > (MAX_DICTIONARY_SIZE * 4)))
		return -1;
	
	//Alloc output buffer
	outStream	= (unsigned char*)memory_zone;
	memory_zone += MAX_OUTSTREAM_SIZE;

	//Alloc internal data
	lzmaInternalData	= (unsigned char*)memory_zone;
	memory_zone += MAX_INTERNAL_SIZE;

	//Alloc the dictionary
	dictionary	= (unsigned char*)memory_zone;
	memory_zone += MAX_DICTIONARY_SIZE;

	//Initialize the LZMA decoder
	LzmaDecoderInit(
						lzmaInternalData, lzmaInternalSize,
						lc, lp, pb,
						dictionary, dictionarySize,
						pFile
					);

	//No error
	return 0;
}

//Reads the compressed file header and initializes object data
int IGPLZMAFile::readHeader()
{
	int				i;
	unsigned char	properties[5];
	unsigned char	prop0;

	//Read file header
	if (!pFile->Read(properties, sizeof(properties)))
		return -1;

	//Compute the size of the decompressed file
	outSize	= 0;
	for (i = 0; i < 4; i++)
	{
		unsigned char b;
		if (!pFile->Read(&b, sizeof(b)))
			return -1;
		outSize += (unsigned int)(b) << (i * 8);
	}
	if (outSize == 0xFFFFFFFF)
		return -2;		//Version not supported

	for (i = 0; i < 4; i++)
	{
		unsigned char b;
		if (!pFile->Read(&b, sizeof(b)))
			return -3;
		if (b)
			return -4;	//File too long
	}

	//Process properties
	prop0 = properties[0];
	if (prop0 >= (9*5*5))
		return -5;	//Properties error
	for (pb = 0; prop0 >= (9 * 5); pb++, prop0 -= (9 * 5));
		for (lp = 0; prop0 >= 9; lp++, prop0 -= 9);
			lc = prop0;

	//Compute the size needed by the decompression algorithm
	lzmaInternalSize = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (lc + lp)))* sizeof(CProb);
	lzmaInternalSize += 100;

	//Compute dictionary size
	for (i = 0; i < 4; i++)
		dictionarySize += (UInt32)(properties[1 + i]) << (i * 8);

	//No error
	return 0;
}

//Reads  from the buffer
int IGPLZMAFile::read(void* buffer, unsigned int size)
{
	if(!bLZMA)
	{
		return (pFile->Read(buffer, size)) ? 0 : -1;
	}

	unsigned char* dest = (unsigned char*)buffer;
	unsigned char* src = (outStream + index);

	while (size--)
	{
		*(dest++) = *(src++);	//Copy data
		index++;				//Increment buffer index

		//See if a new decompression is required
		if (index >= validIndex)
		{
			index = 0;
			src = outStream;
			if (decode() < 0)
				return -1;
		}
	}

	//No error
	return 0;
}

//Skips n bytes from the stream
int IGPLZMAFile::skip(int size)
{
	if(!bLZMA)
	{
		return (pFile->Skip(size));
	}

	for (; ; )
	{
		if (size <= 0)
			return 0;

		if (index + size < validIndex)
		{
			index += size;
			return 0;	//No error
		}

		//Must decode a new buffer
		size	-= (validIndex - index);
		if (decode() < 0)
			return -1;
		index	= 0;
	}

	//Should never get here.
	//	return 0;
}
#else

//Reads  from the buffer
int IGPLZMAFile::read(void* buffer, unsigned int size)
{
	if (!pFile->Read(buffer, size))
		return -1;

	//No error
	return 0;
}

//Skips n bytes from the stream
int IGPLZMAFile::skip(int size )
{
	return (pFile->Skip(size ));
}

#endif

//Reads a character from the buffer
s8 IGPLZMAFile::readChar()
{
	s8 ch;
	read(&ch, sizeof(s8));
	return ch;
}

//Reads a short from the buffer
short IGPLZMAFile::readShort()
{
	short s;
	read(&s, sizeof(short));
	return s;
}

//Reads an int from the buffer
int IGPLZMAFile::readInt()
{
	int i;
	read(&i, sizeof(int));
	return i;
}

double IGPLZMAFile::readDouble()
{
	double i;
	read(&i, sizeof(double));
	return i;
}