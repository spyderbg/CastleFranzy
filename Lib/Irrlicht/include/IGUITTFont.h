#ifndef __I_GUI_TTFONT_H_INCLUDED__
#define __I_GUI_TTFONT_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_FREETYPE_

namespace irr
{
namespace gui
{

class IGUITTFont : public IGUIFont
{
public:

	//! draws an text and clips it to the specified rectangle if wanted
	virtual void draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0) = 0;

	//! draws UTF8-encoded text and clips it to the specified rectangle if wanted
	//virtual void draw(const char* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0) = 0;

	//! modifies the texture passed in parameter by embedding text in it
	virtual void drawInTexture(const wchar_t* text, video::ITexture* tex, core::rect<s32> pos, video::SColor color, bool hcenter=false, bool vcenter=false) = 0;

	//! modifies the texture passed in parameter by embedding UTF8-encoded text in it
	//virtual void drawInTexture(const char* text, video::ITexture* tex, core::rect<s32> pos, video::SColor color, bool hcenter=false, bool vcenter=false) = 0;

	//! sets a border for the font
	virtual void setBorder (u32 borderSize, video::SColor borderColor) = 0;

	//! returns the dimension of a text
	virtual core::dimension2d<s32> getDimension(const wchar_t* text) const = 0;

	//! Calculates the height of a UTF8-encoded text string.
	virtual s32 getHeight(const char* text) const = 0;

	//! returns the height of a text
	virtual s32 getHeight(const wchar_t* text) const = 0;

	//! Calculates the index of the character in the text which is on a specific position.
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) const = 0;

	virtual void setKerningWidth (s32 kerning) = 0;
	virtual void setKerningHeight (s32 kerning) = 0;

	virtual s32 getKerningWidth(const wchar_t* thisLetter=0, const wchar_t* previousLetter=0) const = 0;
	virtual s32 getKerningHeight() const = 0;

	//! sets specified additional letter spacing between letters (the added spacing is even between all characters)
	//! this value is added to the default glyph side bearings
	virtual void setTracking(s32 tracking) = 0;

	//! sets if font supports transparency
	virtual void setTransparency(bool val) = 0;
	virtual bool getTransparency() const = 0;

	//! sets wheter the font will be antialiased or not
	virtual void setAntiAlias(bool value) = 0;
	virtual bool getAntiAlias() const = 0;

	virtual void clearGlyphs() {}

	virtual void setSpaceWidth(s32 width) = 0;
	virtual s32 getSpaceWidth() = 0;

};

} // end namespace gui
} // end namespace irr

#endif
#endif
