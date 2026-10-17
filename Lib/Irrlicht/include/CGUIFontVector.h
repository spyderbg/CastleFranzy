// Author: Jinsong Zhao
// Mail:zhaojs-c@online.sh.cn

#ifndef __C_GUI_FONT_VECTOR_H_INCLUDED__
#define __C_GUI_FONT_VECTOR_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_
#ifdef _IRR_COMPILE_WITH_FREETYPE_
#include "IGUIFontVector.h"
#include "irrString.h"
#include "irrMap.h"
#include "IXMLReader.h"
#include "IReadFile.h"
#include "irrArray.h"
#include "IVideoDriver.h"
#include <new>

#define _IRR_USE_FREETYPE_
#define DEFAULT_PIXEL_SIZE 16

#ifdef _IRR_USE_FREETYPE_
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#else
#ifdef _IRR_WINDOWS_API_
#include <windows.h>
#endif
#endif
namespace irr
{

namespace video
{
	class IVideoDriver;
	class IImage;
	class ITexture;
}

namespace gui
{

	class IGUIEnvironment;


class CGUIFontVector : public IGUIFontVector
{
public:

	//! constructor
	CGUIFontVector(IGUIEnvironment* env);

	//! destructor
	virtual ~CGUIFontVector();

	//! loads a font from a texture file
	bool load(const c8* filename);

	//! loads a font from a texture file
	bool load(io::IReadFile* file);

	//! loads a font from an XML file
	bool load(io::IXMLReader* xml);

	//! draws an text and clips it to the specified rectangle if wanted
	virtual void draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0);
	
	//! draws UTF8-encoded text and clips it to the specified rectangle if wanted
	virtual void draw(const char* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0) {}

	//! modifies the texture passed in parameter by embedding text in it
	void drawInTexture(const wchar_t* text, video::ITexture* tex, core::rect<irr::s32> pos, video::SColor color, bool hcenter=true, bool vcenter=true) {}

	//! modifies the texture passed in parameter by embedding UTF8-encoded text in it
	void drawInTexture(const char* text, video::ITexture* tex, core::rect<irr::s32> pos, video::SColor color, bool hcenter=true, bool vcenter=true) {}

	virtual void setBorder (u32 borderSize, video::SColor borderColor) {};

	//! returns the dimension of a text
	virtual core::dimension2d<s32> getDimension(const wchar_t* text) const;

	//! Calculates the dimension of a UTF8-encoded text string.
	virtual core::dimension2d<s32> getDimension(const char* text) const {return core::dimension2d<s32>(0,0);}

	//! Calculates the index of the character in the text which is on a specific position.
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) const;

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() const { return EGFT_VECTOR; }

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual void setKerningWidth (s32 kerning);
	virtual void setKerningHeight (s32 kerning);

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual s32 getKerningWidth(const wchar_t* thisLetter=0, const wchar_t* previousLetter=0) const;
	virtual s32 getKerningHeight() const;

	virtual void setTracking(irr::s32 tracking) {}
	virtual s32 getTracking() const {return 0;}

	virtual IGUIFontVector* getNewSizeFont(u32 width = DEFAULT_PIXEL_SIZE,u32 height = DEFAULT_PIXEL_SIZE);
private:
	bool Antialias;
	bool Transparency;
	IGUIEnvironment* Environment;
	s32	GlobalKerningWidth, GlobalKerningHeight;
	static core::array<CGUIFontVector*> Fonts;
	// Vector Font
	class CFontFile : public virtual IReferenceCounted
	{
	public:
		CFontFile(const c8* filename,u32 width,u32 height);
		CFontFile(io::IReadFile* File,u32 width,u32 height);
		~CFontFile();
		static CFontFile* getFontFile(const c8* filename,u32 width,u32 height,IGUIEnvironment* env);
		static CFontFile* getFontFile(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env);
		u32 getWidth() const {return Width;}
		u32 getHeight() const {return Height;}
		const c8* getFileName(){return Filename.c_str();}
		bool loadChar(wchar_t c,bool antialias,video::IVideoDriver* driver);
		s32 getCharBufferWidth(){ return Ch.BufferWidth; }
		s32 getCharBufferHeight(){ return Ch.BufferHeight; }
		s32 getCharOffsetX(){ return Ch.OffsetX; }
		s32 getCharOffsetY(){ return Ch.OffsetY; }
		video::ITexture* getCharTexture(){ return Ch.Texture; }
	private:
		io::IReadFile* File;
		u32 Width;
		u32 Height;
		core::stringc Filename;
		static core::array<CFontFile*> FontFiles;

		class IFace;
		IFace* Face;
		static IFace* getFaceFactory(const c8* filename,u32 width,u32 height,IGUIEnvironment* env);
		static IFace* getFaceFactory(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env);

		class CChar;
		CChar* WrongChar;
		core::map<wchar_t,CChar*> CharCache;	
		video::ITexture* getTexture(u32* buffer,u32 width,u32 height,video::IVideoDriver* driver);
		
