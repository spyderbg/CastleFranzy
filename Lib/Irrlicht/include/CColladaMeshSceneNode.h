//-*-c++-*-
#ifndef __CCOLLADAMESHSCENENODE_H__
#define __CCOLLADAMESHSCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IMeshSceneNode.h"
#include "IColladaMesh.h"
#include "SMaterial.h"
#include "SDatabaseCollada.h"
#include "IColladaRootSceneNode.h"

namespace irr
{
namespace scene
{

class CColladaMeshSceneNode
	: public IMeshSceneNode
{
public:

	//! Constructor
	CColladaMeshSceneNode(
		IColladaMesh* mesh,
		collada::IRootSceneNode* root,
		collada::SNode* node = 0,
		s32 id = -1,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::quaternion& rotation = core::quaternion(0,0,0,1.0f),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f)
	);

	virtual ~CColladaMeshSceneNode();

	virtual const char * getUID() const;

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const;

	//! Get the currently defined mesh for display.
	/** \return Pointer to mesh which is displayed by this node. */
	virtual IMesh* getMesh(void);

	//! Get the currently defined mesh for display.
	/** \return Pointer to mesh which is displayed by this node. */
	virtual void setMesh(IMesh *);

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	IRRLICHT_API virtual void setVisible(bool isVisible);

	virtual void notifyVisibilityChanged(bool bVisibility);

	virtual void OnAnimate(u32 timeMs);

	//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
	/** In this way it is possible to change the materials of a mesh
	causing all mesh scene nodes referencing this mesh to change, too.
	\param readonly Flag if the materials shall be read-only. */
	virtual void setReadOnlyMaterials(bool readonly);

	//! Check if the scene node should not copy the materials of the mesh but use them in a read only style
	/** This flag can be set by setReadOnlyMaterials().
	\return Whether the materials are read-only. */
	virtual bool isReadOnlyMaterials() const;

	//! Returns the material based on the zero based index i.
	/** To get the amount of materials used by this scene node, use
	getMaterialCount(). This function is needed for inserting the
	node into the scene hierarchy at an optimal position for
	minimizing renderstate changes, but can also be used to
	directly modify the material of a scene node.
	\param num Zero based index. The maximal value is getMaterialCount() - 1.
	\return The material at that index. */
	virtual video::SMaterial& getMaterial(u32 num);

	const collada::SMaterial& getOriginalMaterial(u32 num) const
	{
		return Materials[num]->getOriginal();
	}

	//! Get amount of materials used by this scene node.
	/** \return Current amount of materials of this scene node. */
	virtual u32 getMaterialCount() const;

	virtual void onMaterialChanged();

	virtual int getRenderVertexCount(void* renderData);

	virtual void render(void* renderData = 0);

	virtual void OnRegisterSceneNode();

	virtual IMeshBuffer* getMeshBuffer(u32 materialIdx);

	void prepareMaterial();

	//virtual core::stringc getUserProperty() const { return *m_pUserProperty; };
	virtual void notifyMaterialChanged();

protected:

	collada::SNode* Node;
	IColladaMesh* Mesh;
	collada::IRootSceneNode* Root;
	bool ReadOnlyMaterials;
	bool MaterialChanged;
	int TransparentMaterialCount;
	int SolidMaterialCount;
	core::array<collada::CMaterial*> Materials;

	enum E_RENDER_SET
	{
		ERS_SOLID = 0,
		ERS_TRANSPARENT = 1,
	};
	core::array<char> MaterialsRenderSet;

};

}; // namespace scene
}; // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
