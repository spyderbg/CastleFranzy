/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _COLLADAEFFECTTECHNIQUE_H_
#define _COLLADAEFFECTTECHNIQUE_H_

class ICEffect;
class ICEffectPass;

#define IC_EFFECT_TECHNIQUE_ID Class_ID(0x2dbc7f77, 0x52c838fb)

class ICEffectTechnique : public ReferenceTarget
{
public:
	enum BlockIDs
	{
		parameters
	};

	enum ParamIDs
	{
		param_parent,
		param_name,
		param_passes,
		param_edited_pass,
	};

private:
	IParamBlock2* mParamBlock;

private:
	void insertPass(ICEffectPass* pass);

public:
	ICEffectTechnique(BOOL isLoading = FALSE);
	ICEffectTechnique(ICEffect* parentP, const fstring& name);
	~ICEffectTechnique();
	virtual void DeleteThis() { delete this; }

	ICEffect* getParent();
	void setParent(ICEffect* p);

	// ReferenceTarget implementation
	Class_ID ClassID() { return IC_EFFECT_TECHNIQUE_ID; }
	SClass_ID SuperClassID() { return REF_TARGET_CLASS_ID; }
	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i) { FUAssert(i == 0,return NULL); return mParamBlock; }
	void SetReference(int i, RefTargetHandle rtarg) { FUAssert(i == 0,return); mParamBlock = (IParamBlock2*)rtarg; }
	RefResult NotifyRefChanged(Interval valid, RefTargetHandle r, PartID& partID, RefMessage msg);

	fstring getName();
	bool setName(const fstring& name);

	bool isNameUnique(const fstring& name);
	size_t getPassCount();
	ICEffectPass* getPass(size_t idx);
	ICEffectPass* addPass(const fstring& name);
	ICEffectPass* removePass(size_t index);

	ICEffectPass* getEditedPass();
	ICEffectPass* setEditedPass(size_t index);

	bool isValid();
};

extern ClassDesc2* GetICEffectTechniqueClassDesc();

#endif // _COLLADAEFFECTTECHNIQUE_H_
