#include "stdafx.h"
#include "ICBinaryIO.h"

namespace ICBinaryIO
{
	void WriteFString( char *&pBuffer, const fstring &string )
	{
		int stringSize = ( int )string.size();
		BWriteInt( pBuffer, stringSize );

		const char **pStrBuf = string.GetDataPtr();
		memcpy( pBuffer, *pStrBuf, stringSize );
		pBuffer += stringSize;
	}

	size_t GetFStringSize( const fstring &string )
	{
		return sizeof( int ) + string.size();
	}

	void ReadFString( char *&pBuffer, fstring &string )
	{
		int stringSize;
		BReadInt( pBuffer, &stringSize );
		string.insert( 0, pBuffer, stringSize );
		pBuffer += stringSize;
	}
}