#include "stdafx.h"
#include "irrlicht.h"
#include "CGUITTFont.h"

#ifdef _IRR_COMPILE_WITH_FREETYPE_

//#include "freetype/ftoutln.h"
#include "freetype/ftbitmap.h"
#include "freetype/ftsynth.h"
//#include "freetype/ftglyph.h"

#include "UTF8.h"


namespace irr
{
namespace gui
{

// --------------------------------------------------------
CGUITTGlyph::CGUITTGlyph()
: IReferenceCounted()
    ,size(0)
	,top(0)
	,left(0)
	,texw(0)
	,texh(0)
	,imgw(0)
	,imgh(0)
	,tex(NULL)
	,top16(0)
	,left16(0)
	,texw16(0)
	,texh16(0)
	,imgw16(0)
	,imgh16(0)
	,tex16(NULL)
	,offset(0)
	,image(NULL)
{
}

CGUITTGlyph::~CGUITTGlyph()
{
	Free();
}

void CGUITTGlyph::Free()
{
	if (image)
		delete[] image;
	image = 0;
	cached = false;
	if (tex) {
		tex->drop();
	}
	tex = 0;
	if (tex16) {
		tex16->drop();
	}
	tex16 = 0;
}

void CGUITTGlyph::cache(u32 idx_, CGUITTFace* ttface_, video::IVideoDriver* driver_, bool bBold /* = false*/)
{
	FT_Face face_ = ttface_->face;
	FT_Set_Pixel_Sizes(face_, 0, size);
	if ( !FT_Load_Glyph(face_, idx_, FT_LOAD_NO_HINTING|FT_LOAD_NO_BITMAP) )
	{
		FT_GlyphSlot glyph = face_->glyph;
		FT_Bitmap  bits;

		if (glyph->format == ft_glyph_format_outline )
		{
			if (!FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL))
			{
				if ( bBold )
				{
					FT_GlyphSlot_Own_Bitmap(glyph);
					FT_Bitmap_Embolden( ttface_->library, &glyph->bitmap, borderSize, borderSize );
				}

				bits = glyph->bitmap;
				u8 *pt = bits.buffer;
				delete[] image;
				image = irrnew u8[bits.width * bits.rows];
				memcpy(image,pt,bits.width * bits.rows);
				top = glyph->bitmap_top;
				left = glyph->bitmap_left;
				imgw = 1;
				imgh = 1;
				texw = bits.width;
				texh = bits.rows;
				for(;;)
				{
					if (imgw > texw)
					{
						break;
					}
					else
					{
						imgw <<= 1;
					}
				}
				for(;;)
				{
					if (imgh > texh)
					{
						break;
					}
					else
					{
						imgh <<= 1;
					}
				}
				if (imgw > imgh)
				{
					imgh = imgw;
				}
				else
				{
					imgw = imgh;
				}
				u32 *texd = irrnew u32[imgw*imgh];
				memset(texd,0,imgw*imgh*sizeof(u32));
				u32 *texp = texd;
				offset = size - bits.rows;
				bool cflag = (driver_->getDriverType() == video::EDT_DIRECT3D8);
				for (int i = 0;i < bits.rows;i++)
				{
					u32 *rowp = texp;
					for (int j = 0;j < bits.width;j++)
					{
						if (*pt)
						{
							if (cflag)
							{
								*rowp = *pt;
								*rowp *= 0x01010101;
							}
							else
							{
								*rowp = *pt << 24;
								*rowp |= 0xffffff;
							}
						}
						else
						{
                            *rowp = 0;
						}
						pt++;
						rowp++;
					}
					texp += imgw;
				}
				c8 name[128];
				sprintf(name,"TTFontGlyph%d",idx_);
				video::IImage *img = driver_->createImageFromData(video::ECF_A8R8G8B8,core::dimension2d<s32>(imgw,imgh),texd);
				bool flg16 = driver_->getOption(video::EVDO_CREATE_TEXTURE_ALWAYS_16_BIT);
				bool flg32 = driver_->getOption(video::EVDO_CREATE_TEXTURE_ALWAYS_32_BIT);
				driver_->setOption(video::EVDO_CREATE_TEXTURE_ALWAYS_16_BIT, false);
				driver_->setOption(video::EVDO_CREATE_TEXTURE_ALWAYS_32_BIT, true);
				tex = driver_->addTexture(name,img);
				//Grab and remove from cache -- no use for this to be in the cache
				tex->grab();
				driver_->removeTexture(tex);

				img->drop();
				driver_->setOption(video::EVDO_CREATE_TEXTURE_ALWAYS_32_BIT, flg32);
				driver_->setOption(video::EVDO_CREATE_TEXTURE_ALWAYS_16_BIT, flg16);
				delete[] texd;
				cached = true;
			}
		}
	}

