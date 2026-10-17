

#ifndef __cSTR__  
#define _TXT(text) (text)
#include <wchar.h>

//#import "Util.h"
#import "IGPDefines.h"
#import "strings.h"
#import "DE.h"
#import "FR.h"
#import "IT.h"
#import "JP.h"
#import "KR.h"
#import "SP.h"
#import "EN.h"

#define LANG_EN		0
#define LANG_FR		1
#define LANG_SP		2
#define LANG_DE		3
#define LANG_IT		4
#define LANG_JP		5
#define	LANG_KR		6



class IGPcStr
{
public:
	~IGPcStr();
    static ACHAR*		  _StrMgr_Packs;
    static unsigned int*  _StrMgr_Offsets;

	static void InitMemory( int buffer_size, int nStrings );
	static void Free();
    static void ConvertOffsets( char* offsets );
	static bool LoadTexts(int lang);
	static ACHAR* GetStringsBuffer() { return _StrMgr_Packs; }
    static NSString* GetNSString(int strId);
	static NSString* GetNSStringFromACHAR(ACHAR *str);
	static void  AllocStringBuffers();
	static char* GetFileName(char* filename, char* p);
//	static int isJapaneseCurrentLanguage(int index);
	static char* currentLanguageAsString(int index);
	//static int getLanguageBinSizeOnIndex(int index);
	//static int LANG_INFO[ 6 ][ 4 ];

};

inline ACHAR* Txt( int id )
{
    if( id == 0 )
        return &IGPcStr::_StrMgr_Packs[ 0 ];
	
    return &IGPcStr::_StrMgr_Packs[ IGPcStr::_StrMgr_Offsets[ id -1 ] ];
}

inline int TxtLen ( int id )
{
	if( id == 0)
	{
		return IGPcStr::_StrMgr_Offsets[ 0 ];
	}
	return (IGPcStr::_StrMgr_Offsets[ id ] - IGPcStr::_StrMgr_Offsets[ id - 1 ]);
}


#endif
