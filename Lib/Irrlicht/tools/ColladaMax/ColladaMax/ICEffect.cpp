/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICEffect.h"
#include "ICEffectPass.h"
#include "ICEffectRenderer.h"
#include "ICEffectParamDlg.h"
#include "FUtils/FUFileManager.h"

// Used in the ICEffectClassDesc class descriptor.
#define DXMATERIAL_DYNAMIC_UI Class_ID(0xef12512, 0x11351ed1)

/**
A class descriptor for the ICEffect class.
*/
class ICEffectClassDesc : public ClassDesc2
{
public:
	int				IsPublic() {return TRUE; }
	void*			Create(BOOL isLoading)
	{
		ICEffect*	effect = new ICEffect(isLoading);

		effect->SetMtlFlag(MTL_DISPLAY_ENABLE_FLAGS,true);
		effect->ClearMtlFlag(MTL_TEX_DISPLAY_ENABLED);

		// tell Max we'll use the hardware renderer
		effect->SetMtlFlag(MTL_HW_MAT_ENABLED, true);

		return effect;
	}

	const TCHAR*	ClassName() {return GetString(IDS_IC_EFFECT_CLASS_NAME);}
	SClass_ID		SuperClassID() {return MATERIAL_CLASS_ID;}
	Class_ID 		ClassID() {return IC_EFFECT_ID;}

	// This disables the DX Manager that hooks to any material in the editor.
	Class_ID		SubClassID() { return DXMATERIAL_DYNAMIC_UI; }

	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("ICEffect"); }
	HINSTANCE		HInstance() { return hInstance; }
};

static ICEffectClassDesc gICEffectCD;
ClassDesc2* GetICEffectDesc()
{
	return &gICEffectCD;
}

class ICEffectPBAccessor : public PBAccessor
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

static ICEffectPBAccessor gICEffectPBAccessor;

static ParamBlockDesc2 gICEffectPBDesc
(
	ICEffect::PB_MAIN_ID
	, _T("parameters")
	,  0
	, &gICEffectCD
	, P_AUTO_CONSTRUCT
	, 0
	,
	// params
	// ParamID, internal_name, ParamType, [tab size], flags, local_name_res_ID, <Param Tags>

	ICEffect::param_techniques, _T("techniques"), TYPE_REFTARG_TAB, 0, P_VARIABLE_SIZE, 0,
		p_accessor, &gICEffectPBAccessor,
		end,

	ICEffect::param_chosen_technique, _T("chosen_technique"), TYPE_REFTARG, 0, 0,
		p_accessor, &gICEffectPBAccessor,
		end,
	
	end
);

// callback function
static void ShaderUpdate(void *param, NotifyInfo *info) {

	MtlBase * mtl = (MtlBase*)param;

	if (info->intcode == NOTIFY_HW_TEXTURE_CHANGED && mtl == (MtlBase*)info->callParam)
	{
		GetCOREInterface()->ForceCompleteRedraw();
	}
}

// ICEffect class

ICEffect::ICEffect(BOOL isLoading)
: mValidInterval(FOREVER)
, mDxRenderer(NULL)
, mUniformParameters(NULL)
, mParamDlg( NULL )
{
	SetReference( PB_MAIN_REF_ID, NULL );
	gICEffectCD.MakeAutoParamBlocks( this );

	if ( mDxRenderer != NULL )
	{
		delete mDxRenderer;
		mDxRenderer = NULL;
	}

	mDxRenderer = new ICEffectRenderer( this );
	RegisterNotification( ShaderUpdate, this, NOTIFY_HW_TEXTURE_CHANGED );

	if ( !isLoading )
	{
		Reset();
	}

	GetCOREInterface()->RegisterTimeChangeCallback( this );

	// debug
	{
		ICEffectTechnique *pEffectTechnique = addTechnique( "t0" );
		setCurrentTechnique( 0 );

		ICEffectPass *pass = pEffectTechnique->addPass( "p0" );
		pEffectTechnique->setEditedPass( 0 );

		fstring file = FS("C:\\Program Files\\Autodesk\\3ds Max 9\\maps\\fx\\BumpReflect.cgfx");
		bool isCgFX = fstricmp(FUFileManager::GetFileExtension(file),"cgfx") == 0;
		bool isSuccess = false;

		if (isCgFX)
		{
			//// choose from which technique/pass
			//{
			//	StringList techNames;
			//	UInt32List passCounts;
			//	StringList passNames;
			//	fstring errorString;

			//	ColladaEffectShader::GetEffectInfo( file, techNames, passCounts, passNames, errorString);
			//}

			//if ( QueryEffectTargets( file ) )
			//{
				isSuccess = pass->loadEffect( file, 0, 0 );
			//}
		}
		//else
		//{
		//	// load Cg Shader
		//	isSuccess = pass->loadShader(isVertex,file, fstring(entry));
		//}

		////UpdateShaders();

		//// load-specific error messages
		//if (isSuccess && pass->isValid())
		//{
		//	//UpdateErrorMessage(file + FS(" loaded successfully!"));
		//}
		//else
		//{
		//	//UpdateErrorMessage(FS("Errors while loading ") + file + FS("\n") + pass->getErrorString());
		//}

		if ( isSuccess )
		{
			reloadUniforms(pass);	
		}
		else
		{
			reloadUniforms(NULL);
		}
		// at this point, you may have been deleted...
		// By changing the content of the custom attrib container, you invalidate the UI.

		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
	}
}