	if (FT_Load_Glyph( face_, idx_, FT_LOAD_NO_BITMAP|FT_LOAD_NO_HINTING|FT_LOAD_RENDER|FT_LOAD_MONOCHROME ))
	{
		FT_GlyphSlot glyph = face_->glyph;
		if (bBold)
		{
			FT_GlyphSlot_Own_Bitmap(glyph);
			FT_Bitmap_Embolden( ttface_->library, &glyph->bitmap, 8, 8 );
		}
		FT_Bitmap bits = glyph->bitmap;
		u8 *pt = bits.buffer;

		top16 = glyph->bitmap_top;
		left16 = glyph->bitmap_left;
		imgw16 = 1;
		imgh16 = 1;
		texw16 = bits.width;
		texh16 = bits.rows;
		for(;;)
		{
			if (imgw16 >= texw16)
			{
				break;
			}
			else
			{
				imgw16 <<= 1;
			}
		}
		for(;;)
		{
			if (imgh16 >= texh16)
			{
				break;
			}
			else
			{
				imgh16 <<= 1;
			}
		}
		if (imgw16 > imgh16)
		{
			imgh16 = imgw16;
		}
		else
		{
			imgw16 = imgh16;
		}
		u16 *texd16 = irrnew u16[imgw16*imgh16];
		memset(texd16,0,imgw16*imgh16*sizeof(u16));
		u16 *texp16 = texd16;
		offset = size - bits.rows;
		for (int y = 0;y < bits.rows;y++)
		{
			u16 *rowp = texp16;
			for (int x = 0;x < bits.width;x++)
			{
				if (pt[y * bits.pitch + (x / 8)] & (0x80 >> (x % 8)))
				{
					*rowp = 0xffff;
				}
				rowp++;
			}
			texp16 += imgw16;
		}
		c8 name[128];
		sprintf(name,"TTFontGlyph%d_16",idx_);
		video::IImage *img = driver_->createImageFromData(video::ECF_A1R5G5B5,core::dimension2d<s32>(imgw16,imgh16),texd16);
		tex16 = driver_->addTexture(name,img); //<-- we could add an option to add to cache or not...
		//Grab and remove from cache -- no use for this to be in the cache
		tex16->grab();
		driver_->removeTexture(tex16);

		img->drop();
		driver_->makeColorKeyTexture(tex16,video::SColor(0,0,0,0));
		delete[] texd16;
	}
}

// --------------------------------------------------------
FT_Library	CGUITTFace::library  = 0;

//! loads a font file from file path
bool CGUITTFace::load(const c8* filename)
{
    if ( !library )
    {
        if (FT_Init_FreeType( &library ))
        {
            return	false;
        }
    }
	if (FT_New_Face( library,filename,0,&face ))
	{
		return	false;
	}
	return	true;
}

//! loads a font file from memory
bool CGUITTFace::load(io::IReadFile* file)
{
    if ( !library )
    {
        if (FT_Init_FreeType( &library ))
        {
            return	false;
        }
    }
	if (FT_New_Memory_Face( library, (unsigned char*)file->getBuffer(0), file->getSize(),0,&face ))
	{
		return	false;
	}
	return	true;
}
CGUITTFace::~CGUITTFace()
{
	FT_Done_Face(face);
	face = 0;
}
// --------------------------------------------------------
//! constructor
CGUITTFont::CGUITTFont(video::IVideoDriver* driver)
: Driver(driver)
, tt_face(0)
, startPos(-1)
, endPos(-1)

{
	#ifdef _DEBUG
	setDebugName("CGUITTFont");
	#endif

	if (Driver)
		Driver->grab();
	AntiAlias = false;
	TransParency = false;
	Tracking = 0;
	SpaceWidth = 0;
}

//! destructor
CGUITTFont::~CGUITTFont()
{
    if ( tt_face )
	{
        tt_face->drop();
		tt_face = 0;
	}
	if (Driver)
	{
		Driver->drop();
		Driver = 0;
	}
    clearGlyphs();
	FT_Done_FreeType(CGUITTFace::library);
	CGUITTFace::library = 0;
}

bool CGUITTFont::attach(CGUITTFace *Face, u32 size, u32 borderSize, video::SColor borderColor)
{
	if (!Driver || !Face)
		return false;

    if ( tt_face )
        tt_face->drop();
	tt_face = Face;
	if ( !tt_face )
        return false;
	tt_face->grab();

    clearGlyphs();

	Glyphs.resize(tt_face->face->num_glyphs);
	//Glyphs.set_used(tt_face->face->num_glyphs);
	GlyphsBold.resize(tt_face->face->num_glyphs);
	//GlyphsBold.set_used(tt_face->face->num_glyphs);
	
	for (int i = 0;i < tt_face->face->num_glyphs;i++)
	{
        CGUITTGlyph& glyph = Glyphs[i];
		CGUITTGlyph& glyphBold = GlyphsBold[i];

		glyph.size = size;
		glyph.cached = false;
		glyphBold.size = size;
		glyphBold.cached = false;
		glyphBold.borderSize = borderSize;
		glyphBold.borderColor = borderColor;
	}
	return	true;
}

void CGUITTFont::setBorder( u32 borderSize, video::SColor borderColor)
{
    if ( !tt_face )
	{
		return;
	}
  
	u32 bs = borderSize * GlyphsBold[0].size;
	for (int i = 0;i < tt_face->face->num_glyphs;i++)
	{
        GlyphsBold[i].borderSize = bs;
		GlyphsBold[i].borderColor = borderColor;
	}
}

void CGUITTFont::clearGlyphs()
{
    for ( unsigned int i=0; i < Glyphs.size(); ++i )
    {
        Glyphs[i].Free();
    }
	Glyphs.clear();

	for ( unsigned int i=0; i < GlyphsBold.size(); ++i )
	{
		GlyphsBold[i].Free();
	}
	GlyphsBold.clear();

	for ( unsigned int i=0; i < GlyphsItalic.size(); ++i )
	{
		GlyphsItalic[i].Free();
	}
	GlyphsItalic.clear();
}

u32 CGUITTFont::getGlyphByChar(wchar_t c) const
{
	u32 idx = FT_Get_Char_Index( tt_face->face, c );
	if ( idx && !Glyphs[idx - 1].cached )
        Glyphs[idx - 1].cache(idx, tt_face, Driver);
	if ( idx && (GlyphsBold[idx - 1].borderSize > 0) && !GlyphsBold[idx - 1].cached )
        GlyphsBold[idx - 1].cache(idx, tt_face, Driver, true);
	return	idx;
}

//! Returns the glyph index in pixels from its UTF8-value
u32 CGUITTFont::getGlyphByValue(u32 c) const
{
	u32 idx = FT_Get_Char_Index( tt_face->face, c );
	if ( idx && !Glyphs[idx - 1].cached )
        Glyphs[idx - 1].cache(idx, tt_face, Driver);
	if ( idx && (GlyphsBold[idx - 1].borderSize > 0) && !GlyphsBold[idx - 1].cached )
        GlyphsBold[idx - 1].cache(idx, tt_face, Driver, true);
	return	idx;
}

//! returns the real height of a text
s32 CGUITTFont::getHeight(const wchar_t* text) const
{
	s32 height = 0;
	for(const wchar_t* p = text; *p; ++p)
	{
		s32 h =  getHeightFromCharacter(*p);
		if (h > height)
			height = h;
	}

	return height;
}

//! returns the real height of a text
s32 CGUITTFont::getHeight(const char* text) const
{
	s32 height = 0;
	//iterate characters of the UTF-8 string
	while (*text)
	{
		u32 c = core::iterateUTF8String(text);
		s32 h =  getHeightFromCharacter(c);
		if (h > height)
			height = h;
	}

	return height;
}

//! returns the dimension of a text
core::dimension2d<s32> CGUITTFont::getDimension(const wchar_t* text) const
{
	core::dimension2d<s32> dim(0, Glyphs[0].size);

	for(const wchar_t* p = text; *p; ++p){
		dim.Width += getWidthFromCharacter(*p);
	}

	return dim;
}

//! returns the dimension of a text
core::dimension2d<s32> CGUITTFont::getDimension(const char* text) const
{
	core::dimension2d<s32> dim(0, Glyphs[0].size);

	//iterate characters of the UTF-8 string
	while (*text)
	{
		u32 c = core::iterateUTF8String(text);
		dim.Width += getWidthFromCharacter(c);
	}

	return dim;
}

float CGUITTFont::getVertBearingFactor()
{
	u32 idx = FT_Get_Char_Index( tt_face->face, L'a' );
	if ( !FT_Load_Glyph(tt_face->face, idx, FT_LOAD_NO_HINTING|FT_LOAD_NO_BITMAP) )
	{
		FT_GlyphSlot glyph = tt_face->face->glyph;

		if ( tt_face->face->face_flags & FT_FACE_FLAG_VERTICAL)
		{
			// no way of knowing in this case
			return 0.7f;
		}
		else
		{
			float fMidBearing = 1.f - (0.5f * (float)glyph->metrics.vertBearingY / ((float)glyph->metrics.vertBearingY + -(float)glyph->metrics.vertBearingX) );
			return fMidBearing;
		}
	}
	return 0.f;
}

inline s32 CGUITTFont::getWidthFromCharacter(u32 c) const
{
	u32 n = getGlyphByValue(c);
	if ( n > 0)
	{
		int w = Glyphs[n - 1].texw;
		s32 left = Glyphs[n - 1].left;
		if (w + left > 0)
            return w + left + Tracking;
	}
	if (c >= 0x2000)
	{
		return	Glyphs[0].size + Tracking;
	}
	if( c < 32)
	{
		return 0;
	}

	return	SpaceWidth > 0 ? SpaceWidth : (Glyphs[0].size / 2 + Tracking);
}

inline s32 CGUITTFont::getHeightFromCharacter(u32 c) const
{
	u32 n = getGlyphByValue(c);
	if ( n > 0)
	{
		return Glyphs[n - 1].texh;
	}
	if( c < 32)
	{
		return 0;
	}

	return	Glyphs[0].size;
}

void CGUITTFont::drawGlyph(const CGUITTGlyph* g, const core::position2d<s32> offset, const core::rect<s32>* clip, video::SColor color)
{
	if (!TransParency)
	{
		color.A = 0xff;
	}

	if ( g->tex16 )
	{
		s32 imgw = g->imgw16 - 1;
		s32 imgh = g->imgh16 - 1;
		s32 offx = offset.X + g->left16;
		s32 offy = offset.Y + (g->size - g->top16);
		Driver->draw2DImage(g->tex16,
                       core::position2d<s32>(offx, offy),
                       core::rect<s32>(0, 0, imgw, imgh),
                       clip, color, true);
	}
	else if ( g->tex )
	{
		s32 imgw = g->imgw - 1;
		s32 imgh = g->imgh - 1;
		s32 offx = offset.X + g->left;
		s32 offy = offset.Y + (g->size - g->top);
		Driver->draw2DImage(g->tex,
                       core::position2d<s32>(offx, offy),
                       core::rect<s32>(0, 0, imgw, imgh),
                       clip, color, true);
	}
}

void CGUITTFont::drawGlyphInTexture(const CGUITTGlyph* g, video::ITexture* tex, const core::position2d<s32> offset, const core::rect<s32>* clip, video::SColor color)
{
	if(!tex)
		return;

	/*u32 *texPixels = (u32*)(tex->lock());

	video::SColor texColor;
	video::SColor glyphColor;
	video::SColor resultColor;

	if (!TransParency)
	{
		color.color |= 0xff000000;
	}

	if ( g->tex16 )
	{
		/*
		// some drivers pad the glyph texture to make its vertical and horizontal dimensions a power of 2. 
		// We determine the vertical padding of the texture here
		u8 texPaddingV = g->tex16->getSize().Height - g->imgh16;
		u8 texPaddingH = g->tex16->getSize().Width - g->imgw16;

		s32 imgw = g->imgw16 - 1;
		s32 imgh = g->imgh16 - 1;
		s32 offx = offset.X + g->left16 - (texPaddingH>>1);
		s32 offy = offset.Y + (g->size - g->top16) - (texPaddingV>>1);

		u32 *glyphPixels = (u32*)g->tex16->lock();

		//draw glyph in the texture
		for(int i=0; i< g->tex16->getSize().Height ; ++i)
		{
			for(int j=0; j< g->tex16->getSize().Width ; ++j)
			{
				//make sure the pixel to draw is within the texture boundaries
				if((j + offx < tex->getSize().Width) && (j + offx >= 0) &&
				   (i + offy < tex->getSize().Height)&& (i + offy >=0))
				{
					s32 texPixelOffset = (offy + i)* tex->getSize().Width + offx + j;
					s32 glyphPixelOffset = i* g->tex16->getSize().Width + j;

					texColor.color = texPixels[texPixelOffset];
					glyphColor.color = glyphPixels[glyphPixelOffset];
		
					//filter the glyph color with the specified color to draw
					glyphColor.setAlpha(glyphColor.getAlpha() & color.getAlpha());
					glyphColor.setRed(glyphColor.getRed() & color.getRed());
					glyphColor.setGreen(glyphColor.getGreen() & color.getGreen());
					glyphColor.setBlue(glyphColor.getBlue() & color.getBlue());
					
					//interpolate the glyph image with the texture proportionnally to the alpha channel specified (1 or 0)
					resultColor = glyphColor.getInterpolated(texColor, (float)glyphColor.getAlpha());

					texPixels[texPixelOffset] = resultColor.color;
				}
			}
		} 
	
		g->tex16->unlock();
*/
/*	}
	else if ( g->tex )
	{
		// some drivers pad the glyph texture to make its vertical and horizontal dimensions a power of 2. 
		// We determine the vertical padding of the texture here
		u8 texPaddingV = g->tex->getSize().Height - g->imgh;
		u8 texPaddingH = g->tex->getSize().Width - g->imgw;

		s32 imgw = g->imgw - 1;
		s32 imgh = g->imgh - 1;
		s32 offx = offset.X + g->left - (texPaddingH >>1);
		s32 offy = offset.Y + (g->size - g->top) - (texPaddingV >>1);

		u32 *glyphPixels = (u32*)g->tex->lock();

		//draw glyph in the texture
		for(int i=0; i< g->tex->getSize().Height ; ++i)
		{
			for(int j=0; j< g->tex->getSize().Width ; ++j)
			{
				//make sure the pixel to draw is within the texture boundaries
				if((j + offx < tex->getSize().Width) && (j + offx >= 0) &&
				   (i + offy < tex->getSize().Height)&& (i + offy >=0))
				{
					s32 texPixelOffset = (offy + i)* tex->getSize().Width + offx + j;
					s32 glyphPixelOffset = i* g->tex->getSize().Width + j;

					texColor.color = texPixels[texPixelOffset];
					glyphColor.color = glyphPixels[glyphPixelOffset];
		
					//filter the glyph color with the specified color to draw
					glyphColor.setAlpha(glyphColor.getAlpha() & color.getAlpha());
					glyphColor.setRed(glyphColor.getRed() & color.getRed());
					glyphColor.setGreen(glyphColor.getGreen() & color.getGreen());
					glyphColor.setBlue(glyphColor.getBlue() & color.getBlue());
					
					//interpolate the glyph image with the texture proportionnally to the alpha channel specified
					resultColor = glyphColor.getInterpolated(texColor, (float)glyphColor.getAlpha()/255);
					
					texPixels[texPixelOffset] = resultColor.color;
				}
			}
		}

		g->tex->unlock();
	}

	tex->unlock();*/

	Driver->setRenderTarget(tex);

	Driver->draw2DImage(tex, irr::core::position2d<irr::s32>(0, 0));

	drawGlyph(g, offset, clip, color);

	Driver->setRenderTarget(0);
}

//! draws text and clips it to the specified rectangle if wanted
void CGUITTFont::draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::dimension2d<s32> textDimension = getDimension(text);
	core::position2d<s32> offset = position.UpperLeftCorner;
	core::position2d<s32> bOffset;

