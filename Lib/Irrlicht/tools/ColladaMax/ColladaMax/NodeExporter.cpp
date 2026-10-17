/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "COLLADAExporter.h"
#include "AnimationExporter.h"
#include "CameraExporter.h"
#include "ControllerExporter.h"
#include "DocumentExporter.h"
#include "EntityExporter.h"
#include "ColladaOptions.h"
#include "ForceExporter.h"
#include "GeometryExporter.h"
#include "LightExporter.h"
#include "MaterialExporter.h"
#include "NodeExporter.h"
#include "EmitterExporter.h"
#include "ForceExporter.h"
#include "ModularSkinExporter.h"
#include "BillboardExporter.h"
#include "XRefFunctions.h"
#include "ColladaIDCreator.h"
#include "Common/ConversionFunctions.h"
#include "ColladaModifiers.h"
#include "AnimationClipManager.h"
#include "IrrCollada.h"
#include "ICObjectPropertyManager.h"
#include "ICBillboardController.h"
#include "DrawLayerManager.h"
#include "MAXHelpers.h"
#include "Common/Light.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDControllerInstance.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDEntityInstance.h"
#include "FCDocument/FCDEntityReference.h"
#include "FCDocument/FCDExternalReferenceManager.h"
#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDEmitterInstance.h"
#include "FCDocument/FCDForceInstance.h"
#include "FCDocument/FCDModularSkinCategory.h"
#include "FCDocument/FCDModularSkinCategoryInstance.h"
#include "FCDocument/FCDModularSkinMasterInstance.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDForceField.h"
#include "FCDocument/FCDocumentTools.h"

#include "FAXICHelpers.h"

#include <ikctrl.h>
#include <iiksys.h>
#include "ilayer.h"

#include "interactiverender.h"

#define PREVIEW_CAMERA_NAME "Camera_Preview"

#define ACTION_TABLE_ID_MACRO_SCRIPTS 647394
#define ACTION_ITEM_DESCRIPTION_CREATE_CAMERA_FROM_VIEW "Create Camera From View"
//#if MAX_VERSION_MAJOR == 8
//#define ACTION_ITEM_ID_CREATE_CAMERA_FROM_VIEW 26504
//#elif MAX_VERSION_MAJOR == 9
//#define ACTION_ITEM_ID_CREATE_CAMERA_FROM_VIEW 26506
//#endif

using namespace FSConvert;

extern IrrCollada g_irrCollada;

NodeExporter::NodeExporter(DocumentExporter* _document)
:	BaseExporter(_document), worldNode(NULL)
, m_previewCameraHandle( NULL )
, m_perspectiveViewportIndex( -1 )
{
}

NodeExporter::~NodeExporter()
{
	CLEAR_POINTER_STD_PAIR_CONT(ExportNodeMap, exportNodes);
	worldNode = NULL;
}

void NodeExporter::HideStrippedNodes()
{
#define ADD_TO_STRIPPED_NODES( __pNode, __strippedNodes, __forcedNodes ) \
{ \
	if ( !__pNode->IsHidden() ) \
	{ \
		__strippedNodes.push_back( __pNode ); \
		__pNode->Hide( true ); \
	} \
	 \
	CalculateForcedNodes( __pNode, __forcedNodes );	\
}

	if ( !OPTS->StripAssociatedNodes() )
	{
		return;
	}

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

		ColladaMaxTypes::MaxType colladaMaxType = ColladaMaxTypes::Get( pNode, pObject );
		int stripFlags = OPTS->LibrariesToStrip();
		NodeList forcedNodes;

		if ( colladaMaxType == ColladaMaxTypes::CAMERA )
		{
			if ( TEST_FLAG( stripFlags, FArchiveXMLOptions::STRIP_LIB_CAMERAS ) )
			{
				ADD_TO_STRIPPED_NODES( pNode, m_strippedNodes, forcedNodes );
			}
		}
		else if ( colladaMaxType == ColladaMaxTypes::LIGHT )
		{
			if ( TEST_FLAG( stripFlags, FArchiveXMLOptions::STRIP_LIB_LIGHTS ) )
			{
				ADD_TO_STRIPPED_NODES( pNode, m_strippedNodes, forcedNodes );
			}
		}
		else if ( colladaMaxType == ColladaMaxTypes::FORCE )
		{
			if ( TEST_FLAG( stripFlags, FArchiveXMLOptions::STRIP_LIB_EXTRALIBS ) )
			{
				ADD_TO_STRIPPED_NODES( pNode, m_strippedNodes, forcedNodes );
			}
		}
		else if ( colladaMaxType == ColladaMaxTypes::PARTICLE_SYSTEM )
		{
			if ( TEST_FLAG( stripFlags, FArchiveXMLOptions::STRIP_LIB_EXTRALIBS ) )
			{
				ADD_TO_STRIPPED_NODES( pNode, m_strippedNodes, forcedNodes );
			}
		}
		else if ( colladaMaxType == ColladaMaxTypes::MESH )
		{
			bool bIsControllerInstance = false;
			{
				ColladaModifierStack modifiers;
				ControllerExporter::ResolveSkinAndMorphModifiers( pNode, pObject, modifiers );
				for (ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end(); ++itM)
				{
					ColladaModifier* modifier = (*itM);
					if ( modifier->GetClassID() == COLLADA_SKIN_CLASS_ID 
						|| modifier->GetClassID() == COLLADA_MORPHER_CLASS_ID )
					{
						bIsControllerInstance = true;
						break;
					}
				}
				CLEAR_POINTER_VECTOR( modifiers );
			}

			if ( bIsControllerInstance )
			{
				if ( TEST_FLAG( stripFlags, FArchiveXMLOptions::STRIP_LIB_CONTROLLERS ) )
				{
					ADD_TO_STRIPPED_NODES( pNode, m_strippedNodes, forcedNodes );
				}
			}
			else
			{
				if ( TEST_FLAG( stripFlags, FArchiveXMLOptions::STRIP_LIB_GEOMETRIES ) )
				{
					ADD_TO_STRIPPED_NODES( pNode, m_strippedNodes, forcedNodes );
				}
			}
		}

		for ( NodeList::iterator nodeIter = forcedNodes.begin(); nodeIter != forcedNodes.end(); ++nodeIter )
		{
			INode *pNode = *nodeIter;
			if ( !pNode->IsHidden() )
			{
				m_strippedNodes.push_back( *nodeIter );
				(*nodeIter)->Hide( true );
			}
		}
	}

#undef ADD_TO_STRIPPED_NODES
}

void NodeExporter::ShowStrippedNodes()
{
	for ( NodeList::iterator nodeIter = m_strippedNodes.begin(); nodeIter != m_strippedNodes.end(); ++nodeIter )
	{
		(*nodeIter)->Hide( false );
	}
}

void NodeExporter::PreExport()
{
	HideStrippedNodes();
	ANIM->GenerateLinearSamplingTimesMaps();
	MSEX->PreExport();
	BBEX->PreExport();
}

void NodeExporter::PostExport()
{
	BBEX->PostExport();
	MSEX->PostExport();
	ShowStrippedNodes();
}

