/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "AnimationExporter.h"
#include "ModularSkinExporter.h"
#include "EntityExporter.h"
#include "NodeExporter.h"
#include "ControllerExporter.h"
#include "MaterialExporter.h"
#include "DocumentExporter.h"
#include "ColladaOptions.h"
#include "ColladaIDCreator.h"
#include "XRefFunctions.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDModularSkinMaster.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDControllerInstance.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDModularSkinCategoryInstance.h"
#include "FCDocument/FCDModularSkinMasterInstance.h"
#include "ICModularSkinMaster.h"

ModularSkinExporter::ModularSkinExporter(DocumentExporter* _document)
:	BaseExporter(_document)
{
}

ModularSkinExporter::~ModularSkinExporter()
{
}

void ModularSkinExporter::CalculateCategoryForcedNodes(INode* node, Object* object, NodeList& forcedNodes)
{
	if (node == NULL || object == NULL) return;

	ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * ) object;

	// force all skinned mesh bones
	int numSkinnedMeshes = pModularSkinCategoryObject->pblock2->Count( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST );
	for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
	{
		INode *pNodeInList = NULL;
		pModularSkinCategoryObject->pblock2->GetValue( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );

		Object* skinnedMeshObject = pNodeInList->GetObjectRef();
		CTRL->CalculateForcedNodes( pNodeInList, skinnedMeshObject, forcedNodes );
	}
}

void ModularSkinExporter::CalculateMasterForcedNodes(INode* node, Object* object, NodeList& forcedNodes)
{
	if (node == NULL || object == NULL) return;

	ICModularSkinMasterObject *pModularSkinMasterObject = ( ICModularSkinMasterObject * ) object;

	// force all categories
	//int numSkinnedMeshCategories = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST );
	//for ( int skinnedMeshCategoryId = 0; skinnedMeshCategoryId < numSkinnedMeshCategories; ++skinnedMeshCategoryId )
	//{
	//	INode *pNodeInList = NULL;
	//	pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, 0, pNodeInList, FOREVER, skinnedMeshCategoryId );
	//	forcedNodes.push_back( pNodeInList );

	//	Object* categoryObject = pNodeInList->GetObjectRef();
	//	CalculateCategoryForcedNodes( pNodeInList, categoryObject, forcedNodes );
	//}
	
	// force all skinned meshe bones
	int numSkinnedMeshes = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST );
	for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
	{
		INode *pNodeInList = NULL;
		pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );

		Object* skinnedMeshObject = pNodeInList->GetObjectRef();
		CTRL->CalculateForcedNodes( pNodeInList, skinnedMeshObject, forcedNodes );
	}
}

void ModularSkinExporter::ExportSkinnedMesh( INode *pNode, FCDModularSkinCategory *pModularSkinCategory, ExportedEntityMap &exportedEntities )
{
	// Export Skinned Mesh Material
	Mtl* nodeMaterial = pNode->GetMtl();
	if ( nodeMaterial != NULL )
	{
		MATE->ExportMaterial( nodeMaterial );
	}

	// Export Skin Controller
	{
		Object* pSkinnedMeshObject = pNode->GetObjectRef();
		ExportedEntityMap::iterator itE = exportedEntities.find( pSkinnedMeshObject );
		EntityList entities;

		if ( itE == exportedEntities.end() )
		{
			ADD_ENTITY( entities, CTRL->ExportController( pNode, pSkinnedMeshObject ) );
		}
		else
		{
			entities = (*itE).second;
		}

		for ( size_t entityId = 0; entityId < entities.size(); ++entityId )
		{
			FCDEntity *pEntity = entities[entityId];
			if ( pEntity->GetType() == FCDEntity::CONTROLLER )
			{
				FCDEntityInstance *pInstance = FCDEntityInstanceFactory::CreateInstance( pModularSkinCategory->GetDocument(), NULL, pEntity->GetType() );
				pInstance->SetEntity( pEntity );
				pInstance->SetUserHandle( pNode );
				pModularSkinCategory->AddControllerInstance( ( FCDControllerInstance * )pInstance );
			}
		}

		if ( entities.size() > 0 )
		{
			exportedEntities.insert( pSkinnedMeshObject, entities );
		}
	}
}

