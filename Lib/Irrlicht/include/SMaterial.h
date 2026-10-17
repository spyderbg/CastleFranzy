//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_H_INCLUDED__
#define __S_MATERIAL_H_INCLUDED__

#include "SColor.h"
#include "matrix4.h"
#include "EMaterialTypes.h"
#include "EMaterialFlags.h"
#include "EMaterialFuncs.h"
#include "EVertexAttributes.h"
#include "SMaterialLayer.h"
#include "IAttributes.h"
#include "IAttributeExchangingObject.h"
#include "coreutil.h"

#define _IRR_MATERIAL_HASH_CODE_FOR_SORTING_

namespace irr
{
namespace video
{
class ITexture;

//! Flag for EMT_ONETEXTURE_BLEND, ( BlendFactor ) BlendFunc = source * sourceFactor + dest * destFactor
enum E_BLEND_FACTOR
{
	EBF_ZERO	= 0,		//!< src & dest	(0, 0, 0, 0)
	EBF_ONE,			//!< src & dest	(1, 1, 1, 1)
	EBF_DST_COLOR, 			//!< src	(destR, destG, destB, destA)
	EBF_ONE_MINUS_DST_COLOR, 	//!< src	(1-destR, 1-destG, 1-destB, 1-destA)
	EBF_SRC_COLOR,			//!< dest	(srcR, srcG, srcB, srcA)
	EBF_ONE_MINUS_SRC_COLOR, 	//!< dest	(1-srcR, 1-srcG, 1-srcB, 1-srcA)
	EBF_SRC_ALPHA,			//!< src & dest	(srcA, srcA, srcA, srcA)
	EBF_ONE_MINUS_SRC_ALPHA,	//!< src & dest	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
	EBF_DST_ALPHA,			//!< src & dest	(destA, destA, destA, destA)
	EBF_ONE_MINUS_DST_ALPHA,	//!< src & dest	(1-destA, 1-destA, 1-destA, 1-destA)
	EBF_SRC_ALPHA_SATURATE		//!< src	(min(srcA, 1-destA), idem, ...)
};

//! MaterialTypeParam: e.g. DirectX: D3DTOP_MODULATE, D3DTOP_MODULATE2X, D3DTOP_MODULATE4X
enum E_MODULATE_FUNC
{
	EMFN_MODULATE_1X	= 1,
	EMFN_MODULATE_2X	= 2,
	EMFN_MODULATE_4X	= 4
};

//! EMT_ONETEXTURE_BLEND: pack srcFact & dstFact and Modulo to MaterialTypeParam
inline f32 pack_texureBlendFunc ( const E_BLEND_FACTOR srcFact, const E_BLEND_FACTOR dstFact, const E_MODULATE_FUNC modulate )
{
	return (f32)(modulate << 16 | srcFact << 8 | dstFact);
}

//! EMT_ONETEXTURE_BLEND: unpack srcFact & dstFact and Modulo to MaterialTypeParam
inline void unpack_texureBlendFunc ( E_BLEND_FACTOR &srcFact, 
									 E_BLEND_FACTOR &dstFact,
									 E_MODULATE_FUNC &modulo, 
									 const f32 param )
{
	const u32 state = (u32)param;
	modulo	= E_MODULATE_FUNC( ( state & 0x00FF0000 ) >> 16 );
	srcFact = E_BLEND_FACTOR ( ( state & 0x0000FF00 ) >> 8 );
	dstFact = E_BLEND_FACTOR ( ( state & 0x000000FF ) );
}

//! Maximum number of texture an SMaterial can have.
const u32 MATERIAL_MAX_TEXTURES = 4;

//! Maximum number of parameters interpreted by material type.
const u32 MATERIAL_TYPE_MAX_PARAMS = 4;

//! Class for holding parameters for a material renderer
/** \note Should be called CMaterial, but left SMaterial for historical reasons. */
class SMaterial
{
public:

