#include "stdafx.h"
// Author: Jinsong Zhao
// Mail:zhaojs-c@online.sh.cn


#include "CGUIFontVector.h"
#ifdef _IRR_COMPILE_WITH_GUI_
#ifdef _IRR_COMPILE_WITH_FREETYPE_
#include "irros.h"
#include "IGUIEnvironment.h"
#include "IXMLReader.h"
#include "IFileSystem.h"
#include "IReadFile.h"
#include "IVideoDriver.h"

namespace irr
{
namespace gui
{
core::array<CGUIFontVector*> CGUIFontVector::Fonts;
core::array<CGUIFontVector::CFontFile*> CGUIFontVector::CFontFile::FontFiles;

//! constructor
CGUIFontVector::CGUIFontVector(IGUIEnvironment* env)
: Environment(env),GlobalKerningWidth(0),GlobalKerningHeight(0),
Antialias(false),Transparency(false),FontFile(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIFontVector");
	#endif

	if (Environment)
		Environment->grab();
}


//! destructor
CGUIFontVector::~CGUIFontVector()
{
	if (Environment)
		Environment->drop();
	
	if (FontFile)
		FontFile->drop();
	
	for(u32 i=0;i<Fonts.size();++i)
	{
		if (this==Fonts[i])
		{
			Fonts.erase(i);
			break;
		}
	}
}

//! loads a font file from xml
bool CGUIFontVector::load(io::IXMLReader* xml)
{
	core::stringc filename;
	u32 width = 0;
	u32 height = 0;
	while (xml->read())
	{
		if (io::EXN_ELEMENT == xml->getNodeType())
		{
			if (core::stringw(L"Source") == xml->getNodeName())
			{
				filename = xml->getAttributeValue(L"filename");
			}else if (core::stringw(L"Attribute") == xml->getNodeName())
			{
				width = (u32)xml->getAttributeValueAsInt(L"width");
				height = (u32)xml->getAttributeValueAsInt(L"height");
				Antialias = core::stringw(L"true").equals_ignore_case(xml->getAttributeValue(L"antialias")) ? true : false;
				Transparency = core::stringw(L"true").equals_ignore_case(xml->getAttributeValue(L"transparency")) ? true : false;
			}
		}
	}
	if (width==0 && height==0)
	{
		width = height = DEFAULT_PIXEL_SIZE;
	}
	FontFile = CFontFile::getFontFile(filename.c_str(),width,height,Environment);
	return (FontFile!=0);
}

//! loads a font file, native file needed, for texture parsing
bool CGUIFontVector::load(io::IReadFile* file)
{
	if (!Environment)
		return false;
	FontFile = CFontFile::getFontFile(file->getFileName(),DEFAULT_PIXEL_SIZE,DEFAULT_PIXEL_SIZE,Environment);
	return (FontFile!=0);
}

//! loads a font file, native file needed, for texture parsing
bool CGUIFontVector::load(const c8* filename)
{
	if (!Environment)
		return false;
	FontFile = CFontFile::getFontFile(filename,DEFAULT_PIXEL_SIZE,DEFAULT_PIXEL_SIZE,Environment);
	return (FontFile!=0);
}

IGUIFontVector* CGUIFontVector::getNewSizeFont(u32 width,u32 height)
{
	if (width==0 && height==0)return 0;
	if (width==FontFile->getWidth() && height==FontFile->getHeight())return 0;
	CFontFile* pFontFile = CFontFile::getFontFile(FontFile->getFileName(),width,height,Environment);
	if(pFontFile)
	{
		if(getReferenceCount()<=2) //using Fonts and a GUI object
		{
			FontFile->drop();
			FontFile = pFontFile;	
			return 0;
		}
		CGUIFontVector* vFont = irrnew CGUIFontVector(Environment);
		vFont->FontFile = pFontFile;
		Fonts.push_back(vFont);
		return vFont;
	}
	return 0;
}

//! returns the dimension of text
core::dimension2d<s32> CGUIFontVector::getDimension(const wchar_t* text) const
{
	s32 offsetX = 0;
	s32 offsetY = 0;
	while(*text)
	{
		if (*text == L'\r' || *text == L'\n') // Mac or Windows and Unix
		{
			if (*(text+1)== L'\n')++text; // Windows
			offsetY = FontFile->getHeight() + GlobalKerningHeight;
			++text;
			continue;
		}
		if(!FontFile->loadChar(*text,Antialias,Environment->getVideoDriver())){ ++text; continue; }
		offsetX = offsetX + FontFile->getCharOffsetX() + GlobalKerningWidth; 
		++text;
	}
	core::dimension2d<s32> dim(offsetX, offsetY);
	return dim;
}


//! set an Pixel Offset on Drawing ( scale position on width )
void CGUIFontVector::setKerningWidth ( s32 kerning )
{
	GlobalKerningWidth = kerning;
}


//! set an Pixel Offset on Drawing ( scale position on width )
s32 CGUIFontVector::getKerningWidth(const wchar_t* thisLetter, const wchar_t* previousLetter) const
{
	s32 ret = GlobalKerningWidth;
	return ret;
}


//! set an Pixel Offset on Drawing ( scale position on height )
void CGUIFontVector::setKerningHeight ( s32 kerning )
{
	GlobalKerningHeight = kerning;
}


//! set an Pixel Offset on Drawing ( scale position on height )
s32 CGUIFontVector::getKerningHeight () const
{
	return GlobalKerningHeight;
}

