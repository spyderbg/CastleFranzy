/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDBillboardManager.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDController.h"

//
// FCDBillboardManager
//

ImplementObjectType(FCDBillboardManager);

FCDBillboardManager::FCDBillboardManager(FCDocument* document)
: FCDEntity(document, "BillboardManager")
, InitializeParameterAnimatable( m_billboardType, 0.f )
, InitializeParameterAnimatable( m_orientationType, 0.f )
, InitializeParameterAnimatable( m_front, FMVector3::ZAxis )
, InitializeParameterAnimatable( m_up, FMVector3::YAxis )
{
}

FCDBillboardManager::~FCDBillboardManager()
{
	CLEAR_POINTER_VECTOR( m_billboardSceneNodeReferences );
}

FCDEntity* FCDBillboardManager::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDBillboardManager* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDBillboardManager(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDBillboardManager::GetClassType())) clone = (FCDBillboardManager*) _clone;

	Parent::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_billboardNodes );

		{
			size_t numSceneNodeRefs = m_billboardSceneNodeReferences.size();
			for ( size_t sceneNodeId = 0; sceneNodeId < numSceneNodeRefs; ++sceneNodeId )
			{
				FCDSceneNode *pSceneNode = ( FCDSceneNode * )const_cast<FCDEntity *>( m_billboardSceneNodeReferences[sceneNodeId]->GetEntity() );
				clone->AddBillboardSceneNodeReference( pSceneNode );
			}
		}

		CLONE_PARAMETER( clone, m_billboardType );
		CLONE_PARAMETER( clone, m_orientationType );
		CLONE_PARAMETER( clone, m_front );
		CLONE_PARAMETER( clone, m_up );
	}

	return clone;
}

void FCDBillboardManager::AddBillboardSceneNodeReference( FCDSceneNode *pSceneNode )
{
	m_billboardSceneNodeReferences.push_back( new FCDEntityReference( GetDocument(), NULL ) );
	m_billboardSceneNodeReferences[m_billboardSceneNodeReferences.size() - 1]->SetEntity( pSceneNode );
}