	//! Default constructor. Creates a solid, lit material with white colors
	SMaterial()
		: HashValue(1)
		, MaterialType(EMT_SOLID)
		, AmbientColor(255,255,255,255)
		, DiffuseColor(255,255,255,255)
		, EmissiveColor(0,0,0,0)
		, SpecularColor(255,255,255,255)
		, Shininess(0.0f)
		, Thickness(1.0f)
		, RenderFlags(EMF_GOURAUD_SHADING
					  | EMF_LIGHTING
					  // per vertex is "on" by default because ambient ==
					  // diffuse and there is lighting; however, the per vertex
					  // material color request flag is not set!
					  | EMF_PER_VERTEX_MATERIAL_COLOR
					  | EMF_ZBUFFER
					  | EMF_ZWRITE_ENABLE
					  | EMF_BACK_FACE_CULLING)
		, Funcs(ECFN_LEQUAL << EMFM_ZBUFFER_FUNC_SHIFT)
		, Flags(ESF_DIRTY_MASK)
	{
		for (int i = MATERIAL_TYPE_MAX_PARAMS; i > 0; )
		{
			--i;
			MaterialTypeParams[i] = 0.0f;
		}
// 		// ??? we shouldn't have to default this to a different value
// 		MaterialTypeParams[2] = 1.0f;
	}

	//!
	E_MATERIAL_TYPE getMaterialType() const
	{
		return MaterialType;
	}

	//!
	SMaterial& setMaterialType(E_MATERIAL_TYPE type)
	{
		if (MaterialType != type)
		{
			Flags |= ESF_MATERIAL_DIRTY;
			MaterialType = type;
		}
		return *this;
	}

	//!
	const SColor& getAmbientColor() const
	{
		return AmbientColor;
	}

	//!
	SMaterial& setAmbientColor(const SColor& color)
	{
		if (AmbientColor != color)
		{
			Flags |= ESF_DIRTY;
			AmbientColor = color;
			checkIfDiffuseIsBatchable();
		}
		return *this;
	}	

	//!
	const SColor& getDiffuseColor() const
	{
		return DiffuseColor;
	}

	//!
	SMaterial& setDiffuseColor(const SColor& color)
	{
		if (color != DiffuseColor)
		{
			Flags |= ESF_DIFFUSE_COLOR_DIRTY;
			DiffuseColor = color;
			checkIfDiffuseIsBatchable();
		}
		return *this;
	}

	//!
	SMaterial& setDiffuseAlpha(u32 alpha)
	{
		if (DiffuseColor.A != alpha)
		{
			Flags |= ESF_DIFFUSE_COLOR_DIRTY;
			DiffuseColor.A = alpha;
			checkIfDiffuseIsBatchable();
		}
		return *this;
	}

	//!
	const SColor& getSpecularColor() const
	{
		return SpecularColor;
	}

	//!
	SMaterial& setSpecularColor(const SColor& color)
	{
		if (color != SpecularColor)
		{
			Flags |= ESF_DIRTY;
			SpecularColor = color;
		}
		return *this;
	}	

	//!
	const SColor& getEmissiveColor() const
	{
		return EmissiveColor;
	}

	//!
	SMaterial& setEmissiveColor(const SColor& color)
	{
		if (color != EmissiveColor)
		{
			Flags |= ESF_DIRTY;
			EmissiveColor = color;
		}
		return *this;
	}	

	//!
	f32 getShininess() const
	{
		return Shininess;
	}

	//!
	SMaterial& setShininess(f32 shininess)
	{
		if (shininess != Shininess)
		{
			Flags |= ESF_DIRTY;
			Shininess = shininess;
		}
		return *this;
	}

	//!
	f32 getMaterialTypeParam(unsigned int i) const
	{
		_IRR_DEBUG_BREAK_IF(i >= MATERIAL_TYPE_MAX_PARAMS);
		return MaterialTypeParams[i];
	}

	//!
	SMaterial& setMaterialTypeParam(unsigned int i, f32 value)
	{
		if (MaterialTypeParams[i] != value)
		{
			Flags |= ESF_DIRTY;
			MaterialTypeParams[i] = value;
		}
		return *this;
	}

	//!
	f32 getThickness() const
	{
		return Thickness;
	}

	//!
	SMaterial& setThickness(f32 thickness)
	{
		if (thickness != Thickness)
		{
			Flags |= ESF_DIRTY;
			Thickness = thickness;
		}
		return *this;
	}

	//!
	bool getFlag(E_MATERIAL_FLAG flag) const
	{
		return (RenderFlags & flag) != 0;
	}

