#include "../../include/Porting.h"
#include "../../include/Game_defines.h"

#ifdef OS_IPHONE
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>

extern unsigned int GetMSTime();
extern "C" void GetUserCountry(char* buf);

#endif

#ifdef OS_WIN32
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#endif


const int MAX_FILE_LIST = 8;

FILE * l_file_list[MAX_FILE_LIST] = { NULL, NULL, NULL, NULL };

char SSEncDec_GetCharFromKeyByIndex(int nKeyIndex);

void * XP_API_MALLOC( int size )
{
	return malloc( size );
}

void XP_API_FREE( void * p )
{
	delete( p );
}

void XP_API_MEMSET( void * data, int value, int size )
{
	memset( data, value, size );
}

void * XP_API_MEMCPY( void * data, const void * src, int size )
{
	memcpy( data, src, size );
	return data;
}

int XP_API_STRLEN( const char * string )
{
	return (int)strlen( string );
}

int XP_API_STRCMP( const char * p1, const char * p2 )
{
	return strcmp( p1, p2 );
}

int XP_API_STRNCMP( const char * p1, const char * p2, int size )
{
	return strncmp( p1, p2, size );
}

char * XP_API_STRNEW( const char * string )
{
	char * data = NULL;
	int len = XP_API_STRLEN( string );

	data = XP_API_NEW char [ len + 1 ];
	XP_API_MEMSET( data, 0, len + 1 );
	XP_API_MEMCPY( data, string, len );
	return data;
}

char * XP_API_STRCPY( char * dest, const char * src )
{
	return strcpy( dest, src );
}

char * XP_API_STRNCPY( char * dest, const char * src, int count )
{
	return strncpy( dest, src, count );
}

char * XP_API_STRCAT( char * dest, const char * src )
{
	return strcat( dest, src );
}

char * XP_API_STRTRIM( char * src )
{
	int len = XP_API_STRLEN( src );
	int space_flag = true;
	int index = 0;

	for (int i=0; i<len; i++ )
	{
		if ( src[i] == ' ' && space_flag )
		{
			;
		}
		else
		{
			space_flag = false;
			src[index++] = src[i];
		}
	}

	src[index] = 0;
	
	return src;
}

char * XP_API_STRSTR( const char * src, const char * substr )
{
	return (char *)strstr( src, substr );
}

char * XP_API_STRCHRFIND( char * src, const char subchar, int count )
{
	int sum = 0;
	int i = 0;

	while( src[i] != NULL && sum < count )
	{
		if ( src[i] == subchar )
		{
			sum++;
		}
		i++;
	}

	if ( sum == count )
	{
		return src+i;
	}
	else
	{
		return NULL;
	}
}

char * XP_API_STRCHR( const char * src, const char subchar, int bFromLeft2Right )
{
	if ( bFromLeft2Right == 1 )
	{

#ifdef OS_WIN32
		return (char *)strchr( src, subchar );
#endif

#ifdef OS_IPHONE
		return strchr( src, subchar );
#endif

	}
	else
	{
#ifdef OS_WIN32
		return (char *)strrchr( src, subchar );
#endif

#ifdef OS_IPHONE
		return strrchr( src, subchar );
#endif		
	}
}

int XP_API_SPRINTF( char * data, const char * format, ... )
{
	va_list args;
	//int num = 0;
	int len = 0;
	int idx = 0;

	va_start( args, format );
	len = XP_API_STRLEN( (char *)format );

	for ( int i=0; i<len; i++ )
	{
		if ( '%' == format[i] )
		{
			switch( format[i+1] )
			{
			case 'd':
				{
					int value = va_arg( args, int );
					char temp[ 12 ] = { 0 };
					XP_API_ITOA( value, temp, 10 );
					int length = XP_API_STRLEN( temp );
					XP_API_MEMCPY( data + idx, temp, length );
					idx += length;
					i++;
				}
				break;
			case 'f':
				{
					float value = va_arg( args, float );
					char temp[ 32 ] = { 0 };
					XP_API_FTOA( value, temp, 10 );
					int length = XP_API_STRLEN( temp );
					XP_API_MEMCPY( data + idx, temp, length );
					idx += length;
					i++;
				}
				break;
			case 's':
				{
					char * value = va_arg( args, char * );
					if ( NULL != value )
					{
						int length = XP_API_STRLEN( value );
						XP_API_MEMCPY( data + idx, value, length );
						idx += length;
					}
					i++;
				}
				break;
			case 'c':
				{
#ifdef OS_WIN32				
					char value = va_arg( args, char );
#endif

#ifdef OS_IPHONE
					char value = va_arg( args, int );
#endif
					data[idx] = value;
					idx += 1;
					i++;
				}
				break;
			default:
				break;
			}			
		}
		else
		{
			data[ idx++ ] = format[i];
		}
	}	

	va_end( args );

	return idx;
}

