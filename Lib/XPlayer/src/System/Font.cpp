#include "stdafx.h"
#include "Font.h"
#include "system/Image.h"
#include "Game.h"
#include "ResourceManager.h"
#include "font_table.h"

typedef char * String ;


//#define DEFAULT_LINE_SPACING -100 (using wu xiao xi define)
#define ARGB_2_RGB565(clr)    ((clr >> 8 ) & 0xf800) | ((clr >> 5 ) & 0x7e0) | ((clr >> 3 ) & 0x1f)

//change the color as you need
const	RGBVAL BrewStringColor[] = { 0xFFFFFF00, //White
									 0xFF000000, //Blue
									 0x00FF0000, //Green
									 0x0000FF00, //Red
									 0x00000000, //Black
									 0xDDDDEE00, //PINK
									 0x11112200, //BROWN
									 0x00CFFF00, //GOLD
									 0x00005200  //GOLDBROWN
									}; //the color is BGR+Reserved

CFont::CFont()
: game_currentLanguage(0)
, _crt_pal(0)
, _need_release(false)
, m_index1(-1)
, m_index2(-1)
, _font_sprite(NULL)
, _font_image(NULL)
{	
	_line_spacing = 1;
	_char_spacing = 1;
	//m_cGame = (( CGame* )(GetGameApp()->m_engine)) ;
}

CFont::~CFont()
{
	if (_need_release)
	{
		if (_font_sprite != NULL)
		{
			SAFE_DELETE(_font_sprite);
		}
		if (_font_image != NULL)
		{
			SAFE_DELETE(_font_image);
		}
	}
}

CImage *CFont::GetImage()
{
	if(_font_sprite != NULL && _font_sprite->_image != NULL)
	{
		return _font_sprite->_image;
	}
	
	return NULL;
}

void CFont::InitFont(ASprite* pSprite, CIndexedImage* pImage)
{
	if (_font_sprite != NULL && _font_sprite != pSprite)
	{
		SAFE_DELETE(_font_sprite);
	}
	if (_font_image != NULL)
	{
		SAFE_DELETE(_font_image);
	}
	_font_sprite = pSprite;
	_font_image = pImage;
	_font_sprite->SetImage(_font_image);

	pImage->EnableColorKey(true);
	pImage->SetColorKey(pImage->GetPaletteData()[0]);
}

void CFont::Load(const char * filename, int imgNum)
{
	ASSERT(!_need_release); 
	_need_release = true;
	_font_sprite = NEW ASprite((const char *) filename, imgNum);
}

 
 
void CFont::SetCurrentPalette(int pal)		{ _crt_pal = pal; }
int CFont::GetCurrentPalette()				{ return _crt_pal; }

int  CFont::GetLineSpacing()				{ return _line_spacing; }
void CFont::SetLineSpacing(int spacing)	{ _line_spacing = spacing; }
void CFont::SetLineSpacingToDefault()		{ _line_spacing = (_font_sprite->GetModuleHeight(0) >> 1); }
void CFont::SetCharSpacing(int spacing)	{ _char_spacing = spacing; }

void CFont::SetSubString(int i1, int i2)
{
    m_index1 = i1;
    m_index2 = i2;
}

void CFont::UpdateStringSize(int num)
{
    AdjustTextSpace();
     
    m_text_w = 0;
    m_text_h = _font_sprite->GetModuleHeight(0);
    int tw = 0;

    if(num < 0)
    {
        num *= -1;
        tw += _char_spacing + _font_sprite->GetFrameWidth(caMap_Char['-']);
    }

    while(num >= 0)
    {
        int c = num % 10 + '0';
        num /= 10;

        tw += _char_spacing + _font_sprite->GetFrameWidth(caMap_Char[c]);
        if(num == 0)
        {
            break;
        }	
    }
    if (tw > m_text_w) m_text_w = tw;
    //if (m_text_w > 0) m_text_w -= _font_sprite->GetFrameWidth(0);
    RestoreTextSpace();
}

void CFont::UpdateStringSize(String s,bool isJpFont)
{
	UpdateStringSizeEN(s);
}

