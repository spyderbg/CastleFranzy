/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _BILLBOARD_EXPORTER_H_
#define _BILLBOARD_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

//#include "ColladaModifierManager.h"

class FCDEntity;
class FCDBillboardManager;
class ICBillboardWSMObject;
class FCDBillboardManager;
class FCDETechnique;

typedef fm::pvector<INode> NodeList;
typedef fm::pvector<FCDEntity> EntityList;
typedef fm::map<Object*, EntityList> ExportedEntityMap;
//typedef fm::map< ICBillboardWSMObject *, FCDBillboardManager *> BillboardModifierObjectMap;
//typedef fm::map< ICBillboardWSMObject *, fm::pvector<FCDBillboardManager>> BillboardModifierObjectMap;

class BillboardExporter : public BaseExporter
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
	NodeList m_billboardModifierNodes;
	//BillboardModifierObjectMap m_billboardModifierObjectMap;

public:
	BillboardExporter(DocumentExporter* document);
	virtual ~BillboardExporter();

	FCDEntity* ExportBillboardManager(INode* node, Object* object, ExportedEntityMap &exportedEntities, int flags = EXPORT_DEFAULTS);
	void ExportBillboardModifier( INode *pNode, FCDETechnique *pTechnique, const fm::string &idPrefix = fm::string(), int flags = EXPORT_DEFAULTS );
	
	void CalculateForcedNodes(INode* node, Object* object, NodeList& forcedNodes);
	void LinkBillboardManagers();

	void PreExport();
	void PostExport();
};

#endif // _BILLBOARD_EXPORTER_H_