	core::stringw wString(text);

	u32 n;
	int j = 0;

	// Do contour font if any is set
	if ( GlyphsBold[0].borderSize != 0 ) // base ourselves on first one to see if this glyph set has a border
	{
		if ( hcenter )
		{
			offset.X += ( position.getWidth() - textDimension.Width ) / 2;
		}

		if ( vcenter )
		{
//			offset.Y += ( position.getHeight() / 2 ) - textDimension.Height * getVertBearingFactor();
			offset.Y += ( position.getHeight() - textDimension.Height) / 2; 
		}

		j = 0;
		while(wString[j])
		{
			n = getGlyphByChar(wString[j]);
			bool renderChar = startPos == -1 || endPos == -1 || (j >= startPos && j <= endPos);
			if ( n > 0 && renderChar)
			{
				bOffset = offset;
				bOffset.X -= ((GlyphsBold[n-1].texw - Glyphs[n-1].texw) / 2);
				bOffset.Y -= ((GlyphsBold[n-1].texh - Glyphs[n-1].texh) / 2);

				drawGlyph(&GlyphsBold[n-1], bOffset, clip, GlyphsBold[n-1].borderColor);
			}

			offset.X += getWidthFromCharacter(wString[j]);
			j++;
		}
	}

	// Actual font
	video::SColor colors[4];
	for (int i = 0;i < 4;i++)
	{
		colors[i] = color;
	}