void NodeExporter::ExportScene()
{
	PreExport();

	CalculateExportNodes( &forcedHiddenXRefs );
	GEOM->PreprocessMorphs( exportNodes );

	// Export the materials during this pass.
	ExportMaterials();

	// Second pass: export all the nodes, with empty objects and instances.
	ExportSceneGraph();
	ExportInstances();

	// Third pass: link all the information about the objects and instances.
	LTEX->LinkLights();
	CAME->LinkCameras();
	CTRL->LinkControllers();
	EMEX->LinkEmitters();
	MSEX->LinkModularSkinMasters();
	MSEX->LinkModularSkinCategories();
	BBEX->LinkBillboardManagers();

	FCDSceneNode* sceneRoot = CDOC->GetVisualSceneInstance();
	if (sceneRoot != NULL)
	{
		// Add the ambient lighting from the environment settings
		Point3 ambientCol = GetCOREInterface()->GetAmbient(0, FOREVER);
		if (!ambientCol.Equals(Point3::Origin))
		{
			FCDEntity* ambientLight = document->GetLightExporter()->ExportAmbientLight(ambientCol);
			if (ambientLight != NULL)
			{
				FCDSceneNode* ambientLightNode = CDOC->GetVisualSceneInstance()->AddChildNode();
				ambientLightNode->AddInstance(ambientLight);
			}
		}
		
		// Write out the frame_rate, inside an extra node.
		FCDExtra* extraInfo = CDOC->GetVisualSceneInstance()->GetExtra();
		FCDETechnique* maxTechnique = extraInfo->GetDefaultType()->AddTechnique(DAEMAX_MAX_PROFILE);

		maxTechnique->AddParameter(DAEMAX_FRAMERATE_PARAMETER, GetFrameRate() );
	}

	// export event animations
	ANIM->ExportEvents();

	PostExport();

#if !EXPORT_MODULAR_SKIN_CATEGORY_AS_NODE
	MSEX->RemoveModularSkinCategoryNodes();
#endif // !EXPORT_MODULAR_SKIN_CATEGORY_AS_NODE
}

void NodeExporter::GetSelectedNodes( NodeList &selectedNodes )
{
	int selectedNodeCount = GetCOREInterface()->GetSelNodeCount();
	for (int i = 0; i < selectedNodeCount; ++i)
	{
		selectedNodes.push_back(GetCOREInterface()->GetSelNode(i));
	}
}

void NodeExporter::CalculateExportNodes( NodeList *hiddenXRefs )
{
	CLEAR_POINTER_MAP( exportNodes, ExportNodeMap::iterator );	

	// Make a list of the selected nodes.
	NodeList selectedNodes;
	if (OPTS->ExportSelected())
	{
		GetSelectedNodes( selectedNodes );
	}

	// Process all the objects in the Max scene
	NodeList forcedNodes;
	for (int j = 0; j < worldNode->NumberOfChildren(); j++)
	{
		CalculateExportNodes(worldNode->GetChildNode(j), selectedNodes, forcedNodes, hiddenXRefs);
	}

	// Process the forced nodes: if they are not already contained
	// in the export map, add them and their parent nodes to it.
	while (!forcedNodes.empty())
	{
		INode* node = forcedNodes.back();
		forcedNodes.pop_back();

		while (node != NULL && !node->IsRootNode() && exportNodes.find(node) == exportNodes.end())
		{
			exportNodes.insert(node, new ExportNode(node, false));
			node = node->GetParentNode();
		}
	}
}

void NodeExporter::ExportMaterials()
{
	for ( ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it )
	{
		if ( it->second->exportObject )
		{
			Mtl* nodeMaterial = it->first->GetMtl();
			if (nodeMaterial != NULL)
			{
				// Export this node's material
				document->GetMaterialExporter()->ExportMaterial(nodeMaterial);
			}
		}
	}
}

bool NodeExporter::ShouldExportNode( INode* currentNode, NodeList& selectedNodes )
{
	bool bExportNode = true;

	// Verify whether this node or its parents are selected.
	if (OPTS->ExportSelected())
	{
		NodeList::iterator it = selectedNodes.find(currentNode);
		if (it == selectedNodes.end())
		{
			bExportNode = false;
		}
		else // Remove our node from the list (we are exported)
		{
			selectedNodes.erase(it);
		}
		//  This case is invalid, sub-objects will still be a single node. TODO
		if (GetCOREInterface()->IsSubObjectSelectionEnabled())
		{
			INode* parentNode = currentNode->GetParentNode();
			while (!bExportNode && parentNode != NULL && parentNode != worldNode)
			{
				bExportNode |= selectedNodes.find(parentNode) != selectedNodes.end();
				parentNode = parentNode->GetParentNode();
			}
		}
	}
	else
	{
		bExportNode = !currentNode->IsHidden();
	}

	return bExportNode;
}

void NodeExporter::CalculateForcedNodes( INode* currentNode, NodeList& forcedNodes )
{
	Object* object = currentNode->GetObjectRef();
	if (object != NULL)
	{
		switch (ColladaMaxTypes::Get(currentNode, object))
		{
		case ColladaMaxTypes::LIGHT: LTEX->CalculateForcedNodes(currentNode, (LightObject*) object, forcedNodes); break;
		case ColladaMaxTypes::CAMERA: CAME->CalculateForcedNodes(currentNode, (CameraObject*) object, forcedNodes); break;
		case ColladaMaxTypes::MESH: CTRL->CalculateForcedNodes(currentNode, object, forcedNodes); break;
		case ColladaMaxTypes::PARTICLE_SYSTEM: EMEX->CalculateForcedNodes(currentNode, object, forcedNodes); break;
		case ColladaMaxTypes::MODULAR_SKIN_MASTER: MSEX->CalculateMasterForcedNodes(currentNode, object, forcedNodes); break;
		case ColladaMaxTypes::MODULAR_SKIN_CATEGORY: MSEX->CalculateCategoryForcedNodes(currentNode, object, forcedNodes); break;
		case ColladaMaxTypes::BILLBOARD_MANAGER: BBEX->CalculateForcedNodes(currentNode, object, forcedNodes); break;
		}
	}
}

bool NodeExporter::CalculateExportNodes(INode* currentNode, NodeList& selectedNodes, NodeList& forcedNodes, NodeList *hiddenXRefs )
{
	TSTR name = currentNode->GetName();
	
	// hide all xrefs if export xref is disabled
	// keep hiddenxref node in list and unhide when export is finished
	// this has the effect of removing xref nodes from the exported visual scene tree
	if ( hiddenXRefs && !OPTS->ExportXRefs() )
	{
		Object* object = currentNode->GetObjectRef();
		ColladaMaxTypes::MaxType type = ColladaMaxTypes::Get(currentNode, object, true );
		if ( type == ColladaMaxTypes::XREF_OBJECT )
		{
			bool bNodeHidden = currentNode->IsHidden();
			if ( !bNodeHidden )
			{
				hiddenXRefs->push_back( currentNode );
				currentNode->Hide( true );
			}
		}
	}

	bool bExportNode = ShouldExportNode( currentNode, selectedNodes );

	if ( bExportNode )
	{
		// Export this node's object
		//if (!currentNode->IsHidden())  This case is also invalid
		{
			CalculateForcedNodes( currentNode, forcedNodes );
		}
		
		exportNodes.insert(currentNode, new ExportNode(currentNode, true));
	}

	// Stack in the child nodes for export node verification
	bool forceThis = false;
	for (int j = 0; j < currentNode->NumberOfChildren(); j++)
	{
		// If any of our children are exported, we must be forced
		forceThis |= CalculateExportNodes(currentNode->GetChildNode(j), selectedNodes, forcedNodes, hiddenXRefs);
	}
	if (!bExportNode && forceThis) // we are not exported, but a child is.  Force export to maintain hierarchy
	{
		forcedNodes.push_back(currentNode);
	}
	return bExportNode || forceThis;
}


void NodeExporter::ExportSceneGraph()
{
	if (!exportNodes.empty())
	{
		// Create the COLLADA visual scene
		FCDSceneNode* colladaScene = CDOC->AddVisualScene();
		TCHAR* sfnc = GetCOREInterface()->GetCurFileName().data();
		if (sfnc == NULL || _tcslen(sfnc) == 0) sfnc = _T("unnamed_scene");
		colladaScene->SetDaeId(sfnc);
		colladaScene->SetName(sfnc);

		// Process the scene nodes to export
		for (ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it)
		{
			ExportNode& exportNode = *(*it).second;
			if (exportNode.colladaNode == NULL) ExportSceneNode(exportNode);
		}
	}
}

