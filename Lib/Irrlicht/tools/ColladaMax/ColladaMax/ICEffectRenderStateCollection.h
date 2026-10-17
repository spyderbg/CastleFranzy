/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_EFFECT_RENDER_STATE_COLLECTION_H_
#define _IC_EFFECT_RENDER_STATE_COLLECTION_H_

#ifndef _FU_DAE_ENUM_H_
#include "FUtils/FUDaeEnum.h"
#endif // _FU_DAE_ENUM_H_

#define IC_EFFECT_RENDER_STATE_COLLECTION_ID Class_ID(0x2dbc7f77, 0x52c838fb)

class DlgTemplateBuilder;

class ICEffect;
class ICEffectPass;

class ICEffectRenderStateCollection : public ReferenceTarget
{
public:
	enum ParamBlock
	{
		PB_MAIN_ID
		, PB_MAIN_REF_ID = PB_MAIN_ID
		, PB_COUNT
	};

	//enum ParamID
	//{
	//	param_parent,
	//	param_name,
	//	param_passes,
	//	param_edited_pass,
	//};

	class MyClassDesc : public ClassDesc2
	{
	public:
		int				IsPublic() {return FALSE; }
		void*			Create(BOOL isLoading)
		{
			// is this ever called if I don't expose the class as a plugin?
			return new ICEffectRenderStateCollection(isLoading);
			//return NULL;
		}

		const TCHAR*	ClassName() {return _T("ICEffectRenderStateCollection");}
		SClass_ID		SuperClassID() {return REF_TARGET_CLASS_ID;}
		Class_ID 		ClassID() {return IC_EFFECT_RENDER_STATE_COLLECTION_ID;}
		const TCHAR* 	Category() {return _T("");}
		const TCHAR*	InternalName() { return _T("ICEffectRenderStateCollection"); }
		HINSTANCE		HInstance() { return hInstance; }
	};

public:
	/** Static cross reference data between Collada names and Cg names.*/
	//static const char* CG_RENDER_STATE_NAMES[];
	//static const FUDaePassState::State CG_RENDER_STATES_XREF[];

private:
	//ICEffectPass *mParent; /**< The pass holding the render state collection.*/
	//FCDEffectPassState* mColladaState; /**< State data holder.*/
	//int mIndex; /**< Used if indexed state only.*/

public:
	ICEffectRenderStateCollection(BOOL isLoading = FALSE);
	ICEffectRenderStateCollection(ICEffectPass* parent);
	~ICEffectRenderStateCollection();

	inline virtual void DeleteThis() { delete this; }

	ICEffectPass *getParent();
	void setParent(ICEffectPass* p);

	// ReferenceTarget implementation
	Class_ID ClassID() { return IC_EFFECT_RENDER_STATE_COLLECTION_ID; }
	SClass_ID SuperClassID() { return REF_TARGET_CLASS_ID; }

	//int NumRefs() { return 1; }
	//RefTargetHandle GetReference(int i) { FUAssert(i == 0,return NULL); return mParamBlock; }
	//void SetReference(int i, RefTargetHandle rtarg) { FUAssert(i == 0,return); mParamBlock = (IParamBlock2*)rtarg; }
	RefResult NotifyRefChanged(Interval valid, RefTargetHandle r, PartID& partID, RefMessage msg);

	//void Initialize(FCDEffectPassState* renderState);
	//bool Initialize(_CGstateassignment* effectState);
	//bool Initialize(FUDaePassState::State stateType);

	//inline const FCDEffectPassState* GetData() const { return colladaState; }
	//FUDaePassState::State GetType() const;
	//
	//void Synchronize();
	//void Use();
	//
	//const char* GetColladaStateName() const;
	//const char* GetCgStateName() const;

	//bool IsIndexed() const;
	//int GetIndex() const { return mIndex; }
	//TSTR GetIndexedName();
	//void SetIndex(int i) { mIndex = i; }

	/**	Builds a Max template for this state.*/
	void GenerateUICommand(DlgTemplateBuilder& builder) const;

public:
	//static uint32 getTotalRenderStateCount();
	//static FUDaePassState::State CgToColladaTranslation(const char* cgName);
	//static const char* ColladaToCgTranslation(FUDaePassState::State stateType);
	//static bool IsStateIndexed(FUDaePassState::State stateType);
	//static FUDaePassState::State TranslateState(_CGstate* state);
	///** Splits the name into a Collada render state name and an index.
	//	@param (in/out) In: indexed name, Out: name without index suffix.
	//	@return The index.*/
	//static uint GetIndexFromIndexedName(fm::string& colladaName);

	static ClassDesc2 *GetMyClassDesc();
};

#endif // _IC_EFFECT_RENDER_STATE_COLLECTION_H_
