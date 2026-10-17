//-*-c++-*-
#ifndef __C_WII_TEXTURE_H_INCLUDED__
#define __C_WII_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_USE_WII_DEVICE_

#include <revolution/gx.h>
#include <revolution/mtx.h>

namespace irr
{
namespace io
{

class IReadFile;

}

namespace video
{

class CWiiDriver;

//! Wii texture.
class CWiiTexture : public ITexture
{
public:

	//! constructor
	CWiiTexture(IImage* surface, const char* name, CWiiDriver* driver=0);
	CWiiTexture(int w, int h, const char* name, CWiiDriver* driver=0);

	CWiiTexture(const char* name, int w, int h, GXTexFmt fmt, void* data);
	//void SetTexData(

#ifdef _IRR_ENABLE_NATIVE_TEXTURE_FORMAT_
	//! Construct from native format in file
	CWiiTexture(io::IReadFile* file, const char* name, CWiiDriver* driver=0, bool refData = false);
#endif

	//! destructor
	virtual ~CWiiTexture();

	//! lock function
	virtual void* lock(bool);

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture.
	virtual const core::dimension2d<s32>& getOriginalSize() const;

	//! Returns size of the texture.
	virtual const core::dimension2d<s32>& getSize() const;

	//! returns driver type of texture (=the driver, who created the texture)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! returns pitch of texture (in bytes)
	virtual u32 getPitch() const;

	//! Regenerates the mip map levels of the texture. Useful after
	//! locking and modifying the texture
	virtual void regenerateMipMapLevels();

	//! sets whether this texture is intended to be used as a render target.
	void setRenderTarget(bool isTarget);

	//! Bind RenderTargetTexture
	void bindRTT();

	//! Unbind RenderTargetTexture
	void unbindRTT();

	//! Returns whether the texture's content is valid.
	bool isValid() const;

	//! Returns the total memory space used by yhe texture.
	size_t getTexSize() const;

	//! Returns true is the LOD bias is clamped.
	bool getIsBiasClamped() const;

	//! Sets whether or not the LOD bias is clamped.
	void setIsBiasClamped(bool value);

	//! Make the texture in use for the given channel.
	void bind(int channel = 0) const;

	//! Updates the texture's parameters (as set in ITexture).
	void updateParameters() const;

	//! Indicates if the object owns the texture data.
	bool ownsTexData() const;

	//! Indicates if the texture is palettized.
	bool isPalettized() const;

private:

	enum E_TEXTURE_FLAG_EX
	{
		ETFE_OWNS_TEX_DATA = ETF_LAST_FLAG << 1,
		ETFE_IS_PALETTIZED = ETFE_OWNS_TEX_DATA << 1,
		ETFE_IS_BIAS_CLAMPED = ETFE_IS_PALETTIZED << 1
	};

	mutable GXTexObj TexObj;
	GXTlutObj TlutObj;
	core::dimension2d<s32>	Dim;
	core::dimension2d<s32>	DimOriginal;
	u8* TexData;
	size_t TexSize;
	io::IReadFile* File;
};

inline bool CWiiTexture::getIsBiasClamped() const
{
	return getFlag(ETFE_IS_BIAS_CLAMPED);
}

inline void CWiiTexture::bind(int channel) const
{
	if (isDirty())
	{
		updateParameters();
	}
	if (isPalettized())
	{
		GXLoadTlut(&TlutObj, GX_TLUT0);
	}
	GXLoadTexObj(&TexObj, (GXTexMapID)channel);
}

inline bool CWiiTexture::ownsTexData() const
{
	return getFlag(ETFE_OWNS_TEX_DATA);
}

inline bool CWiiTexture::isPalettized() const
{
	return getFlag(ETFE_IS_PALETTIZED);
}

} // end namespace video
} // end namespace irr

#endif // _IRR_USE_WII_DEVICE_
#endif // __C_WII_TEXTURE_H_INCLUDED__
