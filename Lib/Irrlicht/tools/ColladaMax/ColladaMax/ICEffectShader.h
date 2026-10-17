/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _EFFECT_SHADER_H_
#define _EFFECT_SHADER_H_

class ICEffectPass;
class ICEffectParameter;

struct ICEffectShaderInfo
{
	fstring filename;

	// Cg specific
	fstring entry;
	CGprogram cgProgram;
	bool isVertex;

	// CgFX specific
	bool isEffect;
	CGeffect cgEffect;
	uint32 techniqueIndex;
	fstring techniqueName; // transient
	uint32 passIndex;
	fstring passName; // transient
	CGprogram cgVertexProgram;
	CGprogram cgFragmentProgram;

	ICEffectShaderInfo();
	~ICEffectShaderInfo(){}
};

#define IC_EFFECT_SHADER_ID Class_ID(0x240c0bd6, 0x68f528d1)

class ICEffectShader : public ReferenceTarget
{
	ICEffectPass* mParent;
	ICEffectShaderInfo mInfo;

	fstring mErrorString;
	bool mIsValid;

private:
	bool compileCg();
	bool compileCgFX();

	bool loadCgParameters(CGprogram prog);
	bool loadCgFXParameters(CGeffect effect);
	void connectParameters(CGprogram prog, bool isFrag);

	// general purpose
	bool compile();
	bool loadParameters();

	// transient parameter list (recreated at load time)
	ICEffectParameterList mParameters;

public:
	ICEffectShader(BOOL isLoading, ICEffectPass* parentP = NULL);
	~ICEffectShader();
	virtual void DeleteThis() { delete this; }

	void prepare();
	bool isValid() const { return mIsValid; }

	// ReferenceTarget implementation
	Class_ID ClassID() { return IC_EFFECT_SHADER_ID; }
	SClass_ID SuperClassID() { return REF_TARGET_CLASS_ID; }
	int NumRefs() { return 0; }
	RefTargetHandle GetReference(int) { return NULL; }
	void SetReference(int, RefTargetHandle){}
	RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage){return REF_SUCCEED;}

	ICEffectPass* getParent(){ return mParent; }
	void setParent(ICEffectPass* parent){ mParent = parent; }

	ICEffectShaderInfo& getInfo() { return mInfo; }
	const ICEffectShaderInfo& getInfo() const { return mInfo; }

	ICEffectParameterList& getParameters() { return mParameters; }

	const fstring& getErrorString() const { return mErrorString; }

	void setParamBlock(IParamBlock2* pb);

	/** Compiles the shader and registers the parameters on the parent pass.*/
	bool initialize();

	// overrides from ReferenceMaker
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

public:
	// Creates a temporary Cg context to parse the given effect file and retrieve
	// its informations.
	static void GetEffectInfo(const fstring& fileName, StringList& techNames, UInt32List& passCounts, StringList& passNames, fstring& errorString);
};

extern ClassDesc2* GetICEffectShaderClassDesc();

#endif // _EFFECT_SHADER_H_