		CFontFile();
		CFontFile(const CFontFile&);
		CFontFile& operator=(const CFontFile&);
		// Cache
		class CChar
		{
		public:
			CChar()
			: BufferWidth(0),BufferHeight(0),OffsetX(0),OffsetY(0),Texture(0)
			{
			}
			CChar(s32 width,s32 height,s32 offsetX,s32 offsetY,video::ITexture *tex)
			: BufferWidth(width),BufferHeight(height),
			OffsetX(offsetX),OffsetY(offsetY),
			Texture(tex)
			{
			}
			~CChar(){}
			s32 BufferWidth;
			s32 BufferHeight;
			s32 OffsetX;
			s32 OffsetY;
			video::ITexture *Texture;
		private:
			CChar(const CChar&);
			CChar& operator=(const CChar&);
		};
		CChar Ch;
		//Interface, FreeType and Windows APIs
		class IFace : public virtual IReferenceCounted
		{
		public:
			virtual ~IFace(){}
			virtual bool loadChar(wchar_t ch,bool antialias,u32 width,u32 height) = 0;
			virtual u32 getCharWidth() = 0;
			virtual u32 getBufferWidth() = 0;
			virtual u32 getBufferHeight() = 0;
			virtual s32 getOffsetX() = 0;
			virtual s32 getOffsetY() = 0;
			virtual u32* getBuffer() = 0;
			virtual IFace* getFace(const c8* filename,u32 width,u32 height,IGUIEnvironment* env) = 0;
			virtual IFace* getFace(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env) = 0; 
		};
		// FreeType Impl
#ifdef _IRR_USE_FREETYPE_
		class CFreeTypeFace : public IFace
		{
		public:
			CFreeTypeFace(const c8* filename,u32 width,u32 height)
				:Face(0),pvBuffer(0),
				Filename(filename),
				Width(width),Height(height)
			{
				#ifdef _DEBUG
				setDebugName("CFreeTypeFace");
				#endif
			}
			~CFreeTypeFace();
			bool loadChar(wchar_t ch,bool antialias,u32 width,u32 height);
			virtual u32 getCharWidth(){return CharWidth;}
			virtual u32 getBufferWidth(){return BufferWidth;}
			virtual u32 getBufferHeight(){return BufferHeight;}
			virtual s32 getOffsetX(){return OffsetX;}
			virtual s32 getOffsetY(){return OffsetY;}
			virtual u32* getBuffer(){return (u32*)pvBuffer;}
			virtual IFace* getFace(const c8* filename,u32 width,u32 height,IGUIEnvironment* env);
			virtual IFace* getFace(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env);
		private:
			static core::array<CFreeTypeFace*> Faces;
			FT_Face Face;
			core::stringc Filename;
			u32 Width;
			u32 Height;
			void* pvBuffer;
			u32 CharWidth;
			u32 BufferWidth;
			u32 BufferHeight;
			s32 OffsetX;
			s32 OffsetY;
			CFreeTypeFace();
			CFreeTypeFace(const CFreeTypeFace&);
			CFreeTypeFace& operator=(const CFreeTypeFace&);

			// FreeType Library
			class CFreeType
			{
			public:
				CFreeType();
				~CFreeType();
				FT_Library& getFreeType(){ return FreeTypeLibrary;}
			private:
				static FT_Library FreeTypeLibrary;
			};
			static CFreeType FreeType;
		};
#else
		// Windows APIs Impl
		class CWinFace : public IFace
		{
		public:
			CWinFace(const c8* filename,u32 width,u32 height)
				:Font(0),pvBuffer(0),
				Filename(filename),
				Width(width),Height(height)
			{
				#ifdef _DEBUG
				setDebugName("CWinFace");
				#endif
			}
			~CWinFace();
			bool loadChar(wchar_t ch,bool antialias,u32 width,u32 height);
			virtual u32 getCharWidth(){return CharWidth;}
			virtual u32 getBufferWidth(){return BufferWidth;}
			virtual u32 getBufferHeight(){return BufferHeight;}
			virtual s32 getOffsetX(){return OffsetX;}
			virtual s32 getOffsetY(){return OffsetY;}
			virtual u32* getBuffer(){return (u32*)pvBuffer;}
			virtual IFace* getFace(const c8* filename,u32 width,u32 height,IGUIEnvironment* env);
			virtual IFace* getFace(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env);
		private:
			core::stringc getFaceName(io::IReadFile* file);
			static core::array<CWinFace*> Faces;
			core::stringc Filename;
			u32 Width;
			u32 Height;
			HFONT Font;
			void* pvBuffer;
			u32 CharWidth;
			u32 BufferWidth;
			u32 BufferHeight;
			s32 OffsetX;
			s32 OffsetY;
			CWinFace();
			CWinFace(const CWinFace&);
			CWinFace& operator=(const CWinFace&);
			
			// Windows DC
			class CWinDC
			{
			public:
				CWinDC();
				~CWinDC();
				HDC getHDC(){ return hdc;} 
			private:
				static HDC hdc;
			};
			static CWinDC WinDC;
		};
#endif
	};
	CFontFile* FontFile;
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_
#endif // _IRR_COMPILE_WITH_FREETYPE_
#endif // __C_GUI_FONT_VECTOR_H_INCLUDED__

