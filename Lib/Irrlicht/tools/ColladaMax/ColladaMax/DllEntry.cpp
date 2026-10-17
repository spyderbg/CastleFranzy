/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ColladaAttrib.h"
#include "ColladaDocumentContainer.h"
#include "ColladaExporter.h"
#include "ColladaImporter.h"
#include "SimpleAttrib.h"
#include "ColladaFX/ColladaEffect.h"
#include "ColladaFX/ColladaEffectTechnique.h"
#include "ColladaFX/ColladaEffectPass.h"
#include "ColladaFX/ColladaEffectShader.h"
#include "ColladaFX/ColladaEffectParameter.h"
#include "ICEffect.h"
#include "ICEffectTechnique.h"
#include "ICEffectPass.h"
#include "ICEffectShader.h"
#include "ICEffectParameter.h"
#include "FColladaPlugin.h"
#include "Controller/ColladaController.h"
#include "IrrCollada.h"
#include "ICSuprPrts.h"
#include "ICAnimationSelector.h"
#include "ICGravity.h"
#include "ICWind.h"
#include "ICDeflector.h"
#include "ICModifierBillboard.h"
#include "ICModularSkinMaster.h"
#include "ICModularSkinCategory.h"
#include "ICBillboardController.h"
#include "ICBillboardHelper.h"
#include "ICMaterial.h"


// Uncomment the line below to run the visual leak detector: you'll also need to
// add the correct folder to the linker includes.
//#include "../External/VisualLeakDetector/vld.h"

HINSTANCE hInstance;
int controlsInit = FALSE;
fm::pvector<ClassDesc> classDescArray;

void InitClassDescArray()
{
	classDescArray.clear();

	classDescArray.push_back( GetCOLLADAExporterDesc() );
	classDescArray.push_back( GetCOLLADAImporterDesc() );

#if ENABLE_IRR_COLLADA_PLUGIN_CLASS
	classDescArray.push_back( GetIrrColladaClassDesc() );
#endif

#if ENABLE_ICPCLOUD_PLUGIN_CLASS
    classDescArray.push_back( GetICPCloudDesc() );
#endif

#if ENABLE_DBG_ICPCLOUD_PLUGIN_CLASS
	classDescArray.push_back( GetDbgPCloudDesc() );
#endif

#if ENABLE_ANIMATION_SELECTOR_PLUGIN_CLASS
	classDescArray.push_back( GetAnimationSelectorClassDesc() );
#endif

#if ENABLE_ICGRAVITY_OBJECT_PLUGIN_CLASS
	classDescArray.push_back( GetICGravityObjDesc() );
#endif

#if ENABLE_ICGRAVITY_MODIFIER_PLUGIN_CLASS
	classDescArray.push_back( GetICGravityModDesc() );
#endif

#if ENABLE_ICWIND_OBJECT_PLUGIN_CLASS
	classDescArray.push_back( GetICWindObjDesc() );
#endif

#if ENABLE_ICWIND_MODIFIER_PLUGIN_CLASS
	classDescArray.push_back( GetICWindModDesc() );
#endif

#if ENABLE_ICDEFLECTOR_OBJECT_PLUGIN_CLASS
	classDescArray.push_back( GetICDeflectObjDesc() );
#endif

#if ENABLE_ICDEFLECTOR_MODIFIER_PLUGIN_CLASS
	classDescArray.push_back( GetICDeflectModDesc() );
#endif

#if ENABLE_ICOSM_BILLBOARD
	classDescArray.push_back( ICBillboardWSMObject::GetMyClassDesc() );
#endif

#if ENABLE_ICWSM_BILLBOARD
	classDescArray.push_back( ICBillboardWSM::GetMyClassDesc() );
#endif

#if ENABLE_IC_MODULAR_SKIN_MASTER
	classDescArray.push_back( GetICModularSkinMasterObjDesc() );
#endif

#if ENABLE_IC_MODULAR_SKIN_CATEGORY
	classDescArray.push_back( GetICModularSkinCategoryObjDesc() );
#endif

#if ENABLE_IC_BILLBOARD_CONTROLLER
	classDescArray.push_back( GetICBillboardControllerDesc() );
#endif

#if ENABLE_IC_BILLBOARD_HELPER
	classDescArray.push_back( GetICBillboardHelperDesc() );
#endif

#if ENABLE_COLLADA_EFFECT
	//classDescArray.push_back( GetSimpleAttribDesc() );
	//classDescArray.push_back( GetCOLLADAAttribDesc() );
	//classDescArray.push_back( GetCOLLADADocumentContainerDesc() );

	classDescArray.push_back( GetCOLLADAEffectDesc() );
	classDescArray.push_back( GetColladaEffectTechniqueClassDesc() );
	classDescArray.push_back( GetColladaEffectPassClassDesc() );
	classDescArray.push_back( GetColladaEffectShaderClassDesc() );
	classDescArray.push_back( GetColladaEffectParameterCollectionClassDesc() );
#endif

#if ENABLE_IC_EFFECT
	classDescArray.push_back( GetICEffectDesc() );
	classDescArray.push_back( GetICEffectTechniqueClassDesc() );
	classDescArray.push_back( GetICEffectPassClassDesc() );
	classDescArray.push_back( GetICEffectShaderClassDesc() );
	classDescArray.push_back( GetICEffectParameterCollectionClassDesc() );
#endif //

#if ENABLE_IC_MATERIAL
	classDescArray.push_back( GetICMaterialDesc() );
#endif //


}
 
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID UNUSED(lpvReserved))
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hinstDLL;
#if MAX_VERSION_MAJOR < 10
		InitCustomControls(hInstance);
#endif // pre-Max 2008 only.
		FCollada::Initialize();
		break;

	case DLL_PROCESS_DETACH:
		FCollada::Release();
		break;
	}
	return TRUE;
}

__declspec(dllexport) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

__declspec(dllexport) int LibNumberClasses()
{
	InitClassDescArray();
	return classDescArray.size();
}

__declspec(dllexport) ClassDesc* LibClassDesc(int i)
{
	// I do not believe this is necessary, however it was in the particle sample
	if ( classDescArray.size() <= 0 )
	{
		InitClassDescArray();
	}

	return i < classDescArray.size() ? classDescArray[i] : NULL;
}

__declspec(dllexport) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
//__declspec(dllexport) ULONG CanAutoDefer()
//{
//	return 1;
//}

TCHAR* GetString(int id)
{
	static TCHAR buf[1024];
	return (hInstance != NULL && LoadString(hInstance, id, buf, sizeof(buf))) ? buf : NULL;
}