	offset = position.UpperLeftCorner;
	if ( hcenter )
	{
		offset.X += ( position.getWidth() - textDimension.Width ) / 2;
	}

	if ( vcenter )
	{
		//offset.Y += ( position.getHeight() / 2 ) - textDimension.Height * getVertBearingFactor();
		offset.Y += ( position.getHeight() - textDimension.Height) / 2;
	}

	j = 0;
	while(wString[j])
	{
		n = getGlyphByChar(wString[j]);
		bool renderChar = startPos == -1 || endPos == -1 || (j >= startPos && j <= endPos);
		if ( n > 0 && renderChar)
		{
			drawGlyph(&Glyphs[n-1], offset, clip, color);
		}

		offset.X += getWidthFromCharacter(wString[j]);
		j++;
	}
	startPos = -1;
	endPos = -1;

}

//! draws text and clips it to the specified rectangle if wanted
void CGUITTFont::draw(const char* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::dimension2d<s32> textDimension = getDimension(text);
	core::position2d<s32> offset = position.UpperLeftCorner;
	core::position2d<s32> bOffset;

	u32 n;
	int j = 0;

	// Do contour font if any is set
	if ( GlyphsBold[0].borderSize != 0 ) // base ourselves on first one to see if this glyph set has a border
	{
		if ( hcenter )
		{
			offset.X += ( position.getWidth() - textDimension.Width ) / 2;
		}

		if ( vcenter )
		{
			//offset.Y += ( position.getHeight() / 2 ) - textDimension.Height * getVertBearingFactor();
			offset.Y += ( position.getHeight() - textDimension.Height) / 2;
		}

		u32 c;
		j = 0;
		while (*text)
		{
			c = core::iterateUTF8String(text);

			n = getGlyphByValue(c);
			bool renderChar = startPos == -1 || endPos == -1 || (j >= startPos && j <= endPos);
			if ( n > 0 && renderChar)
			{
				bOffset = offset;
				bOffset.X -= ((GlyphsBold[n-1].texw - Glyphs[n-1].texw) / 2);
				bOffset.Y -= ((GlyphsBold[n-1].texh - Glyphs[n-1].texh) / 2);

				drawGlyph(&GlyphsBold[n-1], bOffset, clip, GlyphsBold[n-1].borderColor);
			}

			offset.X += getWidthFromCharacter(c);
			++j;
		}
	}

	// Actual font
	video::SColor colors[4];
	for (int i = 0;i < 4;i++)
	{
		colors[i] = color;
	}

	offset = position.UpperLeftCorner;
	if ( hcenter )
	{
		offset.X += ( position.getWidth() - textDimension.Width ) / 2;
	}

	if ( vcenter )
	{
		//offset.Y += ( position.getHeight() / 2 ) - textDimension.Height * getVertBearingFactor();
		offset.Y += ( position.getHeight() - textDimension.Height) / 2;
	}

	u32 c;
	j = 0;
	while(*text)
	{
		c = core::iterateUTF8String(text);

		n = getGlyphByValue(c);
		bool renderChar = startPos == -1 || endPos == -1 || (j >= startPos && j <= endPos);
		if ( n > 0 &&  renderChar)
		{
			drawGlyph(&Glyphs[n-1], offset, clip, color);
		}

		offset.X += getWidthFromCharacter(c);
		++j;
	}
	startPos = -1;
	endPos = -1;
}

