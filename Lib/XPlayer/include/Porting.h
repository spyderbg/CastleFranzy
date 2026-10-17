#ifndef __PORTING_H__
#define __PORTING_H__

#include "Game_defines.h"
#include <stddef.h>
/**
 * It is designed for porting XPlayer to any platform easily.
 *
 *
 * @version 0.0.1
 *
 */

#ifdef OS_WIN32
#define DEBUG_MEMORY_LEAK
#endif

#ifdef DEBUG_MEMORY_LEAK		
	//#define XP_API_NEW new(__FILE__, __LINE__)
	#define XP_API_NEW				new
#else
	#define XP_API_NEW				new
#endif

	#define XP_API_DEL(a)			{ if(a) { delete (a); a=NULL; } }
	#define XP_API_DELETE_ARRAY(a)	{ if(a) { delete [] (a); a=NULL; } }
	#define XP_API_DEL_ARRAY(p, n)	{ if(p) { for (int __i = 0; __i < n;__i++) XP_API_DELETE_ARRAY((p)[__i]); delete[] (p); p = NULL; } }

#ifndef BYTE
	typedef unsigned char	BYTE;
#endif

#ifndef NULL
	#define NULL			0
#endif

#define XP_FILE_HANDLE		int

#ifdef __cplusplus
extern "C" {
#endif

//void   XP_API_DEL( void * data );
//void   XP_API_DEL_ARRAY( char ** data, int len );

/**
 * String
 */
int    XP_API_STRLEN( const char * string );
char * XP_API_STRNEW( const char * string );
int	   XP_API_STRCMP( const char * p1, const char * p2 );
int	   XP_API_STRNCMP( const char * p1, const char * p2, int size );
char * XP_API_STRCPY( char * dest, const char * src );
char * XP_API_STRNCPY( char * dest, const char * src, int count );
char * XP_API_STRCAT( char * dest, const char * src );
char * XP_API_STRTRIM( char * src );
char * XP_API_STRSTR( const char * src, const char * substr );
char * XP_API_STRCHR( const char * src, const char subchar, int bFromLeft2Right );
char * XP_API_STRCHRFIND( char * src, const char subchar, int count );

/**
 * Memory
 */
void * XP_API_MALLOC( int size );
void   XP_API_FREE( void * p );
void   XP_API_MEMSET( void * data, int value, int size );
void * XP_API_MEMCPY( void * data, const void * src, int size );

int    XP_API_SPRINTF( char * data, const char * format, ... );
int    XP_API_ATOI( const char * data );
char * XP_API_ITOA( int num, char * str, int radix );
char * XP_API_FTOA( float num, char * str, int digits );
double XP_API_ATOF( char * str );

/**
 * Debug information
 */
void   XP_DEBUG_INIT();
void XP_DEBUG_FINALIZE();
void   XP_DEBUG_OUT( const char * format, ... );

/**
 * File process
 */
XP_FILE_HANDLE XP_API_FILE_OPEN( const char * filename, const char * flag );
void   XP_API_FILE_CLOSE( XP_FILE_HANDLE handle );
int    XP_API_FILE_READ( void * dest, int element_size, int count, XP_FILE_HANDLE handle );
int    XP_API_FILE_WRITE( void * src, int element_size, int count, XP_FILE_HANDLE handle );
int    XP_API_FILE_GET_LENGTH( XP_FILE_HANDLE handle );

/**
 * Dara parser.
 */
int    XP_API_PARSE_DATA( const char * src, char *dst, int pos, char seperator );
int    XP_API_PARSE_DATA_NEW( const char * src, char **dst, int pos, char seperator );

int	   XP_API_GET_RANDOM( char * name );

int    XP_API_IP_STR2INT( char * ip );
char * XP_API_IP_INT2STR( int ip, char * buffer );

int getValue(const char *src, char *dst, int pos, char label);

/**
 * Date time.
 */
long   XP_API_GET_TIME();

#ifdef LOBBY_THREAD_TEST
void GetCurrentSystemTime( char** string );
#endif //LOBBY_THREAD_TEST

#ifdef __cplusplus
}
#endif

/**
 * Encoding
 */
int ConvertUnicodeToUTF8( char* output, const Unicode2Bytes* input, const int lenInput );
int ConvertUTF8ToUnicode( Unicode2Bytes* output, const char* input, const int lenInput );
int TranslateUnicode2Utf8(char **nameOutput, Unicode2Bytes* nameInput, int lenInput, int bNeedNewMEM);
int TranslateUtf82Unicode(Unicode2Bytes **nameOutput, char* nameInput, int lenInput, int bNeedNewMEM);


//>> sujianhua for wchar
#ifdef UNICODE
#define USE_WCHAR
#endif

#ifdef OS_IPHONE
#define USE_WCHAR
#endif


//<<end for wchar
#ifdef USE_WCHAR

int TranslateWChar2Utf8(char **nameOutput, wchar_t* nameInput, int lenInput, int bNeedNewMEM);
int TranslateUtf82WChar(wchar_t **nameOutput, char* nameInput, int lenInput, int bNeedNewMEM);

#endif

void   UTF8ToUnicode_Old(unsigned short *strsrc, const char *pUTF8);
int    ConvertUnicodeToUTF8_Old( char *strdest,  const unsigned short *strsrc );

char * SSEncDec_String2Blob(char* s);
char * SSEncDec_Blob2String(char* s);
char * SSEncDec_ByteArray2Blob(BYTE* s, int len);
int SSEncDec_Blob2ByteArray(BYTE* *array, char* blob);
char SSEncDec_GetKeyFromChar(char nChar);
void   XP_API_GetNationality( char * data );

//===========================<start :save send and receive data>===========================
void XP_DEBUG_OPEN_SENDDATA_LOG();
void XP_DEBUG_CLOSE_SENDDATA_LOG();
void XP_DEBUG_SAVE_SENDDATA_LOG(const char *data, int length);

void XP_DEBUG_OPEN_RECVDATA_LOG();
void XP_DEBUG_CLOSE_RECVDATA_LOG();
void XP_DEBUG_SAVE_RECVDATA_LOG(const char *data, int length);
//===========================<end :save send and receive data>===========================

void XP_API_TOLOWER(const char *src, char *dest);

#endif // __PORTING_H__