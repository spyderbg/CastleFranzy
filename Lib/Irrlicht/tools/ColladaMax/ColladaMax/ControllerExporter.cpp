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
#include "AnimationExporter.h"
#include "ColladaModifiers.h"
#include "DocumentExporter.h"
#include "GeometryExporter.h"
#include "EntityExporter.h"
#include "NodeExporter.h"
#include "ColladaOptions.h"
#include "XRefFunctions.h"
#include "Common/ConversionFunctions.h"
#include "FCDocument/FCDAnimationCurve.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDController.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDEntityInstance.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometrySource.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDControllerInstance.h"
#include "FCDocument/FCDMorphController.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDSkinController.h"
#include "FCDocument/FCDControllerTools.h"
#include "ControllerExporter.h"
#include "IMorpher/IMorpher.h"
#include <modstack.h>
#include "IInstanceMgr.h"
#include "CS/BipedAPI.h"
#include "CS/Tracks.h"
#include "XRefIncludes.h"

ControllerExporter::ControllerExporter(DocumentExporter* _document)
:	BaseExporter(_document)
,	boneIndex(0)
{
	modifiers.reserve(3);
}

ControllerExporter::~ControllerExporter()
{
	// Delete all the controller information structures.
	size_t controllerCount = CDOC->GetControllerLibrary()->GetEntityCount();
	for (size_t i = 0; i < controllerCount; ++i)
	{
		FCDController* controller = CDOC->GetControllerLibrary()->GetEntity(i);
		ControllerInfo* info = (ControllerInfo*) controller->GetUserHandle();
		SAFE_DELETE(info);
		controller->SetUserHandle(NULL);
	}
}

void ControllerExporter::CalculateForcedNodes(INode* pNode, Object* object, NodeList& forcedNodes)
{
	// Look for possible skin and morpher modifiers
	ColladaModifierManager::ResolveModifiers(pNode, object);

	for (ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end(); ++itM)
	{
		ColladaModifier* mod = (*itM);
        if (mod->GetClassID() == COLLADA_SKIN_CLASS_ID)
		{
			// Here we can call GetInterface without the node, as
			// we access no node specific data.

			// Attempt to locate a (any) node referencing this object.
			// We do not need any instance specific data, but physique
			// requires a node handle even for operations that only
			// access bone information.
			ULONG handle;
			mod->GetModifier()->NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
			INode* node = GetCOREInterface()->GetINodeByHandle(handle);
			assert(node != NULL);

			ColladaSkinInterface* iskin = ((ColladaSkin*)mod)->GetInterface(node);
			int boneCount = iskin->GetNumBones();
			for (int i = 0; i < boneCount; ++i)
			{
				INode* boneNode = iskin->GetBone(i);
				if (boneNode != NULL) forcedNodes.push_back(boneNode);
			}

			iskin->ReleaseMe();
		}
	}
}

FCDEntity* ControllerExporter::ExportController(INode* node, Object* object)
{
	if (object == NULL || node == NULL) 
	{
		return NULL;
	}

	// current object modifiers resolution
	ColladaModifierManager::ResolveModifiers(node, object);

	// Retrieve the initial mesh
	Object* initialMesh = GET_INITIAL_MESH( modifiers, object );

	// FCollada doesn't support external references for controller sources
	// For the moment we dereference the XRef if any
	// TODO: Change as soon as FCollada upgrades
	initialMesh = XRefFunctions::GetXRefObjectSource(initialMesh);


	// Set initial mesh export flags
	int exportFlags = GeometryExporter::EXPORT_DEFAULTS;
	if ( !modifiers.empty() )
	{
		SET_FLAG( exportFlags, GeometryExporter::EXPORT_AFFECTS_CONTROLLERS );
	}

	// Export the initial mesh.
	FCDGeometry* geometry = (FCDGeometry*) document->GetGeometryExporter()->ExportMesh(node, initialMesh, exportFlags);
	
	FCDEntity* entity = geometry;
	for (ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end() && entity != NULL; ++itM)
	{
		ColladaModifier* modifier = (*itM);
		FCDController* colladaController = new FCDController( document->GetCOLLADADocument() );
		fm::string suggId;
		bool bExportController = true;
		if (modifier->GetClassID() == COLLADA_SKIN_CLASS_ID)
		{
			// Create an empty skin controller
			ColladaSkin* skin = (ColladaSkin*) modifier;
			
			ColladaSkinInterface* iskin = skin->GetInterface( node );
			size_t jointCount = iskin->GetNumBones();
			bExportController = jointCount > 0;

			if ( bExportController )
			{
				colladaController->SetUserHandle(new ControllerInfo(node, skin->GetModifier()));
				FCDSkinController* colladaSkin = colladaController->CreateSkinController();
				colladaSkin->SetTarget(entity);
				suggId = entity->GetDaeId() + "-skin";
			}
			else
			{
				LOG( OPTS->m_warningManager, ICWarning::INVALID_SKIN, NULL, node );
			}

			iskin->ReleaseMe();
		}

		else if (modifier->GetClassID() == COLLADA_MORPHER_CLASS_ID)
		{
			// Create an empty morph controller
			ColladaMorph* morph = (ColladaMorph*) modifier;
			colladaController->SetUserHandle(new ControllerInfo(node, morph->GetModifier()));
			FCDMorphController* morpher = colladaController->CreateMorphController();
			morpher->SetBaseTarget(entity);
			suggId = entity->GetDaeId() + "-morpher";
		}
		else FUFail(continue); // Cannot be here.

		if ( bExportController )
		{
			CDOC->GetControllerLibrary()->AddEntity( colladaController );
			ENTE->ExportEntity(modifier->GetModifier(), colladaController, suggId.c_str());
			entity = colladaController;
		}
		else
		{
			SAFE_RELEASE( colladaController );
		}
	}

	return entity;
}