	//!
	SMaterial& setFlag(E_MATERIAL_FLAG flag, bool value)
	{
		if (flag & EMF_PER_VERTEX_MATERIAL_COLOR)
		{
			if (isPerVertexMaterialColorRequested() != value)
			{
				if (value)
				{
					HashValue |= 1;
				}
				else
				{
					HashValue &= ~1;
				}
				checkIfDiffuseIsBatchable();
			}
		}
		else if (getFlag(flag) != value)
		{
			Flags |= (flag & EMF_CRITICAL_MASK) ? ESF_MATERIAL_DIRTY : ESF_DIRTY;
			if (value)
			{
				RenderFlags |= flag;
			}
			else
			{
				RenderFlags &= ~flag;
			}
			if (flag & EMF_LIGHTING)
			{
				checkIfDiffuseIsBatchable();
			}
		}
		return *this;
	}

	//! Returns the original value manually set (via setFlag) for
	//! EMF_PER_VERTEX_MATERIAL_COLOR.
	bool isPerVertexMaterialColorRequested() const
	{
		return (HashValue & 1) != 0;
	}

	//!
	E_COMPARE_FUNC getZBufferFunc() const
	{
		return (E_COMPARE_FUNC)((Funcs & EMFM_ZBUFFER_FUNC) >> EMFM_ZBUFFER_FUNC_SHIFT);
	}

	//!
	SMaterial& setZBufferFunc(E_COMPARE_FUNC func)
	{
		u32 newFuncs = (Funcs & ~EMFM_ZBUFFER_FUNC) | (func << EMFM_ZBUFFER_FUNC_SHIFT);
		if (Funcs != newFuncs)
		{
			Flags |= ESF_DIRTY;
			Funcs = newFuncs;
		}
		return *this;
	}

	//! Gets the immutable texture transformation matrix for level i
	/** \param i The desired level.
		\return Texture matrix for texture level i, or identity matrix for levels larger than MATERIAL_MAX_TEXTURES. */
	const core::matrix4& getTextureMatrix(u32 i) const
	{
		if (i < MATERIAL_MAX_TEXTURES)
			return TextureLayer[i].getTextureMatrix();
		else
			return core::IdentityMatrix;
	}

	//! Tells if a texture matrix is available for the given texture unit.
	bool hasTextureMatrix(u32 i) const
	{
		return i < MATERIAL_MAX_TEXTURES && TextureLayer[i].hasTextureMatrix();
	}

	//! Sets the i-th texture transformation matrix
	/** \param i The desired level.
		\param mat Texture matrix for texture level i. */
	SMaterial& setTextureMatrix(u32 i, const core::matrix4& mat)
	{
		if (i >= MATERIAL_MAX_TEXTURES)
			return *this;
		// we systematically set the dirty flag, because otherwise we might have
		// to test 16 floats
		Flags |= ESF_TEXTURE_MATRIX0_DIRTY + i;
		TextureLayer[i].setTextureMatrix(mat);
		return *this;
	}

	//!
	SMaterial& setTextureMatrixElement(u32 unit, u32 i, u32 j, f32 val)
	{
		if (!hasTextureMatrix(i))
			return *this;
		float& elem = TextureLayer[i].getTextureMatrix()(i , j);
		if (elem != val)
		{
			Flags |= ESF_TEXTURE_MATRIX0_DIRTY + i;
			elem = val;
		}
		return *this;
	}

	//!
	SMaterial& setTextureMatrixElement(u32 unit, u32 i, f32 val)
	{
		if (!hasTextureMatrix(i))
			return *this;
		if (TextureLayer[i].getTextureMatrix()[i] != val)
		{
			Flags |= ESF_TEXTURE_MATRIX0_DIRTY + i;
			TextureLayer[i].getTextureMatrix()[i] = val;
		}
		return *this;
	}

	//! Gets the i-th texture
	/** \param i The desired level.
		\return Texture for texture level i, if defined, else 0. */
	ITexture* getTexture(u32 i) const
	{
		return i < MATERIAL_MAX_TEXTURES ? TextureLayer[i].getTexture() : 0;
	}

	//! Sets the i-th texture
	/** If i>=MATERIAL_MAX_TEXTURES this setting will be ignored.
		\param i The desired level.
		\param tex Texture for texture level i. */
	SMaterial& setTexture(u32 i, ITexture* tex)
	{
		if (i >= MATERIAL_MAX_TEXTURES)
			return *this;
		if (tex != TextureLayer[i].getTexture())
		{
			Flags |= ESF_TEXTURES_DIRTY;
			TextureLayer[i].setTexture(tex);
		}
		return *this;
	}

