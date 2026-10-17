// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_XBOX_PLATFORM_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
#include "CXBOX360D3D9Driver.h"
#include "CXBOX360D3D9Texture.h"
#include "CFileSystem.h"
#include "IReadFile.h"
#include "irros.h"

#include <d3dx9tex.h>
#include <Xgraphics.h>

#ifndef _IRR_COMPILE_WITH_DIRECT3D_8_
// The D3DXFilterTexture function seems to get linked wrong when
// compiling with both D3D8 and 9, causing it not to work in the D3D9 device.
// So mipmapgeneration is replaced with my own bad generation in d3d 8 when
// compiling with both D3D 8 and 9.
// #define _IRR_USE_D3DXFilterTexture_
#endif // _IRR_COMPILE_WITH_DIRECT3D_8_

#ifdef _IRR_USE_D3DXFilterTexture_
#pragma comment(lib, "d3dx9.lib")
#endif

namespace irr
{
namespace video
{

//! rendertarget constructor
CXBOX360D3D9Texture::CXBOX360D3D9Texture(CXBOX360D3D9Driver* driver, const core::dimension2d<s32>& size, const char* name, bool bRTT)
: ITexture(name), Texture(0), RTTSurface(0), Driver(driver),
	TextureSize(size), ImageSize(size), Pitch(0),
	HasMipMaps(false), HardwareMipMaps(true), IsRenderTarget(bRTT), LoadOk(false)
{
	#ifdef _DEBUG
	setDebugName("CXBOX360D3D9Texture");
	#endif

	Device=driver->getExposedVideoData().D3D9.D3DDev9;
	if (Device)
		Device->AddRef();

	if(bRTT)
	{
		//TODO:XBOX 360 Texture: createRenderTarget
		createRenderTarget();
	}
	else
	{
		ColorFormat = getColorFormatFromD3DFormat(D3DFMT_A8R8G8B8);
		D3DXCreateTexture(Device, size.Width, size.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &Texture);

	}
	LoadOk = true;
}

CXBOX360D3D9Texture::CXBOX360D3D9Texture(io::IReadFile* ifile, CXBOX360D3D9Driver* driver, u32 flags, const char* name)
:ITexture(name), Texture(0), RTTSurface(0), Driver(driver), TextureSize(0,0), ImageSize(0,0), Pitch(0),
HasMipMaps(false), HardwareMipMaps(true), IsRenderTarget(false), LoadOk(false)
{
	#ifdef _DEBUG
	setDebugName("CXBOX360D3D9Texture");
	#endif	
	const bool generateMipLevels = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	Device=driver->getExposedVideoData().D3D9.D3DDev9;
	if (Device)
		Device->AddRef();
	//if(HardwareMipMaps)
	{
		HRESULT hr=-1;
		if(ifile)
		{
			hr=-1;
			long size = ifile->getSize();
			c8* buffer = irrnew c8[size];
			ifile->read(buffer, size);
			hr = D3DXCreateTextureFromFileInMemoryEx(Device, buffer, size, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, (generateMipLevels?0:1), 0, D3DFMT_A8R8G8B8, 
				D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_DEFAULT ,0, NULL, NULL, 
				&Texture);
			delete[] buffer;
			
			if(FAILED(hr))
			{
				os::Printer::log("Could not create DIRECT3D9 Texture by Hardware. 2", name, ELL_WARNING);
			}
			else
			{
				//ImageSize
				D3DSURFACE_DESC desc;
				Texture->GetLevelDesc(0, &desc);
				ImageSize.set(desc.Width, desc.Height);
				TextureSize.set(getTextureSizeFromSurfaceSize(desc.Width), getTextureSizeFromSurfaceSize(desc.Height));
				//TODO:yzh:check detial
				ColorFormat = getColorFormatFromD3DFormat(desc.Format);
				LoadOk = true;
				return;
			}
		}
	}

}

//! constructor
CXBOX360D3D9Texture::CXBOX360D3D9Texture(IImage* image, CXBOX360D3D9Driver* driver,
					   u32 flags, const char* name)
: ITexture(name), Texture(0), RTTSurface(0), Driver(driver),
TextureSize(0,0), ImageSize(0,0), Pitch(0),
HasMipMaps(false), HardwareMipMaps(true), IsRenderTarget(false), LoadOk(false)
{
	#ifdef _DEBUG
	setDebugName("CXBOX360D3D9Texture");
	#endif

	const bool generateMipLevels = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);