ICEffect::~ICEffect()
{
	SAFE_DELETE(mDxRenderer);
	UnRegisterNotification(ShaderUpdate,this,NOTIFY_HW_TEXTURE_CHANGED);
	GetCOREInterface()->UnRegisterTimeChangeCallback(this);
}

fstring ICEffect::getName()
{
	fstring name = FS(Mtl::GetName());
	return name;
}

size_t ICEffect::getTechniqueCount()
{
	int count = mParamBlock2->Count(ICEffect::param_techniques);
	return (size_t)count;
	//return 0;
}

ICEffectTechnique* ICEffect::getTechnique(size_t idx)
{
	if (idx >= getTechniqueCount())
		return NULL;

	ReferenceTarget* ref = NULL;
	Interval valid;
	BOOL result = mParamBlock2->GetValue(ICEffect::param_techniques, 0, ref, valid, (int)idx);
	return (result == TRUE) ? (ICEffectTechnique*)ref : NULL;
	//return NULL;
}

void ICEffect::insertTechnique(ICEffectTechnique* tech)
{
	ReferenceTarget* refArray[1] = {tech};
	mParamBlock2->Append(ICEffect::param_techniques, 1, refArray);
	mParamBlock2->Shrink(ICEffect::param_techniques);
	sortTechniques();
}

static int __cdecl techSortPred(const void* t1v, const void* t2v)
{
	// No documentation about this, but the TYPE_REFTARG_TAB stores PB2Value*.
	ICEffectTechnique* t1 = (ICEffectTechnique*)((PB2Value*)t1v)->r;
	ICEffectTechnique* t2 = (ICEffectTechnique*)((PB2Value*)t2v)->r;
	return fstrcmp(t1->getName().c_str(), t2->getName().c_str());
	//return 0;
}

void ICEffect::sortTechniques()
{
	mParamBlock2->Sort(ICEffect::param_techniques, techSortPred);
}

bool ICEffect::isNameUnique(const fstring& name)
{
	size_t tCount = getTechniqueCount();
	for (size_t i = 0; i < tCount; ++i)
	{
		ICEffectTechnique* t = getTechnique(i);
		if (fstrcmp(name.c_str(),t->getName().c_str()) == 0) return false;
	}
	return true;
	//return true;
}

ICEffectTechnique* ICEffect::addTechnique(const fstring& name)
{
	if (!isNameUnique(name)) return NULL;
	ICEffectTechnique* tech = new ICEffectTechnique(this,name);
	insertTechnique(tech);
	return tech;
	//return NULL;
}

ICEffectTechnique* ICEffect::removeTechnique(size_t index)
{
	if (index >= getTechniqueCount())
		return NULL;

	ICEffectTechnique* tech = getTechnique(index);
	mParamBlock2->Delete(ICEffect::param_techniques, (int)index, 1);
	mParamBlock2->Shrink(ICEffect::param_techniques);
	return tech;
	//return NULL;
}

ICEffectTechnique* ICEffect::getCurrentTechnique()
{
	ReferenceTarget* ref = NULL;
	Interval valid;
	mParamBlock2->GetValue(ICEffect::param_chosen_technique, 0, ref, valid);
	return (ICEffectTechnique*)ref;
	//return NULL;
}

ICEffectTechnique* ICEffect::setCurrentTechnique(size_t index)
{
	ReferenceTarget* ref = (ReferenceTarget*)getTechnique(index);
	mParamBlock2->SetValue(ICEffect::param_chosen_technique, 0, ref);
	return (ICEffectTechnique*)ref;
	//return NULL;
}

void ICEffect::releaseTextureParameters()
{
	size_t techCount = getTechniqueCount();
	for (size_t i = 0; i < techCount; ++i)
	{
		ICEffectTechnique* tech = getTechnique(i);
		size_t passCount = tech->getPassCount();
		for (size_t j = 0; j < passCount; ++j)
		{
			ICEffectPass* pass = tech->getPass(j);
			pass->releaseTextureParameters();
		}
	}
}

