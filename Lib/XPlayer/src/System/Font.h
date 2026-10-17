#ifndef __CFont_H__
#define __CFont_H__

#include "Predefine.h"
#include "system/Image.h"
#include "system/MyGraphics.h"
#include "system/Stream.h"
#include "system/ASprite.h"
#include "Game.h"

class CGame ;
class ASprite;
typedef char * String ;


enum FontColor
{
	FontSmallWhite	= 0,
	FontSmallBlack	= 1,
	FontSmallRed	= 2,
	FontSmallOrange	= 3,
	FontSmallDarkRed = 4,
	FontMiddleWhite	= 0,
	FontMiddleBlack	= 1
};

enum ImageType
{
	ImageWhite = 0,
	ImageBlack = 1,
	ImageRed = 2,
	ImageLightBlack = 3,
	ImageCustom = 4,
};

class CFont
{
public:
	enum FontSize
	{
		FS_Small = 0,
		FS_Middle,
		FS_Large,
		FS_Large25,
		FS_Count,
	};

	CFont(); 
	~CFont();

	void Load(const char * file, int imgNum = 1);

	void InitFont(ASprite* ,CIndexedImage* );

	CImage *GetImage();

	void SetCurrentPalette(int pal);
	int GetCurrentPalette();
	int GetLineSpacing();
	void SetLineSpacing(int spacing);
	void SetLineSpacingToDefault();
	void SetCharSpacing(int spacing);
	void SetSubString(int i1, int i2);
	void UpdateStringSize(int num);
	void UpdateStringSize(String s,bool isJpFont = false);
	void UpdateStringSizeEN(String s);
	void DrawNumber(CGraphics * g, int num, int x, int y, int anchor);

	void DrawString(CGraphics * g, String s, int x, int y, int anchor);

	void DrawStringEN(CGraphics * g, String s, int x, int y, int anchor, bool StatesInAbout = false);
#if IPHONE > 0
	void DrawPage(CGraphics * g, String s, int x, int y, int anchor, int distance = 3);
#else
	void DrawPage(CGraphics * g, String s, int x, int y, int anchor, int distance = 0);
#endif
	int InitPage(String s, int width);
	int DrawPage(CGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll,bool StatesInAbout = false);
	int DrawPageEN(CGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll, bool StatesInAbout = false);
	void DrawStringCH(CGraphics * g, String s, int x, int y, int anchor);
	int DrawPageCH(CGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll);
	int * GenPalette(int type, int * pal, int length); //length == pal.length;   
	void SetColor(int col);

	void SetFontColor(int type);


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


private:
	void DRAW_STRINGS_SYSTEM___() {}
	
	void AdjustTextSpace()
	{
        //if(_crt_pal < 3)
        //{
        //    _char_spacing -= 1;
        //    _line_spacing -= 1;
        //}
	}

	void RestoreTextSpace()
	{
        //if(_crt_pal < 3)
        //{
        //    _char_spacing += 1;
        //    _line_spacing += 1;
        //}
	}

	void ___DRAW_STRINGS_SYSTEM() {}

public:

	int	        _crt_pal;

	CIndexedImage * _font_image;
	ASprite * _font_sprite;

	int _m_nColors;

	char *  _map_char;
	int _line_spacing ;
	int _char_spacing ;
	bool _need_release;

	CGame * m_pGame ;

private:
	public:
	char *              m_tempString ;

	int *               m_temp;
	int                 m_drawPageOffset[200] ;
	int                 m_drawPage_nLine;
	bool                m_isDrawPageInit;
	int                 m_index1 ;
	int                 m_index2 ;
	int                 m_text_w ;
	int                 m_text_h;
	//////////////////////////////////////////////////////////////////////////
	int                 game_currentLanguage;
	

	int					m_nFontImageType;

};

#endif