void NodeExporter::ExportSceneNode(ExportNode& exportNode)
{
	INode* node = exportNode.maxNode;

	// Retrieve the parent node information.
	INode* parentNode = node->GetParentNode();

	ExportNodeMap::iterator itP = exportNodes.find(parentNode);
	if (itP != exportNodes.end() && (*itP).second->colladaNode == NULL)
	{
		// The parent node has not yet been exported: force its export.
		ExportSceneNode(*(*itP).second);
	}
	FCDSceneNode* parent = exportNode.colladaParent = itP != exportNodes.end() ? (*itP).second->colladaNode : CDOC->GetVisualSceneInstance();

	// Create the FCollada scene node
	FCDSceneNode* colladaNode = exportNode.colladaNode = parent->AddChildNode();
	TCHAR* nodeName = node->GetName();
	ENTE->ExportEntity(node, colladaNode, (TO_STRING(nodeName) + "-node").c_str());
	colladaNode->SetName(nodeName);

	// Export the transform and create the pivot node, if needed
	//bool exportPivot = !OPTS->DoSampleMatrices(node);
	bool exportPivot = ExportTransforms(node, colladaNode, true);

	ExportSceneNodeVisibility( colladaNode, node );

	// Verify whether this is a Max bone.
	if (node->GetBoneNodeOnOff())
	{
		colladaNode->SetJointFlag(true);
	}

	// Export this node's object
	if (exportNode.exportObject)
	{
		FCDSceneNode* colladaObjectNode = (exportPivot) ? ExportPivotNode(node, colladaNode) : colladaNode;
		Object* object = node->GetObjectRef();
		if (object != NULL)
		{
			ExportedEntityMap::iterator itE = exportedEntities.find(object);
			EntityList entities;
			ColladaMaxTypes::MaxType colladaMaxType = ColladaMaxTypes::Get(node, object, OPTS->ExportXRefs());
			if (itE == exportedEntities.end())
			{
				switch ( colladaMaxType )
				{
				case ColladaMaxTypes::LIGHT: ADD_ENTITY( entities, LTEX->ExportLight(node, (LightObject*) object) ); break;
				case ColladaMaxTypes::CAMERA: ADD_ENTITY( entities, CAME->ExportCamera(node, (CameraObject*) object) ); break;
				case ColladaMaxTypes::NURBS_CURVE: 
				case ColladaMaxTypes::SPLINE:
				{
					GEOM->ExportSpline( node, object, entities );
					break;
				}
				case ColladaMaxTypes::TARGET:
				case ColladaMaxTypes::MESH: ADD_ENTITY( entities, CTRL->ExportController(node, object) ); break;
				case ColladaMaxTypes::BONE: colladaNode->SetJointFlag(true); break;
				case ColladaMaxTypes::HELPER: ExportHelper(node, object, colladaObjectNode); break;
				case ColladaMaxTypes::PARTICLE_SYSTEM: ADD_ENTITY( entities, EMEX->ExportEmitter(node, object) ); break;
				case ColladaMaxTypes::MODULAR_SKIN_MASTER:
				{
					ADD_ENTITY( entities, MSEX->ExportModularSkinMaster( node, object, exportedEntities ) );
					break;
				}
				case ColladaMaxTypes::MODULAR_SKIN_CATEGORY:
				{
					ADD_ENTITY( entities, MSEX->ExportModularSkinCategory( node, object, exportedEntities ) );
					break;
				}
				case ColladaMaxTypes::BILLBOARD_MANAGER:
				{
					ADD_ENTITY( entities, BBEX->ExportBillboardManager( node, object, exportedEntities ) );
					break;
				}
				case ColladaMaxTypes::FORCE: ADD_ENTITY( entities, FREX->ExportForce(node, object) ); break;
				case ColladaMaxTypes::XREF_OBJECT: 
					{
						// TODO. Remove this when Collada controllers will support XRef sources
						if (document->GetControllerExporter()->DoOverrideXRefExport(object))
						{
							ADD_ENTITY( entities, document->GetControllerExporter()->ExportController(node, object) );
							break;
						}

						fstring fnURL = XRefFunctions::GetURL(object);
						if (!fnURL.empty())
						{
							Object *xRefSourceObject = XRefFunctions::GetXRefObjectSource(object);
							ColladaMaxTypes::MaxType xRefSourceObjectType = ColladaMaxTypes::Get( node, object, false );

							EntityList xReffedEntities;
							StringList xRefNameSuffixes;
							if ( xRefSourceObjectType == ColladaMaxTypes::MESH )
							{
								ADD_ENTITY( xReffedEntities, GEOM->ExportMesh(node, object) );
								xRefNameSuffixes.push_back( "-mesh" );
							}
							else if ( xRefSourceObjectType == ColladaMaxTypes::SPLINE )
							{
								GEOM->ExportSpline( node, object, xReffedEntities );
								for ( size_t entityId = 0; entityId < xReffedEntities.size(); ++entityId )
								{
									FUStringBuilder builder( "-spline" );
									// Now using FCollada automatic name conflict resolution instead of providing our own id
									//builder.append( entityId ); // Id must be kept in sync with the one attributed in ExportSpline and ExportNURBS
									xRefNameSuffixes.push_back( builder.ToString() );
								}
							}
							else if ( xRefSourceObjectType == ColladaMaxTypes::PARTICLE_SYSTEM )
							{
								ADD_ENTITY( xReffedEntities, EMEX->ExportEmitter( node, object ) );
								xRefNameSuffixes.push_back( "-emitter" );
							}
							else if ( xRefSourceObjectType == ColladaMaxTypes::FORCE )
							{
								ADD_ENTITY( xReffedEntities, FREX->ExportForce( node, object ) );
								xRefNameSuffixes.push_back( "-force" );
							}
							else if ( xRefSourceObjectType == ColladaMaxTypes::MODULAR_SKIN_MASTER )
							{
								// TODO
								//ADD_ENTITY( xReffedEntities, MSEX->ExportModularSkinMaster( node, object ) );
								//xRefNameSuffixes.push_back( "-modular_skin_master" );
							}
							else if ( xRefSourceObjectType == ColladaMaxTypes::MODULAR_SKIN_CATEGORY )
							{
								// TODO
								//ADD_ENTITY( xReffedEntities, MSEX->ExportModularSkinCategory( node, object ) );
								//xRefNameSuffixes.push_back( "-modular_skin_category" );
							}
							else if ( xRefSourceObjectType == ColladaMaxTypes::BILLBOARD_MANAGER )
							{
								// TODO
							}

							for ( size_t entityId = 0; entityId < xReffedEntities.size(); ++entityId )
							{
								FCDEntity *xRefEntity = xReffedEntities[entityId];
								const fm::string &xRefNameSuffix = xRefNameSuffixes[entityId];
								
								// for the moment, Max only support Geometry instanciation
								// material instanciation is performed in the material exporter.

								// XRefs may have some materials applied to them as well
								// so we must have a Collada entity describing this geometry XRef
								FCDEntityReference* xref = NULL;
								if (xRefEntity != NULL)
								{
									FCDEntityInstance* instance = colladaObjectNode->AddInstance(xRefEntity);
									xref = instance->GetEntityReference();
									xref->SetUri(FUUri(fnURL));
									xref->SetEntityId(xRefEntity->GetDaeId());

									// export the instance
									GEOM->ExportInstance(node, instance);
								}
								SAFE_RELEASE( xRefEntity );

								XRefFunctions::ReplaceMaxWithDae( object, xref );

								if (xref != NULL && !XRefFunctions::IsXRefCOLLADA(object))
								{
									fm::string xRefName = XRefFunctions::GetXRefObjectName(object);
									xRefName += xRefNameSuffix;
									xref->SetEntityId( xRefName );
								}
							}
						}
						break; 
					}
				} // end switch

				if ( entities.size() > 0 )
				{
					exportedEntities.insert(object, entities);
				}
			}
			else
			{
				entities = (*itE).second;
			}

			// Add one instance per exported entity
			for ( size_t entityId = 0; entityId < entities.size(); ++entityId )
			{
				FCDEntity *entity = entities[entityId];
				FCDEntityInstance* instance = colladaObjectNode->AddInstance(entity);
				instance->SetUserHandle(node);
			}

			// Continue iterating using a copy of the scene node:
			// in order to isolate the controller instance.
			{
				bool bIncludesController = false;
				for ( size_t entityId = 0; entityId < entities.size(); ++entityId )
				{
					FCDEntity *entity = entities[entityId];
					if ( entity->GetObjectType().Includes(FCDController::GetClassType()) && node->NumberOfChildren() > 0 )
					{
						bIncludesController = true;
						break;
					}
				}

				if  ( bIncludesController )
				{
					colladaNode = parent->AddChildNode();
					colladaNode->SetDaeId(TO_STRING(nodeName) + "-copy");
					FUDbgAssert( false, );
					ExportTransforms(node, colladaNode, false);
				}
			}

			// Exclude empty meshes from exported scene
			if ( entities.size() <= 0
				&& node->NumberOfChildren() <= 0 
				&& colladaMaxType == ColladaMaxTypes::MESH )
			{
				SAFE_RELEASE( colladaObjectNode );
			}
		}

		if ( colladaObjectNode )
		{
			// Export the node note.
			TSTR userDefinedProperties;
			node->GetUserPropBuffer(userDefinedProperties);
			if (userDefinedProperties.length() > 0)
			{
				colladaObjectNode->SetNote(userDefinedProperties.data());
			}

			// Export IrrCollada extra data
			ExportSceneNodeIrrColladaExtra( colladaObjectNode, node );
		}
	}
}

