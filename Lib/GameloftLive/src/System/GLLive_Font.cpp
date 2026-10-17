#include "GLLiveStdafx.h"
#include "GLLive_Font.h"
#include "Image.h"
#include "GLLiveGame.h"
#include "ResourceManager.h"
#include "FontTable.h"

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

CGLLiveFont::CGLLiveFont()
: game_currentLanguage(0)
, _crt_pal(0)
, _need_release(false)
, m_index1(-1)
, m_index2(-1)
, _font_sprite(NULL)
, _font_image(NULL)
, m_nFontColor(0xFFFFFFFF)
{	
	_line_spacing = 4;
	_char_spacing = 1;
	//m_cGame = (( CGame* )(GetGameApp()->m_engine)) ;
}

CGLLiveFont::~CGLLiveFont()
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

CGLLiveImage *CGLLiveFont::GetImage()
{
	if(_font_sprite != NULL && _font_sprite->_image != NULL)
	{
		return _font_sprite->_image;
	}
	
	return NULL;
}

void CGLLiveFont::InitFont(CGLLiveASprite* pSprite, CGLLiveIndexedImage* pImage)
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

void CGLLiveFont::Load(const char * filename, const char * imgname)
{
	ASSERT(!_need_release); 
	_need_release = true;
	_font_sprite = NEW CGLLiveASprite((const char *) filename, imgname);
}

 
 
void CGLLiveFont::SetCurrentPalette(int pal)		{ _crt_pal = pal; }
int CGLLiveFont::GetCurrentPalette()				{ return _crt_pal; }

int  CGLLiveFont::GetLineSpacing()				{ return _line_spacing; }
void CGLLiveFont::SetLineSpacing(int spacing)	{ _line_spacing = spacing; }
void CGLLiveFont::SetLineSpacingToDefault()		{ _line_spacing = (_font_sprite->GetModuleHeight(0) >> 1); }
void CGLLiveFont::SetCharSpacing(int spacing)	{ _char_spacing = spacing; }

int CGLLiveFont::GetLineHeight()
{
	return _line_spacing + _font_sprite->GetModuleHeight(0);
}

void CGLLiveFont::SetSubString(int i1, int i2)
{
    m_index1 = i1;
    m_index2 = i2;
}

void CGLLiveFont::UpdateStringSize(int num)
{
	m_needIgnoreFormat = false;
    AdjustTextSpace();
     
    m_text_w = 0;
    m_text_h = _font_sprite->GetModuleHeight(0);
    int tw = 0;

    if(num < 0)
    {
        num *= -1;
        tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, '-' - 32);
    }

    int count = 0;
	while(num >= 0)
    {
		count++;
		int c = num % 10 + '0';
        num /= 10;

        tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, c - 32);
        if(num == 0)
        {
            break;
        }	
    }
	if((count - 1) / 3 > 0)
	{
		char ch = 0;
		switch(g_pGLLiveInstance->m_Language)
		{
		case 0: //EN
		case 1: //JP(instead by EN)
			ch = ',';
			break;
		case 2: //DE
			ch = '.';
			break;
		case 3: //FR
		case 4: //IT
		case 5: //SP
			ch = ' ';
			break;
		}
		if((g_pGLLiveInstance->m_Language == 3 || g_pGLLiveInstance->m_Language == 5) && count == 4)
		{
			m_needIgnoreFormat = true;
		}
		else
		{
			tw += (_char_spacing + _font_sprite->GetFrameModuleWidth(0, ch - 32)) * (count / 3);
		}
	}
    if (tw > m_text_w) m_text_w = tw;
    //if (m_text_w > 0) m_text_w -= _font_sprite->GetFrameWidth(0);
    RestoreTextSpace();
}

void CGLLiveFont::UpdateStringSize(String s,bool isJpFont)
{
	UpdateStringSizeEN(s);
}

void CGLLiveFont::UpdateStringSizeEN(String s)
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
        //int c = s[i] & 0xFF;
		int c = GetChar(s, i);
        if (c == ' ')
        {
            tw += _char_spacing + _font_sprite->GetModuleWidth(0);
            continue;
        }
#ifdef USE_LINE_CHANGE_LINE 
        else if (c == '|' || c == '\n')
#else
        else if (c == '\n')
#endif
        {
            if (tw > m_text_w) m_text_w = tw;
            tw = 0;
            RestoreTextSpace();
            _crt_pal = old_pal;
            AdjustTextSpace();
            m_text_h += _line_spacing + _font_sprite->GetModuleHeight(0);
            continue;
        }
//     else if (c == '^')
//     {
//         i++;
//         RestoreTextSpace();
		//char temp_char = s[ i ] ;
