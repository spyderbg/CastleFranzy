//-*-c++-*-
#ifndef __E_VERTEX_TYPE_H_INCLUDED__
#define __E_VERTEX_TYPE_H_INCLUDED__

namespace irr
{
namespace video
{

//! Enumeration for all vertex types there are.
enum E_VERTEX_TYPE
{
	//! Standard vertex type used by the Irrlicht engine, video::S3DVertex.
	EVT_STANDARD = 0,

	//! Vertex with two texture coordinates, video::S3DVertex2TCoords.
	/** Usually used for geometry with lightmaps or other special materials. */
	EVT_2TCOORDS,

	//! Vertex with a tangent and binormal vector, video::S3DVertexTangents.
	/** Usually used for tangent space normal mapping. */
	EVT_TANGENTS,

	//! Vertex with component loaded in array.
	/** Usually used with dae. */
	EVT_COMPONENT_ARRAYS,
};

} // end namespace video
} // end namespace irr

#endif