FCDEntity* ModularSkinExporter::ExportModularSkinMaster(INode* node, Object* object, ExportedEntityMap &exportedEntities, int flags)
{
	if (node == NULL || object == NULL) return NULL;

	ICModularSkinMasterObject *pModularSkinMasterObject = ( ICModularSkinMasterObject * )object;

	bool isXRef = false;
	if (XRefFunctions::IsXRefObject(object))
	{
		// Resolve the source
		object = XRefFunctions::GetXRefObjectSource(object);
		if (OPTS->ExportXRefs()) isXRef = true;
	}

	// Create the COLLADA modularSkin entity and its mesh
	FCDModularSkinMaster* pModularSkinMaster = isXRef ? new FCDModularSkinMaster(CDOC) : CDOC->GetModularSkinMasterLibrary()->AddEntity();

	// temporarily deactivating animation export if xref
	//bool bExportAnimationsBackup = OPTS->ExportAnimations();
	//OPTS->animations = !isXRef;

	// categories
	int numCategories = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST );
	for ( int categoryId = 0; categoryId < numCategories; ++categoryId )
	{
		INode *pNodeInList = NULL;
		pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, 0, pNodeInList, FOREVER, categoryId );
		pModularSkinMaster->AddCategoryNode( pNodeInList );

		// Selected Skinned Meshes
		INode *pSelectedSkinnedMeshNode = NULL;
		pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SELECTED_SKINNED_MESH_LIST, 0, pSelectedSkinnedMeshNode, FOREVER, categoryId );
		pModularSkinMaster->AddSelectedSkinnedMeshNode( pSelectedSkinnedMeshNode );
	}

	// skinned meshes
	int numSkinnedMeshes = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST );
	for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
	{
		INode *pNodeInList = NULL;
		pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );
		ExportSkinnedMesh( pNodeInList, pModularSkinMaster, exportedEntities );
	}

	//OPTS->animations = bExportAnimationsBackup;

	return pModularSkinMaster;
}

FCDEntity* ModularSkinExporter::ExportModularSkinCategory(INode* node, Object* object, ExportedEntityMap &exportedEntities, int flags)
{
	if (node == NULL || object == NULL) return NULL;

	ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )object;

	bool isXRef = false;
	if (XRefFunctions::IsXRefObject(object))
	{
		// Resolve the source
		object = XRefFunctions::GetXRefObjectSource(object);
		if (OPTS->ExportXRefs()) isXRef = true;
	}

	// Create the COLLADA modularSkin entity and its mesh
	FCDModularSkinCategory* pModularSkinCategory = isXRef ? new FCDModularSkinCategory(CDOC) : CDOC->GetModularSkinCategoryLibrary()->AddEntity();

	// temporarily deactivating animation export if xref
	//bool bExportAnimationsBackup = OPTS->ExportAnimations();
	//OPTS->animations = !isXRef;

	int numSkinnedMeshes = pModularSkinCategoryObject->pblock2->Count( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST );
	for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
	{
		INode *pNodeInList = NULL;
		pModularSkinCategoryObject->pblock2->GetValue( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );
		ExportSkinnedMesh( pNodeInList, pModularSkinCategory, exportedEntities );
	}

	//OPTS->animations = bExportAnimationsBackup;

	return pModularSkinCategory;
}

void ModularSkinExporter::LinkSkinnedMeshNodes( FCDModularSkinCategory *pModularSkinCategory )
{
	size_t controllerCount = CDOC->GetControllerLibrary()->GetEntityCount();
	for (size_t i = 0; i < controllerCount; ++i)
	{
		FCDController* controller = CDOC->GetControllerLibrary()->GetEntity(i);
		ControllerInfo* info = (ControllerInfo*) controller->GetUserHandle();
		if (info != NULL)
		{
			INode*  inode  = info->node;
			size_t controllerInstanceCount = pModularSkinCategory->GetControllerInstanceCount();
			for ( size_t controllerInstanceId = 0; controllerInstanceId < controllerInstanceCount; ++controllerInstanceId )
			{
				FCDControllerInstance *pControllerInstance = pModularSkinCategory->GetControllerInstance( controllerInstanceId );
				if ( pControllerInstance->GetEntity() == controller )
				{
					CTRL->LinkController( controller, inode, pControllerInstance);
				}
			}
		}
	}
}

