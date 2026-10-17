////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CFont Class
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"
#include "Sprites/ASprite.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

struct STextInfo
{
	d2f		Size;
	int		Lines;
	array<const TChar*>	Offsets;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CFont
{
public:
				CFont();
	virtual		~CFont();

	bool		Load(const stringw & name);

	/// Substring
	d2f			GetSubStringSize(const TChar* begin, const TChar* end);
	void		DrawSubString(const TChar* begin, const TChar* end, float x, float y, int align, int alpha = 255);

	/// String
	d2f			GetStringSize(const TChar* str);
	void		DrawString(const TChar* str, float x, float y, int align, int alpha = 255);

	/// Text (auto-wrapped string)
	d2f			GetWrappedTextSize(const TChar* str, int maxWidth, bool update = true);
	int			GetWrappedTextLines(const TChar* str, int maxWidth, bool update = true);
	void		DrawWrappedText(const TChar* str, float x, float y, int maxWidth, int align, int alpha = 255, bool update = true);

public:
	void			EnableGradient(bool enable);
	bool			IsGradientEnabled();
	void			SetColor1(SColor color);
	SColor			GetColor1();
	void			SetColor2(SColor color);
	SColor			GetColor2();	
	void			SetAlpha(int alpha);
	int				GetAlpha();

	inline void		SetScale( float scaleX, float scaleY ) { m_scaleX = scaleX; m_scaleY = scaleY; }
	inline void		SetScale( float scale ) { m_scaleX = scale; m_scaleY = scale; }
	inline float	GetScaleX() { return m_scaleX; }
	inline float	GetScaleY() { return m_scaleY; }

	float			CharSpacing;
	float			LineSpacing;
	float			SpaceWidth;
	float			LineHeight;

public:
	static const TChar	SpaceChar;
	static const TChar	NewLineChar;
	static const TChar	NonBreakableSpaceChar;
	static const TChar	OptionalNewLineChar;

	// Available anchor points
	static const int HCENTER = 1;
	static const int VCENTER = 2;
	static const int HVCENTER = HCENTER | VCENTER;
	static const int LEFT = 4;
	static const int RIGHT = 8;
	static const int TOP = 16;
	static const int BOTTOM = 32;

private:
	/// Char 
	int			FindChar(TChar c);
	float		GetCharWidth(TChar c);
	float		DrawChar(TChar c, float x, float y, int alpha = 255);

	ASprite*	m_pSprite;
	int			m_nCharCount;
	TChar*		m_pCharMap;
	int* 		m_pCharIndex;

	void		UpdateTextInfo(const TChar* str, float maxWidth);
	STextInfo	m_TextInfo;

	bool		m_gradient;

	float		m_scaleX;
	float		m_scaleY;

	stringw		m_fileName;

};
