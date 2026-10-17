/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FArchiveXML.h"
#include "FAXICHelpers.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDEntityInstance.h"
#include "FCDocument/FCDEmitterInstance.h"
#include "FCDocument/FCDModularSkinCategoryInstance.h"
#include "FCDocument/FCDModularSkinCategory.h"
#include "FCDocument/FCDModularSkinMasterInstance.h"
#include "FCDocument/FCDModularSkinMaster.h"
#include "FCDocument/FCDBillboardManager.h"
#include "FCDocument/FCDBillboardManagerInstance.h"
#include "FCDocument/FCDForceInstance.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDControllerInstance.h"
#include "FCDocument/FCDMaterialInstance.h"
#include "FCDocument/FCDLightInstance.h"
#include "FCDocument/FCDPhysicsForceFieldInstance.h"
#include "FCDocument/FCDPhysicsModelInstance.h"
#include "FCDocument/FCDPhysicsRigidBodyInstance.h"
#include "FCDocument/FCDPhysicsRigidConstraintInstance.h"
#include "FCDocument/FCDSkinController.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDPhysicsModel.h"
#include "FCDocument/FCDPhysicsRigidConstraint.h"
#include "FCDocument/FCDPhysicsRigidBody.h"
#include "FCDocument/FCDPhysicsRigidBodyParameters.h"
#include "FCDocument/FCDEmitter.h"

typedef fm::pvector<FCDControllerInstance> ControllerInstanceList;

//
// Helpers
//

static const char* GetInstanceClassTypeFCDEntityInstance(FCDEntity::Type type)
{
	const char* instanceEntityName;
	switch (type)
	{
	case FCDEntity::ANIMATION: instanceEntityName = DAE_INSTANCE_ANIMATION_ELEMENT; break;
	case FCDEntity::CAMERA: instanceEntityName = DAE_INSTANCE_CAMERA_ELEMENT; break;
	case FCDEntity::CONTROLLER: instanceEntityName = DAE_INSTANCE_CONTROLLER_ELEMENT; break;
	case FCDEntity::EMITTER: instanceEntityName = DAE_INSTANCE_EMITTER_ELEMENT; break;
	case FCDEntity::FORCE: instanceEntityName = DAE_INSTANCE_FORCE_ELEMENT; break;
	case FCDEntity::MODULAR_SKIN_MASTER: instanceEntityName = DAE_INSTANCE_MODULAR_SKIN_MASTER_ELEMENT; break;
	case FCDEntity::MODULAR_SKIN_CATEGORY: instanceEntityName = DAE_INSTANCE_MODULAR_SKIN_CATEGORY_ELEMENT; break;
	case FCDEntity::BILLBOARD_MANAGER: instanceEntityName = DAE_INSTANCE_BILLBOARD_ELEMENT; break;
	case FCDEntity::EFFECT: instanceEntityName = DAE_INSTANCE_EFFECT_ELEMENT; break;
	case FCDEntity::FORCE_FIELD: instanceEntityName = DAE_INSTANCE_FORCE_FIELD_ELEMENT; break;
	case FCDEntity::GEOMETRY: instanceEntityName = DAE_INSTANCE_GEOMETRY_ELEMENT; break;
	case FCDEntity::LIGHT: instanceEntityName = DAE_INSTANCE_LIGHT_ELEMENT; break;
	case FCDEntity::MATERIAL: instanceEntityName = DAE_INSTANCE_MATERIAL_ELEMENT; break;
	case FCDEntity::PHYSICS_MATERIAL: instanceEntityName = DAE_INSTANCE_PHYSICS_MATERIAL_ELEMENT; break;
    case FCDEntity::PHYSICS_MODEL: instanceEntityName = DAE_INSTANCE_PHYSICS_MODEL_ELEMENT; break;
	case FCDEntity::PHYSICS_RIGID_BODY: instanceEntityName = DAE_INSTANCE_RIGID_BODY_ELEMENT; break;
	case FCDEntity::PHYSICS_RIGID_CONSTRAINT: instanceEntityName = DAE_INSTANCE_RIGID_CONSTRAINT_ELEMENT; break;
	case FCDEntity::SCENE_NODE: instanceEntityName = DAE_INSTANCE_NODE_ELEMENT; break;

	case FCDEntity::ANIMATION_CLIP:
	case FCDEntity::ENTITY:
	case FCDEntity::IMAGE:
	default: FUFail(instanceEntityName = DAEERR_UNKNOWN_ELEMENT);
	}
	return instanceEntityName;
}