void XP_DEBUG_INIT()
{
#ifdef OS_WIN32
	AllocConsole();  
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle,_O_TEXT);
	FILE * hf = _fdopen( hCrt,"w" );  
	*stdout = *hf;
#endif
}

void XP_DEBUG_FINALIZE()
{
#ifdef OS_WIN32
	FreeConsole();
#endif	
}
void XP_DEBUG_OUT( const char * format, ... )
{
	va_list args;
	//int num = 0;
	int len = 0;
	int idx = 0;
	char data[20*ONLINE_MAX_PACKET_SIZE];

	XP_API_MEMSET(data,0,20*ONLINE_MAX_PACKET_SIZE);

	va_start( args, format );
	len = XP_API_STRLEN( (char *)format );

	for ( int i=0; i<len; i++ )
	{
		if ( '%' == format[i] )
		{
			switch( format[i+1] )
			{
			case 'd':
				{
					int value = va_arg( args, int );
					char temp[ 12 ] = { 0 };
					XP_API_ITOA( value, temp, 10 );
					int length = XP_API_STRLEN( temp );
					XP_API_MEMCPY( data + idx, temp, length );
					idx += length;
					i++;
				}
				break;
			case 's':
				{
					char * value = va_arg( args, char * );
					if ( NULL != value )
					{
						int length = XP_API_STRLEN( value );
						XP_API_MEMCPY( data + idx, value, length );
						idx += length;
					}
					i++;
				}
				break;
			default:
				break;
			}			
		}
		else
		{
			data[ idx++ ] = format[i];
		}
	}	

	va_end( args );
	printf("%s",data);
}

int XP_API_ATOI( const char * data )
{
	return atoi( data );
}

char * XP_API_FTOA( float num, char * str, int digits )
{	
#ifdef OS_WIN32
	return _gcvt( num, digits, str );
#endif

#ifdef OS_IPHONE
	sprintf( str, "%f", num );
	return str;
#endif
}

double XP_API_ATOF( char * str )
{
	return atof( str );
}