	Device=driver->getExposedVideoData().D3D9.D3DDev9;
	if (Device)
		Device->AddRef();

	if(HardwareMipMaps)
	//if(0)
	{
		HRESULT hr=-1;
		irr::core::stringc realpath = name;;
		if(realpath.find("game:") == -1)
		{
			realpath = driver->getFileSystem()->getAbsolutePath(name);
		}
		hr = D3DXCreateTextureFromFileEx(Device, realpath.c_str(), D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, (generateMipLevels?0:1), 0, D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED, D3DX_FILTER_LINEAR,D3DX_DEFAULT ,0, NULL, NULL, 
				&Texture);
		if(hr == D3D_OK)
		{
			//ImageSize
			D3DSURFACE_DESC desc;
			Texture->GetLevelDesc(0, &desc);
			ImageSize.set(desc.Width, desc.Height);
			TextureSize.set(getTextureSizeFromSurfaceSize(desc.Width), getTextureSizeFromSurfaceSize(desc.Height));
			//TODO:yzh:check detial
			ColorFormat = getColorFormatFromD3DFormat(desc.Format);
			LoadOk = true;
			return;			
		}
		else
		{
			os::Printer::log("Could not create DIRECT3D9 Texture by Hardware. 1", name, ELL_WARNING);
		}
		io::IReadFile* pFile =  driver->getFileSystem()->createAndOpenFile(name);
		//io::IReadFile* pFile = NULL;
		if(pFile)
		{
			hr=-1;
			long size = pFile->getSize();
			c8* buffer = irrnew c8[size];
			pFile->read(buffer, size);
			hr = D3DXCreateTextureFromFileInMemoryEx(Device, buffer, size, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, (generateMipLevels?0:1), 0, D3DFMT_A8R8G8B8, 
				D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_DEFAULT ,0, NULL, NULL, 
				&Texture);
			delete[] buffer;
			pFile->drop();


			
			if(FAILED(hr))
			{
				os::Printer::log("Could not create DIRECT3D9 Texture by Hardware. 2", name, ELL_WARNING);
			}
			else
			{
				//ImageSize
				D3DSURFACE_DESC desc;
				Texture->GetLevelDesc(0, &desc);
				ImageSize.set(desc.Width, desc.Height);
				TextureSize.set(getTextureSizeFromSurfaceSize(desc.Width), getTextureSizeFromSurfaceSize(desc.Height));
				//TODO:yzh:check detial
				ColorFormat = getColorFormatFromD3DFormat(desc.Format);
				LoadOk = true;
				return;
			}
		}
		else
		{
			os::Printer::log("Could not create DIRECT3D9 Texture by Hardware. 3", name, ELL_WARNING);
		}
	}
	//TODO:[xbox]:new matertial interface make load image mess
	_IRR_DEBUG_BREAK_IF(0);
	//Try create texture and mipmaps manually
	if (image)
	{
		if (createTexture(flags, image))
		{
			if (copyTexture(image) && generateMipLevels)
			{
				// create mip maps.
				#ifdef _IRR_USE_D3DXFilterTexture_
				//TODO:XBOX 360 Texture: _IRR_USE_D3DXFilterTexture_ not tested
					// The D3DXFilterTexture function seems to get linked wrong when
					// compiling with both D3D8 and 9, causing it not to work in the D3D9 device.
					// So mipmapgeneration is replaced with my own bad generation
					HRESULT hr  = D3DXFilterTexture(Texture, NULL, D3DX_DEFAULT, D3DX_DEFAULT);
					if (FAILED(hr))
						os::Printer::log("Could not create direct3d mip map levels.", ELL_WARNING);
					else
						HasMipMaps = true;
				#else
					createMipMaps();
					HasMipMaps = true;
				#endif
			}
			LoadOk = true;
		}
		else
			os::Printer::log("Could not create DIRECT3D9 Texture.", ELL_WARNING);
	}
}


//! destructor
CXBOX360D3D9Texture::~CXBOX360D3D9Texture()
{
	if (Texture)
		Texture->Release();

	if (RTTSurface)
		RTTSurface->Release();

	if (Device)
		Device->Release();
}

void CXBOX360D3D9Texture::copyPixelsFromColorBuffer(const irr::core::rect<irr::s32>& bounder)
{
	if(Device)
	{
		Device->Resolve(D3DRESOLVE_RENDERTARGET0, NULL, Texture, NULL, 0, 0, NULL, 1.0, 0, NULL );
	}
}

void CXBOX360D3D9Texture::createRenderTarget()
{
	// are texture size restrictions there ?
	if(!Driver->queryFeature(EVDF_TEXTURE_NPOT))
	{
		TextureSize.Width = getTextureSizeFromSurfaceSize(TextureSize.Width);
		TextureSize.Height = getTextureSizeFromSurfaceSize(TextureSize.Height);
		if (TextureSize != ImageSize)
			os::Printer::log("RenderTarget size has to be a power of two", ELL_INFORMATION);
	}

	// get backbuffer format to create the render target in the
	// same format

	IDirect3DSurface9* bb;
	D3DFORMAT d3DFormat = D3DFMT_A8R8G8B8;

	if (!FAILED(Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)))
	{
		D3DSURFACE_DESC desc;
		bb->GetDesc(&desc);
		d3DFormat = desc.Format;

		if (d3DFormat == D3DFMT_X8R8G8B8)
			d3DFormat = D3DFMT_A8R8G8B8;

		bb->Release();
	}
	else
	{
		os::Printer::log("Could not create RenderTarget texture: could not get BackBuffer.",
			ELL_WARNING);
		return;
	}