	//! Inequality operator
	/** \param b Material to compare to.
		\return True if the materials differ, else false. */
	bool operator != (const SMaterial& b) const
	{
		// if the hash value is different, for sure the materials are
		if (getHashValue() != b.getHashValue())
		{
			return true;
		}
		bool different =
			MaterialType != b.MaterialType ||
			AmbientColor != b.AmbientColor ||
			DiffuseColor != b.DiffuseColor ||
			EmissiveColor != b.EmissiveColor ||
			SpecularColor != b.SpecularColor ||
			Shininess != b.Shininess ||
			Thickness != b.Thickness ||
			RenderFlags != b.RenderFlags ||
			Funcs != b.Funcs;
		for (u32 i = 0; i < MATERIAL_TYPE_MAX_PARAMS && !different; ++i)
		{
			different |= (MaterialTypeParams[i] != b.MaterialTypeParams[i]);
		}
		for (u32 i = 0; i < MATERIAL_MAX_TEXTURES && !different; ++i)
		{
			different |= (TextureLayer[i] != b.TextureLayer[i]);
		}
		return different;
	}

	//! Equality operator
	/** \param b Material to compare to.
		\return True if the materials are equal, else false. */
	bool operator == (const SMaterial& other) const
	{
		return !(other != *this);
	}

	//! Material comparison for sorting.
	bool operator < (const SMaterial& other) const
	{
#ifdef _IRR_MATERIAL_HASH_CODE_FOR_SORTING_
		u32 hashCode = getHashValue();
		u32 otherHashCode = other.getHashValue();
		
		if (hashCode < otherHashCode)
		{
			return true;
		}
		if (hashCode == otherHashCode)
		{
			s32 diff = 0;
			diff = (s32((MaterialType << 4)
					   | (RenderFlags & EMF_CRITICAL_MASK))
					- s32((other.MaterialType << 4)
						  | (other.RenderFlags & EMF_CRITICAL_MASK)));
			if (diff < 0)
			{
				return true;
			}
			if (diff == 0)
			{
				for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
				{
					if (TextureLayer[i].getTexture()
						< other.TextureLayer[i].getTexture())
					{
						return true;
					}
				}
				// this is a little bit (a lot?) evil and somewhat lazy
				// programming, and it needs profiling to look at the overhead
				// of memcmp in this case
				return memcmp(&RenderFlags,
							  &other.RenderFlags,
							  // the compiler should transform this into a
							  // single constant
							  (sizeof(SMaterial)
							   - sizeof(HashValue)
							   - sizeof(MaterialType)
							   - sizeof(TextureLayer)
							   - sizeof(Flags)
							   - (getFlag(EMF_PER_VERTEX_MATERIAL_COLOR)
								  ? sizeof(DiffuseColor)
								  : 0))) < 0;
			}
		}
#else
		const s32 diff = (s32((MaterialType << 4)
							  | (RenderFlags & EMF_CRITICAL_MASK))
						  - s32((other.MaterialType << 4)
								| (other.RenderFlags & EMF_CRITICAL_MASK)));
		if (diff < 0)
		{
			return true;
		}
		if (diff == 0)
		{
			return TextureLayer[0].getTexture() < other.TextureLayer[0].getTexture();
		}
#endif
		return false;
	}

	//! Almost like the equality operator, except that some fields are not
	//! compared (diffuse color and texture matrices). Two material match each
	//! other if they are equivalent for batching.
	bool matches(const SMaterial& other) const
	{
		bool match = false;
		if (getHashValue() == other.getHashValue())
		{
			match =
				MaterialType == other.MaterialType &&
				AmbientColor == other.AmbientColor &&
				EmissiveColor == other.EmissiveColor &&
				SpecularColor == other.SpecularColor &&
				Shininess == other.Shininess &&
				Thickness == other.Thickness &&
				RenderFlags == other.RenderFlags &&
				Funcs == other.Funcs;
			if (match)
			{
				if (!getFlag(EMF_PER_VERTEX_MATERIAL_COLOR))
				{
					match &= DiffuseColor == other.DiffuseColor;
				}
				for (u32 i = 0; i < MATERIAL_TYPE_MAX_PARAMS && match; ++i)
				{
					match &= (MaterialTypeParams[i] == other.MaterialTypeParams[i]);
				}
				for (u32 i = 0; i < MATERIAL_MAX_TEXTURES && match; ++i)
				{
					match &= (TextureLayer[i].getTexture()
							  == other.TextureLayer[i].getTexture());
				}
			}
		}
		return match;
	}

