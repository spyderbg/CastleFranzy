/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "AnimationExporter.h"
#include "BillboardExporter.h"
#include "EntityExporter.h"
#include "NodeExporter.h"
#include "ControllerExporter.h"
#include "DocumentExporter.h"
#include "ColladaOptions.h"
#include "ColladaIDCreator.h"
#include "XRefFunctions.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDBillboardManager.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDAnimation.h"
#include "FCDocument/FCDAnimationCurve.h"
#include "FCDocument/FCDAnimationChannel.h"
#include "ICBillboardHelper.h"
#include "ICModifierBillboard.h"
#include "ColladaModifiers.h"

BillboardExporter::BillboardExporter(DocumentExporter* _document)
:	BaseExporter(_document)
{
}

BillboardExporter::~BillboardExporter()
{
}

void BillboardExporter::CalculateForcedNodes(INode* node, Object* object, NodeList& forcedNodes)
{
	if (node == NULL || object == NULL) return;

	ICBillboardHelper *pBillboardHelperObject = ( ICBillboardHelper * ) object;

	// force all categories
	//int numBillboards = pBillboardHelperObject->GetBillboardNodeCount();
	//for ( int billboardId = 0; billboardId < numBillboards; ++billboardId )
	//{
	//	INode *pNodeInList = pBillboardHelperObject->GetBillboardNode( billboardId );
	//	forcedNodes.push_back( pNodeInList );
	//}
}

FCDEntity* BillboardExporter::ExportBillboardManager(INode* node, Object* object, ExportedEntityMap &exportedEntities, int flags)
{
	if (node == NULL || object == NULL) return NULL;

	ICBillboardHelper *pBillboardHelperObject = ( ICBillboardHelper * )object;

	bool isXRef = false;
	if (XRefFunctions::IsXRefObject(object))
	{
		// Resolve the source
		object = XRefFunctions::GetXRefObjectSource(object);
		if (OPTS->ExportXRefs()) isXRef = true;
	}

	// temporarily deactivating animation export if xref
	//bool bExportAnimationsBackup = OPTS->ExportAnimations();
	//OPTS->animations = !isXRef;

	// Create the COLLADA modularSkin entity and its mesh
	FCDBillboardManager* pBillboardManager = isXRef ? new FCDBillboardManager(CDOC) : CDOC->GetBillboardManagerLibrary()->AddEntity();
	pBillboardManager->SetName( fm::string( node->GetName() ));
	ENTE->ExportEntity( object, pBillboardManager, pBillboardManager->GetName() );

	{
		fstring idPrefix = pBillboardManager->GetName();
		IParamBlock2 *pParamBlock = pBillboardHelperObject->pblock2;

		// type, orientation
		ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_TYPE, pParamBlock, ICBillboardHelper::PI_BILLBOARD_TYPE, 0, pBillboardManager->GetBillboardType(), NULL );
		ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_ORIENTATION, pParamBlock, ICBillboardHelper::PI_BILLBOARD_ORIENTATION, 0, pBillboardManager->GetOrientationType(), NULL );

		// front, up
		ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_FRONT, pParamBlock, ICBillboardHelper::PI_BILLBOARD_FRONT, 0, pBillboardManager->GetFront(), NULL );
		ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_UP, pParamBlock, ICBillboardHelper::PI_BILLBOARD_UP, 0, pBillboardManager->GetUp(), NULL );
	}

	// targets
	{
		ExportNodeMap &exportNodes = NDEX->GetExportNodes();
		int numBillboards = pBillboardHelperObject->GetBillboardNodeCount();
		for ( int billboardId = 0; billboardId < numBillboards; ++billboardId )
		{
			INode *pNodeInList = pBillboardHelperObject->GetBillboardNode( billboardId );

			if ( exportNodes.find( pNodeInList ) == exportNodes.end() )
			{
				continue;
			}

			pBillboardManager->AddBillboardNode( pNodeInList );
		}
	}

	//OPTS->animations = bExportAnimationsBackup;

	return pBillboardManager;
}

