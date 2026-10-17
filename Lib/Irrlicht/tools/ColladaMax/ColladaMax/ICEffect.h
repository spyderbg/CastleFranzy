/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_EFFECT_H_
#define _IC_EFFECT_H_

#include "resource.h"

#ifndef _STDMAT2_ADAPTER_H_
#include "ColladaFX/StdMat2Adapter.h"
#endif // _STDMAT2_ADAPTER_H_

#include "ICEffectTechnique.h"

// generated using /3dsMax9/maxsdk/help/gencid.exe
#define IC_EFFECT_ID Class_ID(0x715552d2, 0x42a50a4)

// forward declarations
class ICEffectRenderer;
class ICEffectParamDlg;

/**
An hardware material plugin. This class sets Max internals properly in order to
take over the complete rendering of the geometries (Mesh, MNMesh) it affects.

It contains a ICEffectRenderer responsible for the actual DirectX 9.0 rendering.
*/
class ICEffect
	: public StdMat2Adapter
	, public IDX9DataBridge
	, public TimeChangeCallback
{
public:
	enum BlockIDs
	{
		parameters
	};

	enum ParamIDs
	{
		param_techniques,
		param_chosen_technique,
	};

	enum ParamBlock
	{
		PB_MAIN_ID
		, PB_MAIN_REF_ID = PB_MAIN_ID
		, PB_COUNT
	};

	//enum ParamId
	//{
	//	/*PI_MIN_BOX
	//	, PI_MAX_BOX
	//	, PI_DISPLAY_LINKS
	//	, PI_DISPLAY_BILLBOARDED_TARGETS
	//	, PI_DISPLAY_VECTORS_ON_TARGETS
	//	, PI_BILLBOARD_NODE_LIST
	//	, PI_BILLBOARD_TYPE
	//	, PI_BILLBOARD_ORIENTATION
	//	, PI_BILLBOARD_FRONT
	//	, PI_BILLBOARD_UP
	//	, */PI_COUNT
	//};

	//enum Rollup
	//{
	//	R_PARAMETERS
	//	//, R_BILLBOARDS
	//	//, R_BILLBOARD_CONTROLLER
	//	, R_COUNT
	//};

private:
	IParamBlock2* mParamBlock2;
	Interval mValidInterval;

	ICEffectRenderer* mDxRenderer;
	ICEffectParamDlg* mParamDlg;
	CustAttrib* mUniformParameters; // pointer to a parameter collection

private:
	void insertTechnique(ICEffectTechnique* tech);

public:
	ICEffect(BOOL isLoading = FALSE);
	virtual ~ICEffect();

	fstring getName();

	void setParamDlg(ICEffectParamDlg* dlg){ mParamDlg = dlg; }

	bool isNameUnique(const fstring& name);
	void sortTechniques();
	size_t getTechniqueCount();
	ICEffectTechnique* getTechnique(size_t idx);
	ICEffectTechnique* addTechnique(const fstring& name);
	ICEffectTechnique* removeTechnique(size_t index);

	ICEffectTechnique* getCurrentTechnique();
	ICEffectTechnique* setCurrentTechnique(size_t index);

	void releaseTextureParameters();
	void recreateTextureParameters(IDirect3DDevice9* pNewDevice);

	void reloadUniforms(ICEffectPass* selectedPass);

	// from Animatable
	// ===============
	SClass_ID SuperClassID(){ return MATERIAL_CLASS_ID; }
	Class_ID ClassID(){ return IC_EFFECT_ID;}

	// used to propagate notifications to our ICEffectRenderer.
	BaseInterface *GetInterface(Interface_ID id);

	// used to put the type name in the Material Editor
	void GetClassName( CStr &s ){ s = GetString( IDS_IC_EFFECT_NAME ); }
	void DeleteThis(){ delete this; }
	// parameter editing: called when the use "may" edit the item's parameters
	void BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev = NULL);
	void EndEditParams(IObjParam* ip, ULONG flags, Animatable* next = NULL);

	// these are not abstract, but we MUST override them or else automatic UI crashes
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int /*i*/) { return mParamBlock2; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (mParamBlock2->ID() == id) ? mParamBlock2 : NULL; }

	int NumSubs() {return 1;}
	Animatable* SubAnim(int i);
	TSTR SubAnimName(int i);
	int SubNumToRefNum(int subNum) {return subNum;}

	// used for linking the auxiliary files to the scene
	void EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags);

	// from ReferenceMaker
	// ===================
	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i);
	void SetReference(int i, RefTargetHandle rtarg);

	CLONE_REF_TARGET_HANDLE;

	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	   PartID& partID, RefMessage message);
	void NotifyChanged();

	// from MtlBase
	// ============
	void Reset();
	Interval Validity(TimeValue t);
	void Update(TimeValue t, Interval& valid);

	// UI methods
	BOOL SetDlgThing(ParamDlg* /*dlg*/) { return FALSE; }
	ParamDlg* CreateParamDlg(HWND hWindow, IMtlParams* params);

	// SubMaterial access methods (we have none)
	int NumSubMtls() {return 0;}
	Mtl* GetSubMtl(int /*i*/) { return NULL; }
	void SetSubMtl(int /*i*/, Mtl* /*m*/) {}
	TSTR GetSubMtlSlotName(int /*i*/) { return _T(""); }
	TSTR GetSubMtlTVName(int i) { return GetSubMtlSlotName(i); }

	// SubTexmap access methods (none)
	int NumSubTexmaps() {return 0;}
	Texmap* GetSubTexmap(int /*i*/) { return NULL; }
	void SetSubTexmap(int /*i*/, Texmap* /*m*/) {}
	TSTR GetSubTexmapSlotName(int /*i*/) { return _T(""); }
	TSTR GetSubTexmapTVName(int i) { return GetSubMtlSlotName(i); }

	// Mandatory override to get Max notifying us when rendering geometry
	ULONG Requirements(int subMtlNum);

	// from Mtl
	// ========
	virtual void Shade(ShadeContext& sc);
	float EvalDisplacement(ShadeContext& sc);
	Interval DisplacementValidity(TimeValue t);

	// IO (todo)
	// =========
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	// from IDX9DataBridge
	// ===================
	ParamDlg * CreateEffectDlg(HWND /*hWnd*/, IMtlParams* /*imp*/) {return NULL;}
	void DisableUI(){};
	void SetDXData(IHardwareMaterial * pHWMtl, Mtl * pMtl);
	TCHAR * GetName(){return _T("DxICEffect");}
	float GetDXVersion(){return 9.0f;}

	void TimeChanged(TimeValue t);
};

// prototype forward declaration
ClassDesc2* GetICEffectDesc();

#endif // _IC_EFFECT_H_