	//!
	bool isTransparent() const
	{
		return MaterialType==EMT_TRANSPARENT_ADD_COLOR ||
			MaterialType==EMT_TRANSPARENT_ALPHA_CHANNEL ||
			MaterialType==EMT_TRANSPARENT_VERTEX_ALPHA ||
			MaterialType==EMT_TRANSPARENT_REFLECTION_2_LAYER ||
			MaterialType==EMT_TRANSPARENT_TEXTURE_VERTEX_ALPHA || 
			MaterialType==EMT_2D_ALPHA;
	}

	//!
	bool isDiffuseColorDirty() const
	{
		return (Flags & ESF_DIFFUSE_COLOR_DIRTY) != 0;
	}

	//!
	SMaterial& clearDiffuseColorDirtyFlag()
	{
		Flags &= ~ESF_DIFFUSE_COLOR_DIRTY;
		return *this;
	}

	//!
	bool isTextureMatrixDirty(int stage) const
	{
		if (stage >= MATERIAL_MAX_TEXTURES)
		{
			return false;
		}
		return (Flags & (ESF_TEXTURE_MATRIX0_DIRTY + stage)) != 0;
	}

	//!
	SMaterial& clearTextureMatrixDirtyFlag(u32 stage)
	{
		Flags &= ~(ESF_TEXTURE_MATRIX0_DIRTY + stage);
		return *this;
	}

	//! Returns a hash value describing the material.
	/** \note This hash value is appropriate to use for sorting, because the
		most significant bits are affected by material type, the next bits are
		affected by textures and the least significant bits are affected by
		parameters. */
	u32 getHashValue() const
	{
		updateHashValue();
		return HashValue;
	}
		
	void serializeAttributes(io::IAttributes* out,
							 io::SAttributeReadWriteOptions* options=0) const
	{
		out->addInt("Hash Value", HashValue, true);
		out->addEnum("Type", MaterialType, sBuiltInMaterialTypeNames);
		out->addColor("Ambient Color", AmbientColor);
		out->addColor("Diffuse Color", DiffuseColor);
		out->addColor("Emissive Color", EmissiveColor);
		out->addColor("Specular Color", SpecularColor);
		out->addFloat("Shininess", Shininess);
		_IRR_DEBUG_BREAK_IF(MATERIAL_TYPE_MAX_PARAMS != 4);
		out->addFloat("MaterialTypeParam", MaterialTypeParams[0]);
		out->addFloat("MaterialTypeParam2", MaterialTypeParams[1]);
		out->addFloat("MaterialTypeParam3", MaterialTypeParams[2]);
		out->addFloat("MaterialTypeParam4", MaterialTypeParams[3]);
		out->addFloat("Thickness", Thickness);
		out->addBool("Wireframe", getFlag(EMF_WIREFRAME));
		out->addBool("PointCloud", getFlag(EMF_POINTCLOUD));
		out->addBool("GouraudShading", getFlag(EMF_GOURAUD_SHADING));
		out->addBool("Lighting", getFlag(EMF_LIGHTING));
		out->addBool("Per Vertex Material Color", getFlag(EMF_PER_VERTEX_MATERIAL_COLOR), true);
		out->addBool("PerVertexMaterialColorRequested", isPerVertexMaterialColorRequested());
		out->addBool("ZWriteEnable", getFlag(EMF_ZWRITE_ENABLE));
		out->addBool("BackfaceCulling", getFlag(EMF_BACK_FACE_CULLING));
		out->addBool("FrontfaceCulling", getFlag(EMF_FRONT_FACE_CULLING));
		out->addBool("FogEnable", getFlag(EMF_FOG_ENABLE));
		out->addBool("NormalizeNormals", getFlag(EMF_NORMALIZE_NORMALS));
		out->addEnum("ZBuffer Test", getZBufferFunc(), CompareFuncNames);
		out->addBool("2DUseVertexAlpha", getFlag(EMF_2D_USE_VERTEX_ALPHA));
		out->addBool("2DUseTextureAlpha", getFlag(EMF_2D_USE_TEXTURE_ALPHA));
	}

