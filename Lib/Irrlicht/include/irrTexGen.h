#ifndef __IRR_TEX_GEN_H_INCLUDED__
#define __IRR_TEX_GEN_H_INCLUDED__

#include "matrix4.h"
#include "vector3d.h"
#include "vector2d.h"
#include "IVideoDriver.h"

namespace irr {

namespace video {

enum E_TEX_GEN_TYPE
{
	ETGT_NONE,
	ETGT_SPHERE_MAP,
	ETGT_SIMPLE_SPHERE_MAP
	// more to add as required
};

/**
   Computes sphere map texture coordinates.
   \param modelview Model*View current matrix.
   \param modelviewInvXPosed If non null, provides the normal transformation
   matrix matching modelview, which assumes a scaling is present and thus
   transformed normals will be normalized. If null, then modelview is used and
   no normal normalization is computed
   \param startIndex Start index of vertex range to compute in arrays.
   \param endIndex One past last index of vertex range to compute in arrays.
   \param positions Arrays of vertex positions.
   \param positionsStride Distance between start of each vertex positions (in bytes).
   \param normals Arrays of vertex normals.
   \param normalsStride Distance between start of each vertex normals (in bytes).
   \param texCoords Output arrays of vertex texture coordinates.
   \param texCoordsStride Distance between start of each vertex texture coordinates (in bytes).
   \param orientation Device orientation.

   \todo accomodate more types of arrays
 */
void computeSphereMapTexCoords(const core::matrix4& modelview,
							   const core::matrix4* modelviewInvXPosed,
							   u32 startIndex,
							   u32 endIndex,
							   const core::vector3df* positions,
							   u32 positionStride,
							   const core::vector3df* normals,
							   u32 normalsStride,
							   core::vector2df* texCoords,
							   u32 texCoordsStride,
							   E_ORIENTATION orientation = EOO_0);

/**
   Computes sphere map texture coordinates based solely on normal projection.
   \param modelviewInvXPosed Normal transformation matrix.
   \param startIndex Start index of vertex range to compute in arrays.
   \param endIndex One past last index of vertex range to compute in arrays.
   \param normals Arrays of vertex normals.
   \param normalsStride Distance between start of each vertex normals (in bytes).
   \param texCoords Output arrays of vertex texture coordinates.
   \param texCoordsStride Distance between start of each vertex texture coordinates (in bytes).
   \param normalize Indicates if normals should be normalized after transformation.
   \param orientation Device orientation.

   \todo accomodate more types of arrays
 */
void computeSimpleSphereMapTexCoords(const core::matrix4& modelviewInvXPosed,
									 u32 startIndex,
									 u32 endIndex,
									 const core::vector3df* normals,
									 u32 normalsStride,
									 core::vector2df* texCoords,
									 u32 texCoordsStride,
									 bool normalize = false,
									 E_ORIENTATION orientation = EOO_0);

} // end namespace video

} // end namespace irr

#endif