//         _crt_pal = temp_char - '0';
//         AdjustTextSpace();
//          
//         continue;
//     }
		if(c < 128)
			c = c - 32;//caMap_Char[c];
#ifdef USE_OR_CHAR_TABLE
		else
			c = 0;
#endif
		if(c < 0)
			continue;

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

        tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, c);
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

 
void CGLLiveFont::DrawNumber(CGLLiveGraphics * g, int num, int x, int y, int anchor)
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
    if ((anchor & (CGLLiveGraphics::RIGHT | CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE | CGLLiveGraphics::VCENTER)) != 0)
    {
        if ((anchor & CGLLiveGraphics::RIGHT)   != 0)	x -= m_text_w;
        else if ((anchor & CGLLiveGraphics::HCENTER) != 0)	x -= m_text_w>>1;
        if ((anchor & CGLLiveGraphics::BASELINE)  != 0)	y -= m_text_h;
        else if ((anchor & CGLLiveGraphics::VCENTER) != 0)	y -= m_text_h>>1;
    }

    int xx = x + m_text_w;
    int yy = y;

    int count = 0;
	while(num >= 0)
    {
        int c = 0;
		
        if(count % 4 == 3 && count != 0)
		{
			switch(g_pGLLiveInstance->m_Language)
			{
			case 0: //EN
			case 1: //JP(instead by EN)
				c = ',';
				break;
			case 2: //DE
				c = '.';
				break;
			case 3: //FR
			case 4: //IT
			case 5: //SP
				c = ' ';
				break;
			}
		}
		else
		{
			c = num % 10 + '0';
			num /= 10;
		}
		count++;

		if(!(c == ' ' && m_needIgnoreFormat))
		{
			xx -= _char_spacing + _font_sprite->GetFrameModuleWidth(0, c - 32);
			_font_sprite->PaintFModule(g, 0, c - 32, xx, yy, 0, 0, 0);
		}

        if(num == 0)
        {
            break;
        }
    }

    if(bDrawMinus)
    {
		xx -= _char_spacing + _font_sprite->GetFrameModuleWidth(0, '-' - 32);
		_font_sprite-> PaintFModule(g, 0, '-' - 32, xx, yy, 0, 0, 0);
    }
    RestoreTextSpace();
}

void CGLLiveFont::DrawStringEN(CGLLiveGraphics * g, String s, int x, int y, int anchor, bool StatesInAbout)
{
    //y -= _fmodules[2];

    if ((anchor & (CGLLiveGraphics::RIGHT | CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE | CGLLiveGraphics::VCENTER)) != 0)
    {
        UpdateStringSizeEN(s);
        if ((anchor & CGLLiveGraphics::RIGHT)   != 0)	x -= m_text_w;
        else if ((anchor & CGLLiveGraphics::HCENTER) != 0)	x -= m_text_w>>1;
        if ((anchor & CGLLiveGraphics::BASELINE)  != 0)	y -= m_text_h;
        else if ((anchor & CGLLiveGraphics::VCENTER) != 0)	y -= m_text_h>>1;
    }
    AdjustTextSpace();
	//add by liwei2
	x += (_char_spacing >> 1);
    int xx = x;
    int yy = y;

    int old_pal = _crt_pal;

    int index1 = ((m_index1 >= 0) ? m_index1 : 0);
    int index2 = ((m_index2 >= 0) ? m_index2 : (int)strlen(s));

    for (int i = index1; i < index2; i++)
    {
		//char temp_char = s[ i ] ;
        //int c = temp_char & 0xFF;
		int c = GetChar(s, i);

        if (c == ' ')
        {
            xx += _char_spacing + _font_sprite->GetFrameModuleWidth(0, 0);
            continue;
        }
#ifdef USE_LINE_CHANGE_LINE 
        else if (c == '|' || c == '\n')
#else
        else if (c == '\n')
#endif
        {
            xx = x;
            yy += _line_spacing + _font_sprite->GetFrameModuleHeight(0, 0);
            RestoreTextSpace();
            _crt_pal = old_pal;              
            AdjustTextSpace();
            continue;
        }
//        else if (c == '^')
//        {
//            i++;
//            RestoreTextSpace();
//			g->SetARGBColor(0xFFFF0000);
//#if IPHONE > 0
//			_font_sprite->PaintFModule(g, 0, '@' - 32, xx , yy, 0, 0, 0);//146 is the red point.
//#elif SCREEN_SIZE == SCREEN_176_208
//           g->FillCircle(xx+2,yy+2,3);
//#else
//	#if SCREEN_SIZE==SCREEN_176_208
//			g->FillCircle(xx+2,yy+2,2);
//	#else
//			g->FillCircle(xx+2,yy+2,4);
//	#endif
//#endif
//			 xx += _char_spacing + _font_sprite->GetFrameModuleWidth(0, 0);
//			g->SetARGBColor(0xFFFFFFFF);
//            AdjustTextSpace();
//            continue;
//        }

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
		if(c < 128)
			c = c - 32;//caMap_Char[c];
#ifdef USE_OR_CHAR_TABLE
		else
			c = 0;
#endif
		if(c < 0)
			continue;
        //int m = (_fmodules[_frames_fm_start[c]<<2]&0xFF);
        
		if( StatesInAbout && c == FONT_003_MINUS_FRM_INDEX)
			_font_sprite->PaintFModule(g, c, 0, xx, yy + ( FONT_003_MINUS_FRM_HEIGHT), 0, 0, 0);
		else{
			_font_sprite->PaintFModule(g, 0, c, xx , yy, 0, 0, 0);
		}

        xx += _char_spacing + _font_sprite->GetFrameModuleWidth(0, c);
    }

    RestoreTextSpace();
    _crt_pal = old_pal;
}