//! modifies the texture passed in parameter by embedding text in it
void CGUITTFont::drawInTexture(const wchar_t* text, video::ITexture* tex, core::rect<s32> position, video::SColor color, bool hcenter, bool vcenter)
{
	if (!Driver)
		return;

	if(!tex)
		return;

	core::dimension2d<s32> textDimension = getDimension(text);
	core::position2d<s32> offset = position.UpperLeftCorner;
	core::position2d<s32> bOffset;

	core::stringw wString(text);

	u32 n;
	int j = 0;

	u32 *texPixels = (u32*)(tex->lock());
	video::SColor texColor;
	video::SColor resultColor;
	video::SColor glyphColor;

	// Do contour font if any is set
	if ( GlyphsBold[0].borderSize != 0 ) // base ourselves on first one to see if this glyph set has a border
	{
		if ( hcenter )
		{
			offset.X += ( position.getWidth() - textDimension.Width ) / 2;
		}

		if ( vcenter )
		{
			offset.Y += ( position.getHeight() - textDimension.Height ) / 2;
		}

		
		j = 0;
		while(wString[j])
		{
			n = getGlyphByChar(wString[j]);
			if ( n > 0)
			{
				bOffset = offset;
				bOffset.X -= ((GlyphsBold[n-1].texw - Glyphs[n-1].texw) / 2);
				bOffset.Y -= ((GlyphsBold[n-1].texh - Glyphs[n-1].texh) / 2);

				drawGlyphInTexture(&GlyphsBold[n-1], tex, bOffset, 0, GlyphsBold[n-1].borderColor);
			}

			offset.X += getWidthFromCharacter(wString[j]);
			j++;
		}
	}

	// Actual font
	video::SColor colors[4];
	for (int i = 0;i < 4;i++)
	{
		colors[i] = color;
	}

	offset = position.UpperLeftCorner;
	if ( hcenter )
	{
		offset.X += ( position.getWidth() - textDimension.Width ) / 2;
	}

	if ( vcenter )
	{
		offset.Y += ( position.getHeight() - textDimension.Height ) / 2;
	}

	j = 0;
	while(wString[j])
	{
		n = getGlyphByChar(wString[j]);
		if ( n > 0)
		{
			drawGlyphInTexture(&Glyphs[n-1], tex, offset, 0, color);
		}

		offset.X += getWidthFromCharacter(wString[j]);
		j++;
	}

	tex->unlock();

}