void NodeExporter::ExportSceneNodeVisibility( FCDSceneNode *pColladaNode, INode *pMaxNode )
{
	float visibility = pMaxNode->GetVisibility( OPTS->AnimStart() );
	pColladaNode->SetVisibility( visibility );

	Control *pControl = pMaxNode->GetVisController();
	ANIM->ExportAnimatedFloat( pColladaNode->GetName() + "-visibility", pControl, pColladaNode->GetVisibility(), -1, NULL );

	// Export code that takes inherit visibility into account
	//float visibility = pMaxNode->GetInheritVisibility() ? pMaxNode->GetVisibility( OPTS->AnimStart() ) : pMaxNode->GetLocalVisibility( OPTS->AnimStart() );
	//pColladaNode->SetVisibility( visibility );
	//
	//Control *pControl = NULL;
	//
	//INode *pRoot = GetWorldNode();
	//INode *pNode = pMaxNode;
	//while ( pNode->GetInheritVisibility() && pNode != pRoot )
	//{
	//	pControl = pNode->GetVisController();
	//	pNode = pNode->GetParentNode();
	//}

	//ANIM->ExportAnimatedFloat( pColladaNode->GetName() + "-visibility", pControl, pColladaNode->GetVisibility(), -1, &FSConvert::Truncate );
}

void NodeExporter::ExportSceneNodeIrrColladaExtra( FCDSceneNode *pColladaNode, INode *pMaxNode )
{
	if ( !pColladaNode || !pMaxNode )
	{
		return;
	}

	{
		size_t numInstances = pColladaNode->GetInstanceCount();
		FCDEntityInstance *pInstance = numInstances > 0 ? pColladaNode->GetInstance( 0 ) : NULL;	

		fm::pvector<ICUIManager> &iCManagers = g_irrCollada.GetICManagers();
		ICObjectPropertyManager *pICObjectPropertyManager = ( ICObjectPropertyManager * )iCManagers[0];
		
		int numObjectProperties = pICObjectPropertyManager->GetNumObjectProperties();
		if ( numObjectProperties <= 0 )
		{
			return;
		}

		// object properties
		FCDExtra *pExtra = pInstance ? pInstance->GetExtra() : pColladaNode->GetExtra();
		FCDEType* pExtraType = pExtra->GetDefaultType();
		FCDETechnique *pTechnique = pExtraType->AddTechnique( DAE_IRRLICHT_PROFILE );

		// TODO : Generalize this
		// draw layer
		{
			const ICObjectProperty *prop = pICObjectPropertyManager->GetObjectPropertyPtrByCID( IrrCollada::CID_DRAW_LAYER );
			FCDENode *pExtraNode = pTechnique->AddChildNode( DAE_IRR_DRAW_LAYER );
			prop->WriteExtraNodeContentFromAppDataChunk( pMaxNode, pExtraNode );
		}

		// visibility
		//{
		//	const ICObjectProperty *prop = pICObjectPropertyManager->GetObjectPropertyPtrByCID( IrrCollada::CID_VISIBILITY );
		//	FCDENode *pExtraNode = pTechnique->AddChildNode( DAE_IRR_VISIBILITY );
		//	prop->WriteExtraNodeContentFromAppDataChunk( pMaxNode, pExtraNode );
		//}
	}

	// Billboard Modifier
	{
		FCDExtra *pExtra = pColladaNode->GetExtra();
		FCDEType *pExtraType = pExtra->GetDefaultType();
		FCDETechnique *pTechnique = pExtraType->AddTechnique( DAE_IRRLICHT_PROFILE );

		BBEX->ExportBillboardModifier( pMaxNode, pTechnique, pColladaNode->GetName() );
	}
}

void NodeExporter::ExportInstances()
{
    FCDSceneNode* visualScene = CDOC->GetVisualSceneInstance();
	if (visualScene != NULL)
	{
		// Iterate over the whole exported scene, looking for entity instances
		fm::pvector<FCDSceneNode> nodes;
		nodes.push_back(visualScene);
		while (!nodes.empty())
		{
			FCDSceneNode* node = nodes.back();
			nodes.pop_back();

			// Look for instances that need linking.
			size_t instanceCount = node->GetInstanceCount();
			for (size_t i = 0; i < instanceCount; ++i)
			{
				FCDEntityInstance* instance = node->GetInstance(i);
				if (instance->GetObjectType().Includes(FCDGeometryInstance::GetClassType()))
				{
					INode* maxNode = (INode*) instance->GetUserHandle();
					GEOM->ExportInstance(maxNode, instance);
				}
				else if (instance->GetObjectType().Includes(FCDModularSkinCategoryInstance::GetClassType()))
				{
					FCDModularSkinCategoryInstance *pModularSkinCategoryInstance = ( FCDModularSkinCategoryInstance * )instance;
					FCDModularSkinCategory *pModularSkinCategory = ( FCDModularSkinCategory * )pModularSkinCategoryInstance->GetEntity();
					
					size_t controllerInstanceCount = pModularSkinCategory->GetControllerInstanceCount();
					for ( size_t controllerInstanceId = 0; controllerInstanceId < controllerInstanceCount; ++controllerInstanceId )
					{
						FCDControllerInstance *pControllerInstance = pModularSkinCategory->GetControllerInstance( controllerInstanceId );
						INode* maxNode = (INode*) pControllerInstance->GetUserHandle();
						document->GetGeometryExporter()->ExportInstance(maxNode, pControllerInstance);
					}
				}
			}

			// Add all the children node to be processed
			nodes.insert(nodes.end(), node->GetChildren(), node->GetChildren() + node->GetChildrenCount());
		}
	}
}

