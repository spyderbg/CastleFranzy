#ifndef __C_IMAGE_LOADER_PNG_H_INCLUDED__
#define __C_IMAGE_LOADER_PNG_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "IImageLoader.h"
#include <cell/codec/pngdec.h>
namespace irr
{
namespace video
{

//!  Surface Loader for PNG files
class CPS3ImageLoaderPng : public IImageLoader
{
public:
   CPS3ImageLoaderPng();
   ~CPS3ImageLoaderPng();
   //! returns true if the file maybe is able to be loaded by this class
   //! based on the file extension (e.g. ".png")
   virtual bool isALoadableFileExtension(const c8* fileName) const;

   //! returns true if the file maybe is able to be loaded by this class
   virtual bool isALoadableFileFormat(io::IReadFile* file) const;

   //! creates a surface from the file
   virtual IImage* loadImage(io::IReadFile* file) const;
};


} // end namespace video
} // end namespace irr

#endif

