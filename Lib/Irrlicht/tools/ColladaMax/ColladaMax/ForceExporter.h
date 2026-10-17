/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FIELD_EXPORTER_H_
#define _FIELD_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

class FCDForce;
class FCDForceGravity;
class FCDForceWind;
class FCDForceDeflector;
class INode;
class ICGravityObject;
class ICWindObject;
class ICDeflectObject;
class IParamBlock2;

typedef fm::pvector<INode> NodeList;

class ForceExporter : public BaseExporter
{
public:
	enum
	{
		EXPORT_DEFAULTS		                   = 0
	};

private:
	void ExportForceParams( FCDForceGravity *pForceGravity, IParamBlock2 *pParamBlock, fstring idPrefix );
	void ExportWindParams( FCDForceWind *pForceWind, IParamBlock2 *pParamBlock, fstring idPrefix );
	void ExportGravity( FCDForceGravity *pForceGravity, ICGravityObject *pMaxGravity );
	void ExportWind( FCDForceWind *pForceWind, ICWindObject *pMaxWind );
	void ExportDeflector( FCDForceDeflector *pForceDeflector, ICDeflectObject *pMaxDeflector );

public:
	ForceExporter(DocumentExporter* document);
	virtual ~ForceExporter();

	FCDEntity* ExportForce(INode* node, Object* object, int flags = EXPORT_DEFAULTS);
};

#endif // _FIELD_EXPORTER_H_