void CGLLiveFont::DrawStringCH(CGLLiveGraphics * g, String s, int x, int y, int anchor)
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
	//if ((anchor & (CGLLiveGraphics::RIGHT | CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP | CGLLiveGraphics::VCENTER | CGLLiveGraphics::BASELINE | CGLLiveGraphics::LEFT)) != 0)
	//   {
	//       if ((anchor & CGLLiveGraphics::RIGHT)   != 0)		
	//		x -= width;
	//       if ((anchor & CGLLiveGraphics::HCENTER) != 0)		
	//		x -= width >> 1;
	//       if ((anchor & CGLLiveGraphics::TOP)  != 0)	
	//		;
	//       if ((anchor & CGLLiveGraphics::VCENTER) != 0)		
	//		y -= height >> 1;
	//	if ((anchor & CGLLiveGraphics::BASELINE) != 0)		
	//		y -= height;
	//	if ((anchor & CGLLiveGraphics::LEFT) != 0)		
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

void CGLLiveFont::DrawString(CGLLiveGraphics * g, String s, int x, int y, int anchor, int limitLen)
{
	//s must not be NULL
	if(!s)
		return;
	
	String str = XP_API_STRNEW(s);
	UpdateStringSize(str);
	int len = STRLEN(str);
	if(m_text_w > limitLen )
	{
		str[len - 2] = '.';
		str[len - 1] = '.';
		str[len] = 0;
		UpdateStringSize(str);
		len = STRLEN(str);
		while(m_text_w > limitLen)
		{
			if(len > 3)
			{
				str[len - 3] = '.';
				str[len - 2] = '.';
				str[len - 1] = 0;
			}
			else if(len == 3)
			{
				if(str[1] == '.' && str[2] == '.')
				{
					str[2] = 0; //set length to 2
				}
				else
				{
					str[1] = '.';
					str[2] = '.';
				}
			}
			else if(len == 2)
			{
				if(str[0] == '.' && str[1] == '.')
				{
					str[1] = 0;//set length to 1
				}
				else
				{
					str[0] = '.';
					str[1] = '.';
				}
			}
			else if(len == 1)
			{
				if(str[0] == '.')
				{
					//str[0] = 0;
					break;
				}
				else
				{
					str[0] = '.';
				}
			}
			else
			{
				break;
			}
			UpdateStringSize(str);
			len = STRLEN(str);
		}
	}
	//if(str[STRLEN(str)-3] == ' ')
	//	str[STRLEN(str)-3] = '.';
	
	len = STRLEN(str);
	if(len > 3 && str[len - 3] == ' ' && str[len - 2] == '.' && str[len - 1] == '.')
	{
		str[len - 3] = '.';
	}
	DrawString(g, str, x, y, anchor);
	XP_API_DELETE_ARRAY(str);
}

void CGLLiveFont::DrawString(CGLLiveGraphics * g, String s, int x, int y, int anchor)
{
	g->EnableTextureBlend(true);
	g->SetARGBColor(m_nFontColor);
	DrawStringEN(g, s, x, y, anchor);
	g->EnableTextureBlend(false);
}