void ICEffect::recreateTextureParameters(IDirect3DDevice9* pNewDevice)
{
	size_t techCount = getTechniqueCount();
	for (size_t i = 0; i < techCount; ++i)
	{
		ICEffectTechnique* tech = getTechnique(i);
		size_t passCount = tech->getPassCount();
		for (size_t j = 0; j < passCount; ++j)
		{
			ICEffectPass* pass = tech->getPass(j);
			pass->recreateTextureParameters(pNewDevice);
		}
	}
}

void ICEffect::reloadUniforms(ICEffectPass* selectedPass)
{
	ICustAttribContainer* customAttributeContainer = this->GetCustAttribContainer();
	CustAttrib* newAttrib = (selectedPass == NULL) ? NULL : selectedPass->getParameterCollection();
	bool replaced = false;

	if (customAttributeContainer != NULL)
	{
		for (int i = 0; i < customAttributeContainer->GetNumCustAttribs(); ++i)
		{
			CustAttrib* ca = customAttributeContainer->GetCustAttrib(i);
			if (ca == NULL) continue;
			if (ca == mUniformParameters)
			{
				if (newAttrib != NULL)
					customAttributeContainer->SetCustAttrib(i, newAttrib);
				else
					customAttributeContainer->RemoveCustAttrib(i);
				replaced = true;
				break;
			}
		}
	}
	else
	{
		this->AllocCustAttribContainer();
		customAttributeContainer = this->GetCustAttribContainer();
	}

	if (!replaced && customAttributeContainer != NULL && newAttrib != NULL)
	{
		customAttributeContainer->AppendCustAttrib(newAttrib);
	}

	mUniformParameters = newAttrib;
	//mUniformParameters = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// ANIMATABLE

BaseInterface* ICEffect::GetInterface(Interface_ID id)
{
	if (id == VIEWPORT_SHADER_CLIENT_INTERFACE) {
		return static_cast<IDXDataBridge*>(this);
	}
	else if (id == VIEWPORT_SHADER9_CLIENT_INTERFACE) {
		return static_cast<IDXDataBridge*>(this);
	}
	else if (id == DX9_VERTEX_SHADER_INTERFACE_ID) {
		// Max requests this from Mesh::Render, D3DWindow::InitializeShaders
		return static_cast<IDX9VertexShader *>(mDxRenderer);
	}
	else {
		return StdMat2::GetInterface(id);
	}
}

Animatable* ICEffect::SubAnim(int i)
{
	switch (i) {
	case 0: return mParamBlock2;
	default: return NULL;
	}
}

TSTR ICEffect::SubAnimName(int i)
{
	switch (i) {
	case 0: return GetString(IDS_CE_NAME);
	default: return _T("");
	}
}

void ICEffect::BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev)
{
	StdMat2::BeginEditParams(ip,flags,prev);
}

void ICEffect::EndEditParams(IObjParam* ip, ULONG flags, Animatable* next)
{
	StdMat2::EndEditParams(ip,flags,next);
}

void ICEffect::EnumAuxFiles(NameEnumCallback& nameEnum, DWORD flags)
{
	if ((flags & FILE_ENUM_CHECK_AWORK1) && TestAFlag(A_WORK1)) return;

	// sample taken from the MAX SDK
	//if (GetCubeMapFile())
	//{
	//	if (flags & FILE_ENUM_ACCESSOR_INTERFACE)	{
	//		IEnumAuxAssetsCallback* callback = static_cast<IEnumAuxAssetsCallback*>(&nameEnum);
	//		callback->DeclareAsset(CubeMapFileAccessor(this));
	//	}
	//	else	{
	//		MaxSDK::Util::Path path(GetCubeMapFile());
	//		IPathConfigMgr::GetPathConfigMgr()->
	//			RecordInputAsset(path, nameEnum, flags);
	//	}
	//	
	//}
	
	//if (GetVertexShaderFile())
	//{
	//	if (flags & FILE_ENUM_ACCESSOR_INTERFACE)	{
	//		IEnumAuxAssetsCallback* callback = static_cast<IEnumAuxAssetsCallback*>(&nameEnum);
	//		callback->DeclareAsset(CubeMapVertexAccessor(this));
	//	}
	//	else	{
	//		MaxSDK::Util::Path path(GetVertexShaderFile());
	//		IPathConfigMgr::GetPathConfigMgr()->
	//			RecordInputAsset(path, nameEnum, flags);
	//	}
	//}
	
	ReferenceTarget::EnumAuxFiles(nameEnum, flags);
}