	// create texture
	HRESULT hr, hr2;

	hr = Device->CreateTexture(
		TextureSize.Width,
		TextureSize.Height,
		1, // mip map level count, we don't want mipmaps here
		0,
		d3DFormat,
		D3DPOOL_DEFAULT,
		&Texture,
		NULL);

	if (!FAILED(hr))
	{
		hr2 = Device->CreateRenderTarget(TextureSize.Width, TextureSize.Height, d3DFormat, D3DMULTISAMPLE_NONE, 0, 0, 
			&RTTSurface, NULL);
	}
	
	// get irrlicht format from D3D format
	ColorFormat = getColorFormatFromD3DFormat(d3DFormat);

	if (FAILED(hr) || FAILED(hr2))
	{
		if (D3DERR_INVALIDCALL == hr)
			os::Printer::log("Could not create render target texture", "Invalid Call");
		else
		if (D3DERR_OUTOFVIDEOMEMORY == hr)
			os::Printer::log("Could not create render target texture", "Out of Video Memory");
		else
		if (E_OUTOFMEMORY == hr)
			os::Printer::log("Could not create render target texture", "Out of Memory");
		else
			os::Printer::log("Could not create render target texture");
	}


}


bool CXBOX360D3D9Texture::createMipMaps(u32 level)
{
	if (level==0)
		return true;
#ifndef _IRR_XBOX_PLATFORM_
	if (HardwareMipMaps && Texture)
	{
		// generate mipmaps in hardware
		Texture->GenerateMipSubLevels();
		return true;
	}
	// os::Printer::log("manual mipmap");
#endif
	DWORD numberMipLevels = Texture->GetLevelCount();
	if(level >= numberMipLevels)
		return true;

	XGTEXTURE_DESC srcDesc,destDesc;
	XGGetTextureDesc( Texture, level-1, &srcDesc );
	XGGetTextureDesc( Texture, level, &destDesc );

	D3DLOCKED_RECT srcRect, destRect;
	Texture->LockRect( level-1, &srcRect, NULL, D3DLOCK_READONLY );
	Texture->LockRect( level, &destRect, NULL, 0 );

	copy32BitMipMap(((char*)srcRect.pBits), ((char*)destRect.pBits),
					destDesc.Width, destDesc.Height,
					srcRect.Pitch, destRect.Pitch, true);
	
	//c8 *buffer = irrnew c8[destDesc.RowPitch * destDesc.Height]; 
	//copy32BitMipMap(((char*)srcRect.pBits), buffer, destDesc.Width, destDesc.Height,srcRect.Pitch, destRect.Pitch, false);
	//XGTileTextureLevel(destDesc.Width, destDesc.Height, level, XGGetGpuFormat(D3DFMT_A8R8G8B8), 0, destRect.pBits, NULL, buffer, destRect.Pitch, NULL);
	//delete[] buffer;

	Texture->UnlockRect( level );
	Texture->UnlockRect( level-1 );


	bool result=true;
	if (!result || (srcDesc.Width < 3 && srcDesc.Height < 3))
		return result; // stop generating levels
/*
	IDirect3DSurface9* upperSurface = 0;
	IDirect3DSurface9* lowerSurface = 0;

	// get upper level
	HRESULT hr = Texture->GetSurfaceLevel(level-1, &upperSurface);
	if (FAILED(hr) || !upperSurface)
	{
		os::Printer::log("Could not get upper surface level for mip map generation", ELL_WARNING);
		return false;
	}

	// get lower level
	hr = Texture->GetSurfaceLevel(level, &lowerSurface);
	if (FAILED(hr) || !lowerSurface)
	{
		os::Printer::log("Could not get lower surface level for mip map generation", ELL_WARNING);
		upperSurface->Release();
		return false;
	}

	D3DSURFACE_DESC upperDesc, lowerDesc;
	upperSurface->GetDesc(&upperDesc);
	lowerSurface->GetDesc(&lowerDesc);

	D3DLOCKED_RECT upperlr;
	D3DLOCKED_RECT lowerlr;

	// lock upper surface
	if (FAILED(upperSurface->LockRect(&upperlr, NULL, 0)))
	{
		upperSurface->Release();
		lowerSurface->Release();
		os::Printer::log("Could not lock upper texture for mip map generation", ELL_WARNING);
		return false;
	}

	// lock lower surface
	if (FAILED(lowerSurface->LockRect(&lowerlr, NULL, 0)))
	{
		upperSurface->UnlockRect();
		upperSurface->Release();
		lowerSurface->Release();
		os::Printer::log("Could not lock lower texture for mip map generation", ELL_WARNING);
		return false;
	}

	if (upperDesc.Format != lowerDesc.Format)
	{
		os::Printer::log("Cannot copy mip maps with different formats.", ELL_WARNING);
	}
	else
	{
		//UINT lower_offset = XGGetMipLevelOffset(Texture, 0, level);
		//UINT high_offset = XGGetMipLevelOffset(Texture, 0, level-1);


		//if ((upperDesc.Format == D3DFMT_A1R5G5B5) || (upperDesc.Format == D3DFMT_R5G6B5))
		//	copy16BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
		//			lowerDesc.Width, lowerDesc.Height,
		//			upperlr.Pitch, lowerlr.Pitch);
		//else
		if (upperDesc.Format == D3DFMT_A8R8G8B8)
			copy32BitMipMap(((char*)upperlr.pBits), ((char*)lowerlr.pBits),
					lowerDesc.Width, lowerDesc.Height,
					upperlr.Pitch, lowerlr.Pitch);
		else
			os::Printer::log("Unsupported mipmap format, cannot copy. XBOX360 Irrlicht only support D3DFMT_A8R8G8B8 format currrently", ELL_WARNING);
	}

	
	// unlock
	if (FAILED(upperSurface->UnlockRect()))
		result=false;
	if (FAILED(lowerSurface->UnlockRect()))
		result=false;

	// release
	upperSurface->Release();
	lowerSurface->Release();

	if (!result || (upperDesc.Width < 3 && upperDesc.Height < 3))
		return result; // stop generating levels
//*/
	// generate next level
	return createMipMaps(level+1);
}