static void WriteControllerInstances(ControllerInstanceList &controllerInstances, xmlNode* parentNode)
{
	size_t skinnedMeshCount = controllerInstances.size();
	if ( skinnedMeshCount > 0 )
	{
		xmlNode *pSkinnedMeshesNode = AddChild( parentNode, DAE_MODULAR_SKIN_SKINNED_MESHES );
		for ( size_t skinnedMeshId = 0; skinnedMeshId < skinnedMeshCount; ++skinnedMeshId )
		{
			FCDControllerInstance *pControllerInstance = controllerInstances[skinnedMeshId];
			FArchiveXML::WriteControllerInstance( pControllerInstance, pSkinnedMeshesNode );
		}
	}
}

//
// Instance Export
//
static xmlNode* WriteGenericEntityInstanceNode(FCDObject* object, xmlNode* parentNode)
{
	FCDEntityInstance* entityInstance = (FCDEntityInstance*)object;

	const char* instanceEntityName = GetInstanceClassTypeFCDEntityInstance(entityInstance->GetEntityType());
	xmlNode* instanceNode = AddChild(parentNode, instanceEntityName);

	if (!entityInstance->GetWantedSubId().empty())
	{
		AddAttribute(instanceNode, DAE_SID_ATTRIBUTE, entityInstance->GetWantedSubId());
	}
	if (!entityInstance->GetName().empty())
	{
		AddAttribute(instanceNode, DAE_NAME_ATTRIBUTE, entityInstance->GetName());
	}

	return instanceNode;
}

static void WriteGenericEntityInstanceNodeTarget(FCDObject* object, xmlNode* instanceNode)
{
	FCDEntityInstance* entityInstance = (FCDEntityInstance*)object;

	// Only write out our entity if we have one.
	const FUUri& uri = entityInstance->GetEntityUri();
	fstring uriString = entityInstance->GetDocument()->GetFileManager()->CleanUri(uri);
	AddAttribute(instanceNode, DAE_URL_ATTRIBUTE, uriString);
}

static xmlNode* WriteGenericEntityInstance(FCDObject* object, xmlNode* parentNode)
{
	xmlNode *instanceNode = WriteGenericEntityInstanceNode( object, parentNode );
	WriteGenericEntityInstanceNodeTarget( object, instanceNode );
	return instanceNode;
}

static void WriteGeometryInstanceBindMaterial(FCDObject* object, xmlNode* parentNode)
{
	FCDGeometryInstance* geometryInstance = (FCDGeometryInstance*)object;
	if (!(geometryInstance->GetMaterialInstanceCount() == 0))
	{
		xmlNode* bindMaterialNode = AddChild(parentNode, DAE_BINDMATERIAL_ELEMENT);
		size_t parameterCount = geometryInstance->GetEffectParameterCount();
		for (size_t p = 0; p < parameterCount; ++p)
		{
			FArchiveXML::LetWriteObject(geometryInstance->GetEffectParameter(p), bindMaterialNode);
		}
		xmlNode* techniqueCommonNode = AddChild(bindMaterialNode, DAE_TECHNIQUE_COMMON_ELEMENT);
		for (size_t i = 0; i < geometryInstance->GetMaterialInstanceCount(); ++i)
		{
			FArchiveXML::LetWriteObject(geometryInstance->GetMaterialInstance(i), techniqueCommonNode);
		}
	}
}

