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

#ifndef _NODE_EXPORTER_H_
#define _NODE_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

#define ADD_ENTITY( __entities, __exportFunction ) \
{ \
	FCDEntity *__pEntity = (__exportFunction); \
	if ( __pEntity ) \
	{ \
		__entities.push_back( __pEntity ); \
	} \
}

class FCDEntity;
class FCDSceneNode;
class INode;
struct ExportNode;
class FCDTRotation;

typedef fm::pvector<INode> NodeList;
typedef fm::pvector<FCDEntity> EntityList;
typedef fm::map<INode*, ExportNode*> ExportNodeMap;
typedef fm::map<Object*, EntityList> ExportedEntityMap;
typedef fm::map<INode*, Matrix3> NodeTransformMap;

class NodeExporter : public BaseExporter
{
public:
	NodeExporter(DocumentExporter* document);
	~NodeExporter();

	// TODO : Dehack hiddenXRefs thing ( should do something like ModularSkinExporter::PreExport and PostExport instead )
	void CalculateExportNodes( NodeList *hiddenXRefs = NULL );
	bool CalculateExportNodes(INode* currentNode, NodeList& selectedNodes, NodeList& forcedNodes, NodeList *hiddenXRefs = NULL );
	void CalculateForcedNodes(INode* currentNode, NodeList& forcedNodes );

	void GetSelectedNodes( NodeList &selectedNodes );
	bool ShouldExportNode( INode* currentNode, NodeList& selectedNodes );

	void ExportMaterials();

	void ExportScene();
	void ExportSceneGraph();
	void ExportSceneNode(ExportNode& node);
	void ExportSceneNodeIrrColladaExtra( FCDSceneNode *pColladaNode, INode *pMaxNode );
	void ExportSceneNodeVisibility( FCDSceneNode *pColladaNode, INode *pMaxNode );
	void ExportInstances();

	void ExportHelper(INode* node, Object* object, FCDSceneNode* colladaNode);
	bool ExportTransforms(INode* node, FCDSceneNode* colladaNode, bool checkSkin);
	void ExportTransformElements(FCDSceneNode* colladaNode, INode* node, Control* c, const Matrix3& tm);
	FCDSceneNode* ExportPivotNode(INode* node, FCDSceneNode* colladaNode);
	void ExportDecomposedTransform( FCDSceneNode* colladaNode, INode* node, Control* c, const AffineParts &ap );
	void ExportDecomposedWSMPivot( FCDSceneNode* colladaNode, INode* node, Control* c, const AffineParts &ap );
	void FitCubicCurve( FCDSceneNode* colladaNode, INode* node, Control* c, const AffineParts &ap );

	FCDSceneNode* FindExportedNode(INode* node);
	const Matrix3& GetWorldTransform(INode* node);
	const Matrix3& GetLocalTransform(INode* node);

	void ApplyPivotTM(INode* maxNode, Matrix3& tm);

	void SetWorldNode( INode *pNode );
	INode *GetWorldNode();

	ExportNodeMap &GetExportNodes();

	void PostExportXRefs();
	static void AffineDecomp( const Matrix3 &tm, AffineParts &ap );

	void PreExportPreviewCamera();
	void PostExportPreviewCamera();

private:

	void ExportRotationElement(Control* rotController, FCDSceneNode* colladaNode, const Quat &q);
	fm::pvector<FCDTRotation> ExportUnanimatedRotationElement( FCDSceneNode* colladaNode, const Quat &q );
	void PreExport();
	void PostExport();

	void HideStrippedNodes();
	void ShowStrippedNodes();

	NodeList forcedHiddenXRefs;
	ExportNodeMap exportNodes;
	ExportedEntityMap exportedEntities;
	NodeTransformMap worldTransforms;
	NodeTransformMap localTransforms;
	INode* worldNode;
	NodeList m_strippedNodes;

	NodeList m_previewTemporaryNodes;
	ULONG m_previewCameraHandle;
	int m_perspectiveViewportIndex;
};

struct ExportNode
{
	INode* maxNode;
	FCDSceneNode* colladaNode;
	FCDSceneNode* colladaParent;
	bool exportObject;
	ExportNode(INode* _maxNode, bool _exportObject = true)
		: maxNode(_maxNode), colladaParent(NULL), colladaNode(NULL), exportObject(_exportObject) {}
};

inline ExportNodeMap &NodeExporter::GetExportNodes()
{
	return exportNodes;
}

#endif // _NODE_EXPORTER_H