//! modifies the texture passed in parameter by embedding text in it
void CGUITTFont::drawInTexture(const char* text, video::ITexture* tex, core::rect<s32> position, video::SColor color, bool hcenter, bool vcenter)
{
	if (!Driver)
		return;

	if(!tex)
		return;

	core::dimension2d<s32> textDimension = getDimension(text);
	core::position2d<s32> offset = position.UpperLeftCorner;
	core::position2d<s32> bOffset;

	u32 n;
	int j = 0;

	u32 *texPixels = (u32*)(tex->lock());
	video::SColor texColor;
	video::SColor resultColor;
	video::SColor glyphColor;

	// Do contour font if any is set
	if ( GlyphsBold[0].borderSize != 0 ) // base ourselves on first one to see if this glyph set has a border
	{
		if ( hcenter )
		{
			offset.X += ( position.getWidth() - textDimension.Width ) / 2;
		}

		if ( vcenter )
		{
			offset.Y += ( position.getHeight() - textDimension.Height ) / 2;
		}

		
		u32 c;
		while(*text)
		{
			c = core::iterateUTF8String(text);

			n = getGlyphByValue(c);

			if ( n > 0)
			{
				bOffset = offset;
				bOffset.X -= ((GlyphsBold[n-1].texw - Glyphs[n-1].texw) / 2);
				bOffset.Y -= ((GlyphsBold[n-1].texh - Glyphs[n-1].texh) / 2);

				drawGlyphInTexture(&GlyphsBold[n-1], tex, bOffset, 0, GlyphsBold[n-1].borderColor);
			}

			offset.X += getWidthFromCharacter(c);
		}
	}

	// Actual font
	video::SColor colors[4];
	for (int i = 0;i < 4;i++)
	{
		colors[i] = color;
	}

	offset = position.UpperLeftCorner;
	if ( hcenter )
	{
		offset.X += ( position.getWidth() - textDimension.Width ) / 2;
	}

	if ( vcenter )
	{
		offset.Y += ( position.getHeight() - textDimension.Height ) / 2;
	}

	u32 c;
	while(*text)
	{
		c = core::iterateUTF8String(text);

		n = getGlyphByValue(c);

		if ( n > 0)
		{
			drawGlyphInTexture(&Glyphs[n-1], tex, offset, 0, color);
		}

		offset.X += getWidthFromCharacter(c);
	}

	tex->unlock();

}