void ControllerExporter::LinkControllers()
{
	fm::pvector<FCDEntityInstance> gatherList;
	fm::pvector<FCDSceneNode> nodeQueue;
	FCDVisualSceneNodeLibrary* library = CDOC->GetVisualSceneLibrary();
	for (size_t i = 0; i < library->GetEntityCount(); i++)
	{
		nodeQueue.push_back(library->GetEntity(i));
	}

	while (!nodeQueue.empty())
	{
		FCDSceneNode* sceneNode = nodeQueue.back();
		nodeQueue.pop_back();

		// Link children first, as I think we depend on them...
		for (size_t c = 0; c < sceneNode->GetChildrenCount(); c++)
		{
			FCDSceneNode* child = sceneNode->GetChild(c);
			nodeQueue.push_back(child);
		}

		// Now hook up our exports.
		size_t count = sceneNode->GetInstanceCount();
		for (size_t e = 0; e < count; e++)
		{
			FCDEntityInstance* instance = sceneNode->GetInstance(e);
			if (instance && instance->HasType(FCDControllerInstance::GetClassType()))
			{
				gatherList.push_back(instance);
			}
		}
	}

	// Iterate over all the FCollada controllers and fill them in.
	size_t controllerCount = CDOC->GetControllerLibrary()->GetEntityCount();
	for (size_t i = 0; i < controllerCount; ++i)
	{
		FCDController* controller = CDOC->GetControllerLibrary()->GetEntity(i);
		ControllerInfo* info = (ControllerInfo*) controller->GetUserHandle();
		if (info != NULL)
		{
			INode*  inode  = info->node;
			for (FCDEntityInstance** itr = gatherList.begin(); itr != gatherList.end(); itr++)
			{
				if ((*itr)->GetEntity() == controller)
				{
					LinkController(controller, inode, (FCDControllerInstance*)*itr);
				}
			}
		}
	}
}

bool ControllerExporter::LinkController(FCDController* controller, INode* inode, FCDControllerInstance* instance)
{
	FUAssert(controller != NULL, return false);

	if (controller->IsSkin())
		{
		ColladaSkin modifier;
		modifier.Resolve(inode, inode->GetObjectRef());
		LinkSkin(controller->GetSkinController(), instance, inode, &modifier);
	}
	else if (controller->IsMorph())
	{
		ColladaMorph modifier;
		modifier.Resolve(inode, inode->GetObjectRef());
		LinkMorph(controller->GetMorphController(), instance, inode, &modifier);
	}
	else return false;
	return true;
}

