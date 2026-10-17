////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////




#import "IGPcStr.h"
#import "IGPUtil.h"
#import "IGPResLib.h"
#import "langinfo.h"



ACHAR*				IGPcStr::_StrMgr_Packs		=	NULL;
unsigned int*		IGPcStr::_StrMgr_Offsets	=	NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
IGPcStr::~IGPcStr()
{
	SAFE_DELETE( _StrMgr_Packs	);
	SAFE_DELETE( _StrMgr_Offsets );
}
void IGPcStr::InitMemory( int buffer_size, int nStrings )
{	
	
	_StrMgr_Packs	= NEW ACHAR[ buffer_size ];
	_StrMgr_Offsets	= NEW unsigned int[ nStrings ];
}


void IGPcStr::Free()
{
	SAFE_DELETE( _StrMgr_Packs	);
	SAFE_DELETE( _StrMgr_Offsets );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void  IGPcStr::AllocStringBuffers()
{
	int nNoLang;
	nNoLang = LANG_COUNT;
	
	int max_bin_size = 0;
	
	for( int i = 0; i < nNoLang; i++ )
	{
		if( max_bin_size < LANG_INFO[ i ][ 2 ] )
			max_bin_size = LANG_INFO[ i ][ 2 ];
	}
	
	InitMemory( max_bin_size, NUM_STRINGS_EN );
}
////////////////////////////////////////////////////////////////////////////////////////////////////

void IGPcStr::ConvertOffsets( char* offsets )
{
    int i, off, nStrings;

    //if( offsets != null )
    {
        nStrings            = (offsets[0] & 0xFF) | ((offsets[1] & 0xFF) << 8);

	//	GX_ASSERT( nStrings <= NUM_STRINGS_EN );

        for( i = 0, off = 2; i < nStrings; i++ )
        {
            _StrMgr_Offsets[ i ] = (unsigned int)((( offsets[ off + 0 ] & 0xFF) | 
												((offsets[ off + 1 ] & 0xFF) << 8 ) ) / ACHAR_SIZE);
			if(i>0 && _StrMgr_Offsets[ i ] < _StrMgr_Offsets[ i - 1])
				_StrMgr_Offsets[ i ] += (1<<15);

            off += 2;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool IGPcStr::LoadTexts(int lang)
{

	int sizeOff, fileIndex;
	char* sp_off;
	
	fileIndex	= LANG_INFO[ lang ][ 0 ];
	sizeOff		= LANG_INFO[ lang ][ 3 ];
	
	sp_off  = NEW char[ sizeOff ];
	
	//Open the strings library
	
	char filename[255];
	GetFileName ("strings.bar", filename);
	
	IGPResLib* lib = IGPResLib::Open( filename, -1, 0);
	
	
	char* tmp = NULL;
	lib->ReadFile( tmp, fileIndex );
	int nMaxBufferSize = lib->GetFileSize( fileIndex ) / ACHAR_SIZE;
	int index = 0;
	for(int i = 0; i < nMaxBufferSize; i++)
	{
		_StrMgr_Packs[i] = (ACHAR) (((tmp[index] & 0xFF)) | ((tmp[index + 1] & 0xFF) << 8));
		index += 2;
	}
	
	lib->ReadFile( sp_off, fileIndex + 1);
	
	SAFE_DELETE(tmp);
	tmp = NULL;
	 
	IGPResLib::Close( lib );
	
    ConvertOffsets( sp_off );
	
    SAFE_DELETE( sp_off );

	//GetUsedBigChars(TRUE);
	//GetUsedOtherChars(TRUE);
    
	return TRUE;
}


NSString* IGPcStr::GetNSString(int strId)
{
	//ACHAR* str= Txt(strId);

	int strLength=0; //= sizeof(Txt(strId))/sizeof(ACHAR);
	ACHAR* str = Txt(strId);
	
	
// 	while( != 0)
	//int i = 0;
	while(str[strLength] != 0)
		strLength++;
	
/*	printf("---------------------------------------\n");
	for(int r = 0;r<5;r++)
	{
		printf(" %c:%d ",str[r],str[r]);
//		strLength++;
	}
	printf("---------------------------------------\n");*/
			
		
	//return [NSString stringWithCharacters:(unichar*)Txt(strId) length:(TxtLen(strId)*2 - 1)];
	NSMutableString *tempString = [[NSMutableString alloc] initWithCapacity:strLength];
	for (int i=0;i < strLength;i++)
	{
		/*c = [myString characterAtIndex:i];
		if (<TEST c here...>)
		{*/
			[tempString appendFormat:@"%C",str[i]];
		//}
	}
	[tempString autorelease];
	return tempString;

}

NSString* IGPcStr::GetNSStringFromACHAR(ACHAR *str)
{
	#define GX_STRLEN(string)				wcslen(string)
	return [NSString stringWithCharacters:(unichar*)str length:(GX_STRLEN(str)*2 - 1)];
}

char* IGPcStr::GetFileName(char* filename, char* p)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	sprintf(p, "%s", filename);
	
	NSString *path = [NSString  stringWithUTF8String:p];
	
	if(![path isAbsolutePath])
	{
		path = [[NSBundle mainBundle] pathForResource:path ofType:nil];
	}
	
	sprintf(p, "%s", [path UTF8String]);
	
	//if (path) [path release];
	
	[pool release];
	return NULL;
}

//check if currently selected language is japanese
/*int cStr::isJapaneseCurrentLanguage(int index)
{
	return LANG_INFO[index][2] == BIN_SIZE_JP;

}*/

//used to determine which language is being used
char* IGPcStr::currentLanguageAsString(int index)
{
	return LANG_ORDER[index];
}