void NodeExporter::ExportHelper(INode* node, Object* object, FCDSceneNode* colladaNode)
{
	if (object->SuperClassID() != HELPER_CLASS_ID)
		return;

	// Retrieve the helper's bounding box.
	Box3 boundingBox;
	object->GetLocalBoundBox(TIME_EXPORT_START, node, document->GetMaxInterface()->GetActiveViewport(), boundingBox);

	// Export the bounding box as extra information.
	FCDExtra* extra = colladaNode->GetExtra();
	FCDETechnique* technique = extra->GetDefaultType()->AddTechnique(DAEMAX_MAX_PROFILE);
	FCDENode* helperNode = technique->AddChildNode("helper");
	helperNode->AddParameter("bounding_min", ToFMVector3(boundingBox.Min()));
	helperNode->AddParameter("bounding_max", ToFMVector3(boundingBox.Max()));

	// Because we dont actually export an object for a helper, take any extra information
	// defined on it and export it on the colladaNode
	ENTE->ExportEntity(object, colladaNode, NULL);
}

bool NodeExporter::ExportTransforms(INode* node, FCDSceneNode* colladaNode, bool checkSkin)
{
	Object* obj = node->GetObjectRef();
	Control* c = node->GetTMController();

	// if billboard rotation controller, temporarily replacing it with a euler rotation controller
	bool bIsHolding = theHold.Holding();
	bool bIsBillboardRotationController = c && c->GetRotationController() && c->GetRotationController()->ClassID() == BILLBOARD_CONTROLLER_CLASS_ID;
	Control *pEulerRotationController = NULL;
	Control *pBillboardRotationController = NULL;
	if ( bIsBillboardRotationController )
	{
		if ( bIsHolding )
		{
			theHold.Suspend();
		}

		pBillboardRotationController = c->GetRotationController();
		pEulerRotationController = NewDefaultRotationController();
		c->SetRotationController( pEulerRotationController );
	}

	// Special case for footprint controller ( associated to Bip01 Footsteps node )
	// Calling GetNodeTM for time values in biped scene anim range has a 
	// side effect on the result of GetNodeTM called before or at first biped keyframe time
	// The result of this was that two consecutive exports of the same file would yield different transforms
	// for Bip01 Footsteps node.
	// Calling GetNodeTM for every frame in the scene anim range here fixes that issue.
	if ( c )
	{
		Class_ID classId = c->ClassID();
		if ( classId == FOOTPRINT_CLASS_ID )
		{
			Interval keyRange = AnimationClipManager::GetInst()->GetSceneAnimRange();
			if ( !AnimationClipManager::GetInst()->ValidateAnimRange( keyRange ) )
			{
				keyRange = AnimationClipManager::GetInst()->GetTotalAnimRange();
			}
			int animStart = keyRange.Start();
			int animEnd = keyRange.End() + 1;

			AnimationExporter::TimeMappingList keyTimeMappingList;
			ANIM->GetAnimatableTimeMappingList( c, keyTimeMappingList, CTRL->FindConstraint( c ) ? AnimationExporter::GATMLF_USE_SECONDARY_SAMPLE_RATE : AnimationExporter::GATMLF_NONE );
			for ( AnimationExporter::TimeMappingList::iterator ticksToSecsMapIter = keyTimeMappingList.begin(); ticksToSecsMapIter != keyTimeMappingList.end(); ++ticksToSecsMapIter )
			{
				const TimeValue &t = ticksToSecsMapIter->m_ticks;
				node->GetNodeTM( t );
			}
		}
	}

	bool bExportPivot = true;
	if ( ControllerExporter::IsBipedController( c ) )
	{
		bExportPivot = false;
	}
	
	// PRS nodes for others
	Matrix3 tm = GetLocalTransform(node);

	// Check for skinned object. In this special case, the transform for
	// the first skinned instance will be saved with the controller.
	// See https://collada.org/phpBB2/viewtopic.php?t=515 for details.
	bool hasSkinnedObject = checkSkin && document->GetControllerExporter()->IsSkinned(obj);
	if (hasSkinnedObject)
	{
		// Figure out whether this is the instance or the original
		INodeTab instanceNodes;
		IInstanceMgr::GetInstanceMgr()->GetInstances(*node, instanceNodes);
		if (instanceNodes.Count() == 0)
		{
			bExportPivot = false;
			goto end;
		}

		INode* firstInstanceNode = instanceNodes[0];
		if ( node == firstInstanceNode )
		{
			bExportPivot = false;
			goto end;
		}

		// For all instances, export the difference between the first
		// instance's local transformation and the current local transformation.
		// Note that you cannot export animations for this.

		// Max is strange in this. It always moves back the skin to its original position,
		// when animating, whatever the new transform it might contain. So, we need to take out
		// both the initial TM and the first Instance's TM to figure out where to place this instance.

		// The first instance TM include its pivot TM
		Matrix3 firstInstanceTM = firstInstanceNode->GetObjTMBeforeWSM(0);
		INode *parentNode = firstInstanceNode->GetParentNode();
		if (parentNode != NULL && parentNode->IsRootNode() == FALSE)
		{
			Matrix3 parentTM = GetWorldTransform(parentNode);
			parentTM.Invert();
			firstInstanceTM *= parentTM;
		}

		firstInstanceTM.Invert();
		tm *= firstInstanceTM;
	}

	ExportTransformElements(colladaNode, node, c, tm);

end:
	// Restoring billboard controller
	if ( bIsBillboardRotationController )
	{
		c->SetRotationController( pBillboardRotationController );
		if ( bIsHolding )
		{
			theHold.Resume();
		}
	}

	return bExportPivot;
}

#define EXPORT_DECOMPOSED_TRANSFORM( colladaNode, node, ap ) \
	/* Translation */ \
	FCDTTranslation* translation = (FCDTTranslation*) colladaNode->AddTransform(FCDTransform::TRANSLATION); \
	translation->SetTranslation(ToFMVector3(ap.t)); \
	\
	/* Rotation */ \
	fm::pvector<FCDTRotation> rotations = ExportUnanimatedRotationElement( colladaNode, ap.q ); \
	\
	/* Scale */ \
	FCDTScale* scale = (FCDTScale*) colladaNode->AddTransform(FCDTransform::SCALE); \
	scale->SetScale(ToFMVector3(ap.k))

void NodeExporter::ExportDecomposedTransform( FCDSceneNode* colladaNode, INode* node, Control* c, const AffineParts &ap )
{
	EXPORT_DECOMPOSED_TRANSFORM( colladaNode, node, ap );
	ANIM->ExportDecomposedAnimatedTransform( colladaNode->GetDaeId(), node, c, translation->GetTranslation(), scale->GetScale(), rotations, false );
}

void NodeExporter::FitCubicCurve( FCDSceneNode* colladaNode, INode* node, Control* c, const AffineParts &ap )
{
	EXPORT_DECOMPOSED_TRANSFORM( colladaNode, node, ap );
	ANIM->FitCubicCurve( colladaNode->GetDaeId(), node, c, translation->GetTranslation(), scale->GetScale(), rotations, false );
}

void NodeExporter::ExportDecomposedWSMPivot( FCDSceneNode* colladaNode, INode* node, Control* c, const AffineParts &ap )
{
	EXPORT_DECOMPOSED_TRANSFORM( colladaNode, node, ap );

	// This can only be animated if we have a WSM object
	if (node->GetWSMDerivedObject() == NULL)
	{
		return;
	}

	ANIM->ExportDecomposedAnimatedTransform( colladaNode->GetDaeId(), node, c, translation->GetTranslation(), scale->GetScale(), rotations, true );
}

#undef EXPORT_DECOMPOSED_TRANSFORM