void ControllerExporter::ComputeJointBoundingBoxes( FCDSkinController *pSkinController, FCDGeometryMesh *pMesh )
{
	FCDGeometrySource *pGeometrySource = pMesh->GetPositionSource();
	if ( !pGeometrySource )
	{
		return;
	}

	// associate joints with the vertices they influence
	BoneVertexList boneVertexList;
	size_t numInfluences = pSkinController->GetInfluenceCount();
	for ( size_t influenceId = 0; influenceId < numInfluences; ++influenceId )
	{
		FCDSkinControllerVertex *pInfluence = pSkinController->GetVertexInfluence( influenceId );
		size_t numPairs = pInfluence->GetPairCount();
		for ( size_t pairId = 0; pairId < numPairs; ++pairId )
		{
			FCDJointWeightPair *pPair = pInfluence->GetPair( pairId );
			int jointIndex = pPair->jointIndex;
			
			BoneVertexList::iterator boneVertexListIter = boneVertexList.find( jointIndex );
			if ( boneVertexListIter == boneVertexList.end() )
			{
				UInt32List vertexIndices( 1, influenceId ); // influenceId == vertexId
				boneVertexList.insert( jointIndex, vertexIndices );
			}
			else
			{
				boneVertexListIter->second.push_back( influenceId );
			}
		}
	}

#define UPDATE_EXTREMUMS( extremumArray, potentialExtremumAray, stride, __comparisonOperator__ ) \
{ \
	for ( int elementId = 0; elementId < (stride); ++elementId ) \
	{ \
		float potentialMin = (potentialExtremumAray)[elementId]; \
		if ( (extremumArray)[elementId] __comparisonOperator__ potentialMin ) \
		{ \
			(extremumArray)[elementId] = potentialMin; \
		} \
	} \
}

	// compute bounding box for each joint
	int sourceStride = pGeometrySource->GetStride();
	const FMMatrix44 &bindShapeMatrix = pSkinController->GetBindShapeTransform();
	int jointCount = pSkinController->GetJointCount();
	for ( size_t jointId = 0; jointId < jointCount; ++jointId )
	{
		FCDSkinControllerJoint *pJoint = pSkinController->GetJoint( jointId );
		FMVector3 min( FLT_MAX - 1, FLT_MAX - 1, FLT_MAX - 1 );
		FMVector3 max( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		BoneVertexList::iterator boneVertexListIter = boneVertexList.find( jointId );
		if ( boneVertexListIter != boneVertexList.end() )
		{
			const FMMatrix44 &inverseBindMatrix = pJoint->GetBindPoseInverse();
			const FMMatrix44 &localToJointMatrix = inverseBindMatrix * bindShapeMatrix;
			const UInt32List &vertexIndexList = boneVertexListIter->second;
			for ( size_t vertexIndexId = 0; vertexIndexId < vertexIndexList.size(); ++vertexIndexId )
			{
				uint32 vertexIndex = vertexIndexList[vertexIndexId];
				const float *pVertexPos = pGeometrySource->GetValue( vertexIndex );
				const FMVector4 &vertexInJointSpace = localToJointMatrix * FMVector4( FMVector3( pVertexPos, 0 ), 1.f );

				UPDATE_EXTREMUMS( min, vertexInJointSpace, sourceStride, > );
				UPDATE_EXTREMUMS( max, vertexInJointSpace, sourceStride, < );
			}		
		}

		pJoint->SetBoundingBoxMinimum( min );
		pJoint->SetBoundingBoxMaximum( max );
	}

#undef UPDATE_EXTREMUMS
}

void ControllerExporter::LinkSkin(FCDSkinController* colladaSkin,
								  FCDControllerInstance* instance, 
								  INode* node, 
								  ColladaSkin* modifier)
{
	if (!modifier->IsResolved() || colladaSkin == NULL || instance == NULL || node == NULL)
		return;   // only if it has a skin modifier

	// Morphs only link with an instance! 
	FCDEntity* baseTarget = colladaSkin->GetTarget();
	if (baseTarget != NULL && baseTarget->GetType() == FCDEntity::CONTROLLER)
	{
		LinkController((FCDController*)baseTarget, node, instance);
	}

	// Calculate the initial skin local transformation for the first instance.
	// Because everything is uncertain, bake in also the first instance's current pivot
	Matrix3 bindShapeTM;
	ColladaSkinInterface* iskin = modifier->GetInterface(node);
	iskin->GetSkinInitTM(bindShapeTM, true);

	// Fill in the joint information: the bone scene node and its inverse bind-pose transforms
	INodeTab controllerInstanceNodes;
	IInstanceMgr::GetInstanceMgr()->GetInstances(*node, controllerInstanceNodes);

	// calls ISkin::GetNumBones, not GetNumBonesFlat.
	// I don't know why Max exposes a bone list containing NULL entries
	// via GetBoneFlat and GetNumBonesFlat functions. Maybe the indexes in the
	// "flat" list are used somewhere else... -Etienne.
	size_t jointCount = iskin->GetNumBones();
	size_t colladaJointCount = colladaSkin->GetJointCount();
	bool newEntity = (colladaJointCount != jointCount);
	// TODO : Get rid of this map
	fm::map<int, int>boneIndexRemap;

	// reserve the maximum bone count
	colladaSkin->SetBindShapeTransform(ToFMMatrix44(bindShapeTM));
	if (newEntity) colladaSkin->SetJointCount(0);

	const float k_weightTolerance = FLT_TOLERANCE;

	// Excluding unused joints from skin controller data
	BoneWeightsList boneWeightsList;
	if ( OPTS->RemoveUnusedSkinJoints() )
	{
		int numVertices = iskin->GetNumVertices();
		for ( int vertexId = 0; vertexId < numVertices; ++vertexId )
		{
			int numBones = iskin->GetNumAssignedBones( vertexId );
			for ( int boneId = 0; boneId < numBones; ++boneId )
			{
				float weight = iskin->GetBoneWeight( vertexId, boneId );
				int jointIndex = iskin->GetAssignedBone( vertexId, boneId );
				BoneWeightsList::iterator boneWeightsListIter = boneWeightsList.find( jointIndex );
				if ( boneWeightsListIter == boneWeightsList.end() )
				{
					fm::vector<float> weights( 1, weight );
					boneWeightsList.insert( jointIndex, weights );
				}
				else
				{
					boneWeightsListIter->second.push_back( weight );
				}
			}
		}
	}

	// for each entry
	int finalJointIndex = 0;
	fm::map<int,int> jointIndexTranslationMap;
	for (int i = 0; i < (int) jointCount; ++i)
	{
		// Skipping unused joints
		if ( OPTS->RemoveUnusedSkinJoints() )
		{
			// tests if joint has any influence
			BoneWeightsList::iterator boneWeightsListIter = boneWeightsList.find( i );
			if ( boneWeightsList.find( i ) == boneWeightsList.end() )
			{
				continue;
			}

			if ( OPTS->RemoveZeroWeights() )
			{
				// tests if joint has only NULL influences
				bool bHasNonNullWeights = false;
				
				const fm::vector<float> &weights = boneWeightsListIter->second;
				for ( size_t weightId = 0; weightId < weights.size(); ++weightId )
				{
					if ( !IsEquivalent( weights[weightId], 0.f, k_weightTolerance ) )
					{
						bHasNonNullWeights = true;
						break;
					}
				}

				if ( !bHasNonNullWeights )
				{
					continue;
				}
			}
		}

		// there should not be any null bone.
		// the ISkin::GetBone, not GetBoneFlat, function is called here.
		INode* boneNode = iskin->GetBone(i);
		FUDbgAssert( boneNode, );
		if (boneNode == NULL)
		{
			continue;
		}

		FCDSceneNode* sceneNode = NULL;
		sceneNode = document->GetNodeExporter()->FindExportedNode(boneNode);
		// if the boneNode is valid, then we should have it in the document.
		FUAssert(sceneNode != NULL, continue);

		jointIndexTranslationMap[i] = finalJointIndex++;

		int idx = (int) instance->FindJointIndex(sceneNode);
		if (idx >= 0) boneIndexRemap.insert(i, idx);
		else
		{
			boneIndexRemap.insert(i, (int)instance->GetJointCount());
			instance->AddJoint(sceneNode);
			if (newEntity)
			{
				// Calculate the bind-pose transformation.
				// FCollada will do the inversion.
				Matrix3 bindPose;
				if (!iskin->GetBoneInitTM(boneNode, bindPose))
				{
					bindPose = NDEX->GetWorldTransform(boneNode);
				}
	
				// We NEED a sub Id, and we can assume that Bone(i) is unique
				if (sceneNode->GetSubId().empty()) sceneNode->SetSubId("Bone" + TO_STRING(++boneIndex));

				bindPose.Invert();
				colladaSkin->AddJoint(sceneNode->GetSubId(), ToFMMatrix44(bindPose));
			}
		}
	}

	// Generate the entity instance skeleton roots.
	instance->CalculateRootIds();

	FCDEntity::Type targetType = baseTarget->GetType();
	FCDEntity *target = baseTarget;
	while ( target != NULL && target->GetType() == FCDEntity::CONTROLLER )
	{
		FCDController *controller = ( FCDController * )target;
		target = controller->GetBaseTarget();
	}

	// Fill in the vertex influence information.
	// There should always be as many influences as there are vertices.
	if ( newEntity && target != NULL && target->GetType() == FCDEntity::GEOMETRY )
	{
		FCDGeometry *geometry = ( FCDGeometry * )target;
		FCDGeometryMesh *geometryMesh = geometry->GetMesh();
		FCDGeometrySource *vertexPosSource = NULL;
		int sourceCount = geometryMesh->GetSourceCount();
		for ( int i = 0; i < sourceCount; i++ )
		{
			FCDGeometrySource *tmpSource = geometryMesh->GetSource( i );
			if ( tmpSource->GetType() == FUDaeGeometryInput::POSITION )
			{
				vertexPosSource = tmpSource;
				break;
			}
		}

		if ( vertexPosSource )
		{
			const VertexHashMap &hashMap = vertexPosSource->GetHashMapConstRef();

			colladaSkin->SetInfluenceCount( vertexPosSource->GetHashedVertexCount() );
			int newIndex = 0;
			for ( VertexHashMap::const_iterator it = hashMap.begin(); it != hashMap.end(); ++it )
			{
				for ( fm::vector<fm::vector<VertexData>>::const_iterator bucketIter = it->second.begin(); bucketIter != it->second.end(); ++bucketIter )
				{
					int oldIndex = bucketIter->begin()->oldIndex;
					FCDSkinControllerVertex* vertex = colladaSkin->GetVertexInfluence(newIndex);
					int pairCount = iskin->GetNumAssignedBones(oldIndex);
					vertex->SetPairCount(pairCount);
					for (int p = 0; p < pairCount; ++p)
					{
						FCDJointWeightPair* pair = vertex->GetPair(p);
						pair->weight = iskin->GetBoneWeight(oldIndex, p);
						pair->jointIndex = jointIndexTranslationMap[iskin->GetAssignedBone(oldIndex, p)];
					}
					newIndex++;
				}
			}

			FUDbgAssert( newIndex == vertexPosSource->GetHashedVertexCount(), );
		}
	}

	if ( OPTS->RemoveZeroWeights() )
	{
		colladaSkin->ReduceInfluences( -1, k_weightTolerance );
	}

	if ( OPTS->DeindexVertexData() )
	{
		if ( target != NULL && target->GetType() == FCDEntity::GEOMETRY )
		{
			FCDGeometry *geometry = ( FCDGeometry * )target;
			FCDControllerTools::ApplyTranslationMap( colladaSkin, geometry->translationMap );
		}
	}

	// Compute joint bounding boxes
	if ( target != NULL && target->GetType() == FCDEntity::GEOMETRY )
	{
		FCDGeometry *geometry = ( FCDGeometry * )target;
		if ( geometry->IsMesh() )
		{
			FCDGeometryMesh *pMesh = geometry->GetMesh();
			ComputeJointBoundingBoxes( colladaSkin, pMesh );
		}
	}

	// Dont forget to release ourselves
	iskin->ReleaseMe();
}

void ControllerExporter::LinkMorph(FCDMorphController* colladaMorph, 
								   FCDControllerInstance* instance, 
								   INode* node, 
								   ColladaMorph* modifier)
{
	if (!modifier->IsResolved() || colladaMorph == NULL || node == NULL)
		return;   // only if it has a skin modifier

	// Link our base targets if its a skin
	FCDEntity* baseTarget = colladaMorph->GetBaseTarget();
	if (baseTarget != NULL && baseTarget->GetType() == FCDEntity::CONTROLLER)
	{
		LinkController((FCDController*)baseTarget, node, instance);
	}

	// Prepare the FCollada structure to hold the morph targets
	MorphR3* imorph = modifier->GetInterface();
	size_t targetCount = imorph->chanBank.size();
	colladaMorph->SetMethod(FUDaeMorphMethod::NORMALIZED);

	// Setting export flags for morph targets
	int exportFlags = GeometryExporter::EXPORT_DEFAULTS;

	SET_MORPH_GEOMETRY_EXPORTFLAGS( exportFlags );

	// Export all the morph targets
	for (size_t i = 0; i < targetCount; ++i)
	{
		morphChannel& channel = imorph->chanBank[i];
		if (!channel.mActive || channel.mNumPoints == 0) continue;

		// the target mesh geometry for the morpher
		FCDGeometry* targetMesh = NULL;
		INode* targetMaxNode = channel.mConnection;
		if (targetMaxNode == NULL)
		{
			int testId = MISSING_TARGET_TEST;
			LOG( OPTS->m_warningManager, ICWarning::INCOMPATIBLE_MORPH_TARGET, &testId, node, &channel );

			// create the new geometry based on the original geometry
			targetMesh = (FCDGeometry*)document->GetGeometryExporter()->ExportMesh(node, node->GetObjectRef(),exportFlags);
			if (targetMesh == NULL) continue;

			targetMesh->SetDaeId(colladaMorph->GetParent()->GetDaeId() + "-" + (const char*) channel.mName + "-" + TO_STRING(i));

			FCDGeometryMesh *pGeometryMesh = targetMesh->GetMesh();
			FUAssert( pGeometryMesh, continue );
			FCDGeometrySource *vertexPosSource = pGeometryMesh->GetPositionSource();

			// use original geometry vertex hash map to get the right morph offsets
			const VertexHashMap &hashMap = vertexPosSource->GetHashMapConstRef();

			// the object instance has been deleted, BUT we still have the mesh information in
			// the morpher modifier and its morph channels.
			FloatList *targetVertices;
			FloatList hashedVertices;
			FloatList deindexedVertices;

			int posStride = 3;
			hashedVertices.resize(posStride * vertexPosSource->GetHashedVertexCount());
			FloatList::iterator listIt = hashedVertices.begin();

			for ( VertexHashMap::const_iterator it = hashMap.begin(); it != hashMap.end(); ++it )
			{
				for ( fm::vector<fm::vector<VertexData>>::const_iterator bucketIter = it->second.begin(); bucketIter != it->second.end(); ++bucketIter )
				{
					int oldIndex = bucketIter->begin()->oldIndex;

					// each point represents an OFFSET to the original mesh point
					const Point3& pt = channel.mPoints[oldIndex];
					(*listIt++) = pt.x;
					(*listIt++) = pt.y;
					(*listIt++) = pt.z;
				}
			}

			targetVertices = &hashedVertices;

			// apply original geometry deindexation translation map on hashed vertices
			const FCDGeometryIndexTranslationMap &translationMap = targetMesh->translationMap;
			int numDeindexedVertices = FCDControllerTools::ComputeTranslationMapNumDeindexedVertices( translationMap );
			if ( OPTS->DeindexVertexData() && numDeindexedVertices > 0 )
			{
				deindexedVertices.resize(posStride * numDeindexedVertices);

				// Now iterate over all the vertices that we have used and come up with something
				int oldIndexCounter = 0;
				for ( FCDGeometryIndexTranslationMap::const_iterator it = translationMap.begin(); it != translationMap.end(); ++it )
				{
					UInt32List uniqueIndices = it->second;
					int oldVertexIndex = oldIndexCounter * posStride;
					for (UInt32List::iterator uItr = uniqueIndices.begin(); uItr != uniqueIndices.end(); ++uItr)
					{
						int newVertexIndex = (*uItr) * posStride;
						for ( int el = 0; el < posStride; ++el )
						{
							deindexedVertices[newVertexIndex + el] = hashedVertices[oldVertexIndex + el];
						}
					}

					oldIndexCounter++;
				}	

				targetVertices = &deindexedVertices;
			}

			FCDGeometrySource* colladaPositions = targetMesh->GetMesh()->GetPositionSource();
			colladaPositions->SetData(*targetVertices, 3);
		}
		else
		{
			FCDSceneNode* colladaTargetNode = document->GetNodeExporter()->FindExportedNode(targetMaxNode);
			if (colladaTargetNode != NULL)
			{
				if (colladaTargetNode->GetInstanceCount() == 0)
				{
					// we have a target node, but it wasn't instanced (maybe hidden)
					// export that geometry, we need it.
					targetMesh = (FCDGeometry*) document->GetGeometryExporter()->ExportMesh(targetMaxNode, targetMaxNode->GetObjectRef(),exportFlags);
				}

				// Retrieve the mesh instance for the target scene node.
				// NOTE: 3dsMax supports only one instance per target scene node.
				for (size_t t = 0; targetMesh == NULL && t < colladaTargetNode->GetInstanceCount(); ++t)
				{
					FCDEntityInstance* instance = colladaTargetNode->GetInstance(t);
					FCDEntity* entity = instance->GetEntity();
					if (entity->GetObjectType().Includes(FCDController::GetClassType()))
					{
						FCDController* c = (FCDController*) entity;
						targetMesh = c->GetBaseGeometry();
					}
					else if (entity->GetObjectType().Includes(FCDGeometry::GetClassType()))
					{
						targetMesh = (FCDGeometry*) entity;
					}
				}

				// You also need to look at/for pivots.
				for (size_t p = 0; targetMesh == NULL && p < colladaTargetNode->GetChildrenCount(); ++p)
				{
					FCDSceneNode* possiblePivotNode = colladaTargetNode->GetChild(p);
					for (size_t t = 0; targetMesh == NULL && t < possiblePivotNode->GetInstanceCount(); ++t)
					{
						FCDEntityInstance* instance = possiblePivotNode->GetInstance(t);
						FCDEntity* entity = instance->GetEntity();
						if (entity->GetObjectType().Includes(FCDController::GetClassType()))
						{
							FCDController* c = (FCDController*) entity;
							targetMesh = c->GetBaseGeometry();
						}
						else if (entity->GetObjectType().Includes(FCDGeometry::GetClassType()))
						{
							targetMesh = (FCDGeometry*) entity;
						}
					}
				}
			}
			else
			{
				// we have a target node, but it wasn't exported at all (maybe hidden)
				// export that geometry, we need it.
				targetMesh = (FCDGeometry*) document->GetGeometryExporter()->ExportMesh(targetMaxNode, targetMaxNode->GetObjectRef(),exportFlags);
			}
		}

		// now export the animations from the mesh
		if (targetMesh != NULL)
		{
			// Fill up the FCollada morph target structure
			FCDMorphTarget* colladaTarget = colladaMorph->AddTarget(targetMesh);
			if (colladaTarget != NULL)
			{
				Control* weightController = channel.cblock->GetController(morphChannel::cblock_weight_index);
				float weight = channel.cblock->GetFloat(morphChannel::cblock_weight_index, TIME_EXPORT_START);
				colladaTarget->SetWeight(weight * 0.01f);
				ANIM->ExportAnimatedFloat(colladaMorph->GetParent()->GetDaeId() + "-weights", weightController, colladaTarget->GetWeight(), (int) i, &FSConvert::FromPercent, false, AnimationExporter::APA_ANIM_TARGET_MORPH );
			}

			int testId = TARGET_DIFFERENCES_TEST;
			LOG( OPTS->m_warningManager, ICWarning::INCOMPATIBLE_MORPH_TARGET, &testId, colladaMorph->GetBaseTarget(), targetMesh, colladaTarget );
		}
	}
}

void ControllerExporter::ResolveSkinAndMorphModifiers( INode *pNode, Object* obj, ColladaModifierStack &modifiers )
{
	// TODO: Obviously, we need to resolve modifiers in a smarter fashion...
	ColladaSkin skin; skin.Resolve(pNode, obj);
	ColladaMorph morph; morph.Resolve(pNode, obj);
		
	if (skin.IsResolved() && morph.IsResolved())
	{
		// Remember that 3dsMax has the mod stack reversed in its internal structures.
		if (skin.GetModifierStackIndex() > morph.GetModifierStackIndex())
		{
			modifiers.push_back(new ColladaSkin(skin));
			modifiers.push_back(new ColladaMorph(morph));
		}
		else
		{
			modifiers.push_back(new ColladaMorph(morph));
			modifiers.push_back(new ColladaSkin(skin));
		}
	}
	else if (morph.IsResolved()) modifiers.push_back(new ColladaMorph(morph));
	else if (skin.IsResolved()) modifiers.push_back(new ColladaSkin(skin));
}

void ControllerExporter::ResolveModifiers(INode *pNode, Object* obj, ColladaModifierStack &modifiers )
{
	ResolveSkinAndMorphModifiers( pNode, obj, modifiers );
}

bool ControllerExporter::IsSkinned(Object* obj)
{
	ColladaSkin s;
	s.Resolve(NULL, obj);
	return s.IsResolved();
}

bool ControllerExporter::DoOverrideXRefExport(Object* obj)
{
	ColladaSkin s; ColladaMorph m;
	s.Resolve(NULL, obj); m.Resolve(NULL, obj);
	return (s.IsResolved() || m.IsResolved());
}

void ControllerExporter::ReactivateSkins()
{
	// Iterate over all the FCollada controllers and fill them in.
	size_t controllerCount = CDOC->GetControllerLibrary()->GetEntityCount();
	for (size_t i = 0; i < controllerCount; ++i)
	{
		FCDController* controller = CDOC->GetControllerLibrary()->GetEntity(i);
		ControllerInfo* info = (ControllerInfo*) controller->GetUserHandle();
		if (info != NULL)
		{
			info->modifier->EnableModInViews();
			info->modifier->EnableModInRender();
			info->modifier->EnableMod();
		}
	}
}

bool ControllerExporter::IsValidAnimationController( Animatable *pController )
{
	if ( !pController )
	{
		return false;
	}
	
	IKeyControl *pKeyControl = GetKeyControlInterface( pController );
	if ( pKeyControl 
		&& pKeyControl->GetNumKeys() > 0 )
	{
		return true;
	}

	if ( FindConstraint( pController ) )
	{
		return true;
	}

	int nSubs = pController->NumSubs();
	for (int i = 0; i < nSubs; i++)
	{
		Animatable* sub = pController->SubAnim(i);
		if ( IsValidAnimationController( sub ) )
		{
			return true;
		}
	}

	return false;
}

bool ControllerExporter::IsListController( Animatable *a )
{
	if ( !a )
	{
		return false;
	}

	Class_ID classID = a->ClassID();
	return classID == Class_ID( FLOATLIST_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( POINT3LIST_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( POSLIST_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( ROTLIST_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( SCALELIST_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( DUMMY_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( MASTERLIST_CONTROL_CLASS_ID, 0 )
		|| classID == Class_ID( POINT4LIST_CONTROL_CLASS_ID, 0 );
}

bool ControllerExporter::IsBipedController( Animatable *a )
{
	if ( !a )
	{
		return false;
	}

	Class_ID classID = a->ClassID();
	return classID == BIPSLAVE_CONTROL_CLASS_ID
		|| classID == BIPBODY_CONTROL_CLASS_ID
		|| classID == FOOTPRINT_CLASS_ID
		|| classID == BIPED_CLASS_ID // master controller		
		;
}

int ControllerExporter::GetNumControllerKeys( Animatable *pControl, IKeyControl *&pKeyControl )
{
	pKeyControl = pControl ? GetKeyControlInterface( pControl ) : NULL;
	return pKeyControl ? pKeyControl->GetNumKeys() : NULL;
}

// Function searches ctrl and all subanims for presence of a Constraint.
// This is bit of a cheap fix, a slightly better option may be to include
// parent offset calculation in rotation/position sampling (like Matrix sampling).
bool ControllerExporter::FindConstraint(Animatable* ctrl)
{
	if (ctrl->SuperClassID() == CTRL_FLOAT_CLASS_ID) return false;
	Class_ID clid = ctrl->ClassID();
	if (clid.PartB() == 0)
	{
		unsigned long pa = clid.PartA();
		if ( pa == POSITION_CONSTRAINT_CLASS_ID
			|| pa == ORIENTATION_CONSTRAINT_CLASS_ID
			|| pa == LOOKAT_CONSTRAINT_CLASS_ID 
			|| pa == FLOATNOISE_CONTROL_CLASS_ID 
			|| pa == POSITIONNOISE_CONTROL_CLASS_ID 
			|| pa == POINT3NOISE_CONTROL_CLASS_ID 
			|| pa == ROTATIONNOISE_CONTROL_CLASS_ID 
			|| pa == SCALENOISE_CONTROL_CLASS_ID 
			)
		{
			return true;
		}
	}

	int nSubs = ctrl->NumSubs();
	for (int i = 0; i < nSubs; i++)
	{
		Animatable* sub = ctrl->SubAnim(i);
		if (sub != NULL)
		{
			if (FindConstraint(sub)) return true;
		}
	}
	return false;
}

#define INDENT( stringBuilder, indent ) \
{ \
	for ( int __i = 0; __i < indent; ++__i ) \
	{ \
		stringBuilder.append( '\t' ); \
	} \
}

#define APPEND_STRBUILDER_INDENTED( stringBuilder, appendee, indent ) \
{ \
	INDENT( stringBuilder, indent ); \
	stringBuilder.append( appendee ); \
}

#define APPEND_INDENTED( appendee ) APPEND_STRBUILDER_INDENTED( g_warningLog, appendee, indent )
#define APPEND( appendee ) g_warningLog.append( appendee )
#define LINE_FEED APPEND('\n')

#define APPEND_CLASSID( classId ) \
{ \
	APPEND( " ID " ); \
	APPEND( "( " ); \
	APPEND( (classId).PartA() ); \
	APPEND( " , " ); \
	APPEND( (classId).PartB() ); \
	APPEND( " ) " ); \
}

extern FUStringBuilder g_warningLog;

void ControllerExporter::DBG_PrintControllerInfo( Control *controller, int indent )
{
#ifdef _DEBUG
	IKeyControl *keyInterface = controller ? GetKeyControlInterface(controller) : NULL;
	int numKeys = keyInterface ? keyInterface->GetNumKeys() : 0;
	if ( controller )
	{
		APPEND_CLASSID( controller->ClassID() );
		if ( keyInterface )
		{
			APPEND( " => " );
			APPEND_INDENTED( "NumKeys : " );
			APPEND( numKeys );
		}
	}
#endif //_DEBUG
}

void ControllerExporter::DBG_PrintSubControllerInfo( Control *controller, int indent )
{
#ifdef _DEBUG
	if ( !controller )
	{
		return;
	}

	DBG_PrintControllerInfo( controller );
	LINE_FEED;

	++indent;

	APPEND_INDENTED( "X Controller => " );
	DBG_PrintControllerInfo( controller->GetXController(), indent );
	LINE_FEED;

	APPEND_INDENTED( "Y Controller => " );
	DBG_PrintControllerInfo( controller->GetYController(), indent );
	LINE_FEED;

	APPEND_INDENTED( "Z Controller => " );
	DBG_PrintControllerInfo( controller->GetZController(), indent );
	LINE_FEED;

	APPEND_INDENTED( "W Controller => " );
	DBG_PrintControllerInfo( controller->GetWController(), indent );
	LINE_FEED;
#endif //_DEBUG
}

void ControllerExporter::DBG_PrintBipedControllerInfo( Control *controller, int indent )
{
#ifdef _DEBUG
	if ( !controller )
	{
		return;
	}

	Tab<TimeValue> keyTimes;
	Interval range( INT_MIN, INT_MAX );
	int numOutOfRangeKeys = 0;

	numOutOfRangeKeys = controller->GetKeyTimes( keyTimes, range, 0 );
	APPEND_CLASSID( controller->ClassID() );
	APPEND( " => " );
	APPEND( "NumKeys : " );
	APPEND( keyTimes.Count() );
#endif //_DEBUG
}

void ControllerExporter::DBG_PrintCompoundControllerInfo( INode *node, Control *controller, int indent )
{
#ifdef _DEBUG
	if ( !controller )
	{
		return;
	}

	// biped controllers
	if (controller->ClassID() == BIPSLAVE_CONTROL_CLASS_ID
	 || controller->ClassID() == BIPBODY_CONTROL_CLASS_ID
	 || controller->ClassID() == FOOTPRINT_CLASS_ID )
	{
		Animatable *vertical = controller->SubAnim( 0 );
		Animatable *horizontal = controller->SubAnim( 1 );
		Animatable *rotation = controller->SubAnim( 2 );

		APPEND_INDENTED( "Main Biped Controller => " );
		DBG_PrintBipedControllerInfo( controller, indent );
		LINE_FEED;

		++indent;

		APPEND_INDENTED( "Vertical Biped Controller => " );
		DBG_PrintBipedControllerInfo( ( Control * )vertical, indent );
		LINE_FEED;

		APPEND_INDENTED( "Horizontal Biped Controller => " );
		DBG_PrintBipedControllerInfo( ( Control * )horizontal, indent );
		LINE_FEED;

		APPEND_INDENTED( "Rotation Biped Controller => " );
		DBG_PrintBipedControllerInfo( ( Control * )rotation, indent );
		LINE_FEED;

		--indent;

		DBG_PrintBipControllerInfo( node, indent );

		// Get the Biped Export Interface from the controller 
		//IBipedExport *pBipInterface = (IBipedExport *) controller->GetInterface(I_BIPINTERFACE);

		////// Remove the non uniform scale
		//BipIface->RemoveNonUniformScale(1);

		//// Begin or End figure mode
		//BipIface->BeginFigureMode(1);
		//BipIface->EndFigureMode(1);
	}
	{
		APPEND_INDENTED( "Main Controller => " );
		DBG_PrintSubControllerInfo( controller, indent );
		LINE_FEED;

		++indent;
		
		APPEND_INDENTED( "Pos Controller => " );
		DBG_PrintSubControllerInfo( controller->GetPositionController(), indent );
		LINE_FEED;

		APPEND_INDENTED( "Rot Controller => " );
		DBG_PrintSubControllerInfo( controller->GetRotationController(), indent );
		LINE_FEED;

		APPEND_INDENTED( "Scale Controller => " );
		DBG_PrintSubControllerInfo( controller->GetScaleController(), indent );
		LINE_FEED;
			
		APPEND_INDENTED( "Roll Controller => " );
		DBG_PrintSubControllerInfo( controller->GetRollController(), indent );
		LINE_FEED;
	}

#endif //_DEBUG
}

//the node passed in can be any biped node
void ControllerExporter::DBG_PrintBipControllerInfo(INode *node, int indent )
{
#ifdef _DEBUG
	if (node)
	{
		// Get the node's transform control
		Control *c = node->GetTMController();
		int id,link; //the id and link of the node
		// You can test whether or not this is a biped controller with the following pseudo code:
		if (c&&(c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID))
		{
			APPEND_INDENTED( "Bip Slave Controller => " );
			IBipMaster *bipMaster = GetBipMasterInterface(c);
			if(bipMaster)
			{
				BOOL doubleCheck =  bipMaster->GetIdLink(node, id,link);
				if(doubleCheck)
					DBG_PrintBipSlaveControllerInfo(bipMaster,c,id,link,indent);
			}
			LINE_FEED;
		}
		else if(c&&c->ClassID() == BIPBODY_CONTROL_CLASS_ID) //this is the COM
		{
			APPEND_INDENTED( "Bip Body Controller => " );
			IBipMaster *bipMaster = GetBipMasterInterface(c);
			LINE_FEED;
			//the COM has 3 sub controllers for the horz,vert and turn tracks.
			if(bipMaster)
			{
				Control * cCOM;
				
				cCOM = bipMaster->GetVerticalControl(); //this is the vertical 
				APPEND_INDENTED( "Bip Vertical Controller => " );
				DBG_PrintBipSlaveControllerInfo(bipMaster,cCOM,KEY_VERTICAL,0,indent);
				LINE_FEED;
				
				cCOM = bipMaster->GetHorizontalControl(); //this is the horizontal 
				APPEND_INDENTED( "Bip Horizontal Controller => " );
				DBG_PrintBipSlaveControllerInfo(bipMaster,cCOM,KEY_HORIZONTAL,0,indent);
				LINE_FEED;
				
				cCOM = bipMaster->GetTurnControl(); //this is the turn 
				APPEND_INDENTED( "Bip Turn Controller => " );
				DBG_PrintBipSlaveControllerInfo(bipMaster,cCOM,KEY_TURN,0,indent);
				LINE_FEED;

			}
		}
	}
#endif //_DEBUG
}

void ControllerExporter::DBG_PrintAnimatableSubAnims( Animatable *animatable, int indent )
{
#ifdef _DEBUG
	if ( animatable )
	{
		LINE_FEED;
		if ( IsListController( animatable ) )
		{
			APPEND_INDENTED( "List Controller => " );
			IListControl *listControl = GetIListControlInterface( animatable );
			if ( listControl )
			{
				int listCount = listControl->GetListCount();
				APPEND_INDENTED( "Num lists : " );
				APPEND( listCount );
				APPEND( " => " );
			}
		}
		IKeyControl *pKeyInterface = GetKeyControlInterface( animatable );
		int numKeys = pKeyInterface ? pKeyInterface->GetNumKeys() : 0;
		APPEND_INDENTED( "Num Keys : " );
		APPEND( numKeys );
		APPEND( " => " );

		int nSubs = animatable->NumSubs();
		APPEND( "Num Subs : " );
		APPEND( nSubs );
		APPEND( " => " );
		APPEND( " { " );
		LINE_FEED;
		for (int i = 0; i < nSubs; i++)
		{
			Animatable* sub = animatable->SubAnim(i);
			if (sub != NULL)
			{
				APPEND_INDENTED( animatable->SubAnimName( i ) );
				APPEND( " [ " );
				APPEND( i );
				APPEND( " ] " );
				APPEND( ": " );
				APPEND_CLASSID( sub->ClassID() );
				APPEND( " , " );
				DBG_PrintAnimatableSubAnims( sub, indent + 1 );
			}
		}
		APPEND( " } " );
		LINE_FEED;
	}
#endif _DEBUG
}

void ControllerExporter::DBG_PrintBipSlaveControllerInfo(IBipMaster *bipMaster, Control *c,int id,int link, int indent )
{
#ifdef _DEBUG
	//NOTE :the link isn't used here but is being passed in for completeness (since an id and link together form a biped node).
	//NOTE : Another thing to check is whether or not the biped part is seperated or not. BOOL IBipMaster::GetSeparateTracks(int id);
	//If it isn't, then we only need to get the TCBs for the first link(id,0), since the keys are shared
	if(bipMaster)
	{
		//get the key based upon the biped body part type
		IBipedKey *key = NULL; //we use the biped key base since that base class holds the  tcb values
		IBipedVertKey vertKey;
		IBipedHorzKey horzKey;
		IBipedTurnKey turnKey;
		IBipedBodyKey bodyKey;
		IBipedHeadKey headKey;
		IBipedPropKey propKey;

		IBipedBodyKey prevKey;
	
		// IBipedFSKey inherits from IKey, not IBidep key
		IBipedFSKey fsKey;

		// Now set the key-specific properties
		switch(id)
		{
//#define KEY_PELVIS	11 => IBipedKey
//#define KEY_SPINE		8
//#define KEY_NECK	    16
			// 102, 103, 104, 105, etc... ==> when no valid keys
// Max 8 biped related I belive....
//#define KEY_TAIL		9 	
//#define KEY_PONY1	    17
//#define KEY_PONY2	    18

			case KEY_VERTICAL:
				key = &vertKey;
				break;
			case KEY_HORIZONTAL:
				key = &horzKey;
				break;
			case KEY_TURN:
				key = &turnKey;
				break;
			case KEY_LARM:  case KEY_RARM:		
			case KEY_LHAND: case KEY_RHAND:
			case KEY_LLEG:  case KEY_RLEG:
			case KEY_LFOOT: case KEY_RFOOT:
				key = &bodyKey;
				break;
			case KEY_HEAD:
				key = &headKey;
				break;
			case KEY_PROP1:
			case KEY_PROP2:
			case KEY_PROP3:
				key = &propKey;
				break;

			// do not know which key classes corresponds to these key types
			// using bodyKey because it is the largest class
			case KEY_FOOTPRINTS:
			case KEY_PELVIS:
			case KEY_SPINE:
			case KEY_TAIL:
			case KEY_NECK:
			case KEY_PONY1:
			case KEY_PONY2:
			{
				key = &bodyKey;
				APPEND( "[ Unknown Key Type for id : " );
				APPEND( id );
				APPEND( "]" );
				//FUFail( ; );
				break;
			}

			default:
			{
				//FUFail( ; );
				break;
			}

		}
		IKeyControl* keyInterface = GetKeyControlInterface(c);
		if ( c )
		{
			APPEND_CLASSID( c->ClassID() );
		}
		if (keyInterface != NULL)
		{
			int numKeys = keyInterface->GetNumKeys();
			size_t keySize = keyInterface->GetKeySize();
			APPEND( " => " );
			APPEND_INDENTED( "NumKeys : " );
			APPEND( numKeys );
			if ( key )
			{
				APPEND( " => " );
				LINE_FEED;
				APPEND( " { " );
				LINE_FEED;
				bool bIdenticalKeys = true;
				int realKeySize = key == &vertKey ? sizeof( vertKey	) : 
					( key == &horzKey ? sizeof( horzKey	) : 
					( key == &turnKey ? sizeof( turnKey	) : 
					( key == &bodyKey ? sizeof( bodyKey	) : 
					( key == &headKey ? sizeof( headKey	) : 
					( key == &propKey ? sizeof( propKey	) : 
					keySize )))));
				for(int i =0;i<numKeys;++i)
				{
					if ( i > 0 )
					{
						memcpy( &prevKey, key, realKeySize );
					}

					keyInterface->GetKey(i, key);

					APPEND( " [ " );
					APPEND( " Key Size : " );
					APPEND( keySize );
					APPEND( " , Real Key Size : " );
					APPEND( realKeySize );
					APPEND( " ] " );

					APPEND( " { " );
					APPEND( i );
					APPEND( " , " );
					APPEND( key->time );
					APPEND( " , " );
					APPEND( key->type );
					APPEND( " , " );
					APPEND( key->flags );
					APPEND( " , " );
					APPEND( key->tens );
					APPEND( " , " );
					APPEND( key->cont );
					APPEND( " , " );
					APPEND( key->bias );
					APPEND( " , " );
					APPEND( key->easeIn );
					APPEND( " , " );
					APPEND( key->easeOut );
					APPEND( " , " );

					if ( key == &vertKey )
					{
						APPEND( vertKey.ballTens );
						APPEND( " , " );
						APPEND( vertKey.dynBlend );
						APPEND( " , " );
						APPEND( vertKey.z );
					}
					else if ( key == &horzKey )
					{
						APPEND( horzKey.balFac );
						APPEND( " , " );
						APPEND( horzKey.x );
						APPEND( " , " );
						APPEND( horzKey.y );
					}
					else if ( key == &turnKey )
					{
						APPEND( FMVector4( turnKey.q ) );
					}
					else if ( key == &bodyKey )
					{
						APPEND( bodyKey.ik_ankle_ten );
						APPEND( " , " );
						APPEND( bodyKey.ik_blend );
						APPEND( " , " );
						APPEND( bodyKey.ik_joined_pivot );
						APPEND( " , " );
						APPEND( bodyKey.ik_num_pivots );
						APPEND( " , " );
						APPEND( bodyKey.ik_pivot_index );
						APPEND( " , " );
						APPEND( bodyKey.ik_space );
						APPEND( " , " );
						APPEND( " ( " );
						for ( int pivotId = 0; pivotId < NUMPIVOTS; ++pivotId )
						{
							APPEND( FMVector3( bodyKey.ik_pivot_pts[pivotId] ) );
							APPEND( " , " );
						}
						APPEND( " ) " );
					}
					else if ( key == &headKey )
					{
						APPEND( headKey.head_blend );
					}
					else if ( key == &propKey )
					{
						APPEND( propKey.pos_space );
						APPEND( " , " );
						APPEND( propKey.rot_space );
						APPEND( " , " );
					}

					APPEND( " } " );

					if ( i > 0 && bIdenticalKeys )
					{
						uint8 *curKeyPointer = ( uint8 * )key;
						uint8 *prevKeyPointer = ( uint8 * )&prevKey;
						// skipping the first 4 bytes ( time of the key ==> always different )
						curKeyPointer += 4;
						prevKeyPointer += 4;
						for ( int b = 0; b < realKeySize - 4; ++b, ++curKeyPointer, ++prevKeyPointer )
						{
							if ( (*curKeyPointer) != (*prevKeyPointer) )
							{
								bIdenticalKeys = false;
								break;
							}
						}
					}

					if ( key == &turnKey )
					{
						float angles[3];
						QuatToEuler(turnKey.q, angles, EULERTYPE_XYZ);
						for ( int a = 0; a < 3; ++a )
						{
							angles[a] = FMath::RadToDeg( angles[a] );
						}
						APPEND( "Euler Val : " );
						APPEND( " ( " );
						APPEND( FMVector3( angles ) );
						APPEND( " ) " );
					}

					LINE_FEED;
					//TimeValue time = c->GetKeyTime(i); 
					//float tens = (key->tens-25.0f)/25.0f; //put's value between -1 and 1
					//float cont = (key->cont-25.0f)/25.0f;
					//float bias = (key->bias-25.0f)/25.0f;
					//float easeIn = key->easeIn;
					//float easeOut = key->easeOut;
				}
				APPEND( " } " );
				APPEND( bIdenticalKeys ? "Identical Keys" : "Different Keys" );
				LINE_FEED;
			}
		}
	}

	DBG_PrintAnimatableSubAnims( c );
#endif // _DEBUG
}

void ControllerExporter::DBG_PrintValidSubAnimControllerInfo( Animatable *pController, int indent )
{
#ifdef _DEBUG
	if ( !pController )
	{
		return;
	}

	Class_ID classId = pController->ClassID();
	IKeyControl *pKeyControl = GetKeyControlInterface( pController );
	if ( pKeyControl 
		&& pKeyControl->GetNumKeys() > 0 )
	{
		LINE_FEED;
		APPEND_INDENTED( "" );
		APPEND_CLASSID( classId );
		APPEND( " => " );
		APPEND( "Num Keys : " );
		APPEND( pKeyControl->GetNumKeys() );
		return;
	}

	int nSubs = pController->NumSubs();
	for (int i = 0; i < nSubs; i++)
	{
		Animatable* sub = pController->SubAnim(i);
		DBG_PrintValidSubAnimControllerInfo( sub, indent + 1 );
	}
#endif // _DEBUG
}

Control *ControllerExporter::FindController( Control *ctrl, Class_ID cid, bool bResolveXRef )
{
	if ( !ctrl )
	{
		return NULL;
	}

	if ( ctrl->ClassID() == cid )
	{
		return ( Control * )ctrl;
	}

#if MAX_VERSION_MAJOR >= 9
	IXRefCtrl *pXRefController = IXRefCtrl::GetInterface( *ctrl );
	if ( bResolveXRef 
		&& pXRefController )
	{
		Control *pSourceController = pXRefController->GetSourceCtrl( true );
		return FindController( pSourceController, cid, bResolveXRef );
	}
#endif // MAX_VERSION_MAJOR >= 9

	int nSubs = ctrl->NumSubs();
	for ( int i = 0; i < nSubs; i++ )
	{
		Animatable* sub = ctrl->SubAnim( i );
		if ( sub != NULL )
		{
			Control *pControl = FindController( ( Control * )sub, cid, bResolveXRef );
			if ( pControl )
			{
				return pControl;
			}
		}
	}
	return NULL;
}


#undef INDENT
#undef APPEND_STRBUILDER_INDENTED
#undef APPEND
#undef APPEND_INDENTED
#undef APPEND_CLASSID