void ModularSkinExporter::LinkModularSkinMasters()
{
	size_t numModularSkinMasters = CDOC->GetModularSkinMasterLibrary()->GetEntityCount();
	for (size_t index = 0; index < numModularSkinMasters; ++index)
	{
		FCDModularSkinMaster* pModularSkinMaster = CDOC->GetModularSkinMasterLibrary()->GetEntity(index);
		LinkSkinnedMeshNodes( pModularSkinMaster );
		
		// link categories
		size_t categoryCount = pModularSkinMaster->GetCategoryNodeCount();
		for ( size_t categoryId = 0; categoryId < categoryCount; ++categoryId )
		{
			{
				INode *pCategoryNode = ( INode * )pModularSkinMaster->GetCategoryNode( categoryId );
				FCDSceneNode *pCategorySceneNode = document->GetNodeExporter()->FindExportedNode( pCategoryNode );
				if ( !pCategorySceneNode )
				{
					continue;		
				}

				pModularSkinMaster->AddCategorySceneNodeReference( pCategorySceneNode );
			}

			// Selected Skin Controllers
			{
				INode *pSelectedNode = ( INode * )pModularSkinMaster->GetSelectedSkinnedMeshNode( categoryId );

				size_t controllerCount = CDOC->GetControllerLibrary()->GetEntityCount();
				for (size_t i = 0; i < controllerCount; ++i)
				{
					FCDController* controller = CDOC->GetControllerLibrary()->GetEntity(i);
					ControllerInfo* info = (ControllerInfo*) controller->GetUserHandle();
					if ( info->node == pSelectedNode &&	controller->IsSkin() )
					{
						pModularSkinMaster->AddSelectedSkinControllerReference( controller );
						break;
					}
				}
			}
		}
	}
}

void ModularSkinExporter::LinkModularSkinCategories()
{
	size_t numModularSkinCategories = CDOC->GetModularSkinCategoryLibrary()->GetEntityCount();
	for (size_t index = 0; index < numModularSkinCategories; ++index)
	{
		FCDModularSkinCategory* pModularSkinCategory = CDOC->GetModularSkinCategoryLibrary()->GetEntity(index);
		LinkSkinnedMeshNodes( pModularSkinCategory );
	}
}

// Force hides all modular skinned mesh nodes
void ModularSkinExporter::PreExport()
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
		Object *pObject = pNode->GetObjectRef();
		if ( pObject == NULL )
		{
			continue;
		}

		ColladaMaxTypes::MaxType colladaMaxObjectType = ColladaMaxTypes::Get( pNode, pObject );
		
		if ( colladaMaxObjectType == ColladaMaxTypes::MODULAR_SKIN_MASTER )
		{
			ICModularSkinMasterObject *pModularSkinMasterObject = ( ICModularSkinMasterObject * ) pObject;

			int numSkinnedMeshes = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST );
			for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
			{
				INode *pNodeInList = NULL;
				pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );
				if ( !pNodeInList->IsHidden() )
				{
					m_modularSkinnedMeshNodes.push_back( pNodeInList );
					pNodeInList->Hide( true );
				}
			}
		}
		else if ( colladaMaxObjectType == ColladaMaxTypes::MODULAR_SKIN_CATEGORY )
		{
			ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * ) pObject;

			// force all skinned mesh bones
			int numSkinnedMeshes = pModularSkinCategoryObject->pblock2->Count( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST );
			for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
			{
				INode *pNodeInList = NULL;
				pModularSkinCategoryObject->pblock2->GetValue( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );
				if ( !pNodeInList->IsHidden() )
				{
					m_modularSkinnedMeshNodes.push_back( pNodeInList );
					pNodeInList->Hide( true );
				}
			}
		}
	}
}