void CGLLiveFont::DrawScrollString(CGLLiveGraphics * g, String s, int x, int y, int width)
{
	g->SaveClip();
	g->SetClip(x, y, width, _font_sprite->GetModuleHeight(0));
	m_scrollCounter++;
	int offx = m_scrollCounter;
	if(offx > m_text_w + width / 2 || m_scrollCounter < 0)
	{
		m_scrollCounter = 0;
	}
	DrawString(g, s, x + width / 2 - offx, y, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	g->LoadClip();
}

void CGLLiveFont::DrawPage(CGLLiveGraphics * g, String s, int x, int y, int anchor, int line_limit)
{
	
#if IPHONE > 0

	int lines;
	if(line_limit > 0 && m_drawPage_nLine > line_limit)
		lines = line_limit - 1;
	else
		lines = m_drawPage_nLine;

	int len = (int)strlen(s);
#if 0
	int off[MAX_PAGE_LINE];
	memcpy(off, m_drawPageOffset, sizeof(off));

// 	MEMSET(off,0,100);
// 	for (int i = 0; i < len; i++)
// 	{
// 		char temp_char = s[ i ] ;
// 		if (temp_char == '|')
// 			off[lines++] = i;
// 	}
// 	off[lines++] = len;
#else
	int * off = m_drawPageOffset;
#endif

	int th = _line_spacing + _font_sprite->GetModuleHeight(0);

	if ((anchor & CGLLiveGraphics::BASELINE)  != 0)	y -= (th * (lines-1));
	else if ((anchor & CGLLiveGraphics::VCENTER) != 0)	y -= (th * (lines-1)) >> 1;

	for (int j = 0; j < lines; j++)
	{
		m_index1 = (j > 0) ? off[j-1]+1 : 0;
#ifdef USE_LINE_CHANGE_LINE 
		if(s[off[j]] != ' ' && s[off[j]] != '|' && s[off[j]] != '\n' && s[off[j]] != '\0')
#else
		if(s[off[j]] != ' ' && s[off[j]] != '\n' && s[off[j]] != '\0')
#endif

		{
			m_index2 = off[j] + 1 ;
		}
		else
		{
			m_index2 = off[j];
		}
		//if the line is not in clip area, continue
		if((y + (j + 1) * th) < g->GetClipY() || (y + (j - 1) * th) > (g->GetClipY() + g->GetClipHeight()))
			continue;
		DrawString(g, s, x, y + j * th, anchor);
	}
	m_index1 = -1;
	m_index2 = -1;

	if(lines > m_drawPage_nLine)
		DrawString(g, "...", x, y + lines * th, anchor);
#endif
}

int CGLLiveFont::InitPage(String s, int width)
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
		//char temp_char = s[ i ] ;
		//int c = temp_char & 0xFF;
		int _i = i - 1;
		int c = GetChar(s, i);

		if (c == ' ')
		{
			off[lines] = i;

			tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, 0);
			tw_old = tw;
			if (tw < width)
				continue;
		}
#ifdef USE_LINE_CHANGE_LINE 
        else if (c == '|' || c == '\n')
#else
        else if (c == '\n')
#endif
		{
			tw = 0;
			tw_old = 0;

			off[lines++] = i;

			th += _line_spacing + _font_sprite->GetFrameModuleHeight(0, 0);

			continue;
        }
        //else if (c == '^')
        //{
        //    i++;
        //    continue;
        //}
		if(c < 128)
			c = c - 32;//caMap_Char[c];
#ifdef USE_OR_CHAR_TABLE
		else
			c = 0;
#endif
		if(c < 0)
			continue;
        //int m = (_fmodules[_frames_fm_start[c]<<2]&0xFF);

        tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, c);
		
		m_text_w = tw;
        if (tw > width ) 
        {		    
			if(	tw_old != 0)
			{
				tw = tw - tw_old - _char_spacing ;//- _fmodules[1];
				m_text_w = tw_old;
			}
			else
			{
				off[lines] = _i;
				i = _i;
				tw = 0;
			}
			tw_old = 0;
            lines++;

            th += _line_spacing + _font_sprite->GetFrameModuleHeight(0, 0);
        }
    }
    
    off[lines++] = len;
    m_drawPage_nLine = lines;
	return m_drawPage_nLine;

}

int CGLLiveFont::DrawPageEN(CGLLiveGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll, bool StatesInAbout)
{
	return 0;
}

int CGLLiveFont::DrawPageCH(CGLLiveGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll)
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


int CGLLiveFont::DrawPage(CGLLiveGraphics * g, String s, int x, int y, int width, int height, int scroll, bool auto_scroll,bool StatesInAbout)
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
void CGLLiveFont::SetTextColor(int font_palette)
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


