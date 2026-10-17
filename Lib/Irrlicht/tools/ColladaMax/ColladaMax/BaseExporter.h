/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _BASE_EXPORTER_H_
#define _BASE_EXPORTER_H_

class DocumentExporter;

class BaseExporter
{
public:
	BaseExporter(DocumentExporter* _document) : document(_document) {}
	virtual ~BaseExporter() { document = NULL; }

protected:
	DocumentExporter* document;
};

#define ANIM document->GetAnimationExporter()
#define CDOC document->GetCOLLADADocument()
#define ENTE document->GetEntityExporter()
#define OPTS document->GetColladaOptions()
#define NDEX document->GetNodeExporter()
#define MATE document->GetMaterialExporter()
#define GEOM document->GetGeometryExporter()
#define CTRL document->GetControllerExporter()
#define EMEX document->GetEmitterExporter()
#define FREX document->GetForceExporter()
#define MSEX document->GetModularSkinExporter()
#define BBEX document->GetBillboardExporter()
#define LTEX document->GetLightExporter()
#define CAME document->GetCameraExporter()

#endif