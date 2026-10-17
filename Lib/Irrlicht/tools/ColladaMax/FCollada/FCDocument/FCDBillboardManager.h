/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCD_BILLBOARD_MANAGER_H_
#define _FCD_BILLBOARD_MANAGER_H_

class FCDocument;
class FCDExtra;

#ifndef _FCD_ENTITY_H_
#include "FCDocument/FCDEntity.h"
#endif // _FCD_ENTITY_H_
#ifndef _FCD_PARAMETER_ANIMATABLE_H_
#include "FCDocument/FCDParameterAnimatable.h"
#endif // _FCD_PARAMETER_ANIMATABLE_H_

/**
	A billboardManager.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDBillboardManager : public FCDEntity
{
private:
	DeclareObjectType(FCDEntity);

	DeclareParameterList( VoidPtr, m_billboardNodes, "Billboard Nodes" ); // using void pointers to store 3dsMax INodes
	DeclareParameterList( FCDEntityReference, m_billboardSceneNodeReferences, "Billboard Scene Nodes" );

	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_billboardType, FC("Billboard Type"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_orientationType, FC("Orientation Type"));
	DeclareParameterAnimatable(FMVector3, FUParameterQualifiers::VECTOR, m_front, FC("Front Vector"));
	DeclareParameterAnimatable(FMVector3, FUParameterQualifiers::VECTOR, m_up, FC("Up Vector"));

public:
	/** Constructor.
		@param document The COLLADA document that owns the modularSkinMaster. */
	FCDBillboardManager(FCDocument* document);

	/** Destructor. */
	virtual ~FCDBillboardManager();

	/** Retrieves the entity class type.
		@return The entity class type: FORCE */
	virtual Type GetType() const { return BILLBOARD_MANAGER; }

	/** Clones the modularSkinMaster field information.
		@param clone The cloned modularSkinMaster field.
			If this pointer is NULL, a new modularSkinMaster is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, BillboardType, m_billboardType )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, OrientationType, m_orientationType )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableVector3, FMVector3, Front, m_front )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableVector3, FMVector3, Up, m_up )

	inline size_t GetBillboardNodeCount() const
	{
		return m_billboardNodes.size();
	}

	inline void *GetBillboardNode( size_t id )
	{
		FUAssert( id < m_billboardNodes.size(), return NULL );
		return m_billboardNodes[id];
	}

	inline void AddBillboardNode( void *pNode )
	{
		m_billboardNodes.push_back( pNode );
	}

	inline size_t GetBillboardSceneNodeReferenceCount() const
	{
		return m_billboardSceneNodeReferences.size();
	}

	inline const FCDEntityReference &GetBillboardSceneNodeReference( size_t index ) const
	{
		FUAssert( index < m_billboardSceneNodeReferences.size(), );
		return *m_billboardSceneNodeReferences[index];
	}

	void AddBillboardSceneNodeReference( FCDSceneNode *pSceneNode );
};

#endif // _FCD_BILLBOARD_MANAGER_H_