void NodeExporter::ExportTransformElements(FCDSceneNode* colladaNode, INode* node, Control* c, const Matrix3& tm)
{
	// decomposing matrix
	AffineParts ap;
	AffineDecomp( tm, ap );

	LOG( OPTS->m_warningManager, ICWarning::NEGATIVE_SCALE, NULL, colladaNode, ap.k );

	if ( !OPTS->DecomposeMatrices() )
	{
		//if ( OPTS->DoSampleMatrices(node) )
		{
			// Thanks to KevT for his work on baked matrices
			FCDTMatrix* matrix = (FCDTMatrix*) colladaNode->AddTransform(FCDTransform::MATRIX);
			matrix->SetTransform(ToFMMatrix44(tm));
			ANIM->ExportAnimatedTransform(colladaNode->GetDaeId() + "-transform", node, c, matrix->GetTransform(), false );
		}
		//else
		//{
		//	// TODO
		//}
	}
	else
	{
		bool bSampling = OPTS->DoSampleMatrices(node);
		bool bIsBipedController = ControllerExporter::IsBipedController( c );
		
		// NOTE : If c is NULL, no animation track will be exported ( sampling or no sampling )
		bSampling |=  !c || ( !bIsBipedController && OPTS->RotationFormat() != ColladaOptions::EULER_XYZ );

		if ( bSampling )
		{
			ExportDecomposedTransform( colladaNode, node, c, ap );
		}
		else
		{
			// Curve fitting for biped controllers
			if ( bIsBipedController )
			{
				FitCubicCurve( colladaNode, node, c, ap );
			}
			else
			{
				// TODO : Implement option to force export of invalid transformation animations

				// Translation
				FCDTTranslation* translation = (FCDTTranslation*) colladaNode->AddTransform(FCDTransform::TRANSLATION);
				translation->SetTranslation(ToFMVector3(ap.t));
				Control* translationController = (c != NULL) ? c->GetPositionController() : NULL;
				ANIM->ExportAnimatedPoint3(colladaNode->GetDaeId() + "-translation", translationController, translation->GetTranslation());

				// Rotation

				// first try with the Rotation controller
				Control* rotationController = (c != NULL) ? c->GetRotationController() : NULL;
				ExportRotationElement(rotationController, colladaNode, ap.q);

				// Check for animated scale. If not animated and equal to the identity, don't export.
				// Animated scale includes animated scale axis, so export that carefully.
				// Export the scale factors, the basis now follows the given scale axis
				FCDTScale* scale = (FCDTScale*) colladaNode->AddTransform(FCDTransform::SCALE);
				scale->SetScale(ToFMVector3(ap.k));
				Control* scaleController = (c != NULL) ? c->GetScaleController() : NULL;
				bool hasAnimatedScale = scaleController != NULL && scaleController->IsAnimated();
				if (hasAnimatedScale )
				{
					ANIM->ExportAnimatedPoint3(colladaNode->GetDaeId() + "-scale", scaleController, scale->GetScale());
				}
			}
		}
	}

	// dbg
	//g_warningLog.append( "Node " );
	//g_warningLog.append( "'" );
	//g_warningLog.append( node->GetName() );
	//g_warningLog.append( "'" );
	//g_warningLog.append( "\n" );
	//ControllerExporter::DBG_PrintCompoundControllerInfo( node, c, 1 );
}

#define ADD_ROTATION_TRANSFORM( colladaNode, rotationPtr, axisVector, degrees ) \
{ \
	rotationPtr = (FCDTRotation*) colladaNode->AddTransform(FCDTransform::ROTATION); \
	rotationPtr->SetAxis(axisVector); \
	rotationPtr->SetAngle(degrees); \
}

fm::pvector<FCDTRotation> NodeExporter::ExportUnanimatedRotationElement( FCDSceneNode* colladaNode, const Quat &q )
{
	fm::pvector<FCDTRotation> addedRotations;
	int rotationFormat = OPTS->RotationFormat();

	if ( rotationFormat == ColladaOptions::EULER_XYZ )
	{
#define NUM_EULER_ROTATIONS 3

		addedRotations.resize( NUM_EULER_ROTATIONS );

		float angles[NUM_EULER_ROTATIONS];
		QuatToEuler(q, angles, EULERTYPE_XYZ);
		fm::swap( angles[0], angles[2] );

		for ( int i = 0; i < NUM_EULER_ROTATIONS; ++i )
		{
			angles[i] = FMath::RadToDeg(angles[i]);
		}

		// Z,Y,X order must be respected
		ADD_ROTATION_TRANSFORM( colladaNode, addedRotations[0], FMVector3::ZAxis, angles[0] );
		ADD_ROTATION_TRANSFORM( colladaNode, addedRotations[1], FMVector3::YAxis, angles[1] );
		ADD_ROTATION_TRANSFORM( colladaNode, addedRotations[2], FMVector3::XAxis, angles[2] );

#undef NUM_EULER_ROTATIONS
	}
	else if ( rotationFormat == ColladaOptions::ANGLE_AXIS )
	{
		addedRotations.resize( 1 );

		Point3 axis;
		float angle;

		// The two methods do not produce the same axis for certain types of nodes ( camera targets and some biped nodes so far )
		//if ( OPTS->DbgSwitch() )
		//{
			//	Matrix3 qtm;
			//	q.MakeMatrix(qtm);
			//	AngAxis angAxisRot = AngAxis(qtm);
			//	axis = angAxisRot.axis;
			//	angle = angAxisRot.angle;
		//}
		//else
		//{
			AngAxisFromQ( q, &angle, axis );
		//}

		angle = -FMath::RadToDeg(angle);

		ADD_ROTATION_TRANSFORM( colladaNode, addedRotations[0], FMVector3( ( float * )&axis ), angle );
	}
	else
	{
		addedRotations.resize( 1 );

		Point3 axis( ( float * )&q );
		float angle = q[3];
		
		ADD_ROTATION_TRANSFORM( colladaNode, addedRotations[0], FMVector3( ( float * )&axis ), angle );
	}


	return addedRotations;
}

void NodeExporter::ExportRotationElement(Control* rotController, FCDSceneNode* colladaNode, const Quat &q)
{
	if ( rotController == NULL )
	{
		ExportUnanimatedRotationElement( colladaNode, q );
		return;
	}
	
	int superClassID = rotController->SuperClassID();
	FUAssert(superClassID == CTRL_ROTATION_CLASS_ID, return);

	Quat val;
	rotController->GetValue(OPTS->AnimStart(), &val, FOREVER, CTRL_ABSOLUTE);
	fm::pvector<FCDTRotation> rotations = ExportUnanimatedRotationElement( colladaNode, val );

	if ( !rotController->IsAnimated() )
	{
		return;
	}

	ANIM->ExportAnimatedEulerAngles( colladaNode->GetDaeId() + "-rotation", rotController, rotations, &FSConvert::RadToDeg );
}

#undef ADD_ROTATION_TRANSFORM