//! creates the hardware texture
bool CXBOX360D3D9Texture::createTexture(u32 flags, IImage * image)
{
	core::dimension2d<s32> optSize;
	ImageSize = image->getDimension();

	if (Driver->queryFeature(EVDF_TEXTURE_NPOT))
		optSize=ImageSize;
	else
	{
		optSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
		optSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);
	}

	HRESULT hr;
	D3DFORMAT format = D3DFMT_A1R5G5B5;

	switch(getTextureFormatFromFlags(flags))
	{
	case ETCF_ALWAYS_16_BIT:
		format = D3DFMT_A1R5G5B5; break;
	case ETCF_ALWAYS_32_BIT:
		format = D3DFMT_A8R8G8B8; break;
	case ETCF_OPTIMIZED_FOR_QUALITY:
		{
			switch(image->getColorFormat())
			{
			case ECF_R8G8B8:
			case ECF_A8R8G8B8:
				format = D3DFMT_A8R8G8B8; break;
			case ECF_A1R5G5B5:
			case ECF_R5G6B5:
				format = D3DFMT_A1R5G5B5; break;
			}
		}
		break;
	case ETCF_OPTIMIZED_FOR_SPEED:
		format = D3DFMT_A1R5G5B5;
		break;
	default:
		break;
	}
	if (Driver->getTextureCreationFlag(video::ETCF_NO_ALPHA_CHANNEL))
	{
		if (format == D3DFMT_A8R8G8B8)
		{
			format = D3DFMT_R8G8B8;
		}
		else if (format == D3DFMT_A1R5G5B5)
			format = D3DFMT_R5G6B5;
	}

	const bool mipmaps = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);

	DWORD usage = 0;
