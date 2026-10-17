#include "stdafx.h"
#include "irrTexGen.h"
#include "coreutil.h"

namespace irr {

namespace video {

using core::stepPointer;

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
							   E_ORIENTATION orientation)
{
	core::vector3df v;
	core::vector3df n;
	while (endIndex-- > startIndex)
	{
		// transform vertex to eye coordinates
		modelview.transformVect(v, *positions);
		v.normalize();

		// same with normal; if inverse transposed of model view is provided, we
		// assume that there is scaling in the transform, thus we normalize the
		// normal; otherwise, we assume there isn't
		if (modelviewInvXPosed)
		{
			modelviewInvXPosed->rotateVect(n, *normals);
			n.normalize();
		}
		else
		{
			modelview.rotateVect(n, *normals);
		}

		v = v - (2 * (v.dotProduct(n))) * n;
		v.Z += 1.0f;
		f32 invp2 = 0.5f / v.getLength();

		switch (orientation)
		{
			case EOO_0 :
			{
				texCoords->X = v.X * invp2 + 0.5f;
				texCoords->Y = v.Y * invp2 + 0.5f;
			}
			break;

			case EOO_90 :
			{
				texCoords->X = v.Y * invp2 + 0.5f;
				texCoords->Y = 0.5f - v.X * invp2;
			}
			break;

			case EOO_180 :
			{
				texCoords->X = v.X * invp2 + 0.5f;
				texCoords->Y = 0.5f - v.Y * invp2;
			}
			break;

			case EOO_270 :
			{
				texCoords->X = v.Y * invp2 + 0.5f;
				texCoords->Y = v.X * invp2 + 0.5f;
			}
			break;
		}

		positions = stepPointer(positions, positionStride);
		normals = stepPointer(normals, normalsStride);
		texCoords = stepPointer(texCoords, texCoordsStride);
	}
}

void computeSimpleSphereMapTexCoords(const core::matrix4& modelviewInvXPosed,
									 u32 startIndex,
									 u32 endIndex,
									 const core::vector3df* normals,
									 u32 normalsStride,
									 core::vector2df* texCoords,
									 u32 texCoordsStride,
									 bool normalize,
									 E_ORIENTATION orientation)
{
	core::vector3df n;
	while (endIndex-- > startIndex)
	{
		// transform vertex to eye coordinates
		if (normalize)
		{
			modelviewInvXPosed.rotateVect(n, *normals);
			n.normalize();
		}
		else
		{
			modelviewInvXPosed.rotateVect(n, *normals);
// 			// we should make a new matrix method for this
// 			n.X = modelviewInvXPosed[0] * normals->X + modelviewInvXPosed[4] * normals->Y;
// 			n.Y = modelviewInvXPosed[1] * normals->X + modelviewInvXPosed[5] * normals->Y;
		}

		switch (orientation)
		{
			case EOO_0 :
			{
				texCoords->X = n.X * 0.5f + 0.5f;
				texCoords->Y = n.Y * 0.5f + 0.5f;
			}
			break;

			case EOO_90 :
			{
				texCoords->X = n.Y * 0.5f + 0.5f;
				texCoords->Y = 0.5f - n.X * 0.5f;
			}
			break;

			case EOO_180 :
			{
				texCoords->X = n.X * 0.5f + 0.5f;
				texCoords->Y = 0.5f - n.Y * 0.5f;
			}
			break;

			case EOO_270 :
			{
				texCoords->X = n.Y * 0.5f + 0.5f;
				texCoords->Y = n.X * 0.5f + 0.5f;
			}
			break;
		}

		normals = stepPointer(normals, normalsStride);
		texCoords = stepPointer(texCoords, texCoordsStride);
	}
}

} // end namespace video

} // end namespace irr