void CFont::UpdateStringSizeEN(String s)
{
    AdjustTextSpace();

    m_text_w = 0;
    m_text_h = _font_sprite->GetModuleHeight(0);
    int tw = 0;
    int old_pal = _crt_pal;
    int index1 = ((m_index1 >= 0) ? m_index1 : 0);
    int index2 = ((m_index2 >= 0) ?m_index2 : (int)strlen(s));

    for (int i = index1; i < index2; i++)
    {
        int c = s[i] & 0xFF;
        if (c == ' ')
        {
            tw += _char_spacing + _font_sprite->GetModuleWidth(0);
            continue;
        }
        else if (c == '|')
        {
            if (tw > m_text_w) m_text_w = tw;
            tw = 0;
            RestoreTextSpace();
            _crt_pal = old_pal;
            AdjustTextSpace();
            m_text_h += _line_spacing + _font_sprite->GetModuleHeight(0);
            continue;
        }
        else if (c == '^')
        {
            i++;
            RestoreTextSpace();
			char temp_char = s[ i ] ;
            _crt_pal = temp_char - '0';
            AdjustTextSpace();
             
            continue;
        }
		c = caMap_Char[c];
//         else if (c == '{')
//         {
//             c = 95;
//         }    
//         else if (c >= 191)
//         {
//             if(c >= 224)
//             {
//                 c -= 32;
//             }
//             c = c - 191 + 64;
//         }
//         else if (c == 186)
//         {
//             c = 96;
//         }
//         else if (c == 161)
//         {
//             c = 63;
//         }
//         else if (c >= 97)
//         {
//             c = c - 97 + 33;
//         }
//         else if (c > 32)
//         {
//             c = c - 32;
//         }

        tw += _char_spacing + _font_sprite->GetFrameWidth(c);
    }
    if (tw > m_text_w) m_text_w = tw;
    //if (m_text_w > 0)
    //{
    //    m_text_w -= _fmodules[1];
    //    m_text_w -= _char_spacing;
    //}
    RestoreTextSpace();
    _crt_pal = old_pal;
}

 
void CFont::DrawNumber(CGraphics * g, int num, int x, int y, int anchor)
{
    bool bDrawMinus = false;

    if(num < 0)
    {
        num *= -1;
        bDrawMinus = true;
    }

    //y -= _fmodules[2];

    UpdateStringSize(num);
    AdjustTextSpace();
    if ((anchor & (CGraphics::RIGHT | CGraphics::HCENTER | CGraphics::BASELINE | CGraphics::VCENTER)) != 0)
    {
        if ((anchor & CGraphics::RIGHT)   != 0)	x -= m_text_w;
        else if ((anchor & CGraphics::HCENTER) != 0)	x -= m_text_w>>1;
        if ((anchor & CGraphics::BASELINE)  != 0)	y -= m_text_h;
        else if ((anchor & CGraphics::VCENTER) != 0)	y -= m_text_h>>1;
    }

    int xx = x + m_text_w;
    int yy = y;

    while(num >= 0)
    {
        int c = num % 10 + '0';
        num /= 10;

        xx -= _char_spacing + _font_sprite->GetFrameWidth(caMap_Char[c]);
			_font_sprite->PaintFModule(g, 0, caMap_Char[c], xx, yy, 0, 0, 0);
        if(num == 0)
        {
            break;
        }
    }

    if(bDrawMinus)
    {
        xx -= _char_spacing + _font_sprite->GetFrameWidth(caMap_Char['-']);
		   _font_sprite-> PaintFModule(g, 0, caMap_Char['-'], xx, yy, 0, 0, 0);
    }
    RestoreTextSpace();
}