	struct DRAWTEXTURE{
		DRAWTEXTURE(video::ITexture* Texture, s32 x, s32 y, s32 w, s32 h)
		{
			pTex = Texture;offset = core::position2d<s32>(x,y);sourceRect = core::rect<s32>(0,0,w,h);
		}
		video::ITexture* pTex;
		core::position2d<s32> offset;
		core::rect<s32> sourceRect;
	};

//! draws some text and clips it to the specified rectangle if wanted
void CGUIFontVector::draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Environment)
		return;
/*
	struct DRAWTEXTURE{
		DRAWTEXTURE(video::ITexture* Texture, s32 x, s32 y, s32 w, s32 h)
		{
			pTex = Texture;offset = core::position2d<s32>(x,y);sourceRect = core::rect<s32>(0,0,w,h);
		}
		video::ITexture* pTex;
		core::position2d<s32> offset;
		core::rect<s32> sourceRect;
	}; */
	core::array<DRAWTEXTURE> drawTextures;
	s32 offsetX = 0;
	s32 offsetY = 0;
	bool mipmap = Environment->getVideoDriver()->getOption(video::EVDO_CREATE_TEXTURE_MIPMAPS);
	Environment->getVideoDriver()->setOption(video::EVDO_CREATE_TEXTURE_MIPMAPS, false);
	while(*text)
	{
		if (*text == L'\r' || *text == L'\n') // Mac or Windows and Unix
		{
			if (*(text+1)== L'\n')++text; // Windows
			offsetY = FontFile->getHeight() + GlobalKerningHeight;
			++text;
			continue;
		}
		if(!FontFile->loadChar(*text,Antialias,Environment->getVideoDriver())){ ++text; continue; }
		drawTextures.push_back(DRAWTEXTURE(FontFile->getCharTexture(),offsetX,FontFile->getHeight() - FontFile->getCharOffsetY(),FontFile->getCharBufferWidth(),FontFile->getCharBufferHeight()));
		offsetX = offsetX + FontFile->getCharOffsetX() + GlobalKerningWidth;
		++text;
	}
	// set previous mip-map+filter state
	Environment->getVideoDriver()->setOption(video::EVDO_CREATE_TEXTURE_MIPMAPS, mipmap);

	core::position2d<s32> offset = position.UpperLeftCorner;
	video::SColor colors[4];
	for (int i = 0;i < 4;i++){
		colors[i] = color;
	}
    if (hcenter)
	{
		offset.X = ((position.getWidth() - offsetX)>>1) + offset.X;
	}
	if (vcenter)
	{
		offset.Y = ((position.getHeight() - offsetY - FontFile->getHeight() )>>1) + offset.Y;
	}
	for(u32 i = 0;i<drawTextures.size();i++)
	{
		DRAWTEXTURE& Tex = drawTextures[i];
		if (!Transparency)	color.A = 0xff;
		Environment->getVideoDriver()->draw2DImage(Tex.pTex,core::position2d<s32>(Tex.offset.X+offset.X,Tex.offset.Y+offset.Y),Tex.sourceRect,clip,color,true);
	}
}