void CGLLiveFont::SetColor(int col)
{

#if GRAPHIC_OPENGL > 0
	ASSERT(col >= 0);
#else
	ASSERT(col >= 0 && col < (int)((CGLLiveIndexedImage*)(_font_sprite->_image))->GetPaletteEntryNumber());
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
		((CGLLiveIndexedImage*)s_font_chs.m_pFonts[m_nFontStyle].pFontImage)->SetPaletteColor(color,1);
	}
	else if(GameApp().GetLanguageIndex() == LANGUAGE_CHT)
	{
		s_font_cht.SetFontStyle((CGLFont::FontStyle)m_nFontStyle);
		((CGLLiveIndexedImage*)s_font_cht.m_pFonts[m_nFontStyle].pFontImage)->SetPaletteColor(color,1);
	}
#endif
	_font_sprite->SetCurrentPalette(col);
#endif

};

void CGLLiveFont::SetFontColor(int color)
{
	m_nFontColor = color;
}

int CGLLiveFont::GetChar(char * string, int& index)
{
#ifdef ONLY_ASCII
	return str[index];
#else
	unsigned int ret = 0;
	char * str = &string[index];
	if ((*str & 0xe0) == 0xe0 && ((*(str + 1) & 0xc0) == 0x80 && ((*(str + 2)& 0xc0) == 0x80)))
	{
		ret = ((*(str)&0xf ) << 12) | ((*(str + 1)&0x3f ) << 6) | (*(str + 2)&0x3f);
		index += 2;
	}
	else if((*str & 0xc0) == 0xc0 && (*(str + 1) & 0xc0) == 0x80)
	{
		ret = ((*(str)&0x3f) << 6) | (*(str + 1)&0x3f);
		index += 1;
	}
	else if((*str & 0x80) == 0)
	{
		// ASCII
		//if(*str == 0x01 && *(str+1) < 32 )//color control
		//{
		//	str++;
		//}
        ret = *str;
		index += 0;
	}
	else
	{
		//ASSERT(false);
		ret = -1;   // wrong encoding
	}

#ifndef USE_OR_CHAR_TABLE
    // add by luopeng for unicode support
    if(ret> 255)
    {

        if(ret == 0x2122)
        {
            ret = 216;
        }
        else if(ret == 0x20AC)
        {
            ret = 213;
        }
        else if(ret == 0x2022)
        {
            ret = 217;
        }
        else
        {
            ret = UNDEF_CHAR;
        }
    }
    else if(ret> MAP_CHAR_EXT_START)
    {
        ret = _map_char_ext[ret - MAP_CHAR_EXT_START -1];
    }
#endif

	return ret;
#endif
}

void CGLLiveFont::InitLinkPosition(char* str, int x, int y, int width, int index1, int index2, int* position)
{
	if(!str)
		return;

	int lines = 0;
	int len = (int)strlen(str);
	int * off = m_drawPageOffset;
	int tw = 0;
	int th = 0;
	int tw_old = 0;
	m_text_w = 0;

	for(int i=0; i<len; i++)
	{
		if(i == index1)
		{
			position[0] = x + tw;
			position[1] = y + th;
		}
		else if(i == index2)
		{
			position[2] = x + tw;
			position[3] = y + th;
		}
		//char temp_char = s[ i ] ;
		//int c = temp_char & 0xFF;
		int _i = i - 1;
		int c = GetChar(str, i);

		if (c == ' ')
		{
			off[lines] = i;

			tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, 0);
			tw_old = tw;
			if (tw < width)
				continue;
		}
#ifdef USE_LINE_CHANGE_LINE 
        else if (c == '|' || c == '\n')
#else
        else if (c == '\n')
#endif
		{
			tw = 0;
			tw_old = 0;

			off[lines++] = i;

			th += _line_spacing + _font_sprite->GetFrameModuleHeight(0, 0);

			continue;
        }
		if(c < 128)
			c = c - 32;//caMap_Char[c];
#ifdef USE_OR_CHAR_TABLE
		else
			c = 0;
#endif
		if(c < 0)
			continue;

        tw += _char_spacing + _font_sprite->GetFrameModuleWidth(0, c);
		
		m_text_w = tw;
        if (tw > width ) 
        {		    
			if(	tw_old != 0)
			{
				tw = tw - tw_old - _char_spacing ;//- _fmodules[1];
				m_text_w = tw_old;
			}
			else
			{
				off[lines] = _i;
				i = _i;
				tw = 0;
			}
			tw_old = 0;
            lines++;

            th += _line_spacing + _font_sprite->GetFrameModuleHeight(0, 0);
        }
    }
	if(index2 == len)
	{
		position[2] = x + tw;
		position[3] = y + th;
	}
    
    off[lines++] = len;
    m_drawPage_nLine = lines;
}