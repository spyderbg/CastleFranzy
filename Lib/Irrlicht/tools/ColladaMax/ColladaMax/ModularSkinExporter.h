/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _MODULAR_SKIN_EXPORTER_H_
#define _MODULAR_SKIN_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

#include "ColladaModifierManager.h"

class FCDEntity;
class FCDModularSkinCategory;
//class FCDEntityInstance;
//class ExportNode;
//class FCDModularSkin;
//class IParamBlock;
//class INode;
//class FCDSceneNode;

typedef fm::pvector<INode> NodeList;
typedef fm::pvector<FCDEntity> EntityList;
typedef fm::map<Object*, EntityList> ExportedEntityMap;

class ModularSkinExporter : public BaseExporter
{
public:
	enum
	{
		//EXPORT_AFFECTS_CONTROLLERS                 = 0x00000001
		//, EXPORT_REMOVE_DUPLICATE_SOURCE_DATA      = 0x00000002
		//, EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA   = 0x00000004
		//, EXPORT_DEINDEX_VERTEX_DATA			   = 0x00000008
		//, EXPORT_ENABLE_NAIVE_DEINDEXATION		   = 0x00000010
		//, EXPORT_DEFAULTS		                   = EXPORT_REMOVE_DUPLICATE_SOURCE_DATA | EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA | EXPORT_DEINDEX_VERTEX_DATA
		/*,*/ EXPORT_DEFAULTS		                   = 0
	};

private:
	NodeList m_modularSkinnedMeshNodes;

private:
	void LinkSkinnedMeshNodes( FCDModularSkinCategory *pModularSkinCategory );
	void ExportSkinnedMesh( INode *pNode, FCDModularSkinCategory *pModularSkinCategory, ExportedEntityMap &exportedEntities );
	bool RemoveModularSkinCategoryNodes( FCDSceneNode *pSceneNode );

public:
	ModularSkinExporter(DocumentExporter* document);
	virtual ~ModularSkinExporter();

	FCDEntity* ExportModularSkinMaster(INode* node, Object* object, ExportedEntityMap &exportedEntities, int flags = EXPORT_DEFAULTS);
	FCDEntity* ExportModularSkinCategory(INode* node, Object* object, ExportedEntityMap &exportedEntities, int flags = EXPORT_DEFAULTS);
	
	void CalculateMasterForcedNodes(INode* node, Object* object, NodeList& forcedNodes);
	void CalculateCategoryForcedNodes(INode* node, Object* object, NodeList& forcedNodes);

	void LinkModularSkinCategories();
	void LinkModularSkinMasters();

	void PreExport();
	void PostExport();

	void RemoveModularSkinCategoryNodes();
};

#endif // _MODULAR_SKIN_EXPORTER_H_