FCDSceneNode* NodeExporter::ExportPivotNode(INode* node, FCDSceneNode* colladaNode)
{
	// Calculate the pivot transform. It should already be in local space.
	Matrix3 tm( 1 );
	ApplyPivotTM( node, tm );

	// Force pivot export if billboarded node
	bool bBillboardNode = false;
	{
		ColladaModifierStack foundModifiers;
		{
			ColladaModifierStack targetModifiers;
			targetModifiers.push_back( new ICBillboardModifier() );
			ColladaModifierManager::ResolveModifiers( node, node->GetWSMDerivedObject(), targetModifiers, foundModifiers );
			CLEAR_POINTER_VECTOR( targetModifiers );
		}
		bBillboardNode = foundModifiers.size() > 0;
		CLEAR_POINTER_VECTOR( foundModifiers );
	}

	// only export the pivot node if the transform is not an identity
	// of if the node is a group head node (this is a temporary fix until we add a PIVOT type)
	if ( !bBillboardNode &&
		( tm.IsIdentity() || node->IsGroupHead() ) )
	{
		return colladaNode;
	}

	// insert pivot node for the pivot.
	FCDSceneNode* colladaPivot = colladaNode->AddChildNode();
	colladaPivot->SetDaeId(colladaNode->GetDaeId() + PIVOT_SUFFIX);
	colladaPivot->SetName(colladaNode->GetName() + FC(PIVOT_SUFFIX));

	// decomposing matrix
	AffineParts ap;
	AffineDecomp( tm, ap );

	LOG( OPTS->m_warningManager, ICWarning::NEGATIVE_SCALE, NULL, colladaNode, ap.k );

	// Thanks to KevT for his work on baked matrices
	if ( !OPTS->DecomposeMatrices() )
	{
		FCDTMatrix* matrix = (FCDTMatrix*) colladaPivot->AddTransform(FCDTransform::MATRIX);
		matrix->SetTransform(ToFMMatrix44(tm));
		
		// This can only be animated if we have a WSM object
		if ( node->GetWSMDerivedObject() != NULL )
		{
			ANIM->ExportAnimatedTransform(colladaPivot->GetDaeId() + "-transform", node, NULL, matrix->GetTransform(), true );
		}
	}
	else
	{
		ExportDecomposedWSMPivot( colladaPivot, node, NULL, ap );
	}
	return colladaPivot;
}

FCDSceneNode* NodeExporter::FindExportedNode(INode* node)
{
	ExportNodeMap::iterator it = exportNodes.find(node);
	return (it != exportNodes.end()) ? (*it).second->colladaNode : NULL;
}

void NodeExporter::ApplyPivotTM(INode* maxNode, Matrix3& tm)
{

	// When sampling matrices, we apply the sample the ObjTMAfterWSM
	// to get the final 
	//if (OPTS->BakeMatrices())
	//{
		//IDerivedObject* derivedObj = maxNode->GetWSMDerivedObject();
		//if ( derivedObj != NULL )
		//{
		//	// If we have WSM attached, always export a pivot
		//	TimeValue t = OPTS->AnimStart();
		//	tm = maxNode->GetObjTMAfterWSM(t) * Inverse(maxNode->GetNodeTM(t));
		//	return;
		//}
	//}

	Point3 opos = maxNode->GetObjOffsetPos();
	Quat orot = maxNode->GetObjOffsetRot();
	ScaleValue oscale = maxNode->GetObjOffsetScale();

	// this should already be in local space
	// only do this if necessary to preserve identity tags
	ApplyScaling(tm, oscale);
	RotateMatrix(tm, orot);
	tm.Translate(opos);

	tm.ValidateFlags();
}

const Matrix3& NodeExporter::GetWorldTransform(INode* node)
{
	NodeTransformMap::iterator itT = worldTransforms.find(node);
	if (itT != worldTransforms.end()) return (*itT).second;

	Matrix3 tm;
	TimeValue t = OPTS->AnimStart();
	//if (OPTS->IncludeObjOffset(node)) tm = node->GetObjTMAfterWSM(t);
	//else tm = node->GetNodeTM(t);
	tm = node->GetNodeTM(t);

	NodeTransformMap::iterator out = worldTransforms.insert(node, tm);
	return out->second;
}

const Matrix3& NodeExporter::GetLocalTransform(INode* node)
{
	NodeTransformMap::iterator itT = localTransforms.find(node);
	if (itT != localTransforms.end()) return (*itT).second;
	INode *parent = node->GetParentNode();
	// Use NodeTM to be compatible with absolute controllers (lookats etc).
	// bake matricies also include pivots & WSM, but only if not bones.
	Matrix3 tm = GetWorldTransform(node);

	if (parent != NULL && !parent->IsRootNode())
	{
		tm *= Inverse(GetWorldTransform(parent));
	}
	NodeTransformMap::iterator out = localTransforms.insert(node, tm);
	return out->second;
}

void NodeExporter::SetWorldNode( INode *pNode )
{
	worldNode = pNode;
}

INode *NodeExporter::GetWorldNode()
{
	return worldNode;
}

void NodeExporter::PostExportXRefs()
{
	for ( size_t nodeId = 0; nodeId < forcedHiddenXRefs.size(); ++nodeId )
	{
		forcedHiddenXRefs[nodeId]->Hide( false );
	}
}

void NodeExporter::AffineDecomp( const Matrix3 &tm, AffineParts &ap )
{
	decomp_affine( tm, &ap );
	ap.k *= ap.f; ap.f = 1.0f;
}