//! Calculates the index of the character in the text which is on a specific position.
s32 CGUIFontVector::getCharacterFromPos(const wchar_t* text, s32 pixel_x) const
{
	s32 idx = 0;
	s32 offsetX = 0;
	while(*text)
	{
		if (*text == L'\r' || *text == L'\n') // Mac or Windows and Unix
		{
			if (*(text+1)== L'\n')++text; // Windows
			++text;
			continue;
		}
		if(!FontFile->loadChar(*text,Antialias,Environment->getVideoDriver())){ ++text; continue; }
		offsetX = offsetX + FontFile->getCharOffsetX() + GlobalKerningWidth; 
		if (offsetX >= pixel_x)return idx;
		++text;
	}
	return -1;
}

CGUIFontVector::CFontFile::CFontFile(const c8* filename,u32 width,u32 height)
:Filename(filename),Width(width),Height(height),File(0),Face(0),WrongChar(0)
{
	#ifdef _DEBUG
	setDebugName("CFontFile");
	#endif
}

CGUIFontVector::CFontFile::CFontFile(io::IReadFile* file,u32 width,u32 height)
:Filename(file->getFileName()),Width(width),Height(height),File(file),Face(0),WrongChar(0)
{
	#ifdef _DEBUG
	setDebugName("CFontFile");
	#endif

	File->grab();
}

CGUIFontVector::CFontFile::~CFontFile()
{ 
	core::map<wchar_t,CChar*>::ParentLastIterator i(CharCache.getParentLastIterator());
	while(!i.atEnd())
	{
		core::map<wchar_t,CChar*>::Node* n = i.getNode();
		CChar* pChar = n->getValue();
		if (pChar!=WrongChar)
		{
			pChar->Texture->drop();
			delete pChar;
		}
		i++;
	}
	CharCache.clear();
	if(WrongChar){
		WrongChar->Texture->drop();
		delete WrongChar;
	}
	if(File)File->drop();
	if(Face)Face->drop();
	for(u32 i=0;i<FontFiles.size();++i)
	{
		if(this==FontFiles[i])
		{
			FontFiles.erase(i);
			break;
		}
	}
}

bool CGUIFontVector::CFontFile::loadChar(wchar_t c,bool antialias,video::IVideoDriver* driver)
{
	if(!WrongChar)
	{
		if( !Face->loadChar(L'A', antialias,Width,Height) )return false;
		if (Face->getCharWidth()==0)return false;
		u32* buffer = irrnew u32[Face->getBufferWidth()*Face->getBufferHeight()];
		memset(buffer,0,Face->getBufferWidth()*Face->getBufferHeight()*sizeof(u32));
		WrongChar = irrnew CChar(Face->getBufferWidth(),Face->getBufferHeight(),
			Face->getOffsetX(),Face->getOffsetY(),
			getTexture(buffer,Face->getBufferWidth(),Face->getBufferHeight(),driver));
		delete [] buffer;
	}
	core::map<wchar_t,CChar*>::Node* n = CharCache.find(c);
	if (n){
		memcpy(&Ch,n->getValue(),sizeof(Ch));
		return true;
	}
	if( !Face->loadChar(c, antialias,Width,Height) )return false;
	if (Face->getCharWidth()==0)
	{
		CharCache.insert(c,WrongChar);
		memcpy(&Ch,WrongChar,sizeof(Ch));
		return true;
	}
	CChar* pChar = irrnew CChar(Face->getBufferWidth(),Face->getBufferHeight(),
		Face->getOffsetX(),Face->getOffsetY(),
		getTexture(Face->getBuffer(),Face->getBufferWidth(),Face->getBufferHeight(),driver));
	CharCache.insert(c,pChar);
	memcpy(&Ch,pChar,sizeof(Ch));
	return true;
}

video::ITexture* CGUIFontVector::CFontFile::getTexture(u32* buffer,u32 width,u32 height,video::IVideoDriver* driver)
{
	video::IImage *img = driver->createImageFromData(video::ECF_A8R8G8B8,
		core::dimension2d<s32>(width,height),buffer);
	video::ITexture* tex = driver->addTexture("CGUIFontVectorChar",img);
	tex->grab();
	img->drop();
	driver->removeTexture(tex);
	return tex;
}

