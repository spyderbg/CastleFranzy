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

#ifndef _DOCUMENT_EXPORTER_H_
#define _DOCUMENT_EXPORTER_H_

class FCDocument;
class ColladaOptions;
class AnimationExporter;
class CameraExporter;
class ControllerExporter;
class EntityExporter;
class GeometryExporter;
class LightExporter;
class NodeExporter;
class EmitterExporter;
class MaterialExporter;
class ForceExporter;
class EmitterExporter;
class ForceExporter;
class ModularSkinExporter;
class BillboardExporter;
class XRefManager;
class FCDSceneNode;
class COLLADAExporter;

// Main exporter class
class DocumentExporter
{
private:
	Interface*		 pMaxInterface;		// The main MAX UI interface
	FCDocument*		 colladaDocument;	// The FCollada document to fill in and write out.
	COLLADAExporter* m_pCOLLADAExporter;

	// Sub-library exporter(s)
	AnimationExporter* animationExporter;
	CameraExporter* cameraExporter;
	ControllerExporter* controllerExporter;
	EntityExporter* entityExporter;
	GeometryExporter* geometryExporter;
	LightExporter* lightExporter;
	MaterialExporter* materialExporter;
	NodeExporter* nodeExporter;
	EmitterExporter* emitterExporter;
	ForceExporter* forceExporter;
	ModularSkinExporter* modularSkinExporter;
	BillboardExporter* billboardExporter;
	XRefManager* xRefManager;

private:
	void SortVisualScene( FCDSceneNode *pNode );

public:
	DocumentExporter( DWORD options, COLLADAExporter *pCOLLADAExporter = NULL );
	~DocumentExporter();

	// Accessors
	inline FCDocument* GetCOLLADADocument() { return colladaDocument; }
	inline COLLADAExporter* GetCOLLADAExporter() { return m_pCOLLADAExporter; }
	//inline void SetCOLLADAExporter( COLLADAExporter *pCOLLADAExporter ) { m_pCOLLADAExporter = pCOLLADAExporter; }
	inline Interface* GetMaxInterface() { return pMaxInterface; }
	inline ColladaOptions* GetColladaOptions() { return options; }

	inline AnimationExporter* GetAnimationExporter() { return animationExporter; }
	inline CameraExporter* GetCameraExporter() { return cameraExporter; }
	inline ControllerExporter* GetControllerExporter() { return controllerExporter; }
	inline EntityExporter* GetEntityExporter() { return entityExporter; }
	inline GeometryExporter* GetGeometryExporter() { return geometryExporter; }
	inline LightExporter* GetLightExporter() { return lightExporter; }
	inline MaterialExporter* GetMaterialExporter() { return materialExporter; }
	inline NodeExporter* GetNodeExporter() { return nodeExporter; }
	inline EmitterExporter* GetEmitterExporter() { return emitterExporter; }
	inline ForceExporter* GetForceExporter() { return forceExporter; }
	inline ModularSkinExporter* GetModularSkinExporter() { return modularSkinExporter; }
	inline BillboardExporter* GetBillboardExporter() { return billboardExporter; }
	inline XRefManager* GetXRefManager() { return xRefManager; }

	// Export
	void ExportCurrentMaxScene();
	void ExportAsset();
	void PostExportAsset();
	bool ShowExportOptions(BOOL suppressOptions);
	void PreExport();
	void Sort();

	ColladaOptions* options;			// The options container
};


#endif // _DOCUMENT_EXPORTER_H_