void NodeExporter::PreExportPreviewCamera()
{
	if ( document->GetCOLLADAExporter()->GetExportMode() != COLLADAExporter::EM_PREVIEW )
	{
		return;
	}

	// currently only support perspective preview camera
	// find the right viewport
	{
		// active viewport
		{
			ViewExp *pViewport = GetCOREInterface()->GetActiveViewport();
			if ( pViewport->IsPerspView() )
			{
				m_perspectiveViewportIndex = GetCOREInterface7()->getActiveViewportIndex();
			}
			GetCOREInterface()->ReleaseViewport( pViewport );
		}

		// camera viewports
		{
			int viewportCount = GetCOREInterface7()->getNumViewports();
			for ( int viewportId = 0; viewportId < viewportCount && m_perspectiveViewportIndex == -1; ++viewportId )
			{
				ViewExp *pViewport = GetCOREInterface7()->getViewport( viewportId );
				if ( pViewport->IsPerspView() 
					&& pViewport->GetViewType() == VIEW_CAMERA )
				{
					m_perspectiveViewportIndex = viewportId;
				}
				//GetCOREInterface()->ReleaseViewport( pViewport );
			}				
		}

		// TODO : spot viewports

		// perspective viewports
		{
			int viewportCount = GetCOREInterface7()->getNumViewports();
			for ( int viewportId = 0; viewportId < viewportCount && m_perspectiveViewportIndex == -1; ++viewportId )
			{
				ViewExp *pViewport = GetCOREInterface7()->getViewport( viewportId );
				if ( pViewport->IsPerspView() )
				{
					m_perspectiveViewportIndex = viewportId;
				}
				//GetCOREInterface()->ReleaseViewport( pViewport );
			}
		}
	}

	// insert preview camera in scene
	if ( m_perspectiveViewportIndex != -1 )
	{
		INode *pPreviewCameraNode = NULL;

		// camera viewport
		{
			ViewExp *pViewport = GetCOREInterface7()->getViewport( m_perspectiveViewportIndex );
			INode *pCameraNode = pViewport->GetViewCamera();
			if ( pCameraNode )
			{
				INodeTab nodesToClone;
				nodesToClone.AppendNode( pCameraNode );
				
				Point3 offset( 0.f, 0.f, 0.f );

				INodeTab cloneSrcs;
				INodeTab cloneDsts;

				BOOL returnCode = GetCOREInterface()->CloneNodes( nodesToClone, offset, false, NODE_COPY, &cloneSrcs, &cloneDsts );
				if ( returnCode )
				{
					for ( size_t nodeId = 0; nodeId < cloneDsts.Count(); ++nodeId )
					{
						ColladaMaxTypes::MaxType maxType = ColladaMaxTypes::Get( cloneDsts[nodeId], cloneDsts[nodeId]->GetObjectRef(), false );
						if ( maxType == ColladaMaxTypes::CAMERA )
						{
							FUAssert( !pPreviewCameraNode, );
							pPreviewCameraNode = cloneDsts[nodeId];
							pPreviewCameraNode->SetName( PREVIEW_CAMERA_NAME );

							if ( pPreviewCameraNode->IsHidden() )
							{
								pPreviewCameraNode->DeleteReference( NODE_LAYER_REF );
								pPreviewCameraNode->Hide( FALSE );
								pPreviewCameraNode->Freeze( FALSE );
							}
						}
						m_previewTemporaryNodes.push_back( cloneDsts[nodeId] );
					}
				}
			}
		}

		// create camera from current perspective viewport using max action item
		if ( !pPreviewCameraNode )
		{
			INodeTab previousCameraNodes;
			MAXHelpers::GetNodesByColladaMaxType( GetCOREInterface()->GetRootNode(), previousCameraNodes, ColladaMaxTypes::CAMERA );

			IActionManager *pActionManager = GetCOREInterface()->GetActionManager();
			ActionTable *pActionTable = pActionManager->FindTable( ACTION_TABLE_ID_MACRO_SCRIPTS );
			ActionItem *pActionItem = MAXHelpers::FindActionItemByDescriptionText( pActionTable, ACTION_ITEM_DESCRIPTION_CREATE_CAMERA_FROM_VIEW );
			ActionCallback *pCallback = pActionTable ? pActionTable->GetCallback() : NULL;
			BOOL retCode = pCallback && pActionItem ? pCallback->ExecuteAction( pActionItem->GetId() ) : FALSE;

			if ( retCode )
			{
				INodeTab currentCameraNodes;
				MAXHelpers::GetNodesByColladaMaxType( GetCOREInterface()->GetRootNode(), currentCameraNodes, ColladaMaxTypes::CAMERA );

				{
					int currentCameraCount = currentCameraNodes.Count();
					for ( int cameraId = 0; cameraId < currentCameraCount; ++cameraId )
					{
						if ( !previousCameraNodes.Contains( currentCameraNodes[cameraId] ) )
						{
							pPreviewCameraNode = currentCameraNodes[cameraId];
							break;
						}
					}
				}

				if ( pPreviewCameraNode )
				{
					pPreviewCameraNode->SetName( PREVIEW_CAMERA_NAME );
					m_previewTemporaryNodes.push_back( pPreviewCameraNode );

					INode *pTargetNode = pPreviewCameraNode->GetTarget();
					FUDbgAssert( pTargetNode, );
					if ( pTargetNode )
					{
						fm::string targetName = fm::string( PREVIEW_CAMERA_NAME ) + ".Target";
						pTargetNode->SetName( const_cast<char *>( targetName.c_str() ) );
						m_previewTemporaryNodes.push_back( pTargetNode );
					}
				}
			}
		}
			
		// if all else fails create a free cam from the perspective viewport
		if ( !pPreviewCameraNode )
		{
			GenCamera *pCamera = GetCOREInterface()->CreateCameraObject( FREE_CAMERA );
			pPreviewCameraNode = GetCOREInterface()->CreateObjectNode( pCamera );
			pPreviewCameraNode->SetName( PREVIEW_CAMERA_NAME );

			Matrix3 camTM;
			float fov;
			{
				ViewExp *pViewport = GetCOREInterface7()->getViewport( m_perspectiveViewportIndex );
				fov = pViewport->GetFOV();
				pViewport->GetAffineTM( camTM );
				//GetCOREInterface()->ReleaseViewport( pViewport );
			}	

			pCamera->SetOrtho( false );
			pCamera->SetClipDist( OPTS->AnimStart(), CAM_HITHER_CLIP, 1.f );
			pCamera->SetClipDist( OPTS->AnimStart(), CAM_YON_CLIP, 1000.f );
			pCamera->SetFOV( OPTS->AnimStart(), fov );

			// set target
			//{
			//	Object *pTarget = ( Object * )GetCOREInterface()->CreateTargetObject();//CreateInstance( GEOMOBJECT_CLASS_ID, Class_ID( TARGET_CLASS_ID, 0 ) );
			//	INode *pTargetNode = GetCOREInterface()->CreateObjectNode( pTarget );
			//	
			//	fm::string targetName = fm::string( PREVIEW_CAMERA_NAME ) + ".Target";
			//	pTargetNode->SetName( const_cast<char *>( targetName.c_str() ) );

			//	// set target position
			//	//{
			//	//	Matrix3 targetTM = camTM;
			//	//	targetTM.Invert();
			//	//	Point3 camFront = -camTM.GetRow( 2 );
			//	//}

			//	Control *pLookAtController = CreateLookatControl();
			//	pLookAtController->SetTarget( pTargetNode );
			//	//Control *pOldController = pTargetNode->GetTMController();
			//	pPreviewCameraNode->SetTMController( pLookAtController );
			//	//pCamera->UpdateTargDistance( OPTS->AnimStart(), pTargetNode );
			//	//pCamera->SetTDist( OPTS->AnimStart(), 100.f );
			//	//m_previewTemporaryNodes.push_back( pTarget );
			//}

			camTM.Invert();
			pPreviewCameraNode->SetNodeTM( OPTS->AnimStart(), camTM );
			m_previewTemporaryNodes.push_back( pPreviewCameraNode );
		}

		FUAssert( pPreviewCameraNode, return );
		m_previewCameraHandle = pPreviewCameraNode->GetHandle();

		// lights
		{
			INodeTab lightNodes;
			MAXHelpers::GetNodesByColladaMaxType( GetCOREInterface()->GetRootNode(), lightNodes, ColladaMaxTypes::LIGHT );

			if ( lightNodes.Count() < 1 )
			{
				GenLight *pLight = GetCOREInterface()->CreateLightObject( OMNI_LIGHT );
				INode *pLightNode = GetCOREInterface()->CreateObjectNode( pLight );
				pPreviewCameraNode->AttachChild( pLightNode, FALSE );
				m_previewTemporaryNodes.push_back( pLightNode );

				// set light default param
				{
					// Retrieve the parameter block
					IParamBlock *parameters1 = (IParamBlock*) pLight->GetReference( MaxLight::PBLOCK_REF );

					Color color( 1.0f, 1.0f, 1.0f );
					parameters1->SetValue( MaxLight::PB_COLOR, OPTS->AnimStart(), color );
					parameters1->SetValue( MaxLight::PB_DECAY, OPTS->AnimStart(), 0 );

					pLight->SetTDist( OPTS->AnimStart(), -1.f );

					pLight->SetDecayType( 0 );
					pLight->SetDecayRadius( OPTS->AnimStart(), 40.f );
					
					pLight->SetUseAttenNear( FALSE );
					pLight->SetAtten( OPTS->AnimStart(), ATTEN1_START, 0.f );
					pLight->SetAtten( OPTS->AnimStart(), ATTEN1_END, 40.f );
					
					pLight->SetUseAtten( FALSE );
					pLight->SetAtten( OPTS->AnimStart(), ATTEN_START, 80.f );
					pLight->SetAtten( OPTS->AnimStart(), ATTEN_END, 200.f );

					pLight->SetIntensity( OPTS->AnimStart(), 1.0f );
				}
			}
		}
	}
}

void NodeExporter::PostExportPreviewCamera()
{
	if ( document->GetCOLLADAExporter()->GetExportMode() == COLLADAExporter::EM_PREVIEW )
	{
		INode *pNode = GetCOREInterface()->GetINodeByHandle( m_previewCameraHandle );
		if ( pNode )
		{
			for ( ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it )
			{
				if ( pNode == it->first )
				{
					FCDSceneNode *pSceneNode = FAXICHelpers::GetInstanceParentNode( it->second->colladaNode );
					const fm::string daeId = pSceneNode->GetDaeId();
					document->GetCOLLADAExporter()->SetPreviewCameraIdentifier( daeId );
					break;
				}
			}
		}

		for ( NodeList::iterator nodeIter = m_previewTemporaryNodes.begin(); nodeIter != m_previewTemporaryNodes.end(); ++nodeIter )
		{
			GetCOREInterface()->DeleteNode( *nodeIter, FALSE );
		}
	}
}