#ifndef _IRR_XBOX_PLATFORM_
	// This enables hardware mip map generation.
	if (mipmaps && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		LPDIRECT3D9 intf = Driver->getExposedVideoData().D3D9.D3D9;
		D3DDISPLAYMODE d3ddm;
		intf->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		if (D3D_OK==intf->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_AUTOGENMIPMAP,D3DRTYPE_TEXTURE,format))
		{
			usage = D3DUSAGE_AUTOGENMIPMAP;
			HardwareMipMaps = true;
		}
	}
#endif
	//HACK:XBOX 360 Texture: for debug mipmap 0->2
	hr = Device->CreateTexture(optSize.Width, optSize.Height,
		mipmaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		usage, // usage
		format, D3DPOOL_MANAGED , &Texture, NULL);

	if (FAILED(hr))
	{
		// try brute force 16 bit
		HardwareMipMaps = false;
		if (format == D3DFMT_A8R8G8B8)
			format = D3DFMT_A1R5G5B5;
		else if (format == D3DFMT_R8G8B8)
			format = D3DFMT_R5G6B5;
		else
			return false;
		//HACK:XBOX 360 Texture: for debug mipmap 0->2
		hr = Device->CreateTexture(optSize.Width, optSize.Height,
			mipmaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
			0, format, D3DPOOL_MANAGED, &Texture, NULL);
	}

	ColorFormat = getColorFormatFromD3DFormat(format);
	return (SUCCEEDED(hr));
}


D3DFORMAT CXBOX360D3D9Texture::getD3DFormatFromColorFormat(ECOLOR_FORMAT format) const
{
	switch(format)
	{
		case ECF_A1R5G5B5:
			return D3DFMT_A1R5G5B5;
		case ECF_R5G6B5:
			return D3DFMT_R5G6B5;
		case ECF_R8G8B8:
			return D3DFMT_R8G8B8;
		case ECF_A8R8G8B8:
			return D3DFMT_A8R8G8B8;
	}
	return D3DFMT_UNKNOWN;
}


ECOLOR_FORMAT CXBOX360D3D9Texture::getColorFormatFromD3DFormat(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		Pitch = TextureSize.Width * 2;
		return ECF_A1R5G5B5;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		Pitch = TextureSize.Width * 4;
		return ECF_A8R8G8B8;
	case D3DFMT_R5G6B5:
		Pitch = TextureSize.Width * 2;
		return ECF_R5G6B5;
#ifndef _IRR_XBOX_PLATFORM_
	case D3DFMT_R8G8B8:
		Pitch = TextureSize.Width * 3;
		return ECF_R8G8B8;
#endif
	default:
		return (ECOLOR_FORMAT)0;
	};
}