///////////////////////////////////////////////////////////////////////////////
// REFERENCEMAKER

RefTargetHandle ICEffect::GetReference(int i)
{
	if ( i == ICEffect::PB_MAIN_REF_ID )
	{
		return mParamBlock2;
	}
	else
	{
		return NULL;
	}
}

void ICEffect::SetReference(int i, RefTargetHandle rtarg)
{
	if ( i == ICEffect::PB_MAIN_REF_ID )
	{
		mParamBlock2 = (IParamBlock2*) rtarg;
	}
}

RefTargetHandle ICEffect::Clone(RemapDir &remap)
{
	// create new instance
	ICEffect *mnew = new ICEffect(FALSE);

	// copy superclass
	*((MtlBase*)mnew) = *((MtlBase*)this); 

	// clone parameter block
	mnew->ReplaceReference(0,remap.CloneRef(mParamBlock2));

	// clear valid interval
	mnew->mValidInterval.SetEmpty();

	// clone base
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

RefResult ICEffect::NotifyRefChanged(
	Interval UNUSED(changeInt),
	RefTargetHandle hTarget, 
	PartID& UNUSED(partID),
	RefMessage message)
{
		switch (message) {
		case REFMSG_CHANGE:
			mValidInterval.SetEmpty();
			if (hTarget == mParamBlock2){
				// see if this message came from a changing parameter in the pblock,
				// if so, limit rollout update to the changing item and update any active viewport texture
				//ParamID changing_param = mParamBlock2->LastNotifyParamID();
				//gICEffectPBDesc.InvalidateUI(changing_param);
			}

			return (REF_SUCCEED);
		}// end, switch

	return (REF_DONTCARE);
}

void ICEffect::NotifyChanged()
{
	NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE);
}

///////////////////////////////////////////////////////////////////////////////
// MTLBASE

void ICEffect::Reset()
{
	mValidInterval.SetEmpty();

	// reset parameters
	//mParamBlock2->ResetAll();
	//DeleteReference if you keep any
	NotifyChanged();
}

ULONG ICEffect::Requirements(int UNUSED(subMtlNum)) 
{
	// Requirements information provided by Jean-Francois Yelle, Autodesk.
	return (MTLREQ_TRANSP | MTLREQ_TRANSP_IN_VP);
}

Interval ICEffect::Validity(TimeValue UNUSED(t))
{
	return mValidInterval;
}

void ICEffect::Update(TimeValue t, Interval& valid)
{
	if ( !mValidInterval.InInterval( t ) )
	{
		mValidInterval.SetInfinite();
	}

	valid &= mValidInterval;
}

// create the template UI
ParamDlg* ICEffect::CreateParamDlg(HWND hWindow, IMtlParams* params)
{
	ICEffectParamDlg *dlg = new ICEffectParamDlg( hWindow, params, this );
	dlg->LoadDialog( TRUE );
	setParamDlg( dlg );
	return dlg;
}

///////////////////////////////////////////////////////////////////////////////
// MTL

void ICEffect::Shade(ShadeContext& UNUSED(sc))
{
	
}

float ICEffect::EvalDisplacement(ShadeContext& UNUSED(sc))
{
	return 0.0f;
}

Interval ICEffect::DisplacementValidity(TimeValue UNUSED(t))
{
	return FOREVER;
}

///////////////////////////////////////////////////////////////////////////////
// IO

#define MTL_HDR_CHUNK 0x4000

IOResult ICEffect::Save(ISave *isave)
{
	IOResult res;
	isave->BeginChunk( MTL_HDR_CHUNK );
	res = MtlBase::Save( isave );
	if ( res != IO_OK )
	{
		return res;
	}
	isave->EndChunk();

	return IO_OK;
}

IOResult ICEffect::Load(ILoad *iload)
{
	IOResult res;
	int id;
	while ( IO_OK == ( res = iload->OpenChunk() ) )
	{
		switch ( id = iload->CurChunkID() )
		{
			case MTL_HDR_CHUNK:
			{
				res = MtlBase::Load( iload );
				break;
			}
		}

		iload->CloseChunk();
		if ( res != IO_OK ) 
		{
			return res;
		}
	}

	return IO_OK;
}

///////////////////////////////////////////////////////////////////////////////
// IDX9DataBridge

void ICEffect::SetDXData(IHardwareMaterial * UNUSED(pHWMtl), Mtl* UNUSED(pMtl))
{
	// no op.
}

void ICEffect::TimeChanged(TimeValue t)
{
	NotifyChanged();
}