CGUIFontVector::CFontFile* CGUIFontVector::CFontFile::getFontFile(const c8* filename,u32 width,u32 height,IGUIEnvironment* env)
{
	CFontFile* pFontFile = 0;
	for(u32 i=0;i<FontFiles.size();++i)
	{
		pFontFile = FontFiles[i];
		if (pFontFile->Filename==filename && pFontFile->Width==width && pFontFile->Height==height)
		{
			pFontFile->grab();
			return pFontFile;
		}
	}
	pFontFile = irrnew CFontFile(filename,width,height);
	pFontFile->Face = getFaceFactory(filename,width,height,env);
	if (!pFontFile->Face)
	{
		pFontFile->drop();
		return 0;
	}
	FontFiles.push_back(pFontFile);
	return pFontFile;
}

CGUIFontVector::CFontFile* CGUIFontVector::CFontFile::getFontFile(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env)
{
	CFontFile* pFontFile = 0;
	for(u32 i=0;i<FontFiles.size();++i)
	{
		pFontFile = FontFiles[i];
		if (pFontFile->Filename==file->getFileName() && pFontFile->Width==width && pFontFile->Height==height)
		{
			pFontFile->grab();
			return pFontFile;
		}
	}
	pFontFile = irrnew CFontFile(file,width,height);
	pFontFile->Face = getFaceFactory(file,width,height,env);
	if (!pFontFile->Face)
	{
		pFontFile->drop();
		return 0;
	}
	FontFiles.push_back(pFontFile);
	return pFontFile;
}

#ifdef _IRR_USE_FREETYPE_

//#ifdef _DEBUG
//#pragma comment(lib, "freetype237ST_D.lib")
//#else
//#pragma comment(lib, "freetype237_D.lib")
//#endif

CGUIFontVector::CFontFile::CFreeTypeFace::CFreeType::CFreeType()
{
	if (!FreeTypeLibrary)
	{
		_IRR_DEBUG_BREAK_IF(FT_Init_FreeType( &FreeTypeLibrary ))
	}
}

CGUIFontVector::CFontFile::CFreeTypeFace::CFreeType::~CFreeType()
{
	if (!FreeTypeLibrary)
		FT_Done_FreeType(FreeTypeLibrary);
}