void BillboardExporter::ExportBillboardModifier( INode *pNode, FCDETechnique *pTechnique, const fm::string &idPrefix, int flags )
{
	//Object *object = pMaxNode->GetObjectRef();
	ColladaModifierStack foundModifiers;
	{
		ColladaModifierStack targetModifiers;
		targetModifiers.push_back( new ICBillboardModifier() );
		ColladaModifierManager::ResolveModifiers( pNode, pNode->GetWSMDerivedObject(), targetModifiers, foundModifiers );
		CLEAR_POINTER_VECTOR( targetModifiers );
	}

	ICBillboardWSMObject *pBillboardModifierObject = NULL;
	for ( ColladaModifierStack::iterator colladaModifierIter = foundModifiers.begin(); colladaModifierIter != foundModifiers.end() && !pBillboardModifierObject; ++colladaModifierIter )
	{
		ICBillboardModifier *pBillboardColladaModifier = ( ICBillboardModifier * )*colladaModifierIter;
		ICBillboardWSM *pBillboardModifier = pBillboardColladaModifier->GetInterface();
		pBillboardModifierObject = pBillboardModifier->GetBillboardWSMObjectPtr();
	}
	CLEAR_POINTER_VECTOR( foundModifiers );

	if ( !pBillboardModifierObject )
	{
		return;
	}

	FCDENode *pBillboardNode = pTechnique->AddChildNode( DAE_IRR_BILLBOARD );
	pBillboardNode->AddAttribute( DAE_SID_ATTRIBUTE, DAE_IRR_BILLBOARD );

	// TODO: Export Billboard Modifier Object
	bool isXRef = false;
	//if (XRefFunctions::IsXRefObject(object))
	//{
	//	// Resolve the source
	//	object = XRefFunctions::GetXRefObjectSource(object);
	//	if (OPTS->ExportXRefs()) isXRef = true;
	//}

	// temporarily deactivating animation export if xref
	//bool bExportAnimationsBackup = OPTS->ExportAnimations();
	//OPTS->animations = !isXRef;

	// Create the COLLADA modularSkin entity and its mesh
	FCDBillboardManager *pBillboardManager = NULL;
	//BillboardModifierObjectMap::iterator billboardModifierObjectIter = m_billboardModifierObjectMap.find( pBillboardModifierObject );
	//if ( billboardModifierObjectIter == m_billboardModifierObjectMap.end() )
	{
		pBillboardManager = isXRef ? new FCDBillboardManager(CDOC) : CDOC->GetBillboardManagerLibrary()->AddEntity();
		//m_billboardModifierObjectMap.insert( pBillboardModifierObject, pBillboardManager );
		//pBillboardManager->SetName( fm::string( pNode->GetName() ));
		//ENTE->ExportEntity( pBillboardModifierObject, pBillboardManager, pBillboardManager->GetName() );

		{
			IParamBlock2 *pParamBlock = pBillboardModifierObject->pblock2;

			// type, orientation
			ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_TYPE, pParamBlock, ICBillboardWSMObject::PI_BILLBOARD_TYPE, 0, pBillboardManager->GetBillboardType(), NULL );
			ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_ORIENTATION, pParamBlock, ICBillboardWSMObject::PI_BILLBOARD_ORIENTATION, 0, pBillboardManager->GetOrientationType(), NULL );

			// front, up
			ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_FRONT, pParamBlock, ICBillboardWSMObject::PI_BILLBOARD_FRONT, 0, pBillboardManager->GetFront(), NULL );
			ANIM->ExportProperty( idPrefix + "_" + DAE_BILLBOARD_UP, pParamBlock, ICBillboardWSMObject::PI_BILLBOARD_UP, 0, pBillboardManager->GetUp(), NULL );
		}

		{
			FCDENode *pParamNode = pBillboardNode->AddChildNode( DAE_BILLBOARD_TYPE );
			pParamNode->SetContent( fm::string( FUDaeBillboard::ToStringBillboardType( ( FUDaeBillboard::BillboardType )( int )pBillboardManager->GetBillboardType() ) ).ToUpper() );
		}

		{
			FCDENode *pParamNode = pBillboardNode->AddChildNode( DAE_BILLBOARD_ORIENTATION );
			pParamNode->SetContent( fm::string( FUDaeBillboard::ToStringOrientationType( ( FUDaeBillboard::OrientationType )( int )pBillboardManager->GetOrientationType() ) ).ToUpper() );
		}
		
		{
			FCDENode *pParamNode = pBillboardNode->AddParameter( DAE_BILLBOARD_FRONT, pBillboardManager->GetFront() );
			pParamNode->GetAnimated()->Copy( pBillboardManager->GetFront().GetAnimated() );
		}
		
		{
			FCDENode *pParamNode = pBillboardNode->AddParameter( DAE_BILLBOARD_UP, pBillboardManager->GetUp() );
			pParamNode->GetAnimated()->Copy( pBillboardManager->GetUp().GetAnimated() );
		}
	}
}

void BillboardExporter::LinkBillboardManagers()
{
	size_t numBillboardManagers = CDOC->GetBillboardManagerLibrary()->GetEntityCount();
	for (size_t index = 0; index < numBillboardManagers; ++index)
	{
		FCDBillboardManager* pBillboardManager = CDOC->GetBillboardManagerLibrary()->GetEntity(index);
		
		// link categories
		size_t billboardCount = pBillboardManager->GetBillboardNodeCount();
		for ( size_t billboardId = 0; billboardId < billboardCount; ++billboardId )
		{
			INode *pBillboardNode = ( INode * )pBillboardManager->GetBillboardNode( billboardId );
			FCDSceneNode *pBillboardSceneNode = document->GetNodeExporter()->FindExportedNode( pBillboardNode );
			if ( !pBillboardSceneNode )
			{
				continue;		
			}

			pBillboardManager->AddBillboardSceneNodeReference( pBillboardSceneNode );
		}
	}
}

void BillboardExporter::PreExport()
{
	NDEX->CalculateExportNodes( NULL );
	ExportNodeMap &exportNodes = NDEX->GetExportNodes();

	for ( ExportNodeMap::iterator exportNodeIter = exportNodes.begin(); exportNodeIter != exportNodes.end(); ++exportNodeIter )
	{
		if ( !exportNodeIter->second->exportObject )
		{
			continue;
		}

		INode *pNode = exportNodeIter->first;
		if ( pNode->IsHidden() )
		{
			continue;
		}

		Object *pObject = pNode->GetObjectRef();
		if ( pObject == NULL )
		{
			continue;
		}

		ColladaMaxTypes::MaxType colladaMaxObjectType = ColladaMaxTypes::Get( pNode, pObject );
		if ( colladaMaxObjectType != ColladaMaxTypes::BILLBOARD_MODIFIER_OBJECT )
		{
			continue;
		}

		m_billboardModifierNodes.push_back( pNode );
		pNode->Hide( true );
	}
}

void BillboardExporter::PostExport()
{
	for ( NodeList::iterator nodeIter = m_billboardModifierNodes.begin(); nodeIter != m_billboardModifierNodes.end(); ++nodeIter )
	{
		(*nodeIter)->Hide( false );
	}
}