static xmlNode *WriteModularSkinCategoryInstanceNoExtra( FCDObject* object, xmlNode* parentNode )
{
	FCDModularSkinCategoryInstance* pInstance = (FCDModularSkinCategoryInstance*)object;
	xmlNode *instanceNode = WriteGenericEntityInstanceNode( object, parentNode );
	FCDModularSkinCategory *pModularSkinCategory =  ( FCDModularSkinCategory * )pInstance->GetEntity();

	size_t skinnedMeshCount = pModularSkinCategory->GetControllerInstanceCount();
	ControllerInstanceList controllerInstances( skinnedMeshCount );
	for ( size_t skinnedMeshId = 0; skinnedMeshId < skinnedMeshCount; ++skinnedMeshId )
	{
		controllerInstances[skinnedMeshId] = pModularSkinCategory->GetControllerInstance( skinnedMeshId );
	}

	WriteControllerInstances( controllerInstances, instanceNode );
	return instanceNode;
}

xmlNode* FArchiveXML::WriteEntityInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDEntityInstance* entityInstance = (FCDEntityInstance*)object;
	xmlNode *pInstanceNode = WriteGenericEntityInstance(entityInstance, parentNode);
	FArchiveXML::WriteEntityInstanceExtra(entityInstance, pInstanceNode);
	return pInstanceNode;
}