void CFont::DrawStringEN(CGraphics * g, String s, int x, int y, int anchor, bool StatesInAbout)
{
    //y -= _fmodules[2];

    if ((anchor & (CGraphics::RIGHT | CGraphics::HCENTER | CGraphics::BASELINE | CGraphics::VCENTER)) != 0)
    {
        UpdateStringSizeEN(s);
        if ((anchor & CGraphics::RIGHT)   != 0)	x -= m_text_w;
        else if ((anchor & CGraphics::HCENTER) != 0)	x -= m_text_w>>1;
        if ((anchor & CGraphics::BASELINE)  != 0)	y -= m_text_h;
        else if ((anchor & CGraphics::VCENTER) != 0)	y -= m_text_h>>1;
    }
    AdjustTextSpace();
    int xx = x;
    int yy = y;

    int old_pal = _crt_pal;

    int index1 = ((m_index1 >= 0) ? m_index1 : 0);
    int index2 = ((m_index2 >= 0) ? m_index2 : (int)strlen(s));

    for (int i = index1; i < index2; i++)
    {
		char temp_char = s[ i ] ;
        int c = temp_char & 0xFF;

        if (c == ' ')
        {
            xx += _char_spacing + _font_sprite->GetFrameWidth(c);
            continue;
        }
        else if (c == '|')
        {
            xx = x;
            yy += _line_spacing + _font_sprite->GetModuleHeight(0);
            RestoreTextSpace();
            _crt_pal = old_pal;              
            AdjustTextSpace();
            continue;
        }
        else if (c == '^')
        {
            i++;
            RestoreTextSpace();
			g->SetARGBColor(0xFFFF0000);
#if IPHONE > 0
			_font_sprite->PaintFrame(g, 146, xx , yy, 0, 0, 0);//146 is the red point.
#elif SCREEN_SIZE == SCREEN_176_208
           g->FillCircle(xx+2,yy+2,3);
#else
	#if SCREEN_SIZE==SCREEN_176_208
			g->FillCircle(xx+2,yy+2,2);
	#else
			g->FillCircle(xx+2,yy+2,4);
	#endif
#endif
			 xx += _char_spacing + _font_sprite->GetFrameWidth(0);
			g->SetARGBColor(0xFFFFFFFF);
            AdjustTextSpace();
            continue;
        }

//         if (c == '{')
//         {
//             c = 95;
//         }    
//         else if (c >= 191)
//         {
//             if(c >= 224)
//             {
//                 c -= 32;
//             }        
//             c = c - 191 + 64;
//         }
//         else if (c == 186)
//         {
//             c = 96;
//         }
//         else if (c == 161)
//         {
//             c = 63;
//         }    
//         else if (c >= 97)
//         {
//             c = c - 97 + 33;
//         }
//         else if (c > 32)
//         {
//             c = c - 32;
//         }
		c = caMap_Char[c];
        //int m = (_fmodules[_frames_fm_start[c]<<2]&0xFF);
        
		if( StatesInAbout && c == FONT_003_MINUS_FRM_INDEX)
			_font_sprite->PaintFModule(g, c, 0, xx, yy + ( FONT_003_MINUS_FRM_HEIGHT), 0, 0, 0);
		else{
			_font_sprite->PaintFrame(g, c, xx , yy, 0, 0, 0);
			
		}

        xx += _char_spacing + _font_sprite->GetFrameModuleWidth(c, 0);
    }

    RestoreTextSpace();
    _crt_pal = old_pal;
}

void CFont::DrawStringCH(CGraphics * g, String s, int x, int y, int anchor)
{
	//unsigned short* src = (unsigned short*)s;
	//   int length = WSTRLEN(src);
	//unsigned short* dest = NEW unsigned short[length + 5];
	//unsigned short* tmpStr = dest;
	//   for(int i = 0; i < length; ++i)
	//   {
	//       if(src[i] != ('|' )) //||(s[i] == '|' && s[i -1] == '|'))
	//       {
	//           *tmpStr = src[i];
	//           ++tmpStr;
	//       }
	//   }
	////chenxiao check if this  operation is needed
	//   *tmpStr = '\0';

	//int width = pFont->GetStringWidth(dest);
	//int height = pFont->GetCharHeight();
	//if ((anchor & (CGraphics::RIGHT | CGraphics::HCENTER | CGraphics::TOP | CGraphics::VCENTER | CGraphics::BASELINE | CGraphics::LEFT)) != 0)
	//   {
	//       if ((anchor & CGraphics::RIGHT)   != 0)		
	//		x -= width;
	//       if ((anchor & CGraphics::HCENTER) != 0)		
	//		x -= width >> 1;
	//       if ((anchor & CGraphics::TOP)  != 0)	
	//		;
	//       if ((anchor & CGraphics::VCENTER) != 0)		
	//		y -= height >> 1;
	//	if ((anchor & CGraphics::BASELINE) != 0)		
	//		y -= height;
	//	if ((anchor & CGraphics::LEFT) != 0)		
	//		;
	//}
	//pFont->SetColor(0xff, 0xff, 0xff);
	//pFont->DrawString(dest, x + 1, y);
	//pFont->DrawString(dest, x - 1, y);
	//pFont->DrawString(dest, x, y + 1);
	//pFont->DrawString(dest, x, y - 1);
	//pFont->SetColor(0x00, 0x00, 0x00);
	//pFont->DrawString(dest, x, y);
	//DELETE[] dest;
}


