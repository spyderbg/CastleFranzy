//-*-c++-*-
#ifndef __CTEXTUREMANAGER_H__
#define __CTEXTUREMANAGER_H__

#include "irrTypes.h"
#include "irrString.h"
#include "dimension2d.h"
#include "IImage.h"
#include "irrArray.h"

namespace irr
{

namespace io
{
class IReadFile;
} // namespace io

namespace video
{

class ITexture;
class CNullDriver;

class CTextureManager
{
public:

	CTextureManager(CNullDriver *driver);

	//! Creates a texture from a loaded IImage.
	ITexture* addTexture(const c8* name, IImage* image);

	//! adds a surface, not loaded or created by the Irrlicht Engine
	void addTexture(video::ITexture* texture);

	//! Creates a software image from a file.
	IImage* createImageFromFile(const char* filename);

	//! Creates a software image from a file.
	IImage* createImageFromFile(io::IReadFile* file);

	//! deletes all textures
	void deleteAllTextures();

	//! looks if the image is already loaded
	ITexture* findTexture(const c8* filename);

	//! opens the file and loads it into the surface
	virtual ITexture* loadTextureFromFile(io::IReadFile* file, const c8 *hashName = 0, bool refData = 0);

	//! loads a Texture
	virtual ITexture* getTexture(const c8* filename);

	//! loads a Texture
	virtual ITexture* getTexture(const core::stringc& filename)
	{ return getTexture(filename.c_str()); }

	//! loads a Texture
	virtual ITexture* getTexture(io::IReadFile* file, bool refData = false);

	//! Returns a texture by index
	virtual ITexture* getTextureByIndex(u32 index);

	//! Returns amount of textures currently loaded
	virtual u32 getTextureCount() const;

	//! Renames a texture
	virtual void renameTexture(ITexture* texture, const c8* newName);

	//! Removes a texture from the texture cache and deletes it, freeing lot of
	//! memory.
	void removeTexture(ITexture* texture);

	//! Removes all texture from the texture cache and deletes them, freeing lot of
	//! memory.
	void  removeAllTextures();

	//! creates a Texture
	virtual ITexture* addTexture(const core::dimension2d<s32>& size, const c8* name, ECOLOR_FORMAT format = ECF_A8R8G8B8);
	void setFullPathMode(bool use) {FullPath = use;}
protected:

	struct SSurface
	{
		ITexture* Surface;

		bool operator < (const SSurface& other) const;
	};

	core::array<SSurface> Textures;
	CNullDriver* Driver;
	bool		FullPath;
};

} // namespace video
} // namespace irr

#endif