xmlNode* FArchiveXML::WriteEmitterInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDEmitterInstance* emitterInstance = (FCDEmitterInstance*)object;
	xmlNode* instanceNode = WriteGenericEntityInstance(object, parentNode);
	WriteGeometryInstanceBindMaterial( object, instanceNode );

	// forces
	FCDEmitter *pEmitter = ( FCDEmitter * )emitterInstance->GetEntity();
	size_t numForces = pEmitter->GetNumForceFCDSceneNodes();
	if ( numForces > 0 )
	{
		xmlNode *pForceNodesNode = AddChild( instanceNode, DAE_FORCE_NODES );
		for ( size_t forceId = 0; forceId < numForces; ++forceId )
		{
			// find node containing instance
			FCDSceneNode *pInstanceParentNode = FAXICHelpers::GetInstanceParentNode( pEmitter->GetForceFCDSceneNode( forceId ) );
			if ( !pInstanceParentNode )
			{
				continue;
			}

			const FUUri uri = FAXICHelpers::GetEntityUri( pEmitter->GetDocument(), pInstanceParentNode );
			fstring uriString = pEmitter->GetDocument()->GetFileManager()->CleanUri( uri );

			// get instance sid
			// only one category instance per node
			FCDEntityInstance *pInstance = pInstanceParentNode->GetInstance( 0 );
			const fstring &forceInstanceSid = pInstance->GetWantedSubId();
			if ( forceInstanceSid.length() > 0 )
			{
				uriString += "/" + forceInstanceSid;
			}

			xmlNode *pForceNodeNode = AddChild( pForceNodesNode, DAE_FORCE_NODE );
			FAXICHelpers::AddUriContent( pForceNodeNode, uriString );
		}
	}

	FArchiveXML::WriteEntityInstanceExtra(emitterInstance, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WriteForceInstance(FCDObject* object, xmlNode* parentNode)
{
	return WriteEntityInstance( object, parentNode );
}

xmlNode* FArchiveXML::WriteModularSkinCategoryInstance(FCDObject* object, xmlNode* parentNode)
{
	xmlNode *instanceNode = WriteModularSkinCategoryInstanceNoExtra( object, parentNode );
	FArchiveXML::WriteEntityInstanceExtra( ( FCDEntityInstance * )object, instanceNode );
	return instanceNode;
}

xmlNode* FArchiveXML::WriteModularSkinMasterInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDModularSkinMasterInstance* pInstance = (FCDModularSkinMasterInstance*)object;
	xmlNode *instanceNode = WriteGenericEntityInstanceNode( object, parentNode );
	FCDModularSkinMaster *pModularSkinMaster =  ( FCDModularSkinMaster * )pInstance->GetEntity();

	{
		size_t skinnedMeshCount = pModularSkinMaster->GetControllerInstanceCount();
		ControllerInstanceList controllerInstances( skinnedMeshCount );
		for ( size_t skinnedMeshId = 0; skinnedMeshId < skinnedMeshCount; ++skinnedMeshId )
		{
			controllerInstances[skinnedMeshId] = pModularSkinMaster->GetControllerInstance( skinnedMeshId );
		}

		WriteControllerInstances( controllerInstances, instanceNode );
	}

	if ( s_options.ExportModularSkinCategoryAsNode() )
	{
		size_t categoryCount = pModularSkinMaster->GetCategorySceneNodeReferenceCount();
		if ( categoryCount > 0 )
		{
			xmlNode *pCategoriesNode = AddChild( instanceNode, DAE_MODULAR_SKIN_CATEGORIES );
			for ( size_t categoryId = 0; categoryId < categoryCount; ++categoryId )
			{		
				xmlNode *pCategoryNode = NULL;
				{
					// find node containing instance
					const FCDEntityReference &categoryNodeRef = pModularSkinMaster->GetCategorySceneNodeReference( categoryId );
					FCDSceneNode *pInstanceParentNode = FAXICHelpers::GetInstanceParentNode( ( FCDSceneNode * )categoryNodeRef.GetEntity() );
					if ( !pInstanceParentNode )
					{
						continue;
					}

					const FUUri uri = FAXICHelpers::GetEntityUri( pModularSkinMaster->GetDocument(), pInstanceParentNode );
					fstring uriString = pModularSkinMaster->GetDocument()->GetFileManager()->CleanUri( uri );
					
					// get instance sid
					// only one category instance per node
					FCDEntityInstance *pInstance = pInstanceParentNode->GetInstance( 0 );
					const fstring &forceInstanceSid = pInstance->GetWantedSubId();
					if ( forceInstanceSid.length() > 0 )
					{
						uriString += "/" + forceInstanceSid;
					}
					
					pCategoryNode = AddChild( pCategoriesNode, DAE_MODULAR_SKIN_CATEGORY );
					AddAttribute( pCategoryNode, DAE_TARGET_ATTRIBUTE, uriString );
				}

				{
					// selected skin
					const FCDEntityReference &selectedSkinControllerRef = pModularSkinMaster->GetSelectedSkinControllerReference( categoryId );
					const FUUri uri = selectedSkinControllerRef.GetUri();
					fstring uriString = pModularSkinMaster->GetDocument()->GetFileManager()->CleanUri( uri );
					if ( !IsEquivalent( uriString, "#" ) )
					{
						AddAttribute( pCategoryNode, DAE_MODULAR_SKIN_SELECTED_SKIN, uriString );
					}
				}
			}	
		}
	}
	else
	{
		size_t categoryCount = pModularSkinMaster->GetCategoryInstanceCount();
		if ( categoryCount > 0 )
		{
			xmlNode *pCategoriesNode = AddChild( instanceNode, DAE_MODULAR_SKIN_CATEGORIES );
			for ( size_t categoryId = 0; categoryId < categoryCount; ++categoryId )
			{		
				xmlNode *pCategoryNode = NULL;
				{
					// find node containing instance
					FCDModularSkinCategoryInstance *pCategoryInstance = pModularSkinMaster->GetCategoryInstance( categoryId );
					pCategoryNode = WriteModularSkinCategoryInstanceNoExtra( pCategoryInstance, pCategoriesNode );

					FCDEntity *pCategoryEntity = ( FCDEntity * )pCategoryInstance->GetEntity();
					FUAssert( pCategoryEntity, continue );
					AddAttribute( pCategoryNode, DAE_NAME_ATTRIBUTE, pCategoryEntity->GetName() );
				}

				{
					// selected skin
					const FCDEntityReference &selectedSkinControllerRef = pModularSkinMaster->GetSelectedSkinControllerReference( categoryId );
					const FUUri uri = selectedSkinControllerRef.GetUri();
					fstring uriString = pModularSkinMaster->GetDocument()->GetFileManager()->CleanUri( uri );
					if ( !IsEquivalent( uriString, "#" ) )
					{
						AddAttribute( pCategoryNode, DAE_MODULAR_SKIN_SELECTED_SKIN, uriString );
					}
				}
			}	
		}
	}

	FArchiveXML::WriteEntityInstanceExtra(pInstance, instanceNode);
	return instanceNode;
}

