/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCD_MODULAR_SKIN_CATEGORY_H_
#define _FCD_MODULAR_SKIN_CATEGORY_H_

class FCDocument;
class FCDExtra;

#ifndef _FCD_ENTITY_H_
#include "FCDocument/FCDEntity.h"
#endif // _FCD_ENTITY_H_

/**
	A modularSkinCategory.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDModularSkinCategory : public FCDEntity
{
private:
	DeclareObjectType(FCDEntity);

	DeclareParameterList( FCDControllerInstance, m_controllerInstances, "Controller Instances" );
	DeclareParameterList( VoidPtr, m_skinnedMeshNodes, "Skinned Mesh Nodes" ); // using void pointers to store 3dsMax INodes

public:
	/** Constructor.
		@param document The COLLADA document that owns the modularSkinCategory. */
	FCDModularSkinCategory(FCDocument* document, const char* baseId = "ModularSkinCategory");

	/** Destructor. */
	virtual ~FCDModularSkinCategory();

	/** Retrieves the entity class type.
		@return The entity class type: FORCE */
	virtual Type GetType() const { return MODULAR_SKIN_CATEGORY; }

	/** Clones the modularSkinCategory field information.
		@param clone The cloned modularSkinCategory field.
			If this pointer is NULL, a new modularSkinCategory is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	inline size_t GetSkinnedMeshNodeCount() const
	{
		return m_skinnedMeshNodes.size();
	}

	inline void *GetSkinnedMeshNode( size_t skinnedMeshId )
	{
		FUAssert( skinnedMeshId < m_skinnedMeshNodes.size(), return NULL );
		return m_skinnedMeshNodes[skinnedMeshId];
	}

	inline void AddSkinnedMeshNode( void *pNode )
	{
		m_skinnedMeshNodes.push_back( pNode );
	}

	inline size_t GetControllerInstanceCount() const
	{
		return m_controllerInstances.size();
	}

	inline FCDControllerInstance *GetControllerInstance( size_t index )
	{
		FUAssert( index < m_controllerInstances.size(), return NULL );
		return m_controllerInstances[index];
	}

	inline void AddControllerInstance( FCDControllerInstance *pControllerInstance )
	{
		m_controllerInstances.push_back( pControllerInstance );
	}

};

#endif // _FCD_MODULAR_SKIN_CATEGORY_H_