//! copies the image to the texture
bool CXBOX360D3D9Texture::copyTexture(IImage * image)
{
	if (Texture && image)
	{
		D3DSURFACE_DESC desc;
		Texture->GetLevelDesc(0, &desc);

		TextureSize.Width = desc.Width;
		TextureSize.Height = desc.Height;

		D3DLOCKED_RECT rect;
		HRESULT hr = Texture->LockRect(0, &rect, 0, 0);
		if (FAILED(hr))
		{
			os::Printer::log("Could not lock D3D9 Texture.", ELL_ERROR);
			return false;
		}

		Pitch = rect.Pitch;
		image->copyToScaling(rect.pBits, TextureSize.Width, TextureSize.Height, ColorFormat, Pitch);

		hr = Texture->UnlockRect(0);
		if (FAILED(hr))
		{
			os::Printer::log("Could not unlock D3D9 Texture.", ELL_ERROR);
			return false;
		}
	}

	return true;
}


//! lock function
void* CXBOX360D3D9Texture::lock(bool readOnly)
{
	if (!Texture)
		return 0;

	HRESULT hr;
	D3DLOCKED_RECT rect;
	if(!IsRenderTarget)
	{
		hr = Texture->LockRect(0, &rect, 0, readOnly?D3DLOCK_READONLY:0);
	}
	else
	{
		//TODO:XBOX 360 Texture: fix RTTSurface at here.
		D3DSURFACE_DESC desc;
		Texture->GetLevelDesc(0, &desc);
		if (!RTTSurface)
		{
#ifndef _IRR_XBOX_PLATFORM_
			hr = Device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &RTTSurface, 0);
			if (FAILED(hr))
#endif
			{
				os::Printer::log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
				return 0;
			}
		}

		IDirect3DSurface9 *surface = 0;
		hr = Texture->GetSurfaceLevel(0, &surface);
		if (FAILED(hr))
		{
			os::Printer::log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}
#ifndef _IRR_XBOX_PLATFORM_
		hr = Device->GetRenderTargetData(surface, RTTSurface);
#endif
		surface->Release();
		if(FAILED(hr))
		{
			os::Printer::log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}
		hr = RTTSurface->LockRect(&rect, 0, readOnly?D3DLOCK_READONLY:0);
		if(FAILED(hr))
		{
			os::Printer::log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
			return 0;
		}
		return rect.pBits;
	}
	if (FAILED(hr))
	{
		os::Printer::log("Could not lock DIRECT3D9 Texture.", ELL_ERROR);
		return 0;
	}

	return rect.pBits;
}


//! unlock function
void CXBOX360D3D9Texture::unlock()
{
	if (!Texture)
		return;

	if (!IsRenderTarget)
		Texture->UnlockRect(0);
	else if (RTTSurface)
		RTTSurface->UnlockRect();
}


//! Returns original size of the texture.
const core::dimension2d<s32>& CXBOX360D3D9Texture::getOriginalSize() const
{
	return ImageSize;
}


//! Returns (=size) of the texture.
const core::dimension2d<s32>& CXBOX360D3D9Texture::getSize() const
{
	return TextureSize;
}


//! returns the size of a texture which would be the optimize size for rendering it
inline s32 CXBOX360D3D9Texture::getTextureSizeFromSurfaceSize(s32 size) const
{
	s32 ts = 0x01;

	while(ts < size)
		ts <<= 1;

	return ts;
}



//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE CXBOX360D3D9Texture::getDriverType() const
{
	return EDT_DIRECT3D9;
}



//! returns color format of texture
ECOLOR_FORMAT CXBOX360D3D9Texture::getColorFormat() const
{
	return ColorFormat;
}



//! returns pitch of texture (in bytes)
u32 CXBOX360D3D9Texture::getPitch() const
{
	return Pitch;
}



//! returns the DIRECT3D9 Texture
IDirect3DTexture9* CXBOX360D3D9Texture::getDX9Texture() const
{
	return Texture;
}


//! returns if texture has mipmap levels
bool CXBOX360D3D9Texture::hasMipMaps() const
{
	return HasMipMaps;
}