//! Calculates the index of the character in the text which is on a specific position.
s32 CGUITTFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x) const
{
	s32 x = 0;
	s32 idx = 0;

	while (text[idx]){
		x += getWidthFromCharacter(text[idx]);
		if (x >= pixel_x)
			return idx;
		++idx;
	}

	return -1;
}

//! Calculates the index of the character in the text which is on a specific position.
s32 CGUITTFont::getCharacterFromPos(const char* text, s32 pixel_x) const
{
	s32 x = 0;
	s32 idx = 0;

	//iterate characters of the UTF-8 string
	while (*text)
	{
		u32 c = core::iterateUTF8String(text);

		x += getWidthFromCharacter(c);
		if (x >= pixel_x)
			return idx;

		++idx;
	}

	return -1;
}

void CGUITTFont::setKerningWidth(s32 kerning){
}

void CGUITTFont::setKerningHeight(s32 kerning){
}

s32 CGUITTFont::getKerningWidth(const wchar_t* thisLetter, const wchar_t* previousLetter) const{
	return 0;
}

s32 CGUITTFont::getKerningHeight() const{
	return 0;
}

void CGUITTFont::setTracking(s32 tracking) {
	if(tracking<0)
		Tracking = 0;
	else
		Tracking = tracking;	
}

s32 CGUITTFont::getTracking() const {
	return Tracking;
}

void CGUITTFont::setAntiAlias(bool value){
	AntiAlias = value;
}

bool CGUITTFont::getAntiAlias() const{
	return AntiAlias;
}

//! sets if font supports transparency
void CGUITTFont::setTransparency(bool val)
{
	TransParency = val;
}

bool CGUITTFont::getTransparency() const
{
	return TransParency;
}

void CGUITTFont::setSpaceWidth(s32 width)
{
	SpaceWidth = width;
}

s32 CGUITTFont::getSpaceWidth()
{
	return SpaceWidth;
}


} // end namespace gui
} // end namespace irr
#endif // _IRR_COMPILE_WITH_FREETYPE_