CGUIFontVector::CFontFile::CFreeTypeFace::~CFreeTypeFace()
{
	if (pvBuffer)delete [] pvBuffer;
	if (Face)FT_Done_Face(Face);
	for (u32 i=0;i<Faces.size();++i)
	{
		if (this==Faces[i])
		{
			Faces.erase(i);
			break;
		}
	}
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::CFreeTypeFace::getFace(const c8* filename,u32 width,u32 height,IGUIEnvironment* env)
{
	for(u32 i=0;i<Faces.size();++i)
	{
		CFreeTypeFace* pFace = Faces[i];
		if (pFace->Filename==filename)
		{
			pFace->grab();
			return pFace;
		}
	}
	CFreeTypeFace* pFace = irrnew CFreeTypeFace(filename,width,height);
	if(FT_New_Face(FreeType.getFreeType(),filename,0,&pFace->Face))
	{
		pFace->drop();
		return 0;
	}
	Faces.push_back(pFace);
	return pFace;
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::CFreeTypeFace::getFace(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env)
{
	for(u32 i=0;i<Faces.size();++i)
	{
		CFreeTypeFace* pFace = Faces[i];
		if (pFace->Filename==file->getFileName())
		{
			pFace->grab();
			return pFace;
		}
	}
	CFreeTypeFace* pFace = irrnew CFreeTypeFace(file->getFileName(),width,height);
	FT_Byte* buffer = irrnew FT_Byte[file->getSize()];
	file->read(buffer,file->getSize());
	if(FT_New_Memory_Face(FreeType.getFreeType(),buffer,file->getSize(),0,&pFace->Face))
	{
		delete [] buffer;
		pFace->drop();
		return 0;
	}
	delete [] buffer;
	Faces.push_back(pFace);
	return pFace;
}

bool CGUIFontVector::CFontFile::CFreeTypeFace::loadChar(wchar_t ch,bool antialias,u32 width,u32 height)
{
	FT_Int32 flags;
	if (antialias)flags = FT_LOAD_RENDER;
	else flags = FT_LOAD_RENDER|FT_LOAD_NO_HINTING;
	FT_Set_Pixel_Sizes(Face,width,height);
	if(FT_Load_Char(Face, ch, flags))return false;
	CharWidth = Face->glyph->metrics.width;
	BufferWidth = Face->glyph->bitmap.width;
	BufferHeight = Face->glyph->bitmap.rows;
	OffsetX = Face->glyph->metrics.width/64+(Face->glyph->metrics.vertAdvance - Face->glyph->metrics.width)/256;
	OffsetY = Face->glyph->metrics.height/64;
	if (pvBuffer) delete [] pvBuffer;
	u32* imgbuf = irrnew u32[Face->glyph->bitmap.width*Face->glyph->bitmap.rows];
	memset(imgbuf,0,Face->glyph->bitmap.width*Face->glyph->bitmap.rows*sizeof(u32));
	u8 *pbuf = Face->glyph->bitmap.buffer;
	u32 *pimgbuf = imgbuf;
	for (s16 i = 0;i < Face->glyph->bitmap.rows;i++)
	{
		for (s16 j = 0;j < Face->glyph->bitmap.width;j++)
		{
			if (*pbuf)
			{
				*pimgbuf = *pbuf << 24;
				*pimgbuf |= 0xffffff;
			}
			pbuf++;
			pimgbuf++;
		}
	}
	pvBuffer = imgbuf;
	return true;
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::getFaceFactory(const c8* filename,u32 width,u32 height,IGUIEnvironment* env)
{
	static CFreeTypeFace f("",0,0);
	return f.getFace(filename,width,height,env);
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::getFaceFactory(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env)
{
	static CFreeTypeFace f("",0,0);
	return f.getFace(file,width,height,env);
}

CGUIFontVector::CFontFile::CFreeTypeFace::CFreeType CGUIFontVector::CFontFile::CFreeTypeFace::FreeType;
FT_Library CGUIFontVector::CFontFile::CFreeTypeFace::CFreeType::FreeTypeLibrary = 0;
core::array<CGUIFontVector::CFontFile::CFreeTypeFace*> CGUIFontVector::CFontFile::CFreeTypeFace::Faces;
#else

#ifdef _IRR_WINDOWS_API_

CGUIFontVector::CFontFile::CWinFace::CWinDC::CWinDC()
{
	if (!hdc)
	{
		hdc = CreateCompatibleDC (NULL);
		_IRR_DEBUG_BREAK_IF(hdc==0)
	}
}

CGUIFontVector::CFontFile::CWinFace::CWinDC::~CWinDC()
{
	if (!hdc)
		DeleteDC (hdc);
}

CGUIFontVector::CFontFile::CWinFace::~CWinFace()
{
	::DeleteObject(Font);
	if( pvBuffer ) delete [] pvBuffer;
	for (u32 i=0;i<Faces.size();++i)
	{
		if (this==Faces[i])
		{
			Faces.erase(i);
			break;
		}
	}
}

typedef struct _tagTT_OFFSET_TABLE{
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
}TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY{
	char	szTag[4];			//table name
	ULONG	uCheckSum;			//Check sum
	ULONG	uOffset;			//Offset from beginning of file
	ULONG	uLength;			//length of the table in bytes
}TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER{
	USHORT	uFSelector;			//format selector. Always 0
	USHORT	uNRCount;			//Name Records count
	USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
}TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;	//from start of storage area
}TT_NAME_RECORD;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))
#define ALIGN_UP(n,align) (( (n) + ( (align)-1 ))&(~((align)-1)) )
#define ALIGN_DOWN(n,align) (( (n) - ( (align)-1 ))&(~((align)-1)) )

core::stringc CGUIFontVector::CFontFile::CWinFace::getFaceName(io::IReadFile* file)
{
	core::stringc fontName;
	TT_OFFSET_TABLE ttOffsetTable;
	if(file->read(&ttOffsetTable, sizeof(TT_OFFSET_TABLE))!=sizeof(TT_OFFSET_TABLE))
	{
		return fontName;
	}
	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);
	if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
	{
		return fontName;
	}
	TT_TABLE_DIRECTORY tblDir;
	bool bFound = false;
	for(int i=0; i< ttOffsetTable.uNumOfTables; i++){
		if( file->read(&tblDir, sizeof(TT_TABLE_DIRECTORY))!= sizeof(TT_TABLE_DIRECTORY))
		{
			return fontName;
		}
		core::stringc sName;
		sName.append((c8)tblDir.szTag[0]);
		sName.append((c8)tblDir.szTag[1]);
		sName.append((c8)tblDir.szTag[2]);
		sName.append((c8)tblDir.szTag[3]);
		if(sName.equals_ignore_case(core::stringc("name"))){
			bFound = true;
			tblDir.uLength = SWAPLONG(tblDir.uLength);
			tblDir.uOffset = SWAPLONG(tblDir.uOffset);
			break;
		}else if(sName.size()==0){
			break;
		}
	}
	if (bFound==false || file->seek(tblDir.uOffset)==false)
	{
		return fontName;
	}
	TT_NAME_TABLE_HEADER ttNTHeader;
	if( file->read(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER))!=sizeof(TT_NAME_TABLE_HEADER))
	{
		return fontName;
	}
	ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
	ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
	TT_NAME_RECORD ttRecord;
	for(int i=0;i<ttNTHeader.uNRCount && fontName.size()==0;i++)
	{
		if( file->read(&ttRecord, sizeof(TT_NAME_RECORD))!=sizeof(TT_NAME_RECORD))
		{
			return fontName;
		}
		ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
		ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
		ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

		if(ttRecord.uNameID == 1)
		{
			int nPos = file->getPos();
			file->seek(tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset);
			c8* pBuffer = irrnew c8[ttRecord.uStringLength+1];
			memset(pBuffer,0,ttRecord.uStringLength+1);
			if( file->read(pBuffer,ttRecord.uStringLength)!=ttRecord.uStringLength )
			{
				delete [] pBuffer;
				return fontName;
			}
			if(strlen(pBuffer) > 0){
				fontName.size()==0 ? fontName = pBuffer : void(0);
			}
			delete [] pBuffer;
			file->seek(nPos);
		}
	}
	return fontName;
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::CWinFace::getFace(const c8* filename,u32 width,u32 height,IGUIEnvironment* env)
{
	for(u32 i=0;i<Faces.size();++i)
	{
		CWinFace* pFace = Faces[i];
		if (pFace->Filename==filename && pFace->Width==width && pFace->Height==height)
		{
			pFace->grab();
			return pFace;
		}
	}
	io::IReadFile* rf = env->getFileSystem()->createAndOpenFile(filename);
	if (rf==0) return 0;
	core::stringc fontName = getFaceName(rf);
	rf->drop();
	if (fontName.size()==0)return 0;
	CWinFace* pFace = irrnew CWinFace(filename,width,height);
	LOGFONT lf;
	memset(&lf,0,sizeof(lf));
	lf.lfWidth = width;
	lf.lfHeight = height;	
	strncpy(lf.lfFaceName,fontName.c_str(),sizeof(lf.lfFaceName)-1); 
	//strncpy(lf.lfFaceName,filename,sizeof(lf.lfFaceName)-1);
	lf.lfWeight = 500;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfEscapement = 0;
	lf.lfItalic = 0;
	lf.lfOrientation = 0;
	lf.lfStrikeOut = 0;
	lf.lfUnderline = 0;
	pFace->Font = ::CreateFontIndirect(&lf);
	if(pFace->Font==0)
	{
		pFace->drop();
		return 0;
	}
	::SelectObject(WinDC.getHDC(),pFace->Font);
	Faces.push_back(pFace);
	return pFace;
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::CWinFace::getFace(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env)
{
	for(u32 i=0;i<Faces.size();++i)
	{
		CWinFace* pFace = Faces[i];
		if (pFace->Filename==file->getFileName() && pFace->Width==width && pFace->Height==height)
		{
			pFace->grab();
			return pFace;
		}
	}
	core::stringc fontName = getFaceName(file);
	if (fontName.size()==0)return 0;
	CWinFace* pFace = irrnew CWinFace(file->getFileName(),width,height);
	LOGFONT lf;
	memset(&lf,0,sizeof(lf));
	lf.lfWidth = width;
	lf.lfHeight = height;	
	strncpy(lf.lfFaceName,fontName.c_str(),sizeof(lf.lfFaceName)-1); 
	//strncpy(lf.lfFaceName,file->getFileName(),sizeof(lf.lfFaceName)-1);
	lf.lfWeight = 500;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfEscapement = 0;
	lf.lfItalic = 0;
	lf.lfOrientation = 0;
	lf.lfStrikeOut = 0;
	lf.lfUnderline = 0;
	pFace->Font = ::CreateFontIndirect(&lf);
	if(pFace->Font==0)
	{
		pFace->drop();
		return 0;
	}
	::SelectObject(WinDC.getHDC(),pFace->Font);
	Faces.push_back(pFace);
	return pFace;
}

bool CGUIFontVector::CFontFile::CWinFace::loadChar(wchar_t ch,bool antialias,u32 width,u32 height)
{
	TEXTMETRIC   tm;
	GLYPHMETRICS gm;
	MAT2 mat2;
	memset(&mat2,0,sizeof(mat2));
	mat2.eM11.value = 1;
	mat2.eM22.value = 1;
	memset(&gm,0,sizeof(GLYPHMETRICS));
	if (::GetTextMetrics(WinDC.getHDC(),&tm)==false ) return false;
	unsigned long jcBuffer = ::GetGlyphOutlineW(WinDC.getHDC(),ch,GGO_GRAY8_BITMAP,&gm,0,NULL,&mat2);
	if (jcBuffer==0 || jcBuffer==GDI_ERROR) return false;
	unsigned char* charBuffer = irrnew unsigned char[jcBuffer];
	if ( ::GetGlyphOutlineW(WinDC.getHDC(),ch,GGO_GRAY8_BITMAP,&gm,jcBuffer,charBuffer,&mat2)!=jcBuffer ) return false;
	CharWidth = Width;
	BufferWidth = ALIGN_UP( gm.gmBlackBoxX, 4 );
	BufferHeight = gm.gmBlackBoxY;
	OffsetX = gm.gmCellIncX-gm.gmptGlyphOrigin.x;
	OffsetY = gm.gmptGlyphOrigin.y;
	if (pvBuffer) delete [] pvBuffer;
	unsigned char* tmem = irrnew unsigned char[BufferWidth*BufferHeight*4];
	float gamma = 4.f;
	for( u16 i = 0;i<BufferHeight;i++ ){
		for( u16 j = 0;j<BufferWidth;j++ ){
			unsigned char* p = &tmem[(i*BufferWidth+j)*4];
			p[0] = 0xff;
			p[1] = 0xff;
			p[2] = 0xff;
			float alpha = (float)charBuffer[i*BufferWidth+j];
			int val = static_cast<int>(alpha*gamma);
			if( val > 255.f )val = 0xff;
			p[3] = (unsigned char) val;
		}
	}
	pvBuffer = tmem;
	delete [] charBuffer;
	return true;
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::getFaceFactory(const c8* filename,u32 width,u32 height,IGUIEnvironment* env)
{
	static CWinFace f("",0,0);
	return f.getFace(filename,width,height,env);
}

CGUIFontVector::CFontFile::IFace* CGUIFontVector::CFontFile::getFaceFactory(io::IReadFile* file,u32 width,u32 height,IGUIEnvironment* env)
{
	static CWinFace f("",0,0);
	return f.getFace(file,width,height,env);
}

CGUIFontVector::CFontFile::CWinFace::CWinDC CGUIFontVector::CFontFile::CWinFace::WinDC;
HDC CGUIFontVector::CFontFile::CWinFace::CWinDC::hdc = 0;
core::array<CGUIFontVector::CFontFile::CWinFace*> CGUIFontVector::CFontFile::CWinFace::Faces;

#endif //_IRR_WINDOWS_API_
#endif //_IRR_USE_FREETYPE_

} // end namespace gui
} // end namespace irr
#endif // _IRR_COMPILE_WITH_FREETYPE_
#endif // _IRR_COMPILE_WITH_GUI_