void CXBOX360D3D9Texture::copy16BitMipMap(char* src, char* tgt,
				   s32 width, s32 height,
				   s32 pitchsrc, s32 pitchtgt) const
{
	UNIMPLEMENTED("CXBOX360D3D9Texture::copy16BitMipMap is unimplement. Currently, only support 32 bit textures");
	//for (s32 y=0; y<height; ++y)
	//{
	//	for (s32 x=0; x<width; ++x)
	//	{
	//		u32 a=0, r=0, g=0, b=0;

	//		for (s32 dy=0; dy<2; ++dy)
	//		{
	//			const s32 tgy = (y*2)+dy;
	//			for (s32 dx=0; dx<2; ++dx)
	//			{
	//				const s32 tgx = (x*2)+dx;

	//				SColor c;
	//				if (ColorFormat == ECF_A1R5G5B5)
	//					c = A1R5G5B5toA8R8G8B8(*(u16*)(&src[(tgx*2)+(tgy*pitchsrc)]));
	//				else
	//					c = R5G6B5toA8R8G8B8(*(u16*)(&src[(tgx*2)+(tgy*pitchsrc)]));

	//				a += c.getAlpha();
	//				r += c.getRed();
	//				g += c.getGreen();
	//				b += c.getBlue();
	//			}
	//		}

	//		a /= 4;
	//		r /= 4;
	//		g /= 4;
	//		b /= 4;

	//		u16 c;
	//		if (ColorFormat == ECF_A1R5G5B5)
	//			c = RGBA16(r,g,b,a);
	//		else
	//			c = A8R8G8B8toR5G6B5(SColor(a,r,g,b).color);
	//		*(u16*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
	//	}
	//}
}


void CXBOX360D3D9Texture::copy32BitMipMap(char* src, char* tgt,
				   s32 width, s32 height,
				   s32 pitchsrc, s32 pitchtgt, bool swizzled) const
{
	//TODO:XBOX 360 Texture:swizzle texture.....copy32BitMipMap also have bugs
	u32* u32_tgt = (u32*)tgt;
	u32* u32_src = (u32*)src;
	u32 src_pitch_texel = pitchsrc/4;
	u32 tgt_pitch_texel = pitchtgt/4;

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			u32 a=0, r=0, g=0, b=0;
			SColor c;

			for (s32 dy=0; dy<2; ++dy)
			{
				const s32 tgy = (y*2)+dy;
				for (s32 dx=0; dx<2; ++dx)
				{
					const s32 tgx = (x*2)+dx;
					//if(!swizzled)
					//{
					//	c = *(u32*)(&src[(tgx*4)+(tgy*pitchsrc)]);
					//}
					//else
					{
						UINT src_offset = XGAddress2DTiledOffset(tgx , tgy, src_pitch_texel, 4);
						c = u32_src[src_offset];
					}

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			c.set(a, r, g, b);
			if(!swizzled)
			{
				*(u32*)(&tgt[(x*4)+(y*pitchtgt)]) = c.toARGB8();
			}
			else
			{
				UINT tgt_offset = XGAddress2DTiledOffset(x ,y, tgt_pitch_texel, 4);
				u32_tgt[tgt_offset] = c.toARGB8();	
			}
			
			
		}
	}
}


//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
void CXBOX360D3D9Texture::regenerateMipMapLevels()
{
	if (HasMipMaps)
		createMipMaps();
}


//! returns if it is a render target
bool CXBOX360D3D9Texture::isRenderTarget() const
{
	return IsRenderTarget;
}

//! Returns pointer to the render target surface
IDirect3DSurface9* CXBOX360D3D9Texture::getRenderTargetSurface()
{
	if (!IsRenderTarget)
		return 0;

	return RTTSurface;
	//IDirect3DSurface9 *pRTTSurface = 0;
	//if (Texture)
	//	Texture->GetSurfaceLevel(0, &pRTTSurface);

	//if (pRTTSurface)
	//	pRTTSurface->Release();

	//return pRTTSurface;
}

