////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Fonts/Font.h"

#include "Sprites/ASprite.h"

#include "Game/Config/GameConfig.h"
#include "Game/Misc.h"

#include <algorithm>

const TChar	CFont::SpaceChar = L' ';
const TChar	CFont::NewLineChar = L'\n';
const TChar	CFont::NonBreakableSpaceChar = 160;// ' '
const TChar CFont::OptionalNewLineChar = L'|';

////////////////////////////////////////////////////////////////////////////////////////////////////

CFont::CFont() :
	m_pSprite(NULL),
	m_nCharCount(0),
	m_pCharMap(NULL),
	m_pCharIndex(NULL),
	m_scaleX(1.0f),
	m_scaleY(1.0f)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CFont::~CFont()
{
	SAFE_DEL(m_pSprite);
	SAFE_DEL_ARRAY(m_pCharMap);
	SAFE_DEL_ARRAY(m_pCharIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

class IndexCompare 
{
public:
	IndexCompare(const TChar * arr)
	{
		m_arr = arr;
	}

	bool operator()(const int x, const int y) const 
	{
		return m_arr[x] < m_arr[y]; 
	}
private:
	const TChar * m_arr;
};

void GetSortedIndices(const TChar * arr, int len, int * result)
{
	for (int i = 0; i < len; i++)
		result[i] = i;

	IndexCompare cmp(arr);

	std::sort(result, result + len, cmp);
}

class IndexCompareFloat
{
public:
	IndexCompareFloat(const float * arr)
	{
		m_arr = arr;
	}

	bool operator()(const int x, const int y) const 
	{
		return m_arr[x] < m_arr[y]; 
	}
private:
	const float * m_arr;
};

void GetSortedIndices(const float * arr, int len, int * result)
{
	for (int i = 0; i < len; i++)
		result[i] = i;

	IndexCompareFloat cmp(arr);

	std::sort(result, result + len, cmp);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFont::Load(const stringw & name)
{
	m_fileName = name;
	IAttributes * attr = CGameConfig::GetInstance()->getAttributes(L"font", name);

	stringc prefix = "Fonts/";
	stringc filename = attr->getAttributeAsString("FileName");
	stringc charmap = prefix + filename + ".txt";
	stringc sprite = prefix + filename;

	/// Load Sprite
	m_pSprite = NEW ASprite();
	m_pSprite->Load(sprite.c_str());
	m_pSprite->_material->getTexture(0)->setMinFilter(ETFT_LINEAR);
	m_pSprite->_material->getTexture(0)->setMagFilter(ETFT_LINEAR);
	m_pSprite->_material->setFlag(EMF_ZBUFFER, false);
	m_pSprite->_material->setFlag(EMF_ZWRITE_ENABLE, false);
	m_pSprite->_material->setFlag(EMF_LIGHTING, false);
	m_pSprite->_material->setFlag(EMF_BACK_FACE_CULLING, false);
	m_pSprite->_material->setFlag(EMF_2D_USE_VERTEX_ALPHA, true);
	m_pSprite->_material->setFlag(EMF_2D_USE_TEXTURE_ALPHA, true);
	m_pSprite->_material->setEmissiveColor(SColor(1,1,1,1));

	/// Load Character Map
	IReadFile * file = __IRR_FILE_SYSTEM()->createAndOpenFile(charmap.c_str());
	if( file == NULL )
	{
		DBG_ERROR( "Font not loaded: %s", name.c_str() );
		return false;
	}

	// ignore Unicode header
	u16 header;
	file->read(&header, 2);
	GX_ASSERT(header == 0xFEFF);
	
	/// Char Map Size
	m_nCharCount = (file->getSize() - sizeof(header)) / 2;

	/// Read char map
	m_pCharMap = NEW TChar [m_nCharCount];
	m_pCharIndex = NEW int [m_nCharCount];

	for (int i = 0; i < m_nCharCount; i++)
	{
		u16 c;
		file->read(&c, 2);
		m_pCharMap[i] = c;
	}

	file->drop();

	// sort
	GetSortedIndices(m_pCharMap, m_nCharCount, m_pCharIndex);

	/// Font Color/Alpha
	SColor color1 = SColor(255, 255, 255, 255);
	SColor color2 = SColor(0, 0, 0, 0);
	int alpha = 255;
	bool gradient = false;

	if (attr->existsAttribute("Color"))
	{
		color1 = attr->getAttributeAsColor("Color");
	}

	if (attr->existsAttribute("Color1"))
	{
		color1 = attr->getAttributeAsColor("Color1");
	}

	if (attr->existsAttribute("Color2"))
	{
		gradient = true;
		color2 = attr->getAttributeAsColor("Color2");
	}

	if (attr->existsAttribute("Alpha"))
	{
		alpha = attr->getAttributeAsInt("Alpha");
	}

	SetColor1(color1);
	SetColor2(color2);
	SetAlpha(alpha);
	EnableGradient(gradient);

	/// Other font settings
	CharSpacing = attr->getAttributeAsInt("CharSpacing");
	LineSpacing = attr->getAttributeAsInt("LineSpacing");
	SpaceWidth = attr->getAttributeAsInt("SpaceWidth");

	if (SpaceWidth == 0)
		SpaceWidth = GetCharWidth(L' ');

	LineHeight = m_pSprite->GetFModuleHeight(0, 0);

	if (attr->existsAttribute("FontSize"))
	{
		float desiredFontSize = attr->getAttributeAsFloat("FontSize");
		SetScale( desiredFontSize/LineHeight );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int CFont::FindChar(TChar c)
{
	int left = 0;
	int right = m_nCharCount - 1;
	int mid;
	
	while (left <= right)
	{
		mid = (left + right)/2;
		
		if (m_pCharMap[m_pCharIndex[mid]] == c)
		{
			return m_pCharIndex[mid];
		}
		else if (m_pCharMap[m_pCharIndex[mid]] < c)
		{
			left = mid + 1;
		}
		else
		{
			right = mid - 1;
		}
	}

	DBG_OUT("Char not found: %X\n", c);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float CFont::GetCharWidth(TChar c)
{
	int index = FindChar(c);
	return m_pSprite->GetFModuleWidth(0, index)*m_scaleX;
}

float CFont::DrawChar(TChar c, float x, float y, int alpha)
{
	bool shouldScaleCharacter = (ABS(m_scaleX - 1.0f) > 0.01f) || (ABS(m_scaleY - 1.0f) > 0.01f);
	
	if( shouldScaleCharacter )
		m_pSprite->PushTransform( irr::core::vector3df(x,y,0.0f) , irr::core::vector3df(m_scaleX,m_scaleY,1.0f), irr::core::vector3df(0.0f,0.0f,0.0f) );

	int index = FindChar(c);
	m_pSprite->PaintFModule(0, index, x, y, 0, 0, 0, alpha);

	if( shouldScaleCharacter )
		m_pSprite->PopTransform();

	return m_pSprite->GetFModuleWidth(0, index)*m_scaleX;
}

/////////////////////////////////// /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

d2f CFont::GetSubStringSize(const TChar* begin, const TChar* end)
{
	GX_ASSERT(begin);

	d2f size(0, 0);
	float width = 0;

	const TChar * iter = begin;

	while ((!end && *iter) || (iter < end))
	{
		if (*iter == SpaceChar || *iter == NonBreakableSpaceChar)
		{
			width += SpaceWidth*m_scaleX;
		}
		else if (*iter == NewLineChar)
		{
			size.Width = MAX(size.Width, width);
			size.Height += (LineHeight*m_scaleY + LineSpacing); 
			width = 0;
		}
		else if (*iter == OptionalNewLineChar)
		{
		}
		else
		{
			width += GetCharWidth(*iter) + (width > 0 ? CharSpacing : 0);
		}

		iter++;
	}

	if (width > 0)
	{
		size.Width = MAX(size.Width, width);
		size.Height += ((size.Height ? LineHeight*m_scaleY + LineSpacing : LineHeight*m_scaleY));
	}

	return size;
}

void CFont::DrawSubString(const TChar* begin, const TChar* end, float x, float y, int align, int alpha)
{
	GX_ASSERT(begin);
	GX_ASSERT(!!(align & LEFT) + !!(align & RIGHT) + !!(align & HCENTER) <= 1);
	GX_ASSERT(!!(align & TOP) + !!(align & BOTTOM) + !!(align & VCENTER) <= 1);

	if (align)
	{			
		d2f size = GetSubStringSize(begin, end);

		if(align & HCENTER)	x -= size.Width/2;
		if(align & VCENTER)	y -= size.Height/2;
		if(align & RIGHT)	x -= size.Width;
		if(align & BOTTOM)	y -= size.Height;
	}

	p2f pos(x, y);

	pos.Y -= m_pSprite->GetFModuleOffY(0, 0)*m_scaleY;

	const TChar * iter = begin;

	while ((!end && *iter) || (iter < end))
	{
		if (*iter == SpaceChar || *iter == NonBreakableSpaceChar)
		{
			pos.X += SpaceWidth*m_scaleX;
		}
		else if (*iter == NewLineChar)
		{
			pos.X = x;
			pos.Y += (LineHeight*m_scaleY + LineSpacing); 
		}
		else if (*iter == OptionalNewLineChar)
		{
		}
		else
		{
			pos.X += DrawChar(*iter, pos.X, pos.Y, alpha) + CharSpacing*m_scaleX;
		}

		iter++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

d2f CFont::GetStringSize(const TChar * str)
{
	return GetSubStringSize(str, NULL);
}

void CFont::DrawString(const TChar* str, float x, float y, int align, int alpha)
{
	GX_ASSERT(str);
	GX_ASSERT(!!(align & LEFT) + !!(align & RIGHT) + !!(align & HCENTER) <= 1);
	GX_ASSERT(!!(align & TOP) + !!(align & BOTTOM) + !!(align & VCENTER) <= 1);

	if (align)
	{			
		d2f size = GetStringSize(str);

		if(align & VCENTER)	y -= size.Height/2;
		if(align & BOTTOM)	y -= size.Height;

		align &= ~VCENTER;
		align &= ~BOTTOM;
	}

	p2f pos(x, y);

	const TChar * iter = str;
	const TChar * last = str;

	while (*iter)
	{
		if (*iter == NewLineChar)
		{
			DrawSubString(last, iter, pos.X, pos.Y, align, alpha);
			pos.Y += (LineHeight*m_scaleY + LineSpacing);
			last = iter + 1;
		}
		
		iter++;
	}

	if (last < iter)
	{
		DrawSubString(last, iter, pos.X, pos.Y, align, alpha);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

d2f CFont::GetWrappedTextSize(const TChar* str, int maxWidth, bool update)
{
	if (update)
		UpdateTextInfo(str, maxWidth);

	return m_TextInfo.Size;
}

int CFont::GetWrappedTextLines(const TChar* str, int maxWidth, bool update)
{
	if (update) 
		UpdateTextInfo(str, maxWidth);

	return m_TextInfo.Lines;
}

void CFont::UpdateTextInfo(const TChar* str, float maxWidth)
{
	GX_ASSERT(str);

	m_TextInfo.Size.set(0, 0);
	m_TextInfo.Lines = 0;
	m_TextInfo.Offsets.set_used(0);

	const TChar * iter = str;
	const TChar * begin = str;
	const TChar * last = 0;
	float lastWidth = 0;

	float width = 0;

	while (*iter)
	{
		if (*iter == OptionalNewLineChar)
		{
			last = iter;
			lastWidth = width;
		}
		else if (*iter == SpaceChar)
		{
			last = iter;
			lastWidth = width;

			width += SpaceWidth*m_scaleX;
		}
		else if (*iter == NonBreakableSpaceChar)
		{
			width += SpaceWidth*m_scaleX;
		}
		else if (*iter == NewLineChar)
		{
			m_TextInfo.Size.Width = MAX(m_TextInfo.Size.Width, width);
			m_TextInfo.Size.Height += (LineHeight*m_scaleY + LineSpacing);
			m_TextInfo.Lines++;
			m_TextInfo.Offsets.push_back(begin);
			m_TextInfo.Offsets.push_back(iter);

			width = 0;
			begin = iter + 1;
			last = NULL;
		}
		else
		{
			width += GetCharWidth(*iter) + (width > 0 ? CharSpacing*m_scaleX : 0);
		}

		if (width > maxWidth && last)
		{
			m_TextInfo.Size.Width = MAX(m_TextInfo.Size.Width, lastWidth);
			m_TextInfo.Size.Height += (LineHeight*m_scaleY + LineSpacing);
			m_TextInfo.Lines++;
			m_TextInfo.Offsets.push_back(begin);
			m_TextInfo.Offsets.push_back(last);

			width -= lastWidth;
			if (*last == SpaceChar)
				width -= SpaceWidth*m_scaleX;

			begin = last + 1;
			last = NULL;
		}

		iter++;
	}

	if (width > 0)
	{
		m_TextInfo.Size.Width = MAX(m_TextInfo.Size.Width, width);
		m_TextInfo.Size.Height += (m_TextInfo.Size.Height ? LineHeight*m_scaleY + LineSpacing : LineHeight*m_scaleY); 
		m_TextInfo.Lines++;
		m_TextInfo.Offsets.push_back(begin);
		m_TextInfo.Offsets.push_back(iter);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CFont::DrawWrappedText(const TChar* str, float x, float y, int maxWidth, int align, int alpha, bool update)
{
	GX_ASSERT(str);
	GX_ASSERT(!!(align & LEFT) + !!(align & RIGHT) + !!(align & HCENTER) <= 1);
	GX_ASSERT(!!(align & TOP) + !!(align & BOTTOM) + !!(align & VCENTER) <= 1);

	if (update)
		UpdateTextInfo(str, maxWidth);

	if (align)
	{
		if(align & VCENTER)	y -= m_TextInfo.Size.Height/2;
		if(align & BOTTOM)	y -= m_TextInfo.Size.Height;

		align &= ~VCENTER;
		align &= ~BOTTOM;
	}

	p2f pos(x, y);

	for (int i = 0; i < m_TextInfo.Lines; i++)
	{
		DrawSubString(m_TextInfo.Offsets[i*2+0], m_TextInfo.Offsets[i*2+1], pos.X, pos.Y, align, alpha);
		pos.Y += LineHeight*m_scaleY + LineSpacing;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CFont::EnableGradient(bool enable)
{ 
	m_gradient = enable; 
	if (enable)
		m_pSprite->_material->setMaterialType(EMT_A8I8_GRADIENT);
	else
		m_pSprite->_material->setMaterialType(EMT_TRANSPARENT_TEXTURE_VERTEX_ALPHA);
}

void CFont::SetColor1(SColor color)
{
	m_pSprite->_color.R = color.R;
	m_pSprite->_color.G = color.G;
	m_pSprite->_color.B = color.B;
}

void CFont::SetColor2(SColor color)		
{
	SColor c = m_pSprite->_material->getEmissiveColor();
	c.R = color.R;
	c.G = color.G;
	c.B = color.B;
	m_pSprite->_material->setEmissiveColor(c);
}

void CFont::SetAlpha(int alpha)
{
	CLAMP(alpha, 0, 255);
	m_pSprite->_color.A = alpha;
}

bool CFont::IsGradientEnabled()
{ 
	return m_gradient; 
}

SColor CFont::GetColor1()
{
	SColor color = m_pSprite->_color;
	color.A = 255;
	return color; 
}

SColor CFont::GetColor2()
{ 
	SColor color = m_pSprite->_material->getEmissiveColor();
	color.A = 255;
	return color; 
}

int CFont::GetAlpha()
{
	return m_pSprite->_color.A;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
