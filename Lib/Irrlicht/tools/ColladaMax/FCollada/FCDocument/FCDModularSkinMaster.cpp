/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDModularSkinMaster.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDController.h"

//
// FCDModularSkinMaster
//

ImplementObjectType(FCDModularSkinMaster);

FCDModularSkinMaster::FCDModularSkinMaster(FCDocument* document)
:	FCDModularSkinCategory(document, "ModularSkinMaster")
{
}

FCDModularSkinMaster::~FCDModularSkinMaster()
{
	ClearCategorySceneNodeReferences();
	CLEAR_POINTER_VECTOR( m_categoryInstances );
	CLEAR_POINTER_VECTOR( m_selectedSkinControllerReferences );
}

FCDEntity* FCDModularSkinMaster::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDModularSkinMaster* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDModularSkinMaster(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDModularSkinMaster::GetClassType())) clone = (FCDModularSkinMaster*) _clone;

	Parent::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_categoryNodes );
		CLONE_PARAMETER( clone, m_selectedSkinnedMeshNodes );
		CLONE_PARAMETER( clone, m_categoryInstances );

		{
			size_t numSceneNodeRefs = m_categorySceneNodeReferences.size();
			for ( size_t sceneNodeId = 0; sceneNodeId < numSceneNodeRefs; ++sceneNodeId )
			{
				FCDSceneNode *pSceneNode = ( FCDSceneNode * )const_cast<FCDEntity *>( m_categorySceneNodeReferences[sceneNodeId]->GetEntity() );
				clone->AddCategorySceneNodeReference( pSceneNode );
			}
		}

		{
			size_t numSkinControllerRefs = m_selectedSkinControllerReferences.size();
			for ( size_t skinControllerRefId = 0; skinControllerRefId < numSkinControllerRefs; ++skinControllerRefId )
			{
				FCDController *pController = ( FCDController * )const_cast<FCDEntity *>( m_selectedSkinControllerReferences[skinControllerRefId]->GetEntity() );
				clone->AddSelectedSkinControllerReference( pController );
			}
		}
	}

	return clone;
}

void FCDModularSkinMaster::AddCategorySceneNodeReference( FCDSceneNode *pSceneNode )
{
	m_categorySceneNodeReferences.push_back( new FCDEntityReference( GetDocument(), NULL ) );
	m_categorySceneNodeReferences[m_categorySceneNodeReferences.size() - 1]->SetEntity( pSceneNode );
}

void FCDModularSkinMaster::ClearCategorySceneNodeReferences()
{
	CLEAR_POINTER_VECTOR( m_categorySceneNodeReferences );
}

void FCDModularSkinMaster::AddSelectedSkinControllerReference( FCDController *pController )
{
	m_selectedSkinControllerReferences.push_back( new FCDEntityReference( GetDocument(), NULL ) );
	m_selectedSkinControllerReferences[m_selectedSkinControllerReferences.size() - 1]->SetEntity( pController );
}