// unhides all modular skinned mesh nodes forcefully hidden
void ModularSkinExporter::PostExport()
{
	for ( NodeList::iterator nodeIter = m_modularSkinnedMeshNodes.begin(); nodeIter != m_modularSkinnedMeshNodes.end(); ++nodeIter )
	{
		(*nodeIter)->Hide( false );
	}
}

void ModularSkinExporter::RemoveModularSkinCategoryNodes()
{
	size_t numModularSkinMasters = CDOC->GetModularSkinMasterLibrary()->GetEntityCount();
	for (size_t index = 0; index < numModularSkinMasters; ++index)
	{
		FCDModularSkinMaster* pModularSkinMaster = CDOC->GetModularSkinMasterLibrary()->GetEntity(index);
		
		// link categories
		size_t categoryCount = pModularSkinMaster->GetCategorySceneNodeReferenceCount();
		for ( size_t categoryId = 0; categoryId < categoryCount; ++categoryId )
		{
			FCDEntityReference &entityRef = pModularSkinMaster->GetCategorySceneNodeReference( categoryId );
			FCDSceneNode *pSceneNode = ( FCDSceneNode * )entityRef.GetEntity();
			FUAssert( pSceneNode, continue );

			size_t categoryInstanceCount = pSceneNode->GetInstanceCount();
			FUAssert( categoryInstanceCount == 1, continue );

			FCDModularSkinCategoryInstance *pInstance = ( FCDModularSkinCategoryInstance * )pSceneNode->GetInstance( 0 );
			FCDModularSkinCategoryInstance *newInstance = ( FCDModularSkinCategoryInstance * )pInstance->Clone();
			pModularSkinMaster->AddCategoryInstance( newInstance );

			FCDEntity *pCategoryEntity = ( FCDEntity * )newInstance->GetEntity();
			FUAssert( pCategoryEntity, continue );
			pCategoryEntity->SetName( pSceneNode->GetName() );
		}
	}

	for (size_t index = 0; index < numModularSkinMasters; ++index)
	{
		FCDModularSkinMaster* pModularSkinMaster = CDOC->GetModularSkinMasterLibrary()->GetEntity(index);
		pModularSkinMaster->ClearCategorySceneNodeReferences();
	}

	RemoveModularSkinCategoryNodes( CDOC->GetVisualSceneInstance() );
}

bool ModularSkinExporter::RemoveModularSkinCategoryNodes( FCDSceneNode *pSceneNode )
{
	if ( !pSceneNode )
	{
		return false;
	}

	FCDSceneNodeList childrenToRemove;
	{
		size_t numChildren = pSceneNode->GetChildrenCount();
		for ( size_t childId = 0; childId < numChildren; ++childId )
		{
			FCDSceneNode *pChild = pSceneNode->GetChild( childId );
			if ( RemoveModularSkinCategoryNodes( pChild ) )
			{
				childrenToRemove.push_back( pChild );
			}
		}
	}

	for ( FCDSceneNodeList::iterator iter = childrenToRemove.begin(); iter != childrenToRemove.end(); ++iter )
	{
		FCDSceneNode *pNodeToRemove = *iter;
		pSceneNode->RemoveChildNode( pNodeToRemove );
		pNodeToRemove->Release();
	}

	if ( pSceneNode->GetInstanceCount() == 1 )
	{
		FCDEntityInstance *pInstance = pSceneNode->GetInstance( 0 );
		if ( pInstance->HasType( FCDModularSkinCategoryInstance::GetClassType() ) &&
			!pInstance->HasType( FCDModularSkinMasterInstance::GetClassType() ) )
		{
			pSceneNode->RemoveInstance( pInstance );
			pInstance->Release();

			if ( pSceneNode->GetChildrenCount() <= 0 )
			{
				return true;
			}
		}
	}

	return false;
}