static const D3DTEXTUREFILTERTYPE FilterMap[] =
{
	D3DTEXF_POINT,                // ETFT_NEAREST
	D3DTEXF_LINEAR,                 // ETFT_LINEAR
	D3DTEXF_POINT, // ETFT_NEAREST_MIPMAP_NEAREST
	D3DTEXF_LINEAR,  // ETFT_LINEAR_MIPMAP_NEAREST
	D3DTEXF_POINT,  // ETFT_NEAREST_MIPMAP_LINEAR
	D3DTEXF_LINEAR    // ETFT_LINEAR_MIPMAP_LINEAR
};

static const D3DTEXTUREFILTERTYPE FilterMipMap[] =
{
	D3DTEXF_POINT,                // ETFT_NEAREST
	D3DTEXF_LINEAR,                 // ETFT_LINEAR
	D3DTEXF_POINT, // ETFT_NEAREST_MIPMAP_NEAREST
	D3DTEXF_POINT,  // ETFT_LINEAR_MIPMAP_NEAREST
	D3DTEXF_LINEAR,  // ETFT_NEAREST_MIPMAP_LINEAR
	D3DTEXF_LINEAR    // ETFT_LINEAR_MIPMAP_LINEAR
};

static const D3DTEXTUREADDRESS WrapModeMap[] =
{
	D3DTADDRESS_WRAP,          // ETC_REPEAT
	D3DTADDRESS_CLAMP,           // ETC_CLAMP
	D3DTADDRESS_CLAMP,   // ETC_CLAMP_TO_EDGE
	D3DTADDRESS_BORDER, // ETC_CLAMP_TO_BORDER
	D3DTADDRESS_MIRROR, // ETC_MIRROR
};
void CXBOX360D3D9Texture::updateParameters(IDirect3DDevice9* pID3DDevice, int stage) const
{
	D3DCAPS9 &caps = Driver->getCaps();
	bool bmag_anisotropic = (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) && getFlag(ETF_ANISOTROPY_DIRTY);
	bool bmin_anisotropic = (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) && getFlag(ETF_ANISOTROPY_DIRTY);
	if (getFlag(ETF_MIN_FILTER_DIRTY))
	{
		if(bmin_anisotropic)
		{
			pID3DDevice->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			pID3DDevice->SetSamplerState(stage, D3DSAMP_MINFILTER, FilterMap[getMinFilter()]);
		}
		pID3DDevice->SetSamplerState(stage, D3DSAMP_MIPFILTER, FilterMipMap[getMinFilter()]);
	}
	if (getFlag(ETF_MAG_FILTER_DIRTY))
	{
		if(bmag_anisotropic)
		{
			pID3DDevice->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		}
		else
		{
			pID3DDevice->SetSamplerState(stage, D3DSAMP_MAGFILTER, FilterMap[getMagFilter()]);
		}
	}

	if (getFlag(ETF_WRAP_S_DIRTY))
	{
		pID3DDevice->SetSamplerState(stage, D3DSAMP_ADDRESSU, WrapModeMap[getWrapS()] );
	}
	if (getFlag(ETF_WRAP_T_DIRTY))
	{
		pID3DDevice->SetSamplerState(stage, D3DSAMP_ADDRESSV, WrapModeMap[getWrapT()] );
	}


	//			pID3DDevice->SetSamplerState(st, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	//			pID3DDevice->SetSamplerState(st, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	//			pID3DDevice->SetSamplerState(st, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	
//#ifdef _IRR_COMPILE_WITH_OPENGL_ES_
//	#ifdef GL_EXT_texture_lod_bias
//	if (getFlag(ETF_LOD_BIAS_DIRTY))
//	{
//		glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, getLODBias());
//	}
//	#endif
//#else
//	if (getFlag(ETF_LOD_BIAS_DIRTY))
//	{
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, getLODBias());
//	}
//	if (getFlag(ETF_MIN_LOD_DIRTY))
//	{
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, getMinLOD());
//	}
//	if (getFlag(ETF_MAX_LOD_DIRTY))
//	{
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, getMaxLOD());
//	}
//#endif

	clearDirtyFlags();	
}


} // end namespace video
} // end namespace irr

#endif // _IRR_XBOX_PLATFORM_


