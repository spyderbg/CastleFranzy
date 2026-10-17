/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FArchiveXML.h"
#include "FCDocument/FCDForce.h"

//
// Emitter Import
//

bool FArchiveXML::LoadForce(FCDObject* object, xmlNode* node)				
{
	FCDForce* force = (FCDForce*)object;

	bool status = FArchiveXML::LoadEntity(force, node);
	if (!status) return status;
	if (!IsEquivalent(node->name, DAE_FORCE_ELEMENT))
	{
		FUError::Error(FUError::WARNING_LEVEL, FUError::WARNING_UNKNOWN_ELEMENT, node->line);
		return status;
	}


	force->SetDirtyFlag();
	return status;
}

