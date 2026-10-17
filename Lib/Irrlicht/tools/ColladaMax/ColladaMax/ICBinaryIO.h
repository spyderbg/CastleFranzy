#ifndef _IC_BINARY_WRITER_H_
#define _IC_BINARY_WRITER_H_

namespace ICBinaryIO
{
	void WriteFString( char *&pBuffer, const fstring &string );
	void ReadFString( char *&pBuffer, fstring &string );
	
	// includes string size bytes
	size_t GetFStringSize( const fstring &string );
}

#endif // _IC_BINARY_WRITER_H_
