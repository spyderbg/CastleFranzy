/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ColladaMaxTypes.h"
#include "Common/MaxParticleDefinitions.h"
#include "MaxForceDefinitions.h"
#include "ICModularSkinMaster.h"
#include "ICBillboardHelper.h"
#include "ICModifierBillboard.h"
#include "XRefFunctions.h"

#include <surf_api.h>
#include <modstack.h>
#include <CS/Bipexp.h>
#include "ICPCloud.h"
#include "DbgICPCloud.h"

using ColladaMaxTypes::MaxType;

MaxType ColladaMaxTypes::Get(INode* node, Animatable* p, bool detectXRef)
{
	if (p == NULL)
		return ColladaMaxTypes::UNKNOWN;

	Animatable* base = p;
	// Modifiers are applied to the object, acquire the base object
	while (base->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *dobj = (IDerivedObject*)base;
		base = dobj->GetObjRef();
	}

	// check for an XRef
	if (XRefFunctions::IsXRefObject(base))
	{
		if (detectXRef) return XREF_OBJECT;
		else
		{
			// replace the current animatable by the x-ref object
			base = XRefFunctions::GetXRefObjectSource((Object*)base);
			if (base == NULL) return ColladaMaxTypes::UNKNOWN;
		}
	}
	else if (XRefFunctions::IsXRefMaterial(base))
	{
		if (detectXRef) return XREF_MATERIAL;
		else
		{
			base = XRefFunctions::GetXRefMaterialSource((Mtl*)base);
			if (base == NULL) return ColladaMaxTypes::UNKNOWN;
		}
	}

	SClass_ID sid = base->SuperClassID();
	Class_ID cid = base->ClassID();

	switch (sid)
	{
	case GEOMOBJECT_CLASS_ID: {

		// Check for a Max bone mesh
		if (cid == BONE_OBJ_CLASSID)
			return BONE;
		
		if ( cid == ICPCLOUD_CLASS_ID )
		{
			return PARTICLE_SYSTEM;
		}

		if ( cid.PartA() == TARGET_CLASS_ID )
		{
			return TARGET;
		}

		// Check for biped
		Control* c = node->GetTMController();
		if (c != NULL)
		{
			Class_ID ccid = c->ClassID();
			if (ccid == BIPSLAVE_CONTROL_CLASS_ID || ccid == BIPBODY_CONTROL_CLASS_ID || ccid == FOOTPRINT_CLASS_ID || ccid == BIPED_CLASS_ID)
				return BONE;
		}
		return MESH; }

	case CAMERA_CLASS_ID: return CAMERA;
	case LIGHT_CLASS_ID: return LIGHT;

	case SHAPE_CLASS_ID:
		// Modifiers can act on a spline to produce a mesh
		if (p != base)
		{
			// BUG368: For some reason the CanConvertToType function stopped working.
			// This is the best option anyway, evaluate the object actually created by
			// the modifier stack.
			ObjectState os = ((IDerivedObject*)p)->Eval(0);
			return Get(node, os.obj, detectXRef);
		}
		if (cid == EDITABLE_SURF_CLASS_ID || cid == EDITABLE_CVCURVE_CLASS_ID || cid == EDITABLE_FPCURVE_CLASS_ID)
		{
			return NURBS_CURVE;
		}
		else
		{
			return SPLINE;
		}

	case HELPER_CLASS_ID:
	{
		if ( cid.PartA() == BONE_CLASS_ID )
		{
			return BONE;
		}
		else if ( cid == MODULAR_SKIN_MASTER_OBJECT_CLASSID )
		{
			return MODULAR_SKIN_MASTER;
		}
		else if ( cid == MODULAR_SKIN_CATEGORY_OBJECT_CLASSID )
		{
			return MODULAR_SKIN_CATEGORY;
		}
		else if ( cid == BILLBOARD_HELPER_OBJECT_CLASSID )
		{
			return BILLBOARD_MANAGER;
		}
		else
		{
			return HELPER;
		}
	}
	case MATERIAL_CLASS_ID: return MATERIAL;
	case WSM_OBJECT_CLASS_ID:
	{
		if ( cid == FSForceGravity::IC_OBJECT_CLASSID 
			|| cid == FSForceWind::IC_OBJECT_CLASSID
			|| cid == FSDeflector::IC_OBJECT_CLASSID
			)
		{
			return FORCE;
		}
		else if ( cid == IC_BILLBOARD_OBJECT_CLASSID )
		{
			return BILLBOARD_MODIFIER_OBJECT;
		}


		return UNKNOWN;
	}
	default:
		return UNKNOWN;
	}
}