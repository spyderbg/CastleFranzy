/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCD_MODULAR_SKIN_MASTER_H_
#define _FCD_MODULAR_SKIN_MASTER_H_

class FCDModularSkinCategoryInstance;
//class FCDExtra;

//#ifndef _FCD_ENTITY_H_
//#include "FCDocument/FCDEntity.h"
//#endif // _FCD_ENTITY_H_

#include "FCDocument/FCDModularSkinCategory.h"

/**
	A modularSkinMaster.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDModularSkinMaster : public FCDModularSkinCategory
{
private:
	DeclareObjectType(FCDModularSkinCategory);

	DeclareParameterList( VoidPtr, m_categoryNodes, "Category Nodes" ); // using void pointers to store 3dsMax INodes
	DeclareParameterList( FCDEntityReference, m_categorySceneNodeReferences, "Category Scene Nodes" );
	DeclareParameterList( FCDEntityInstance, m_categoryInstances, "Category Instances" );

	DeclareParameterList( VoidPtr, m_selectedSkinnedMeshNodes, "Selected Skinned Mesh Nodes" ); // using void pointers to store 3dsMax INodes
	DeclareParameterList( FCDEntityReference, m_selectedSkinControllerReferences, "Selected Skin Controllers" );

public:
	/** Constructor.
		@param document The COLLADA document that owns the modularSkinMaster. */
	FCDModularSkinMaster(FCDocument* document);

	/** Destructor. */
	virtual ~FCDModularSkinMaster();

	/** Retrieves the entity class type.
		@return The entity class type: FORCE */
	virtual Type GetType() const { return MODULAR_SKIN_MASTER; }

	/** Clones the modularSkinMaster field information.
		@param clone The cloned modularSkinMaster field.
			If this pointer is NULL, a new modularSkinMaster is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	inline size_t GetCategoryNodeCount() const
	{
		return m_categoryNodes.size();
	}

	inline void *GetCategoryNode( size_t categoryId )
	{
		FUAssert( categoryId < m_categoryNodes.size(), return NULL );
		return m_categoryNodes[categoryId];
	}

	inline void AddCategoryNode( void *pNode )
	{
		m_categoryNodes.push_back( pNode );
	}

	inline size_t GetCategorySceneNodeReferenceCount() const
	{
		return m_categorySceneNodeReferences.size();
	}

	inline const FCDEntityReference &GetCategorySceneNodeReference( size_t index ) const
	{
		FUAssert( index < m_categorySceneNodeReferences.size(), );
		return *m_categorySceneNodeReferences[index];
	}

	inline FCDEntityReference &GetCategorySceneNodeReference( size_t index )
	{
		FUAssert( index < m_categorySceneNodeReferences.size(), );
		return *m_categorySceneNodeReferences[index];
	}

	void AddCategorySceneNodeReference( FCDSceneNode *pSceneNode );
	void ClearCategorySceneNodeReferences();

	inline void AddCategoryInstance( FCDModularSkinCategoryInstance *pInstance )
	{
		m_categoryInstances.push_back( pInstance );
	}

	inline size_t GetCategoryInstanceCount() const
	{
		return m_categoryInstances.size();
	}

	inline FCDModularSkinCategoryInstance *GetCategoryInstance( size_t index )
	{
		FUAssert( index < m_categoryInstances.size(), );
		return ( FCDModularSkinCategoryInstance * )m_categoryInstances[index];
	}

	inline size_t GetSelectedSkinnedMeshNodeCount() const
	{
		return m_selectedSkinnedMeshNodes.size();
	}

	inline void *GetSelectedSkinnedMeshNode( size_t skinnedMeshId )
	{
		FUAssert( skinnedMeshId < m_selectedSkinnedMeshNodes.size(), return NULL );
		return m_selectedSkinnedMeshNodes[skinnedMeshId];
	}

	inline void AddSelectedSkinnedMeshNode( void *pNode )
	{
		m_selectedSkinnedMeshNodes.push_back( pNode );
	}

	inline size_t GetSelectedSkinControllerReferenceCount() const
	{
		return m_selectedSkinControllerReferences.size();
	}

	inline const FCDEntityReference &GetSelectedSkinControllerReference( size_t index ) const
	{
		FUAssert( index < m_selectedSkinControllerReferences.size(), );
		return *m_selectedSkinControllerReferences[index];
	}

	void AddSelectedSkinControllerReference( FCDController *pController );
};

#endif // _FCD_MODULAR_SKIN_MASTER_H_
