#include "stdafx.h"
#include "IrrCompileConfig.h" 
#include "CImageLoaderDDS.h" 

#ifdef _IRR_COMPILE_WITH_DDS_LOADER_

#include "IVideoDriver.h" 
#include "CReadFile.h"
#include "CCompressedImage.h" 
#include "irros.h" 

#define DDS_MAGIC         0x20534444 

//  S_SURFACE_FORMAT_HEADER.flags 
#define DDSD_CAPS         0x00000001 
#define DDSD_HEIGHT         0x00000002 
#define DDSD_WIDTH         0x00000004 
#define DDSD_PITCH         0x00000008 
#define DDSD_PIXELFORMAT   0x00001000 
#define DDSD_MIPMAPCOUNT   0x00020000 
#define DDSD_LINEARSIZE      0x00080000 
#define DDSD_DEPTH         0x00800000 

//  S_SURFACE_FORMAT_HEADER.S_PIXEL_FORMAT.flags 
#define DDPF_ALPHAPIXELS   0x00000001 
#define DDPF_ALPHA         0x00000002 
#define DDPF_FOURCC         0x00000004 
#define DDPF_RGB         0x00000040 
#define DDPF_COMPRESSED      0x00000080 
#define DDPF_LUMINANCE      0x00020000 

//  S_SURFACE_FORMAT_HEADER.caps.caps1 
#define DDSCAPS1_COMPLEX   0x00000008 
#define DDSCAPS1_TEXTURE   0x00001000 
#define DDSCAPS1_MIPMAP      0x00400000 

//  S_SURFACE_FORMAT_HEADER.caps.caps2 
#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#ifndef MAKEFOURCC 
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
   ((s32)(u8)(ch0) | ((s32)(u8)(ch1) << 8) | \
   ((s32)(u8)(ch2) << 16) | ((s32)(u8)(ch3) << 24 ))
#endif

#define D3DFMT_R8G8B8               20 
#define D3DFMT_A8R8G8B8             21 
#define D3DFMT_X8R8G8B8             22 
#define D3DFMT_R5G6B5               23 
#define D3DFMT_A1R5G5B5             25 
#define D3DFMT_A4R4G4B4             26 
#define D3DFMT_A8B8G8R8             32 
#define D3DFMT_X8B8G8R8             33 
#define D3DFMT_DXT1                 MAKEFOURCC('D', 'X', 'T', '1') 
#define D3DFMT_DXT2                 MAKEFOURCC('D', 'X', 'T', '2') 
#define D3DFMT_DXT3                 MAKEFOURCC('D', 'X', 'T', '3') 
#define D3DFMT_DXT4                 MAKEFOURCC('D', 'X', 'T', '4') 
#define D3DFMT_DXT5                 MAKEFOURCC('D', 'X', 'T', '5') 

namespace irr 
{ 
namespace video 
{ 
   //! constructor 
   CImageLoaderDDS::CImageLoaderDDS() 
   { 
#ifdef _DEBUG 
      setDebugName("CImageLoaderDDS"); 
#endif 
   } 

   //! destructor 
   CImageLoaderDDS::~CImageLoaderDDS() 
   { 
   } 

   //! returns true if the file maybe is able to be loaded by this class 
   //! based on the file extension (e.g. ".tga") 
   bool CImageLoaderDDS::isALoadableFileExtension(const c8* fileName)  const
   { 
      return strstr(fileName, ".dds") != 0; 
   } 

   //! returns true if the file maybe is able to be loaded by this class 
   bool CImageLoaderDDS::isALoadableFileFormat(irr::io::IReadFile* file)  const
   { 
      if(!file) 
         return false; 

      char magicWord[4]; 
      file->read( &magicWord, 4 ); 
      return (magicWord[0] == 'D' && magicWord[1] == 'D' && magicWord[2] == 'S' ); 
   } 