void CFont::DrawString(CGraphics * g, String s, int x, int y, int anchor)
{
	if (m_nFontImageType == ImageCustom)
	{
	}
	else if(m_nFontImageType == ImageBlack)
	{
		g->SetARGBColor(0xFF101010);
	}
	else if (m_nFontImageType == ImageRed)
	{
		g->SetARGBColor(0xFFFF0000);
	}
	else
	{
		g->SetARGBColor(0xFFFFFFFF);
	}

	DrawStringEN(g, s, x, y, anchor);
} 

void CFont::DrawPage(CGraphics * g, String s, int x, int y, int anchor, int distance)
{
	
#if IPHONE > 0
	if (m_nFontImageType == ImageCustom)
	{
	}
	else if(m_nFontImageType == ImageBlack)
	{
		g->SetARGBColor(0xFF101010);
	}
	else if (m_nFontImageType == ImageRed)
	{
		g->SetARGBColor(0xFFFF0000);
	}
	else
	{
		g->SetARGBColor(0xFFFFFFFF);
	}

	int lines = m_drawPage_nLine;
	int len = (int)strlen(s);
	int off[200];
	memcpy(off, m_drawPageOffset, sizeof(int) * 200);

// 	MEMSET(off,0,100);
// 	for (int i = 0; i < len; i++)
// 	{
// 		char temp_char = s[ i ] ;
// 		if (temp_char == '|')
// 			off[lines++] = i;
// 	}
// 	off[lines++] = len;

	int th = _line_spacing + _font_sprite->GetModuleHeight(0) + distance;

	if ((anchor & CGraphics::BASELINE)  != 0)	y -= (th * (lines-1));
	else if ((anchor & CGraphics::VCENTER) != 0)	y -= (th * (lines-1)) >> 1;

	for (int j = 0; j < lines; j++)
	{
		m_index1 = (j > 0) ? off[j-1]+1 : 0;
		m_index2 = off[j];
		DrawString(g, s, x, y + j * th, anchor);
	}

	m_index1 = -1;
	m_index2 = -1;
#endif
}

int CFont::InitPage(String s, int width)
{

	int lines = 0;
	int len = (int)strlen(s);
	int * off = m_drawPageOffset;
	int tw = 0;
	int th = 0;
	int tw_old = 0;
	m_text_w = 0;

	for(int i=0; i<len; i++)
	{
		char temp_char = s[ i ] ;
		int c = temp_char & 0xFF;

		if (c == ' ')
		{
			tw_old = tw;
			off[lines] = i;

			tw += _char_spacing + _font_sprite->GetFrameWidth(c);

			continue;
		}
		else if (c == '|')
		{
			tw = 0;
			tw_old = 0;

			off[lines++] = i;

			th += _line_spacing + _font_sprite->GetModuleHeight(0);

			continue;
        }
        else if (c == '^')
        {
            i++;

            continue;
        }
		c = caMap_Char[c];
        //int m = (_fmodules[_frames_fm_start[c]<<2]&0xFF);

        tw += _char_spacing + _font_sprite->GetFrameWidth(c);
		
		m_text_w = tw;
        if (tw > width && tw_old != 0) 
        {		    

            tw = tw - tw_old - _char_spacing ;//- _fmodules[1];
			m_text_w = tw_old;
           tw_old = 0;

            lines++;

            th += _line_spacing + _font_sprite->GetModuleHeight(0);
        }
    }
    
    off[lines++] = len;
    m_drawPage_nLine = lines;
	return m_drawPage_nLine;

}

int CFont::DrawPageEN(CGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll, bool StatesInAbout)
{
	return 0;
}