	void deserializeAttributes(io::IAttributes* in,
							   io::SAttributeReadWriteOptions* options=0)
	{
		setMaterialType((irr::video::E_MATERIAL_TYPE)in->getAttributeAsEnumeration("Type", sBuiltInMaterialTypeNames));
		setAmbientColor(in->getAttributeAsColor("Ambient Color"));
		setDiffuseColor(in->getAttributeAsColor("Diffuse Color"));
		setEmissiveColor(in->getAttributeAsColor("Emissive Color"));
		setSpecularColor(in->getAttributeAsColor("Specular Color"));
		setShininess(in->getAttributeAsFloat("Shininess"));
		_IRR_DEBUG_BREAK_IF(MATERIAL_TYPE_MAX_PARAMS != 4);
		setMaterialTypeParam(0, in->getAttributeAsFloat("MaterialTypeParam"));
		setMaterialTypeParam(1, in->getAttributeAsFloat("MaterialTypeParam2"));
		setMaterialTypeParam(2, in->getAttributeAsFloat("MaterialTypeParam3"));
		setMaterialTypeParam(3, in->getAttributeAsFloat("MaterialTypeParam4"));
		setThickness(in->getAttributeAsFloat("Thickness"));
		setFlag(EMF_WIREFRAME, in->getAttributeAsBool("Wireframe"));
		setFlag(EMF_POINTCLOUD, in->getAttributeAsBool("PointCloud"));
		setFlag(EMF_GOURAUD_SHADING, in->getAttributeAsBool("GouraudShading"));
		setFlag(EMF_LIGHTING, in->getAttributeAsBool("Lighting"));
 		setFlag(EMF_PER_VERTEX_MATERIAL_COLOR, in->getAttributeAsBool("PerVertexMaterialColorRequested"));
		setFlag(EMF_ZWRITE_ENABLE, in->getAttributeAsBool("ZWriteEnable"));
		setFlag(EMF_BACK_FACE_CULLING, in->getAttributeAsBool("BackfaceCulling"));
		setFlag(EMF_FRONT_FACE_CULLING, in->getAttributeAsBool("FrontfaceCulling"));
		setFlag(EMF_FOG_ENABLE, in->getAttributeAsBool("FogEnable"));
		setFlag(EMF_NORMALIZE_NORMALS, in->getAttributeAsBool("NormalizeNormals"));
		setZBufferFunc((E_COMPARE_FUNC)in->getAttributeAsEnumeration("ZBuffer Test", CompareFuncNames));
		setFlag(EMF_2D_USE_VERTEX_ALPHA, in->getAttributeAsBool("2DUseVertexAlpha"));
		setFlag(EMF_2D_USE_TEXTURE_ALPHA, in->getAttributeAsBool("2DUseTextureAlpha"));
	}

private:

	void checkIfDiffuseIsBatchable()
	{
		if (!getFlag(EMF_LIGHTING)
			|| isPerVertexMaterialColorRequested()
			|| DiffuseColor == AmbientColor)
		{
			if (!getFlag(EMF_PER_VERTEX_MATERIAL_COLOR))
			{
				Flags |= ESF_MATERIAL_DIRTY;
			}
			RenderFlags |= EMF_PER_VERTEX_MATERIAL_COLOR;
		}
		else
		{
			if (getFlag(EMF_PER_VERTEX_MATERIAL_COLOR))
			{
				Flags |= ESF_MATERIAL_DIRTY;
			}
			RenderFlags &= ~EMF_PER_VERTEX_MATERIAL_COLOR;
		}
	}