   u32 CImageLoaderDDS::getImageSizeInBytes(core::dimension2d<s32>& dimension, u32 blockSize, u32 mipMapCount) const
   {
		u32 size = 0;
		int nWidth = dimension.Width;
		int nHeight = dimension.Height;

		u32 mipMap = mipMapCount;

		if (mipMap == 0)
		{
			mipMap = 1;
		}

		for( u32 i = 0; i < mipMap; ++i )
		{
			if( nWidth  == 0 ) nWidth  = 1;
			if( nHeight == 0 ) nHeight = 1;

			size += ((nWidth+3)/4) * ((nHeight+3)/4) * blockSize;

			// Half the image size for the next mip-map level...
			nWidth  = (nWidth  / 2);
			nHeight = (nHeight / 2);
		}

		return size;
   }

   //! creates a surface from the file 
   IImage* CImageLoaderDDS::loadImage(irr::io::IReadFile* file) const 
   { 
      IImage* image = NULL; 
      S_SURFACE_FORMAT_HEADER   header; 

      file->seek( 4 ); 
      file->read( &header, sizeof( S_SURFACE_FORMAT_HEADER ) ); 

#ifdef __BIG_ENDIAN__
	  header.size = os::Byteswap::byteswap(header.size);
	  header.flags = os::Byteswap::byteswap(header.flags);
	  header.height = os::Byteswap::byteswap(header.height);
	  header.width = os::Byteswap::byteswap(header.width);
	  header.pitch = os::Byteswap::byteswap(header.pitch);
	  header.depth = os::Byteswap::byteswap(header.depth);
	  header.mipMapCount = os::Byteswap::byteswap(header.mipMapCount);

	  header.pixelFormat.size = os::Byteswap::byteswap(header.pixelFormat.size);
	  header.pixelFormat.flags = os::Byteswap::byteswap(header.pixelFormat.flags);
	  header.pixelFormat.fourCC = os::Byteswap::byteswap(header.pixelFormat.fourCC);
	  header.pixelFormat.RGBBitCount = os::Byteswap::byteswap(header.pixelFormat.RGBBitCount);
	  header.pixelFormat.redBitMask = os::Byteswap::byteswap(header.pixelFormat.redBitMask);
	  header.pixelFormat.greenBitMask = os::Byteswap::byteswap(header.pixelFormat.greenBitMask);
	  header.pixelFormat.blueBitMask = os::Byteswap::byteswap(header.pixelFormat.blueBitMask);
	  header.pixelFormat.RGBAlphaMask = os::Byteswap::byteswap(header.pixelFormat.RGBAlphaMask);

	  header.caps.caps1 = os::Byteswap::byteswap(header.caps.caps1);
	  header.caps.caps2 = os::Byteswap::byteswap(header.caps.caps2);
	  header.caps.reserved[0] = os::Byteswap::byteswap(header.caps.reserved[0]);
	  header.caps.reserved[1] = os::Byteswap::byteswap(header.caps.reserved[1]);

	  header.reserved1[0] = os::Byteswap::byteswap(header.reserved1[0]);
	  header.reserved1[1] = os::Byteswap::byteswap(header.reserved1[1]);
	  header.reserved1[2] = os::Byteswap::byteswap(header.reserved1[2]);
	  header.reserved1[3] = os::Byteswap::byteswap(header.reserved1[3]);
	  header.reserved1[4] = os::Byteswap::byteswap(header.reserved1[4]);
	  header.reserved1[5] = os::Byteswap::byteswap(header.reserved1[5]);
	  header.reserved1[6] = os::Byteswap::byteswap(header.reserved1[6]);
	  header.reserved1[7] = os::Byteswap::byteswap(header.reserved1[7]);
	  header.reserved1[8] = os::Byteswap::byteswap(header.reserved1[8]);
	  header.reserved1[9] = os::Byteswap::byteswap(header.reserved1[9]);
	  header.reserved1[10] = os::Byteswap::byteswap(header.reserved1[10]);
	  header.reserved2 = os::Byteswap::byteswap(header.reserved2);
#endif
      if( header.size == 124 && ( header.flags & DDSD_PIXELFORMAT ) && ( header.flags & DDSD_CAPS ) ) 
      { 

         // determine if texture is 3d( has depth ) 
         bool is3D = header.depth > 0 && ( header.flags & DDSD_DEPTH ); 

         if( !is3D ) 
		 {
            header.depth = 1; 
		 }
		 else
		 {
			os::Printer::log( "UNSUPORTED DDS FORMAT TEXTURE", ELL_ERROR );
			return image; 
		 }

         // determine if texture has alpha 
         bool usingAlpha = header.pixelFormat.flags & DDPF_ALPHAPIXELS; 

         // color format to create image 
         ECOLOR_FORMAT format = ECF_UNKNOWN; 

         u32 dataSize = 0; 
#if 0
         if( header.pixelFormat.flags & DDPF_RGB ) 
         { 
            u32 byteCount = header.pixelFormat.RGBBitCount / 8; 
          
            if( header.flags & DDSD_PITCH ) 
               dataSize = header.pitch * header.height * header.depth * ( header.pixelFormat.RGBBitCount / 8 ); 
            else 
               dataSize = header.width * header.height * header.depth * ( header.pixelFormat.RGBBitCount / 8 ); 

            u8* data = irrnew u8[dataSize]; 
            file->read( data, dataSize ); 
             
            // has an alpha mask 
            switch( header.pixelFormat.RGBBitCount ) 
            { 
               case 16: 
               { 
                  if( usingAlpha ) 
                  { 
                     if( header.pixelFormat.RGBAlphaMask == 0x8000 ) 
                     { 
                        format = ECF_A1R5G5B5; 
                        os::Printer::print( "DDS : ECF_A1R5G5B5 format" ); 
                     } 
                     else if( header.pixelFormat.RGBAlphaMask == 0xf000 ) 
                     { 
                        format = ECF_A4R4G4B4; 
                        os::Printer::print( "DDS : ECF_A4R4G4B4 format" ); 
                     } 
                     else 
                     { 
                        os::Printer::print( "DDS : Unsupported 16 bit format with alpha" ); 
                     } 
                  } 
                  else 
                  { 
                     if( header.pixelFormat.redBitMask == 0xf800 ) 
                     { 
                        format = ECF_R5G6B5; 
                        os::Printer::print( "DDS : ECF_R5G6B5 format" ); 
                     } 
                  } 
                  break; 
               } 
               case 24: 
               { 
                  if( usingAlpha ) 
                  { 
                     os::Printer::print( "DDS : Unsupported 24 bit format with alpha" ); 
                  } 
                  else 
                  { 
                     if( header.pixelFormat.redBitMask & 0xff0000 ) 
                     { 
                        format = ECF_R8G8B8; 
                        os::Printer::print( "DDS : ECF_R8G8B8 format" ); 
                     } 
                  } 
                  break; 
               } 
               case 32: 
               { 
                  if( usingAlpha ) 
                  { 
                     if( header.pixelFormat.redBitMask & 0xff0000 ) 
                     { 
                        format = ECF_A8R8G8B8; 
                        os::Printer::print( "DDS : ECF_A8R8G8B8 format" ); 
                     } 
                     else if( header.pixelFormat.redBitMask & 0xff ) 
                     { 
                        format = ECF_A8R8G8B8; 
                        os::Printer::print( "DDS : ECF_A8R8G8B8 format" ); 

                        // convert data from A8B8G8R8 to A8R8G8B8 
                        u8 tmp = 0; 
                        for( u32 x=0; x<dataSize; x+=4 ) 
                        { 
                           tmp = data[x]; 
                           data[x] = data[x+2]; 
                           data[x+2] = tmp; 
                        } 
                     } 
                     else 
                     { 
                        os::Printer::print( "DDS : Unsupported 32 bit alpha format" ); 
                     } 
                  } 
                  else 
                  { 
                     if( header.pixelFormat.redBitMask & 0xff0000 ) 
                     { 
                        format = ECF_X8R8G8B8; 
                        os::Printer::print( "DDS : ECF_X8R8G8B8 format" ); 
                     } 
                     else if( header.pixelFormat.redBitMask & 0xff ) 
                     { 
                        format = ECF_X8R8G8B8; 
                        os::Printer::print( "DDS : ECF_X8R8G8B8 format" ); 

                        // convert data from X8B8G8R8 to X8R8G8B8 
                        u8 tmp = 0; 
                        for( u32 x=0; x<dataSize; x+=4 ) 
                        { 
                           data[x+3] = 255; 
                           tmp = data[x]; 
                           data[x] = data[x+2]; 
                           data[x+2] = tmp; 
                        } 
                     } 
                  } 
                  break; 
               } 
            } 

            if( format != ECF_UNKNOWN ) 
            { 
               if( is3D ) 
                  image = irrnew CImage( format, core::vector3d< s32 >( header.width, header.height, header.depth ), data ); 
               else 
                  image = irrnew CImage( format, core::vector2di( header.width, header.height ), data ); 
            } 
         } 
         else 
#endif
		 if( header.pixelFormat.flags & DDPF_FOURCC ) 
         { 
		    core::dimension2d<s32> dimension(header.width, header.height);

            switch( header.pixelFormat.fourCC ) 
            { 
               case D3DFMT_DXT1: 
               { 
				  dataSize = getImageSizeInBytes(dimension, 8, header.mipMapCount);
                  format = ECF_COMPRESSED_RGB_S3TC_DXT1; 
                  os::Printer::log( "DDS : ECF_DXT1 format" ); 
                  break; 
               } 
               case D3DFMT_DXT2: 
               case D3DFMT_DXT3: 
               { 
				  dataSize = getImageSizeInBytes(dimension, 16, header.mipMapCount);
                  format = ECF_COMPRESSED_RGBA_S3TC_DXT3; 
                  os::Printer::log( "DDS : ECF_DXT3 format" ); 
                  break; 
               } 
               case D3DFMT_DXT4: 
               case D3DFMT_DXT5: 
               { 
				  dataSize = getImageSizeInBytes(dimension, 16, header.mipMapCount);
                  format = ECF_COMPRESSED_RGBA_S3TC_DXT5; 
                  os::Printer::log( "DDS : ECF_DXT5 format" ); 
                  break; 
               } 
            } 

            if( format != ECF_UNKNOWN ) 
            { 
               u8* data = irrnew u8[ dataSize ]; 
			   irr::u32 bytesRead = file->read( data, dataSize ); 

               image = irrnew CCompressedImage( format, core::dimension2d<s32>( header.width, header.height ), data, dataSize, header.mipMapCount ); 
            } 
         } 
#if 0
         else if( header.pixelFormat.flags & DDPF_LUMINANCE ) // 8-bit luminance [ D3DFMT_L8 ] 
         { 
            format = ECF_L8; 
            dataSize = header.width * header.height * header.depth; 

            if( format != ECF_UNKNOWN ) 
            { 
               u8* data = irrnew u8[ dataSize ]; 
               file->read( data, dataSize ); 
    
               if( is3D ) 
                  image = irrnew CImage( format, core::dimension2d<s32>( header.width, header.height ), data ); 
               else 
                  image = irrnew CImage( format, core::dimension2d<s32>( header.width, header.height ), data ); 
            } 
         } 
#endif
         else 
         { 
            os::Printer::log( "UNKNOWN DDS FORMAT TEXTURE", ELL_ERROR ); 
         } 
      } 

      return image; 
   } 

   //! creates a loader which is able to load DDS images 
   IImageLoader* createImageLoaderDDS() 
   { 
      return irrnew CImageLoaderDDS(); 
   } 

} // end namespace video 
} // end namespace irr 

#endif
