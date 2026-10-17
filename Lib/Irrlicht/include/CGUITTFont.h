#ifndef __C_GUI_TTFONT_H_INCLUDED__
#define __C_GUI_TTFONT_H_INCLUDED__

#ifdef _IRR_COMPILE_WITH_FREETYPE_
#include <ft2build.h>
#include <freetype/freetype.h>
#include <irrlicht.h>
#include <IGUITTFont.h>

#include <vector>

namespace irr
{
namespace gui
{

class CGUITTFace : public IReferenceCounted
{
public:
	static FT_Library	library;    // handle to library
	FT_Face		face;               // handle to face
	bool load(const c8* filename);
	bool load(io::IReadFile* file);

	CGUITTFace() : face(0) {};
	virtual ~CGUITTFace();
};

class CGUITTGlyph : public IReferenceCounted
{
public:
	CGUITTGlyph();
	virtual ~CGUITTGlyph();

	void Free();

    bool cached;
	void cache(u32 idx_, CGUITTFace* ttface_, video::IVideoDriver* driver_, bool bBold = false);

	u32 size;
	u32 top;
	u32 left;
	u32 texw;
	u32 texh;
	u32 imgw;
	u32 imgh;
	u32 top16;
	u32 left16;
	u32 texw16;
	u32 texh16;
	u32 imgw16;
	u32 imgh16;
	s32 offset;

	video::ITexture *tex;
	video::ITexture *tex16;
	u8 *image;

	u32 borderSize;
	video::SColor borderColor;
//private:
//	CGUITTGlyph(const CGUITTGlyph&);
//	CGUITTGlyph& operator=(const CGUITTGlyph&);
};

class CGUITTFont : public IGUITTFont
{
public:

	//! constructor
	CGUITTFont(video::IVideoDriver* Driver);

	//! destructor
	virtual ~CGUITTFont();

	//! loads a truetype font file
	virtual bool attach(CGUITTFace *filename,u32 size, u32 borderSize = 0, video::SColor borderColor = video::SColor(0, 0, 0, 0));

	//! sets a border for the font
	virtual void setBorder( u32 borderSize, video::SColor borderColor);

	//! draws an text and clips it to the specified rectangle if wanted
	virtual void draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0);
	void drawGlyph(const CGUITTGlyph* g, const core::position2d<s32> offset, const core::rect<s32>* clip, video::SColor color);
	
	//! modifies the texture passed in parameter by embedding text in it
	virtual void drawInTexture(const wchar_t* text, video::ITexture* tex, core::rect<s32> position, video::SColor color, bool hcenter=false, bool vcenter=false);
	void drawGlyphInTexture(const CGUITTGlyph* g,  video::ITexture* tex, const core::position2d<s32> offset, const core::rect<s32>* clip, video::SColor color);

	//! Draws UTF-8 encoded text and clips it to the specified rectangle if wanted.
	virtual void draw(const char* text, const core::rect<s32>& position,
		video::SColor color, bool hcenter=false, bool vcenter=false,
		const core::rect<s32>* clip=0);

	//! modifies the texture passed in parameter by embedding UTF8-encoded text in it
	virtual void drawInTexture(const char* text, video::ITexture* tex, core::rect<irr::s32> pos, 
		video::SColor color, bool hcenter=false, bool vcenter=false);

	//! Calculates the dimension of a UTF8-encoded text string.
	virtual core::dimension2d<s32> getDimension(const char* text) const;
	
	//! returns the dimension of a text
	virtual core::dimension2d<s32> getDimension(const wchar_t* text) const;

	//! Calculates the height of a UTF8-encoded text string.
	virtual s32 getHeight(const char* text) const;

	//! returns the height of a text
	virtual s32 getHeight(const wchar_t* text) const;

	//! returns the vertical bearing of the letter "a"
	float getVertBearingFactor();

	//! Calculates the index of the character in the text which is on a specific position.
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) const;
	virtual s32 getCharacterFromPos(const char* text, s32 pixel_x) const;

	virtual void setKerningWidth (s32 kerning);
	virtual void setKerningHeight (s32 kerning);

	virtual s32 getKerningWidth(const wchar_t* thisLetter=0, const wchar_t* previousLetter=0) const;
	virtual s32 getKerningHeight() const;

	//! sets specified additional letter spacing between letters (the added spacing is even between all characters)
	//! this value is added to the default glyph side bearings
	virtual void setTracking(s32 tracking);
	virtual s32 getTracking() const;

	//! sets if font supports transparency
	virtual void setTransparency(bool val);
	virtual bool getTransparency() const;

	//! sets wheter the font will be antialiased or not
	virtual void setAntiAlias(bool value);
	virtual bool getAntiAlias() const;

	virtual void setSpaceWidth(s32 width);
	virtual s32 getSpaceWidth();

	void clearGlyphs();
	void setTrimmingPositions(s32 start=-1, s32 end=-1){startPos = start; endPos = end;};

private:
	s32 getWidthFromCharacter(u32 c) const;
	s32 getHeightFromCharacter(u32 c) const;
	u32 getGlyphByChar(wchar_t c) const;
	u32 getGlyphByValue(u32 c) const;
	video::IVideoDriver* Driver;
	
	mutable std::vector<CGUITTGlyph> Glyphs;
	mutable std::vector<CGUITTGlyph> GlyphsBold;
	mutable std::vector<CGUITTGlyph> GlyphsItalic;

	//core::array< CGUITTGlyph> Glyphs;
	//core::array< CGUITTGlyph> GlyphsBold;
	//core::array< CGUITTGlyph> GlyphsItalic;
	
	CGUITTFace *tt_face;

	bool AntiAlias;
	bool TransParency;
	s32 Tracking;
	s32 SpaceWidth;

	s32 startPos;
	s32 endPos;

};

} // end namespace gui
} // end namespace irr
#endif // _IRR_COMPILE_WITH_FREETYPE_
#endif