int CFont::DrawPageCH(CGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll)
{
	int scroll_delay;
    if (auto_scroll)
        scroll_delay = SCROLL_DELAY_TIME;
    else
        scroll_delay = 0;

	int x_left = x - (width >> 1);
	int y_top = y - (height >> 1);
	int th = 0;
	//pFont->SetClip(x_left, y_top, width, height);
	//th = LINESPACE + pFont->GetCharHeight();
	//th = 3 + pFont->GetCharHeight();

	int lines = 0;// CHStringWrap(s, width);

	//int total_height = th * lines - LINESPACE;
	int total_height = th * lines - 3;

	if (!auto_scroll)
    {
        return (total_height - scroll);
    }

    if (total_height >= height && scroll >= scroll_delay) 
    {
        return (total_height - height - scroll + (scroll_delay<<1));
    }
    else
    {
        return (total_height - height);
    }
}


int CFont::DrawPage(CGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll,bool StatesInAbout)
{    
	if (game_currentLanguage > 5)
	{
		return DrawPageCH(g, s, x, y, width, height, scroll, auto_scroll);
	}
	else
	{
		return DrawPageEN(g, s, x, y, width, height, scroll, auto_scroll);
	}
}

////////////////////////////////////////////////////////////////////////////

//To set the text color, colors such as BREW_FONT_PINK is defined already 
void CFont::SetTextColor(int font_palette)
{
	int BackColor;
	int ForeColor;
	switch(font_palette)
	{
	case CGame_k_iFont_Pal_0:
		BackColor = BREW_FONT_WHITE;
		ForeColor = BREW_FONT_WHITE;
		break;
	case CGame_k_iFont_Pal_1:
		BackColor = BREW_FONT_BLACK;
		ForeColor = BREW_FONT_BLACK;
		break;
	case CGame_k_iFont_Pal_2:
		BackColor = BREW_FONT_RED;
		ForeColor = BREW_FONT_RED;
		break;
	case CGame_k_iFont_Pal_3:
		BackColor = BREW_FONT_WHITE;
		ForeColor = BREW_FONT_BLACK;
		break;
	case CGame_k_iFont_Pal_4:
		BackColor = BREW_FONT_BLACK;
		ForeColor = BREW_FONT_WHITE;
		break;
	case CGame_k_iFont_Pal_5:
		BackColor = BREW_FONT_WHITE;
		ForeColor = BREW_FONT_RED;
	default:
		BackColor = BREW_FONT_WHITE;
		ForeColor = BREW_FONT_BLUE;
		break;
	}
	m_TextForeColor = BrewStringColor[ForeColor];
	m_TextBackColor = BrewStringColor[BackColor];
}


void CFont::SetColor(int col)
{

#if GRAPHIC_OPENGL > 0
	ASSERT(col >= 0);
#else
	ASSERT(col >= 0 && col < (int)((CIndexedImage*)(_font_sprite->_image))->GetPaletteEntryNumber());
#if NGAGE
	int color = 0;
	if((col == FontSmallWhite) || (col == FontMiddleWhite))
		color = 0xFFFFFF;
	else// if ((col == FontSmallBlack) || (col == FontMiddleBlack))
	{
		color = 0;
	}
	if (GameApp().GetLanguageIndex() == LANGUAGE_CHS)
	{
		s_font_chs.SetFontStyle((CGLFont::FontStyle)m_nFontStyle);
		((CIndexedImage*)s_font_chs.m_pFonts[m_nFontStyle].pFontImage)->SetPaletteColor(color,1);
	}
	else if(GameApp().GetLanguageIndex() == LANGUAGE_CHT)
	{
		s_font_cht.SetFontStyle((CGLFont::FontStyle)m_nFontStyle);
		((CIndexedImage*)s_font_cht.m_pFonts[m_nFontStyle].pFontImage)->SetPaletteColor(color,1);
	}
#endif
	_font_sprite->SetCurrentPalette(col);
#endif

};

void CFont::SetFontColor(int type)
{
#if (IPHONE > 0)

	if(NULL != _font_sprite->_image_array[type] && type < ImageCustom)
	{
		_font_sprite->_image = _font_sprite->_image_array[type];
	}

	m_nFontImageType = type;
#endif
}

