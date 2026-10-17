#ifndef __C_MESH_COLLISION_H_INCLUDED__
#define __C_MESH_COLLISION_H_INCLUDED__

#include "line3d.h"
#include "vector3d.h"
#include "triangle3d.h"

namespace irr
{
namespace scene
{
class ISceneNode;
class IMesh;

//! Class to check a collision between a ray and a mesh from a Irrlicht 
//! Scene Node.
//! This class does not make local copy of the mesh as TriangleSelector does.
class CMeshCollision
{
public:
	//! Constructor: specify the mesh and scene node
	CMeshCollision(const IMesh* mesh, 
				   const ISceneNode* node);

	//! Constructor: Try to automatically find the right mesh from the scenenode
	CMeshCollision(const ISceneNode* node);

	//! Check if there is a collision with any meshbuffers in the mesh/SceneNode
	//! associated with the current CMeshCollision instance.
	bool isRayCollideMesh(const core::line3df& ray) const;

protected:
	//! Check collision between a line and a tringle with some sortcuts, 
	//! must be called by 'isRayCollideMesh'
	bool checkCollision(const core::triangle3df &triangle, 
						const core::line3df &line ) const;

	//! Pointer to the scenenode, we'll use it's transformation matrix
	const ISceneNode* SceneNode;

	//! Pointer to the mesh object to check against.
	const IMesh* Mesh;

	//! Tell if the mesh is in worldspace. This will tells us if we have to 
	//! modify the ray to be in object-space.
	bool MeshIsInWorldspace;
	
	//! working variables used as local buffer for sortcuts in 'checkCollision'
	mutable core::vector3df Min;
	mutable core::vector3df Max;
	mutable f32 RayLength;
	mutable core::vector3df RayVect;
};

}
}
#endif
