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

#ifndef _CONTROLLER_EXPORTER_H_
#define _CONTROLLER_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

#include "ColladaModifierManager.h"

class FCDSceneNode;
class FCDEntity;
class FCDController;
class FCDSkinController;
class FCDMorphController;
class FCDEntityInstance;
class FCDControllerInstance;
class ColladaSkin;
class ColladaMorph;
class INode;
class IBipMaster;
class FCDGeometryMesh;

typedef fm::pvector<INode> NodeList;
typedef fm::map<Object*, FCDEntity*> ExportedControllerMap;
typedef fm::map<uint32, UInt32List> FCDGeometryIndexTranslationMap;
typedef fm::map<int,UInt32List> BoneVertexList;
typedef fm::map<int,fm::vector<float>> BoneWeightsList;


struct ControllerInfo
{
public:
	INode* node;
	Modifier *modifier;
	ControllerInfo(INode* _node, Modifier *_modifier) : node(_node), modifier(_modifier) {}
};

class ControllerExporter : public BaseExporter, public ColladaModifierManager
{
private:
	int boneIndex;

public:
	ControllerExporter(DocumentExporter* document);
	virtual ~ControllerExporter();

	FCDEntity* ExportController(INode* node, Object* object);

	void LinkControllers();
	bool LinkController(FCDController* controller, INode* inode, FCDControllerInstance* instance);
	void LinkSkin(FCDSkinController *colladaSkin, FCDControllerInstance* instance, INode* node, ColladaSkin* modifier);
	void LinkMorph(FCDMorphController* colladaMorph, FCDControllerInstance* instance, INode* node, ColladaMorph* modifier);

	void CalculateForcedNodes(INode* node, Object* object, NodeList& forcedNodes);

	bool IsSkinned(Object* obj);

	// method that checks for Skin or Morph modifiers
	bool DoOverrideXRefExport(Object *obj);

	// Done at the very end of the export process: iterate over the known
	// controllers and re-active the skinning UI
	void ReactivateSkins();

	static Control *FindController( Control *ctrl, Class_ID cid, bool bResolveXRef );
	static bool IsListController( Animatable *a );
	static bool IsBipedController( Animatable *a );
	static bool IsValidAnimationController( Animatable *pController );
	static int GetNumControllerKeys( Animatable *pControl, IKeyControl *&pKeyControl );

	static void DBG_PrintControllerInfo( Control *controller, int indent = 0 );
	static void DBG_PrintSubControllerInfo( Control *controller, int indent = 0 );
	static void DBG_PrintCompoundControllerInfo( INode *node, Control *controller, int indent = 0 );
	static void DBG_PrintBipedControllerInfo( Control *controller, int indent = 0 );
	static void DBG_PrintBipControllerInfo(INode *node, int indent = 0 );
	static void DBG_PrintBipSlaveControllerInfo(IBipMaster *bipMaster, Control *c,int id,int link, int indent = 0 );
	static void DBG_PrintAnimatableSubAnims( Animatable *animatable, int indent = 0 );
	static void DBG_PrintValidSubAnimControllerInfo( Animatable *pController, int indent = 0 );

	static bool FindConstraint(Animatable* ctrl);

	static void ResolveSkinAndMorphModifiers( INode *pNode, Object* obj, ColladaModifierStack &modifiers );
	void ResolveModifiers( INode *pNode, Object* obj, ColladaModifierStack &modifiers );

private:
	void ComputeJointBoundingBoxes( FCDSkinController *pSkinController, FCDGeometryMesh *pMesh );
};

#endif // _CONTROLLER_EXPORTER_H_