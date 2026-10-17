//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_LAYER_H_INCLUDED__
#define __S_MATERIAL_LAYER_H_INCLUDED__

#include "matrix4.h"
#include "irrAllocator.h"
#include "ITexture.h"

namespace irr
{
namespace video
{

class ITexture;

//! Struct for holding material parameters which exist per texture layer
class SMaterialLayer
	// Private inheritance to profit from the empty base class optimization
	: private core::irrAllocator<irr::core::matrix4>
{
public:
	//! Default constructor
	SMaterialLayer()
		: Texture(0)
		, TextureMatrix(0)
	{
	}

	//! Copy constructor
	/** \param other Material layer to copy from. */
	SMaterialLayer(const SMaterialLayer& other)
	{
		// This pointer is checked during assignment
		TextureMatrix = 0;
		Texture = 0;
		*this = other;
	}

	//! Destructor
	~SMaterialLayer()
	{
		if(TextureMatrix)
		{
			destruct(TextureMatrix);
			deallocate(TextureMatrix); 
		}
		if (Texture)
		{
			Texture->drop();
		}
	}

	//! Assignment operator
	/** \param other Material layer to copy from.
		\return This material layer, updated. */
	SMaterialLayer& operator=(const SMaterialLayer& other)
	{
		setTexture(other.Texture);
		if (TextureMatrix)
		{
			if (other.TextureMatrix)
				*TextureMatrix = *other.TextureMatrix;
			else
			{
				destruct(TextureMatrix);
				deallocate(TextureMatrix); 
				TextureMatrix = 0;
			}
		}
		else
		{
			if (other.TextureMatrix)
			{
				TextureMatrix = allocate(1);
				construct(TextureMatrix,*other.TextureMatrix);
			}
			else
				TextureMatrix = 0;
		}

		return *this;
	}

	ITexture* getTexture() const
	{
		return Texture;
	}

	void setTexture(ITexture* texture)
	{
		if (texture)
			texture->grab();
		if (Texture)
			Texture->drop();
		Texture = texture;
	}

	//! Gets the texture transformation matrix
	/** \return Texture matrix of this layer. */
	core::matrix4& getTextureMatrix()
	{
		if (!TextureMatrix)
		{
			TextureMatrix = allocate(1);
			construct(TextureMatrix,core::IdentityMatrix);
		}
		return *TextureMatrix;
	}

	//! Gets the immutable texture transformation matrix
	/** \return Texture matrix of this layer. */
	const core::matrix4& getTextureMatrix() const
	{
		if (TextureMatrix)
			return *TextureMatrix;
		else
			return core::IdentityMatrix;
	}

	//! Tells if a texture matrix is available.
	bool hasTextureMatrix() const
	{
		return TextureMatrix != NULL;
	}

	//! Sets the texture transformation matrix to mat
	/** \param mat New texture matrix for this layer. */
	void setTextureMatrix(const core::matrix4& mat)
	{
		if (!TextureMatrix)
		{
			TextureMatrix = allocate(1);
			construct(TextureMatrix,mat);
		}
		else
			*TextureMatrix = mat;
	}

	//! Inequality operator
	/** \param b Layer to compare to.
		\return True if layers are different, else false. */
	inline bool operator!=(const SMaterialLayer& b) const
	{
		return (Texture != b.Texture
				|| ((TextureMatrix != b.TextureMatrix) &&
					TextureMatrix && b.TextureMatrix &&
					(*TextureMatrix != *(b.TextureMatrix))));
	}

	//! Equality operator
	/** \param b Layer to compare to.
		\return True if layers are equal, else false. */
	inline bool operator==(const SMaterialLayer& b) const
	{
		return !(b!=*this);
	}

private:

	friend class SMaterial;

	//! Texture
	ITexture* Texture;

	//! Texture Matrix
	/** Do not access this element directly as the internal
		ressource management has to cope with Null pointers etc. */
	core::matrix4* TextureMatrix;
};

} // end namespace video
} // end namespace irr

#endif // __S_MATERIAL_LAYER_H_INCLUDED__
