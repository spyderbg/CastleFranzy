/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICEffect.h"
#include "ICEffectTechnique.h"
#include "ICEffectPass.h"

class ICEffectTechniqueClassDesc : public ClassDesc2
{
public:
	int				IsPublic() {return FALSE; }
	void*			Create(BOOL isLoading)
	{
		// is this ever called if I don't expose the class as a plugin?
		return new ICEffectTechnique(isLoading);
		//return NULL;
	}

	const TCHAR*	ClassName() {return _T("ICEffectTechnique");}
	SClass_ID		SuperClassID() {return REF_TARGET_CLASS_ID;}
	Class_ID 		ClassID() {return IC_EFFECT_TECHNIQUE_ID;}
	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("ICEffectTechnique"); }
	HINSTANCE		HInstance() { return hInstance; }
};
static ICEffectTechniqueClassDesc gICEffectTechniqueCD;

ClassDesc2* GetICEffectTechniqueClassDesc()
{
	return &gICEffectTechniqueCD;
}

///////////////////////////////////////////////////////////////////////////////
// PARAMETER BLOCKS
///////////////////////////////////////////////////////////////////////////////

class ICEffectTechniquePBAccessor : public PBAccessor
{
public:
	void Set(PB2Value &v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)
	{
		PBAccessor::Set(v,owner,id,tabIndex,t);
	}

	void TabChanged(tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count)
	{
		PBAccessor::TabChanged(changeCode,tab,owner,id,tabIndex,count);
	}
};

static ICEffectTechniquePBAccessor gICEffectTechniquePBAccessor;

static ParamBlockDesc2 gICEffectTechniquePBDesc(
	ICEffectTechnique::parameters, _T("parameters"),  0, &gICEffectTechniqueCD, P_AUTO_CONSTRUCT, 0,

	// params
	// ParamID, internal_name, ParamType, [tab size], flags, local_name_res_ID, <Param Tags>

	ICEffectTechnique::param_parent, _T("parent"), TYPE_REFTARG, P_NO_REF, 0,
		p_accessor, &gICEffectTechniquePBAccessor,
		end,

	ICEffectTechnique::param_name, _T("name"), TYPE_STRING, 0, 0,
		p_accessor, &gICEffectTechniquePBAccessor,
		end,

	ICEffectTechnique::param_passes, _T("passes"), TYPE_REFTARG_TAB, 0, P_VARIABLE_SIZE, 0,
		p_accessor, &gICEffectTechniquePBAccessor,
		end,

	ICEffectTechnique::param_edited_pass, _T("edited_pass"), TYPE_REFTARG, 0, 0,
		p_accessor, &gICEffectTechniquePBAccessor,
		end,
	
	end
);

///////////////////////////////////////////////////////////////////////////////
// COLLADAEFFECTTECHNIQUE CLASS
///////////////////////////////////////////////////////////////////////////////

ICEffectTechnique::ICEffectTechnique(BOOL UNUSED(isLoading))
:	mParamBlock(NULL)
{
	gICEffectTechniqueCD.MakeAutoParamBlocks(this);
}

ICEffectTechnique::ICEffectTechnique(ICEffect* parentP, const fstring& nameP)
:	mParamBlock(NULL)
{
	gICEffectTechniqueCD.MakeAutoParamBlocks(this);
	setParent(parentP);
	setName(nameP);
}

ICEffectTechnique::~ICEffectTechnique()
{
	//CLEAR_POINTER_VECTOR(mPasses);
}

RefResult ICEffectTechnique::NotifyRefChanged(
	Interval UNUSED(valid), RefTargetHandle UNUSED(r), PartID& UNUSED(partID), RefMessage UNUSED(msg))
{
	return REF_SUCCEED;
}

ICEffect* ICEffectTechnique::getParent()
{
	ReferenceTarget* ref = NULL;
	Interval valid;
	mParamBlock->GetValue(ICEffectTechnique::param_parent, 0, ref, valid);
	return (ICEffect*)ref;
}

void ICEffectTechnique::setParent(ICEffect* p)
{
	mParamBlock->SetValue(ICEffectTechnique::param_parent, 0, (ReferenceTarget*)p);
}

fstring ICEffectTechnique::getName()
{
	TCHAR* val = NULL;
	Interval valid;
	mParamBlock->GetValue(ICEffectTechnique::param_name, 0, val, valid);
	return FS(val);
}

bool ICEffectTechnique::setName(const fstring& name)
{
	ICEffect* parent = getParent();
	if (parent == NULL) return false;
	if (!parent->isNameUnique(name)) return false;
	TSTR s = name.c_str();
	mParamBlock->SetValue(ICEffectTechnique::param_name, 0, (TCHAR*)s.data());
	parent->sortTechniques();
	return true;
}

size_t ICEffectTechnique::getPassCount()
{
	int count = mParamBlock->Count(ICEffectTechnique::param_passes);
	return (size_t)count;
}

ICEffectPass* ICEffectTechnique::getPass(size_t idx)
{
	if (idx >= getPassCount()) return NULL;
	ReferenceTarget* ref = NULL;
	Interval valid;
	mParamBlock->GetValue(ICEffectTechnique::param_passes, 0, ref, valid, (int)idx);
	return (ICEffectPass*)ref;
}

bool ICEffectTechnique::isNameUnique(const fstring& name)
{
	size_t count = getPassCount();
	for (size_t i = 0; i < count; ++i)
	{
		ICEffectPass* p = getPass(i);
		if (fstrcmp(p->getName().c_str(), name.c_str()) == 0) return false;
	}
	return true;
}

void ICEffectTechnique::insertPass(ICEffectPass* pass)
{
	ReferenceTarget* refArray[1] = {pass};
	mParamBlock->Append(ICEffectTechnique::param_passes, 1, refArray);
	mParamBlock->Shrink(ICEffectTechnique::param_passes);
}

ICEffectPass* ICEffectTechnique::addPass(const fstring& name)
{
	if (!isNameUnique(name)) return NULL;
	ICEffectPass* pass = new ICEffectPass(FALSE,this,name);
	insertPass(pass);
	return pass;
}

ICEffectPass* ICEffectTechnique::removePass(size_t index)
{
	if (index >= getPassCount())
		return NULL;

	ICEffectPass* pass = getPass(index);
	mParamBlock->Delete(ICEffectTechnique::param_passes, (int)index, 1);
	mParamBlock->Shrink(ICEffectTechnique::param_passes);
	return pass;
}

ICEffectPass* ICEffectTechnique::getEditedPass()
{
	ReferenceTarget* ref = NULL;
	Interval valid;
	mParamBlock->GetValue(ICEffectTechnique::param_edited_pass, 0, ref, valid);
	return (ICEffectPass*)ref;
}

ICEffectPass* ICEffectTechnique::setEditedPass(size_t index)
{
	ReferenceTarget* ref = (ReferenceTarget*)getPass(index);
	mParamBlock->SetValue(ICEffectTechnique::param_edited_pass, 0, ref);
	return (ICEffectPass*)ref;
}

bool ICEffectTechnique::isValid()
{
	size_t pc = getPassCount();
	if (pc == 0) return false;
	for (size_t i = 0; i < pc; ++i)
	{
		ICEffectPass* pass = getPass(i);
		if (pass == NULL || !pass->isValid()) return false;
	}
	return true;
}
