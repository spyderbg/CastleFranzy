#ifndef __CGLLiveFont_H__
#define __CGLLiveFont_H__

#include "GLLivePredefine.h"
#include "Image.h"
#include "MyGraphics.h"
#include "GLLive_Stream.h"
#include "GLLive_ASprite.h"
#include "GLLiveGame.h"

class CGLLive ;
class CGLLiveASprite;
typedef char * String ;

#define FONT_BIG	0
#define FONT_MID	1
#define FONT_SMALL	2

#define FONT_COLOR_WHITE	0xFFFFFFFF
#define FONT_COLOR_BLACK	0xFF000000

#define MAX_PAGE_LINE	1000

class CGLLiveFont
{
public:

	CGLLiveFont(); 
	~CGLLiveFont();

	void Load(const char * file, const char * imgname = NULL);

	void InitFont(CGLLiveASprite* ,CGLLiveIndexedImage* );

	CGLLiveImage *GetImage();

	void SetCurrentPalette(int pal);
	int GetCurrentPalette();
	int GetLineSpacing();
	int GetLineHeight();
	void SetLineSpacing(int spacing);
	void SetLineSpacingToDefault();
	void SetCharSpacing(int spacing);
	void SetSubString(int i1, int i2);
	void UpdateStringSize(int num);
	void UpdateStringSize(String s,bool isJpFont = false);
	void UpdateStringSizeEN(String s);
	int  GetStringSize(){return m_text_w;}
	void DrawNumber(CGLLiveGraphics * g, int num, int x, int y, int anchor);

	void DrawString(CGLLiveGraphics * g, String s, int x, int y, int anchor, int limitLen);
	void DrawString(CGLLiveGraphics * g, String s, int x, int y, int anchor);
	void ResetScrollCounter()	{ m_scrollCounter = 0; }
	void DrawScrollString(CGLLiveGraphics * g, String s, int x, int y, int width);

	void DrawStringEN(CGLLiveGraphics * g, String s, int x, int y, int anchor, bool StatesInAbout = false);
#if IPHONE > 0
	void DrawPage(CGLLiveGraphics * g, String s, int x, int y, int anchor, int line_limit = 0);
#else
	void DrawPage(CGLLiveGraphics * g, String s, int x, int y, int anchor, int distance = 0);
#endif
	int InitPage(String s, int width);
	int DrawPage(CGLLiveGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll,bool StatesInAbout = false);
	int DrawPageEN(CGLLiveGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll, bool StatesInAbout = false);
	void DrawStringCH(CGLLiveGraphics * g, String s, int x, int y, int anchor);
	int DrawPageCH(CGLLiveGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll);
	int * GenPalette(int type, int * pal, int length); //length == pal.length;   
	void SetColor(int col);

	void SetFontColor(int color);


// Variables for DrawBrewString, should in .h file
//MAX_TEXT_LENGTH is defined as the MAX length of your sentence
	char	m_stringWrapped[MAX_TEXT_LENGTH];
	char	m_stringDelWrap[MAX_TEXT_LENGTH];
	int		m_off[MAX_TEXT_LINES];
	RGBVAL	m_TextForeColor;
	RGBVAL	m_TextBackColor;
	int		_text_w;
	int		_text_h;

	bool	String_InPage;

	int CHStringWrap(const char* s, int width);
	void SetTextColor(int font_palette);
	void InitLinkPosition(char* str, int x, int y, int width, int index1, int index2, int* position);


private:
	int	GetChar(char * string, int& index);
	void DRAW_STRINGS_SYSTEM___() {}
	
	void AdjustTextSpace()
	{
		//_char_spacing -= 1;
        //_line_spacing -= 1;
	}

	void RestoreTextSpace()
	{
		//_char_spacing += 1;
		//_line_spacing += 1;
	}

	void ___DRAW_STRINGS_SYSTEM() {}

public:

	int	        _crt_pal;

	CGLLiveIndexedImage * _font_image;
	CGLLiveASprite * _font_sprite;

	int _m_nColors;

	char *  _map_char;
	int _line_spacing ;
	int _char_spacing ;
	bool _need_release;

	CGLLive * m_pGame ;

private:
	public:
	char *              m_tempString ;

	int *               m_temp;
	int                 m_drawPageOffset[MAX_PAGE_LINE] ;
	int                 m_drawPage_nLine;
	bool                m_isDrawPageInit;
	int                 m_index1 ;
	int                 m_index2 ;
	int                 m_text_w ;
	int                 m_text_h;
	//////////////////////////////////////////////////////////////////////////
	int                 game_currentLanguage;
	

	int					m_nFontColor;
	int					m_scrollCounter;
	bool				m_needIgnoreFormat;

};

#endif
