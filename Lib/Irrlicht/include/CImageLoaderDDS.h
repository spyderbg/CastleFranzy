#ifndef __C_IMAGE_LOADER_DDS_H_INCLUDED__ 
#define __C_IMAGE_LOADER_DDS_H_INCLUDED__ 

#include "IrrCompileConfig.h" 

#ifdef _IRR_COMPILE_WITH_DDS_LOADER_

#include "IImageLoader.h" 

namespace irr 
{ 
namespace video 
{ 
   class CImageLoaderDDS : public IImageLoader 
   { 
   public: 
      //! constructor 
      CImageLoaderDDS(); 

      //! destructor 
      virtual ~CImageLoaderDDS(); 

      //! returns true if the file maybe is able to be loaded by this class 
      //! based on the file extension (e.g. ".tga") 
      virtual bool isALoadableFileExtension(const c8* fileName) const; 

      //! returns true if the file maybe is able to be loaded by this class 
      virtual bool isALoadableFileFormat(irr::io::IReadFile* file) const; 

      //! creates a surface from the file 
      virtual IImage* loadImage(irr::io::IReadFile* file) const;

	  u32 getImageSizeInBytes(core::dimension2d<s32>& dimension, u32 blockSize, u32 mipMapCount) const;

   private:        

      struct S_PIXEL_FORMAT 
      { 
         u32   size; 
         u32 flags; 
         u32 fourCC; 
         u32 RGBBitCount; 
         u32   redBitMask; 
         u32 greenBitMask; 
         u32 blueBitMask; 
         u32   RGBAlphaMask; 
      }; 

      struct S_CAPS2 
      { 
         u32   caps1; 
         u32   caps2; 
         u32   reserved[2]; 
      }; 

      struct S_SURFACE_FORMAT_HEADER 
      { 
         u32            size; 
         u32            flags; 
         u32            height; 
         u32            width; 
         u32            pitch; 
         u32            depth; 
         u32            mipMapCount; 
         u32            reserved1[11]; 
         S_PIXEL_FORMAT   pixelFormat; 
         S_CAPS2         caps; 
         u32            reserved2; 
      }; 
#if 0
      struct S_R5G6B5 
      { 
         u16 b : 5; 
         u16 g : 6; 
         u16 r : 5; 

         u32 toA8R8G8B8() 
         { 
            video::SColor tmp( 255, 
               r * 0xFF / 0x1F, 
               g * 0xFF / 0x3F, 
               b * 0xFF / 0x1F ); 
            return tmp.toA8R8G8B8(); 
            //return 0xFF000000 | ( ( r * 0xFF ) / 0x1F ) | ( ( g * 0xFF ) / 0x3F ) | ( ( b * 0xFF ) / 0x1F ); 
         } 

         bool operator > ( const S_R5G6B5& other ) const 
         { 
            video::SColor tmp1( 255, 
               r * 0xFF / 0x1F, 
               g * 0xFF / 0x3F, 
               b * 0xFF / 0x1F ); 
            video::SColor tmp2( 255, 
               other.r * 0xFF / 0x1F, 
               other.g * 0xFF / 0x3F, 
               other.b * 0xFF / 0x1F ); 
            return tmp1.toA8R8G8B8() > tmp2.toA8R8G8B8(); 
            //return (*(u16*)this) > (*(u16*)&other); 
         } 
      }; 
#endif
   }; 

} // end namespace video 
} // end namespace demon 

#endif
#endif // __C_IMAGE_LOADER_DDS_H_INCLUDED__ 