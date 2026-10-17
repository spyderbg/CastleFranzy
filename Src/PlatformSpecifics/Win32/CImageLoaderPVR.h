// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_LOADER_PVR_H_INCLUDED__
#define __C_IMAGE_LOADER_PVR_H_INCLUDED__

#include <IImageLoader.h>

namespace irr
{
namespace video
{

class CImageLoaderPVR : public IImageLoader
{
public:

	//! constructor
	CImageLoaderPVR();

	//! returns true if the file maybe is able to be loaded by this class
	//! based on the file extension (e.g. ".tga")
	virtual bool isALoadableFileExtension(const c8* fileName) const;

	//! returns true if the file maybe is able to be loaded by this class
	virtual bool isALoadableFileFormat(io::IReadFile* file) const;

	//! creates a surface from the file
	virtual IImage* loadImage(io::IReadFile* file) const;

private:

};

} // end namespace video
} // end namespace irr

#endif