char * XP_API_ITOA( int num, char * str, int radix )
{
	const char table[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *ptr = str;
	bool negative = false;
	
	if( num == 0 )
	{
		//num=0 
		*ptr++ = '0';
		*ptr = '\0';		// don`t forget the end of the string is '\0'!!!!!!!!!
		return str;
	}

	if( num < 0 )
	{
		//if num is negative ,the add '-'and change num to positive 
		*ptr++ = '-';
		num *= -1;
		negative = true;
	}

	while(num)
	{
		*ptr++ = table[ num % radix ];
		num /= radix;
	}
	*ptr = '\0'; //if num is negative ,the add '-'and change num to positive
	// in the below, we have to converse the string
	char *start =( negative ? str+1: str ); //now start points the head of the string
	ptr--; //now prt points the end of the string
	while( start<ptr )
	{
		char temp = *start; 
		*start = *ptr;
		*ptr = temp;
		start++;
		ptr--;
	} 
	
	return str;
}

#define         MASKBITS                0x3F
#define         MASKBYTE                0x80
#define         MASK2BYTES              0xC0
#define         MASK3BYTES              0xE0

int ConvertUnicodeToUTF8( char* output, const Unicode2Bytes* input, const int lenInput )
{
	int lenOutput = 0;
	for(int i=0; i < lenInput; i++)
	{
		// 0xxxxxxx
		if(input[i] < 0x80)
		{
			*output = (char)input[i];		output++; lenOutput++;
		}
		// 110xxxxx 10xxxxxx
		else if(input[i] < 0x800)
		{
			*output = (char)(MASK2BYTES | input[i] >> 6);			output++;	lenOutput++;
			*output = (char)(MASKBYTE | input[i] & MASKBITS);		output++;	lenOutput++;
		}
		// 1110xxxx 10xxxxxx 10xxxxxx
		else if(input[i] < 0x10000)
		{
			*output = ((char)(MASK3BYTES | input[i] >> 12));			output++;	lenOutput++;
			*output = ((char)(MASKBYTE | input[i] >> 6 & MASKBITS));	output++;	lenOutput++;
			*output = ((char)(MASKBYTE | input[i] & MASKBITS));			output++;	lenOutput++;
		}
	}
	*output = 0;

	return lenOutput;
}

int ConvertUTF8ToUnicode( Unicode2Bytes* output, const char* input, const int lenInput )
{
	int lenOutput = 0;
	for(int i=0; i < lenInput;)
	{
		// 1110xxxx 10xxxxxx 10xxxxxx
		if((input[i] & MASK3BYTES) == MASK3BYTES)
		{
			*output = ((input[i] & 0x0F) << 12) | ((input[i+1] & MASKBITS) << 6)
						| (input[i+2] & MASKBITS);
			i += 3;
			lenOutput++;
		}
		// 110xxxxx 10xxxxxx
		else if((input[i] & MASK2BYTES) == MASK2BYTES)
		{
			*output = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
			i += 2;
			lenOutput++;
		}
		// 0xxxxxxx
		else if(input[i] < MASKBYTE)
		{
			*output = input[i];
			i += 1;
			lenOutput++;
		}

		output++;
	}
	*output = 0;

	return lenOutput;
}


int TranslateUnicode2Utf8( char **nameOutput, Unicode2Bytes* nameInput, int lenInput, int bNeedNewMEM )
{
	if( bNeedNewMEM )
		*nameOutput = XP_API_NEW char[lenInput * PER_CHARACTER_COUNT + 1];
	XP_API_MEMSET( *nameOutput, 0, lenInput * PER_CHARACTER_COUNT + 1);

	int lenConvertName = ConvertUnicodeToUTF8( *nameOutput, nameInput, lenInput );

	return lenConvertName;
}

int TranslateUtf82Unicode( Unicode2Bytes **nameOutput, char* nameInput, int lenInput, int bNeedNewMEM )
{
	if( bNeedNewMEM )
		*nameOutput = XP_API_NEW Unicode2Bytes[lenInput + 1];
	XP_API_MEMSET( *nameOutput, 0, (lenInput + 1) * sizeof(Unicode2Bytes) );

	int lenConvertName = ConvertUTF8ToUnicode( *nameOutput, nameInput, lenInput );

	return lenConvertName;
}

#ifdef USE_WCHAR
int TranslateWChar2Utf8(char **nameOutput, wchar_t* nameInput, int lenInput, int bNeedNewMEM)
{
	int nwWidth = sizeof(wchar_t) ;
	
	if (nwWidth == 2 || nwWidth == 4)
	{
		Unicode2Bytes *unicode = XP_API_NEW Unicode2Bytes[lenInput] ;
		int iCopy = 0 ;
		while(iCopy < lenInput)
		{
			XP_API_MEMCPY(unicode + iCopy, nameInput + iCopy, 2) ;
			iCopy ++ ;
		}
		int ret =  TranslateUnicode2Utf8( nameOutput, unicode, lenInput, bNeedNewMEM ) ;	
		XP_API_DELETE_ARRAY(unicode) ;
		return ret ;
	}

	return -1 ;
}


int TranslateUtf82WChar(wchar_t **nameOutput, char* nameInput, int lenInput, int bNeedNewMEM)
{
	int nwWidth = sizeof(wchar_t) ;
	
	if (nwWidth == 2 || nwWidth == 4)
	{
		Unicode2Bytes *unicode  ;
		int len = TranslateUtf82Unicode(&unicode, nameInput, lenInput, 1) ;

		if (bNeedNewMEM)
		{
			*nameOutput = XP_API_NEW wchar_t[len + 1] ;
			XP_API_MEMSET(*nameOutput, 0, sizeof(wchar_t) * (len + 1)) ;
		}

		int iCopy = 0 ;
		while(iCopy < len)
		{
			XP_API_MEMCPY((*nameOutput) + iCopy, unicode + iCopy, 2) ;
			iCopy ++ ;
		}

		XP_API_DELETE_ARRAY(unicode) ;
		return len ;
	}
	return -1 ;
}
#endif

void UTF8ToUnicode_Old(unsigned short *strsrc, const char *pUTF8)
{
	while( *pUTF8 != 0 )
	{
		if ((char)pUTF8[0] >= 0)	//first bit is 0, one byte
		{
			*strsrc = *pUTF8++;
		}
		else
		{
			int i = 1;
			char c = *pUTF8;
			for (c <<= 2; c < 0; ++i)
				c <<= 1;
			//ASSERT(i < 6);	//should never more than 6 bytes
			if (i >= 6)
			{
				*strsrc = *pUTF8++;
			}
			else
			{
				unsigned short ch = c >> (i + 1);
				for (++pUTF8; i > 0; --i)
				{
					ch = (ch << 6) | (*pUTF8++ & 0x3F);
				}
				*strsrc = ch;
			}
		}
		strsrc++;
	}
}
int ConvertUnicodeToUTF8_Old( char *strdest,  const unsigned short *strsrc )
{
	unsigned short code0, code1, code2;
    unsigned short unicode;
    unsigned char *pdest = (unsigned char *)strdest;
	unsigned char *psrc= (unsigned char *)strsrc;

	if(!strdest)
	{
		int size = 1;
		for( ; ; ) 
		{
			unicode = *psrc++;
			unicode += *psrc++ << 8;
			if(!unicode) 
			{
				break;
			}
			
			if ( unicode <= 0x007F ) 
			{
				size++;
			}
			else if ( unicode <= 0x07FF ) 
			{
				// 0080 - 07FF 110xxxxx 10xxxxxx
				size += 2;
			} else 
			{
				// 0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
				size += 3;
			}
		}      
		return size;
	}
	else
	{
		// two bytes not alignment
		if ( (unsigned long)strsrc & 0x00000001 ) 
		{
			unsigned char *psrc= (unsigned char *)strsrc;
			unicode = *psrc++;
			unicode += *psrc++ << 8;
			
			while ( unicode != 0 ) 
			{
				if ( unicode <= 0x007F ) 
				{
					*pdest ++ = (unsigned char )unicode;
				}
				else if ( unicode <= 0x07FF ) 
				{
					// 0080 - 07FF 110xxxxx 10xxxxxx
					code1 = (( unicode & 0x07c0 ) >> 6 ) | 0x00c0;
					code0 = (unicode & 0x003f) | 0x0080;
					*pdest++ = (unsigned char)code1;
					*pdest++ = (unsigned char)code0;
				} else 
				{
					// 0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
					code2 = (( unicode & 0xf000) >> 12 ) | 0x00e0;
					code1 = (( unicode & 0x0fc0 ) >> 6 ) | 0x0080;
					code0 = (unicode & 0x003f) | 0x0080;
					*pdest++ = (unsigned char)code2;
					*pdest++ = (unsigned char)code1;
					*pdest++ = (unsigned char)code0;
				}
				
				unicode = *psrc++;
				unicode += *psrc++ << 8;
			}      
		}
		else 
		{ // two bytes alignment
			unsigned short *psrc= (unsigned short *)strsrc;
			unicode = *psrc++;
			while ( unicode != 0 ) 
			{
				if ( unicode <= 0x007F ) 
				{   
					*pdest ++ = (unsigned char )unicode;
				} 
				else if ( unicode <= 0x07FF ) 
				{
					// 0080 - 07FF 110xxxxx 10xxxxxx
					code1 = (( unicode & 0x07c0 ) >> 6 ) | 0x00c0;
					code0 = (unicode & 0x003f) | 0x0080;
					*pdest++ = (unsigned char)code1;
					*pdest++ = (unsigned char)code0;
				} else 
				{
					// 0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
					code2 = (( unicode & 0xf000) >> 12 ) | 0x00e0;
					code1 = (( unicode & 0x0fc0 ) >> 6 ) | 0x0080;
					code0 = (unicode & 0x003f) | 0x0080;
					*pdest++ = (unsigned char)code2;
					*pdest++ = (unsigned char)code1;
					*pdest++ = (unsigned char)code0;
				}
				unicode = *psrc++;
			}  
		}
		*pdest = 0;  
		return ((unsigned long)pdest - (unsigned long)strdest);
	}
}

XP_FILE_HANDLE XP_API_FILE_OPEN(const char * filename, const char * flag)
{
	int i = 0;

	while(i < MAX_FILE_LIST && l_file_list[i] != NULL)
	{
		i++;
	}
	
	if (i == MAX_FILE_LIST)
	{
		return 0;
	}
	
	FILE * file = fopen(filename,flag);

	if ( NULL == file )
	{
		return 0;
	}

	l_file_list[i] = file;
	return i+1;
}

void XP_API_FILE_CLOSE(XP_FILE_HANDLE handle)
{
	fclose(l_file_list[handle-1]);
	l_file_list[handle-1] = NULL;
}

int XP_API_FILE_READ( void * dest,int element_size, int count, XP_FILE_HANDLE handle )
{
	return fread(dest,element_size,count,l_file_list[handle-1]);
}

int XP_API_FILE_WRITE( void * src,int element_size, int count, XP_FILE_HANDLE handle )
{
	return fwrite(src,element_size,count,l_file_list[handle-1]);
}

int XP_API_FILE_GET_LENGTH( XP_FILE_HANDLE handle )
{
	int pos_beg = 0;
	int pos_end = 0;

	fseek( l_file_list[handle-1], 0, SEEK_SET );
	pos_beg = ftell( l_file_list[handle-1] );

	fseek( l_file_list[handle-1], 0, SEEK_END );
	pos_end = ftell( l_file_list[handle-1] );

	fseek( l_file_list[handle-1], 0, SEEK_SET );
	return (pos_end - pos_beg);
}

/**
 * Parse one line once, and the seperator is "\n".
 *
 * @param: 
 *		int pos: start from 0
 * @return
 *		int:
 */
int XP_API_PARSE_DATA( const char * src, char *dst, int pos, char seperator )
{
	dst[0] = 0;
	int idx = 0;
	int idx0 = 0;
	int idx_beg = -1;

	while( src[idx] != 0 )
	{
		if ( src[idx] == seperator )
		{
			idx++;
			pos--;
			continue;
		}

		if ( pos == 0 )
		{
			if ( idx_beg < 0 )
			{
				idx_beg = idx;
			}
			dst[idx0] = src[idx];
			dst[idx0+1] = 0;
			idx0++;
		}
		else if (pos < 0)
		{
			break;
		}
		
		idx++;
	}

	return idx_beg;
}

int XP_API_PARSE_DATA_NEW( const char * src, char **dst, int pos, char seperator )
{
	int srcCurrent = 0;
	int srcStart = -1;

	while( src[srcCurrent] != 0 )
	{
		if ( src[srcCurrent] == seperator )
		{
			srcCurrent++;
			pos--;
			continue;
		}

		if ( pos == 0 )
		{
			if ( srcStart < 0 )
			{
				srcStart = srcCurrent;
			}
		}
		else if (pos < 0)
		{
			break;
		}
		
		srcCurrent++;
	}

	int len = srcCurrent-srcStart-1;
	*dst = XP_API_NEW char[len+1];
	XP_API_MEMSET(*dst, 0, len+1);
	XP_API_MEMCPY(*dst, src+srcStart, len);

	return srcStart;
}

long XP_API_GET_TIME()
{
#ifdef OS_WIN32
	return GetTickCount();
#endif

#ifdef OS_IPHONE
	return GetMSTime();
#endif
}

#ifdef LOBBY_THREAD_TEST
#include <time.h>
void GetCurrentSystemTime( char** string )
{
	*string = XP_API_NEW char[20+1];
	memset( *string, 0, 21 );

	time_t sysTime;
	time(&sysTime);

	struct tm t;
	localtime_s(&t,&sysTime);

	sprintf( *string, "#%d:%d:%d#", t.tm_hour, t.tm_min, t.tm_sec );
	//strftime( *string , 20, "%Y%m%d%H%M%S", &t );
}
#endif

/***************************************************************************
* FUNCTION SSEncDec_GetCharFromKeyByIndex
* 
* DESCRIPTION
* - Get a char from the Key by index. see SSEncDec_String2Blob for more detail.
* - Key is the following: (64 chars total, which is the max num can be
*   represented with 6 bits)
* 1         2         3         4         5         6
* 0123456789012345678901234567890123456789012345678901234567890123
* abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-
* so key[0] is a, key[1] is b.....
* - note that the easiest way to do this is to put key in a char array then
*   simply return by array index, what I do here is more complicated by can
*   save some memory
* 
* PARAMETERS:
* @param nKeyIndex - the index
* 
* RETURN:
* The char in key with the given index
***************************************************************************/
char SSEncDec_GetCharFromKeyByIndex(int nKeyIndex)
{
   if(nKeyIndex<26)
   {//key index 0-25 is a-z
      return nKeyIndex + 97; //convert 0...25 to a...z
   }
   else if(nKeyIndex<52)
   {//key index 26-51 is A-Z
      return nKeyIndex + 39; //convert 26...51 to A...Z
   }
   else if(nKeyIndex<62)
   {//key index 52-61 is 0-9
      return nKeyIndex - 4; //convert 52...61 to 0...9
   }
   else if(nKeyIndex==62)
   {//key index 62 is '_'
      return '_';
   }
   else
   {//key index 63 is '-'
      return '-';
   }

   //should never get here
   return 0;
}

char SSEncDec_GetKeyFromChar(char nChar)
{
    if(nChar == '-')
    {
        return 63;
    }
    else if(nChar == '_')
    {
        return 62;
    }
    else if(nChar < 58)
    {//48-57 is '0'-'9', index is 52-61
        return (char)(nChar + 4);
    }
    else if(nChar < 91)
    {//65-91 is 'A'-'Z', index is 26-51
        return (char)(nChar - 39);
    }
    else
    {//97-122 is 'a'-'z', index is 0-25
        return (char)(nChar - 97);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For encoding/decoding between ScoreService and server

/***************************************************************************
* FUNCTION SSEncDec_String2Blob()
* 
* DESCRIPTION
* Encode a clear string into BLOB (encode a string with alphanumeric
* characters)
* Encode as fallowing:
* Take 6 bits from stirng at a time, use it as an integer index to get a char
* from key.
* See SSEncDec_GetCharFromKeyByIndex for detail of key.
* These chars from key form the blob.
* Take bits from string as fallowing: (say we have "abc")
* take a char from string one at a time starting from left to right (we first
* take a which is 1100001) 
* then take the right most 6 bits from it. (that's 00001)
* take what's left of this char, put it on the right (we have 11 left)
* so we need more bits to fill the lefthand side blank (we need 4 more in this
* case to make a 6 bit num)
* we take it from next char (that's b in this case, which is 1100010)
* take the right bits first (so we take 4 bits from right, which is 0010)
* put them together to form a 6 bit number (we have 001011)
* then take what's left of this cahr, put it on the right, so on so forth....
* PARAMETERS:
* @param s - the string to encode
* 
* RETURN:
* Pointer to blob
* NOTE: Caller of this function need to free blob
***************************************************************************/
char * SSEncDec_String2Blob(char* s)
{
	return SSEncDec_ByteArray2Blob((BYTE*)s, strlen(s)) ;
}

char *SSEncDec_Blob2String(char* blob)
{
	BYTE *b = XP_API_NEW BYTE[strlen(blob)];
	XP_API_MEMSET(b,0, strlen(blob)) ;
	int nuLen = SSEncDec_Blob2ByteArray(&b, blob) ;  //unblob
	return (char*)b ;
}

/***************************************************************************
* @FUNCTION SSEncDec_ByteArray2Blob()
* 
* DESCRIPTION
* Encode a byte array into BLOB 
***************************************************************************/

char * SSEncDec_ByteArray2Blob(BYTE* s, int len)
{
   char *sBlob;          //blob buffer
   int  nBlobPos     = 0; //current index of blob
   int  nSPos        = 0; //current index of string
   int  nBitsNotUsed = 8; //how many bits not used in the current s[nSPos]
   int  nKeyIndex = 0;        //the index of key calcualted using s
	
   //calculate blob length and allocate memory   
   int nBlobLength = len*8/6;
   if(nBlobLength)
   {//has remainder, need one more char to hold it
      nBlobLength += 2; //remainder + ending 0
   }
   else
   {
      nBlobLength ++; //need space for ending 0
   }
   sBlob = XP_API_NEW char[nBlobLength+1];//brew 0's MALLOC'ed mem
   XP_API_MEMSET(sBlob, 0, nBlobLength+1);
   if(!sBlob)
   {//error allocating mem for blob
      return NULL;
   }

   // -> go though each char in s, calculate a key index using 6 of its bits at a time,
   //   use that index to get a char from key, these chars will form the blob
   // -> go though s from left to right, but get bits from each char in s from right to left
  
   while( nSPos < len)
   {
      //get next 6 not used bits from s

      //first shift not used bits in current pos all the way to right
      nKeyIndex = s[nSPos] >> (8 - nBitsNotUsed);

      if(nBitsNotUsed < 6)
      {//not enough bits in current pos
         if(++nSPos < len)
         {//there's still char in the next pos, so use its bits (nSPos points to next pos now)
            //get 6-nBitsNotUsed number of bits from next char
            //to do so, we first shift the char in next pos to left nBitsNotUsed times
            //then | with leftover bits to form a 6 bit number
            nKeyIndex |= (s[nSPos] << nBitsNotUsed);
            nBitsNotUsed += 2; //since 6-nBitsNotUsed bits is used from next char, 8-(6-nBitsNotUsed) will be left there
         }
         //else
         //there's no next char, just use what we have left in the current pos (nSPos is now pointing pass the end of s)
      }
      else
      {//enough bits not used in current pos
         nBitsNotUsed -= 6; //we used 6 more bits from current pos

         if(nBitsNotUsed==0)
         {//used up all the bits in current pos, so go to next
            nBitsNotUsed = 8;
            nSPos ++;
         }
      }

      //use right most 6 bits as key index
      nKeyIndex &= 63;   //& with 00111111(63) to take only the right most 6 bits

      //then use index to get a char from key to fill the blob
      sBlob[nBlobPos++] = SSEncDec_GetCharFromKeyByIndex(nKeyIndex);
   }

   return sBlob;
}

/***************************************************************************
* @FUNCTION SSEncDec_Blob2ByteArray()
* 
* DESCRIPTION
* Encode an BLOB into byte array.
* Return the len of byte array.
***************************************************************************/

int SSEncDec_Blob2ByteArray(BYTE* *arr, char* blob)//modify by heyi
{
	char* b = blob;//.getBytes();

	BYTE* s; //string buffer

	int nBlobPos = 0; //current index of blob
	int nSPos = 0; //current index of string
	int nBitsNotSet = 8; //how many bits not set in the current s[nSPos]
	char nKeyIndex; //the index of key calculated using

	// Calculate string length and allocate memory
	// It's ok to cut off the remainder, and we need space for the
	// termination character.
	int nStrLength = ((strlen(blob)) * 6 / 8);//modify by heyi
	s = new BYTE[nStrLength + 1];

	// Initialize the string.
	for (int k = 0; k < nStrLength + 1; ++k)
	{
		s[k] = 0;
	}

	for (nBlobPos = 0; nBlobPos < XP_API_STRLEN(blob); nBlobPos++)
	{
		// Each character in the blob represents 6 bits in the decoded string.
		nKeyIndex = SSEncDec_GetKeyFromChar(b[nBlobPos]);

		// If "a" represents the bits of the key from the first character of
		// the blob, "b" the bits of the key from the second character in
		// the blob, "b" the bits of the key from the third character in the
		//  blob, etc.
		//  Then the decoded string would be comprised as follows:
		// s[0] = bbaaaaaa;
		// s[1] = ccccbbbb;
		// s[2] = ddddddcc;
		// s[3] = ffeeeeee;
		// .
		// .
		// .

		// Reconstruct the string's character value. First fill in it's low
		// order bits first. Then shift to the high order bits that haven't
		// been set yet.
		s[nSPos] |= nKeyIndex << (8 - nBitsNotSet);

		if (nBitsNotSet > 6)
		{
			// The low order 6 bits of this string character have been set.
			// Only the top 2 bits are left.
			nBitsNotSet -= 6;
		}
		else
		{
			if (nSPos < (nStrLength - 1))//modify by heyi
			{
				// There may be bits in the key that haven't been used. Go to
				// the string's next character and place the remaining bits
				// from the key into the character's low order bits. When
				// nBitsNotSet is 6 all of the bits of the key have been used
				// and this code will not set anything in the next character.
				s[++nSPos] |= nKeyIndex >> nBitsNotSet;

				// (6 - nBitsNotSet) low order bits have been set in the
				// string's current character.

				// Increment in order to lineup with the high order bits of the
				// string's character that haven't been set yet.
				nBitsNotSet += 2;
			}
			// else {}
			// we filled all char in s, so what's left is just padding bits,
			// ignore them
		}
	} // End of for loop.
	*arr = s;
	//memcpy( arr, s, nStrLength );
	return nStrLength;
}

void XP_API_GetNationality( char * data )
{
#ifdef OS_WIN32
		XP_API_STRCPY( data, "CN" );
#endif

#ifdef OS_IPHONE
		GetUserCountry( data );
#endif
}

int XP_API_GET_RANDOM( char * name )
{
	int offset = 0;
	srand ( (unsigned) time (NULL) );
	if ( NULL != name )
	{
		int len = XP_API_STRLEN( name );
		for ( int i=0; i<len; i++ )
		{
			offset += name[i];
		}
	}
	return ( rand() + offset );
}

int	XP_API_IP_STR2INT( char * ip )
{
	int value = 0;
	char * last = NULL;
	char * curr = NULL;

	last = ip;

	for ( int i=0; i<4; i++ )
	{
		int len = 0;
		char ip_addr[16] = { 0 };

		curr = XP_API_STRSTR( last, "." );
		if ( NULL == curr )
		{
			XP_API_MEMSET( ip_addr, 0, 16 );
			XP_API_STRCPY( ip_addr, last );
			value |= ( XP_API_ATOI( ip_addr ) & 0xFF ) << 8*i;
			break;
		}
		len = curr - last;
		XP_API_MEMSET( ip_addr, 0, 16 );
		XP_API_MEMCPY( ip_addr, last, len );

		value |= ( XP_API_ATOI( ip_addr ) & 0xFF ) << 8*i;

		last = curr+1;
	}
	
	return value;
}

char * XP_API_IP_INT2STR( int ip, char * buffer )
{
	int ip1 = ip & 0x000000FF;
	int ip2 = ( ip & 0x0000FF00 ) >> 8;
	int ip3 = ( ip & 0x00FF0000 ) >> 16;
	int ip4 = ( ip & 0xFF000000 ) >> 24;

	XP_API_SPRINTF( buffer, "%d.%d.%d.%d", ip1, ip2, ip3, ip4 );
	return buffer;
}

//===========================<start :save send and receive data>===========================
FILE *fp_send;
FILE *fp_recv;

void XP_DEBUG_OPEN_SENDDATA_LOG()
{
	fp_send = fopen("log_send.txt", "wt");
}

void XP_DEBUG_CLOSE_SENDDATA_LOG()
{
	fclose(fp_send);
}

void XP_DEBUG_SAVE_SENDDATA_LOG(const char *data, int length)
{
	fwrite(data, sizeof(char), length, fp_send);
}

void XP_DEBUG_OPEN_RECVDATA_LOG()
{
	fp_recv = fopen("log_recv.txt", "wt");
}

void XP_DEBUG_CLOSE_RECVDATA_LOG()
{
	fclose(fp_recv);
}

void XP_DEBUG_SAVE_RECVDATA_LOG(const char *data, int length)
{
	fwrite(data, sizeof(char), length, fp_recv);
}
//====================<end :save send and receive data>======================

int getValue(const char *src, char *dst, int pos , char label)
{
   dst[0] = 0;
   int idx = 0;
   int idx0 = 0;
   while(src[idx] != 0)
   {
      if (src[idx] == label)
      {
         idx++;
         pos--;
         continue;
      }

      if (pos == 0)
      {
         dst[idx0] = src[idx];
         dst[idx0+1] = 0;
         idx0++;
      }
      else if (pos < 0)
      {
         break;
      }
      idx++;
   }
   return 0;
}

void XP_API_TOLOWER(const char *src, char *dest)
{
	if(src == NULL || dest == NULL)
	{
		return;
	}
	
	int i = 0;
	for(; src[i] != '\0'; i++)
	{
		if(src[i] >= 'A' && src[i] <= 'Z')
		{
			dest[i] = src[i] + ('a' - 'A');
		}
		else
		{
			dest[i] = src[i];
		}
	}
	dest[i] = '\0';
}