xmlNode *FArchiveXML::WriteBillboardManagerInstance( FCDObject* object, xmlNode* parentNode )
{
	FCDBillboardManagerInstance* pInstance = (FCDBillboardManagerInstance*)object;
	xmlNode *instanceNode = WriteGenericEntityInstanceNode( object, parentNode );
	FCDBillboardManager *pBillboardManager =  ( FCDBillboardManager * )pInstance->GetEntity();

	// type, orientation
	WRITE_IC_ENUM_PARAM_AS_STRING( FUDaeBillboard, instanceNode, BillboardType, pBillboardManager, GetBillboardType, DAE_BILLBOARD_TYPE );
	WRITE_IC_ENUM_PARAM_AS_STRING( FUDaeBillboard, instanceNode, OrientationType, pBillboardManager, GetOrientationType, DAE_BILLBOARD_ORIENTATION );

	// front, up vectors
	FAXICHelpers::WriteAnimatedParameterContent( instanceNode, DAE_BILLBOARD_FRONT, DAEFC_FLOAT3_ATTRIBUTE_TYPE, pBillboardManager->GetFront() );
	FAXICHelpers::WriteAnimatedParameterContent( instanceNode, DAE_BILLBOARD_UP, DAEFC_FLOAT3_ATTRIBUTE_TYPE, pBillboardManager->GetUp() );

	// targets
	{
		size_t billboardCount = pBillboardManager->GetBillboardSceneNodeReferenceCount();
		if ( billboardCount > 0 )
		{
			xmlNode *pBillboardTargetsNode = AddChild( instanceNode, DAE_BILLBOARD_TARGETS );
			for ( size_t billboardId = 0; billboardId < billboardCount; ++billboardId )
			{		
				{
					// find target node
					FCDSceneNode *pTargetNode = NULL;
					{
						const FCDEntityReference &billboardNodeRef = pBillboardManager->GetBillboardSceneNodeReference( billboardId );
						FCDSceneNode *pBillboardSceneNode = ( FCDSceneNode * )billboardNodeRef.GetEntity();
						FUAssert( pBillboardSceneNode, continue );

						pTargetNode = FAXICHelpers::GetInstanceParentNode( pBillboardSceneNode );
						if ( !pTargetNode )
						{
							pTargetNode = FAXICHelpers::FindFirstPivotDescendant( pBillboardSceneNode, 1 );
							pTargetNode = pTargetNode ? pTargetNode : pBillboardSceneNode;
						}
					}

					const FUUri uri = FAXICHelpers::GetEntityUri( pBillboardManager->GetDocument(), pTargetNode );
					fstring uriString = pBillboardManager->GetDocument()->GetFileManager()->CleanUri( uri );
					
					AddChild( pBillboardTargetsNode, DAE_BILLBOARD_TARGET, uriString );
				}
			}
		}
	}

	FArchiveXML::WriteEntityInstanceExtra(pInstance, instanceNode);
	return instanceNode;
}


xmlNode* FArchiveXML::WriteSpriteInstance(FCDEntityInstance* object, xmlNode* parentNode)
{
	xmlNode* theRealExport = WriteGeometryInstance(object, parentNode);
	// Rename from geometry_instance to sprite
	RenameNode(theRealExport, DAE_INSTANCE_SPRITE_ELEMENT);
	// The sprite doesnt have a URL
	RemoveAttribute(theRealExport, DAE_URL_ATTRIBUTE);
	return theRealExport;
}