	//! Updates the materials's hash code.
	/** \note Should be non const, but it actually updates a cached value that
		could be used in other const methods (thus HashValue is mutable). */
	void updateHashValue() const
	{
#ifdef _IRR_MATERIAL_HASH_CODE_FOR_SORTING_
		if (Flags & (ESF_DIRTY | ESF_MATERIAL_DIRTY | ESF_TEXTURES_DIRTY))
		{
			// compute a hash value; diffuse color and texture matrices are not
			// used to compute the hash value because when batching (if active),
			// they will be "baked" into the batch.

			if (Flags & ESF_MATERIAL_DIRTY)
			{
				// Compress MaterialType and critical render flags in the 8 most
				// significant bits
				HashValue
					= ((HashValue & 0x00FFFFFF) |
					   ((((MaterialType & 0xF) * 13 + MaterialType) << 28)
						// we actually ignore "gouraud shading" flag, is for
						// games it is mostly likely always set
						| ((RenderFlags & (EMF_CRITICAL_MASK
										   & ~EMF_GOURAUD_SHADING)) << 23)));
			}

			if (Flags & ESF_TEXTURES_DIRTY)
			{
				// Use next 12 bits for textures
				u32 texHash = 1973;
				for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
				{
					const ITexture* tex = TextureLayer[i].getTexture();
					texHash = core::hashBytes(&tex, texHash, sizeof(ITexture*));
				}
				HashValue = ((HashValue & 0xFF000FFF)
							 | (((texHash >> 8) & 0xFFF) << 12));
			}

			if (Flags & ESF_DIRTY)
			{
				// Lower 11 bits are used for the rest (ROPs and parameters)
				u32 nonCriticalFlags = RenderFlags & ~EMF_CRITICAL_MASK;
				u32 restHash = core::hashBytes(
					&Funcs,
					core::hashBytes(&nonCriticalFlags, 1973, 4),
					(sizeof(u32)
					 + 3 * sizeof(SColor)
					 + (2 + MATERIAL_TYPE_MAX_PARAMS) * sizeof(f32))
				);
				HashValue = ((HashValue & 0xFFFFF001)
							 | ((restHash >> 8) & 0xFFE));
			}

			Flags &= ~(ESF_DIRTY | ESF_MATERIAL_DIRTY | ESF_TEXTURES_DIRTY);
		}
#else
		if (Flags & ESF_DIRTY)
		{
			int hash = 1973;
			hash = core::hashBytes(&MaterialType, hash, sizeof(MaterialType));
			hash = core::hashBytes(&AmbientColor, hash, sizeof(AmbientColor));
			hash = core::hashBytes(&EmissiveColor, hash, sizeof(EmissiveColor));
			hash = core::hashBytes(&SpecularColor, hash, sizeof(SpecularColor));
			hash = core::hashBytes(&Shininess, hash, sizeof(Shininess));
			hash = core::hashBytes(&MaterialTypeParams[0], hash, sizeof(MaterialTypeParams));
			hash = core::hashBytes(&Thickness, hash, sizeof(Thickness));
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				ITexture* tex = TextureLayer[i].getTexture();
				hash = core::hashBytes(&tex, hash, sizeof(tex));
			}
			hash = core::hashBytes(&RenderFlags, hash, sizeof(RenderFlags));
			hash = core::hashBytes(&Funcs, hash, sizeof(Funcs));

			HashValue = (HashValue & 1) | (hash << 1);
			Flags &= ~ESF_DIRTY;
		}
#endif
	}

	//! Masks for material funcs packing.
	enum E_MATERIAL_FUNC_MASK
	{
		EMFM_ZBUFFER_FUNC = 0x00000007,
		EMFM_ZBUFFER_FUNC_SHIFT = 0
	};

	//! Collection of flags concerning the state if the material.
	enum E_STATE_FLAGS
	{
		//! Indicates that the material has changed (excludes diffuse color and
		//! texture matrices).
		ESF_MATERIAL_DIRTY = 1,

#ifdef _IRR_MATERIAL_HASH_CODE_FOR_SORTING_
		//!
		ESF_TEXTURES_DIRTY = ESF_MATERIAL_DIRTY << 1,

		//!
		ESF_DIRTY = ESF_TEXTURES_DIRTY << 1,
#else
		//!
		ESF_TEXTURES_DIRTY = ESF_MATERIAL_DIRTY,

		//!
		ESF_DIRTY = ESF_TEXTURES_DIRTY,
#endif

		//! Indicates that the diffuse color has changed.
		ESF_DIFFUSE_COLOR_DIRTY = ESF_DIRTY << 1,

		//! Indicates that the texture matrix of unit 0 has changed.
		ESF_TEXTURE_MATRIX0_DIRTY = ESF_DIFFUSE_COLOR_DIRTY << 1,

