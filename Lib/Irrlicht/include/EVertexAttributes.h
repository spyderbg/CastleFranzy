//-*-c++-*-
#ifndef __E_VERTEX_ATTRIBUTES_H_INCLUDED__
#define __E_VERTEX_ATTRIBUTES_H_INCLUDED__

namespace irr
{
namespace video
{

//! Bit masks for vertex attribute requirements of material/shaders.
enum E_VERTEX_ATTRIBUTE
{
	//! Position attribute (always present).
	EVA_POSITION = 1,

	//! Normal attribute.
	EVA_NORMAL = EVA_POSITION << 1,

	//! Vertex Color attribute.
	EVA_COLOR0 = EVA_NORMAL << 1,

	//! Secondary vertex Color attribute.
	EVA_COLOR1 = EVA_COLOR0 << 1,

	//! Texture coordinates, unit 0.
	EVA_TEXCOORD0 = EVA_COLOR1 << 1,

	//! Texture coordinates, unit 1.
	EVA_TEXCOORD1 = EVA_TEXCOORD0 << 1,

	//! Texture coordinates, unit 2.
	EVA_TEXCOORD2 = EVA_TEXCOORD1 << 1,

	//! Texture coordinates, unit 3.
	EVA_TEXCOORD3 = EVA_TEXCOORD2 << 1,
	
	// Add more EVA_TEXCOORDi if MATERIAL_MAX_TEXTURES > 4

	//! Mask combining all supported texture units.
	/** \see MATERIAL_MAX_TEXTURES */
	EVA_TEXCOORDS_MASK = (unsigned int)(EVA_TEXCOORD0
										| EVA_TEXCOORD1
										| EVA_TEXCOORD2
										| EVA_TEXCOORD3),

	//! Greatest bit mask used for all vertex attributes.
	EVA_LAST_ATTRIBUTE = EVA_TEXCOORD3
};

//! Returns the "normalized" texture coordinate attribute mask, i.e. shifted
//! such that TEXCOORD0 is the least significant bit.
inline u32 getTexCoordMask(u32 attributeMask)
{
	return (attributeMask & EVA_TEXCOORDS_MASK) >> 4;
}

} // end namespace video
} // end namespace irr

#endif
