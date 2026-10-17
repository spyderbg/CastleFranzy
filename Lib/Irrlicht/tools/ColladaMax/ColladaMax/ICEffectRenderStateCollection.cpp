/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICEffectRenderStateCollection.h"
//#include "ICEffectRenderStateCollection.h"
//#include "ICEffectPass.h"

ClassDesc2 *ICEffectRenderStateCollection::GetMyClassDesc()
{
	static ICEffectRenderStateCollection::MyClassDesc myClassDesc;
	return &myClassDesc;
}

static ParamBlockDesc2 gICEffectRenderStateCollectionPBDesc
(
	ICEffectRenderStateCollection::PB_MAIN_ID
	, _T("parameters")
	,  0
	, ICEffectRenderStateCollection::GetMyClassDesc()
	, P_AUTO_CONSTRUCT
	, 0

	// params
	// ParamID, internal_name, ParamType, [tab size], flags, local_name_res_ID, <Param Tags>

	//, ICEffectRenderStateCollection::param_parent, _T("parent"), TYPE_REFTARG, P_NO_REF, 0,
	//	p_accessor, &gICEffectRenderStateCollectionPBAccessor,
	//	end,

	//ICEffectRenderStateCollection::param_name, _T("name"), TYPE_STRING, 0, 0,
	//	end,

	//ICEffectRenderStateCollection::param_passes, _T("passes"), TYPE_REFTARG_TAB, 0, P_VARIABLE_SIZE, 0,
	//	end,

	//ICEffectRenderStateCollection::param_edited_pass, _T("edited_pass"), TYPE_REFTARG, 0, 0,
	//	end,
	
	, end
);

///////////////////////////////////////////////////////////////////////////////
// COLLADAEFFECTTECHNIQUE CLASS
///////////////////////////////////////////////////////////////////////////////

ICEffectRenderStateCollection::ICEffectRenderStateCollection(BOOL UNUSED(isLoading))
//:	mParamBlock(NULL)
{
	//gICEffectRenderStateCollectionCD.MakeAutoParamBlocks(this);
}

ICEffectRenderStateCollection::ICEffectRenderStateCollection(ICEffectPass* parentP)
//:	mParamBlock(NULL)
{
	//gICEffectRenderStateCollectionCD.MakeAutoParamBlocks(this);
	setParent(parentP);
}

ICEffectRenderStateCollection::~ICEffectRenderStateCollection()
{
	//CLEAR_POINTER_VECTOR(mPasses);
}

RefResult ICEffectRenderStateCollection::NotifyRefChanged( Interval UNUSED(valid), RefTargetHandle UNUSED(r), PartID& UNUSED(partID), RefMessage UNUSED(msg))
{
	return REF_DONTCARE;
}

ICEffectPass* ICEffectRenderStateCollection::getParent()
{
	//ReferenceTarget* ref = NULL;
	//Interval valid;
	//mParamBlock->GetValue( ICEffectRenderStateCollection::param_parent, 0, ref, valid );
	//return (ICEffect*)ref;
	return NULL;
}

void ICEffectRenderStateCollection::setParent(ICEffectPass* p)
{
	//mParamBlock->SetValue(ICEffectRenderStateCollection::param_parent, 0, (ReferenceTarget*)p);
}