		//! Indicates that the texture matrix of unit 1 has changed.
		ESF_TEXTURE_MATRIX1_DIRTY = ESF_TEXTURE_MATRIX0_DIRTY << 1,

		//! Indicates that the texture matrix of unit 2 has changed.
		ESF_TEXTURE_MATRIX2_DIRTY = ESF_TEXTURE_MATRIX1_DIRTY << 1,

		//! Indicates that the texture matrix of unit 3 has changed.
		ESF_TEXTURE_MATRIX3_DIRTY = ESF_TEXTURE_MATRIX2_DIRTY << 1,

		// add more ESF_TEXTURE_MATRIXi_DIRTY of MATERIAL_MAX_TEXTURES > 4

		//! Bit mask to test global dirtiness.
		ESF_DIRTY_MASK = (unsigned int)(ESF_DIRTY
										| ESF_DIFFUSE_COLOR_DIRTY
										| ESF_TEXTURE_MATRIX0_DIRTY
										| ESF_TEXTURE_MATRIX1_DIRTY
										| ESF_TEXTURE_MATRIX2_DIRTY
										| ESF_TEXTURE_MATRIX3_DIRTY),

		//! Greatest bit mask used for dirty flags.
		ESF_LAST_DIRTY_BIT = ESF_TEXTURE_MATRIX3_DIRTY
	};

	// Attributes are almost ordered in priority order for sorting

	//!
	mutable u32 HashValue;

	//! Type of the material. Specifies how everything is blended together
	E_MATERIAL_TYPE MaterialType;

	//! Texture layer array.
	SMaterialLayer TextureLayer[MATERIAL_MAX_TEXTURES];

	//! Collects together all material flags (see E_MATERIAL_FLAGS),
	//! i.e. rendering flags.
	u32 RenderFlags;

	//! Collects together all functions used in conjunction of some material
	//! flags (e.g. z-buffer func).
	u32 Funcs;

	//! How much ambient light (a global light) is reflected by this material.
	/** The default is full white, meaning objects are completely
		globally illuminated. Reduce this if you want to see diffuse
		or specular light effects. */
	SColor AmbientColor;

	//! Light emitted by this material. Default is to emit no light.
	SColor EmissiveColor;

	//! How much specular light (highlights from a light) is reflected.
	/** The default is to reflect white specular light. See
		SMaterial::Shininess on how to enable specular lights. */
	SColor SpecularColor;

	//! Value affecting the size of specular highlights.
	/** A value of 20 is common. If set to 0, no specular
		highlights are being used. To activate, simply set the
		shininess of a material to a value other than 0:
		\code
		sceneNode->getMaterial(0).Shininess = 20.0f;
		\endcode

		You can change the color of the highlights using
		\code
		sceneNode->getMaterial(0).SpecularColor.set(255,255,255,255);
		\endcode

		The specular color of the dynamic lights
		(SLight::SpecularColor) will influence the the highlight color
		too, but they are set to a useful value by default when
		creating the light scene node. Here is a simple example on how
		to use specular highlights:
		\code
		// load and display mesh
		scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(
		smgr->getMesh("data/faerie.md2"));
		node->setMaterialTexture(0, driver->getTexture("data/Faerie2.pcx")); // set diffuse texture
		node->setMaterialFlag(video::EMF_LIGHTING, true); // enable dynamic lighting
		node->getMaterial(0).Shininess = 20.0f; // set size of specular highlights

		// add white light
		scene::ILightSceneNode* light = smgr->addLightSceneNode(0,
		core::vector3df(5,5,5), video::SColorf(1.0f, 1.0f, 1.0f));
		\endcode */
	f32 Shininess;

	//! Thickness of non-3dimensional elements such as lines and points.
	f32 Thickness;

	//! Free parameters, dependent on the material type.
	/** Mostly ignored, used for example in EMT_PARALLAX_MAP_SOLID
		and EMT_TRANSPARENT_ALPHA_CHANNEL. */
	f32 MaterialTypeParams[MATERIAL_TYPE_MAX_PARAMS];

	//! How much diffuse light coming from a light source is reflected by this material.
	/** The default is full white. */
	SColor DiffuseColor;

	//! Collects more flags not related to rendering pipeline state.
	mutable u32 Flags;
};

} // end namespace video
} // end namespace irr

#endif