xmlNode* FArchiveXML::WriteGeometryInstance(FCDObject* object, xmlNode* parentNode)
{
	xmlNode* instanceNode = WriteGenericEntityInstance(object, parentNode);
	WriteGeometryInstanceBindMaterial( object, instanceNode );
	FArchiveXML::WriteEntityInstanceExtra(( FCDEntityInstance * )object, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WriteControllerInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDControllerInstance* controllerInstance = (FCDControllerInstance*)object;

	// Export the geometry instantiation information.
	xmlNode* instanceNode = FArchiveXML::WriteGeometryInstance(controllerInstance, parentNode);
	xmlNode* insertBeforeNode = (instanceNode != NULL) ? instanceNode->children : NULL;

	// Retrieve the parent joints and export the <skeleton> elements.
	FUUriList& skeletonRoots = controllerInstance->GetSkeletonRoots();
	for (FUUriList::iterator itS = skeletonRoots.begin(); itS != skeletonRoots.end(); ++itS)
	{
		// TODO: External references (again)
		fm::string fragment = TO_STRING((*itS).GetFragment());
		FUSStringBuilder builder; builder.set('#'); builder.append(fragment);
		xmlNode* skeletonNode = InsertChild(instanceNode, insertBeforeNode, DAE_SKELETON_ELEMENT);
		AddContent(skeletonNode, builder);
	}

	return instanceNode;
}

xmlNode* FArchiveXML::WriteMaterialInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDMaterialInstance* materialInstance = (FCDMaterialInstance*)object;

	xmlNode* instanceNode = WriteGenericEntityInstance(materialInstance, parentNode);

	// no url in instance_material
	RemoveAttribute(instanceNode, DAE_URL_ATTRIBUTE);

	AddAttribute(instanceNode, DAE_SYMBOL_ATTRIBUTE, materialInstance->GetSemantic());

	// Only write out our entity if we have one.
	const FUUri& uri = materialInstance->GetEntityReference()->GetUri();
	fstring uriString = materialInstance->GetDocument()->GetFileManager()->CleanUri(uri);
	AddAttribute(instanceNode, DAE_TARGET_ATTRIBUTE, uriString);

	// Write out the bindings.
	for (size_t i = 0; i < materialInstance->GetBindingCount(); ++i)
	{
		const FCDMaterialInstanceBind& bind = *materialInstance->GetBinding(i);
		xmlNode* bindNode = AddChild(instanceNode, DAE_BIND_ELEMENT);
		AddAttribute(bindNode, DAE_SEMANTIC_ATTRIBUTE, bind.semantic);
		AddAttribute(bindNode, DAE_TARGET_ATTRIBUTE, bind.target);
	}
	
	// Write out the vertex input bindings.
	for (size_t i = 0; i < materialInstance->GetVertexInputBindingCount(); ++i)
	{
		const FCDMaterialInstanceBindVertexInput* bind = materialInstance->GetVertexInputBinding(i);
		xmlNode* bindNode = AddChild(instanceNode, DAE_BIND_VERTEX_INPUT_ELEMENT);
		AddAttribute(bindNode, DAE_SEMANTIC_ATTRIBUTE, bind->semantic);
		AddAttribute(bindNode, DAE_INPUT_SEMANTIC_ATTRIBUTE, FUDaeGeometryInput::ToString(bind->GetInputSemantic()));
		AddAttribute(bindNode, DAE_INPUT_SET_ATTRIBUTE, bind->inputSet);
	}


	FArchiveXML::WriteEntityInstanceExtra(materialInstance, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WriteLightInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDLightInstance* lightInstance = (FCDLightInstance*)object;
	xmlNode* instanceNode = WriteGenericEntityInstance(lightInstance, parentNode);
	FArchiveXML::WriteEntityInstanceExtra(lightInstance, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WritePhysicsForceFieldInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDPhysicsForceFieldInstance* physicsForceFieldInstance = (FCDPhysicsForceFieldInstance*)object;

	xmlNode* instanceNode = WriteGenericEntityInstance(physicsForceFieldInstance, parentNode);

	// nothing interesting in force field instance to write

	FArchiveXML::WriteEntityInstanceExtra(physicsForceFieldInstance, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WritePhysicsModelInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDPhysicsModelInstance* physicsModelInstance = (FCDPhysicsModelInstance*)object;

	xmlNode* instanceNode = WriteGenericEntityInstance(physicsModelInstance, parentNode);

	// The sub-instances must be ordered correctly: force fields first, then rigid bodies; rigid constraints are last.
	for (size_t i = 0; i < physicsModelInstance->GetInstanceCount(); ++i)
	{
		if (physicsModelInstance->GetInstance(i)->GetEntityType() == FCDEntity::FORCE_FIELD)
		{
			FArchiveXML::LetWriteObject(physicsModelInstance->GetInstance(i), instanceNode);
		}
	}
	for (size_t i = 0; i < physicsModelInstance->GetInstanceCount(); ++i)
	{
		if (physicsModelInstance->GetInstance(i)->GetEntityType() == FCDEntity::PHYSICS_RIGID_BODY)
		{
			FArchiveXML::LetWriteObject(physicsModelInstance->GetInstance(i), instanceNode);
		}
	}
	for (size_t i = 0; i < physicsModelInstance->GetInstanceCount(); ++i)
	{
		if (physicsModelInstance->GetInstance(i)->GetEntityType() == FCDEntity::PHYSICS_RIGID_CONSTRAINT)
		{
			FArchiveXML::LetWriteObject(physicsModelInstance->GetInstance(i), instanceNode);
		}
	}

	FArchiveXML::WriteEntityInstanceExtra(physicsModelInstance, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WritePhysicsRigidBodyInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDPhysicsRigidBodyInstance* physicsRigidBodyInstance = (FCDPhysicsRigidBodyInstance*)object;

	xmlNode* instanceNode = WriteGenericEntityInstance(physicsRigidBodyInstance, parentNode);

	AddAttribute(instanceNode, DAE_TARGET_ATTRIBUTE, fm::string("#") + physicsRigidBodyInstance->GetTargetNode()->GetDaeId());
	AddAttribute(instanceNode, DAE_BODY_ATTRIBUTE, physicsRigidBodyInstance->GetEntity()->GetDaeId());

	//inconsistency in the spec
	RemoveAttribute(instanceNode, DAE_URL_ATTRIBUTE);
	
	xmlNode* techniqueNode = AddChild(instanceNode, DAE_TECHNIQUE_COMMON_ELEMENT);

	// almost same as FCDPhysicsRigidBody
	FArchiveXML::AddPhysicsParameter(techniqueNode, DAE_ANGULAR_VELOCITY_ELEMENT, physicsRigidBodyInstance->GetAngularVelocity());
	FArchiveXML::AddPhysicsParameter(techniqueNode, DAE_VELOCITY_ELEMENT, physicsRigidBodyInstance->GetVelocity());
	FArchiveXML::WritePhysicsRigidBodyParameters(physicsRigidBodyInstance->GetParameters(), techniqueNode);

	FArchiveXML::WriteEntityInstanceExtra(physicsRigidBodyInstance, instanceNode);
	return instanceNode;
}

xmlNode* FArchiveXML::WritePhysicsRigidConstraintInstance(FCDObject* object, xmlNode* parentNode)
{
	FCDPhysicsRigidConstraintInstance* physicsRigidConstraintInstance = (FCDPhysicsRigidConstraintInstance*)object;

	xmlNode* instanceNode = WriteGenericEntityInstance(physicsRigidConstraintInstance, parentNode);

	if (physicsRigidConstraintInstance->GetEntity() != NULL && physicsRigidConstraintInstance->GetEntity()->GetObjectType() == FCDPhysicsRigidConstraint::GetClassType())
	{
		FCDPhysicsRigidConstraint* constraint = (FCDPhysicsRigidConstraint*) physicsRigidConstraintInstance->GetEntity();
		AddAttribute(instanceNode, DAE_CONSTRAINT_ATTRIBUTE, constraint->GetSubId());
	}

	//inconsistency in the spec
	RemoveAttribute(instanceNode, DAE_URL_ATTRIBUTE);

	FArchiveXML::WriteEntityInstanceExtra(physicsRigidConstraintInstance, instanceNode);
	return instanceNode;
}
