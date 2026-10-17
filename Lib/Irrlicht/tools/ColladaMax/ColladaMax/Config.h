#ifndef _CONFIG_H_
#define _CONFIG_H_

#define ENABLE_IRR_COLLADA_PLUGIN_CLASS 1
#define ENABLE_ANIMATION_SELECTOR_PLUGIN_CLASS 1 // if this is disabled no clips will be exported

#define ENABLE_ICPCLOUD_PLUGIN_CLASS 1
#define ENABLE_ICGRAVITY_OBJECT_PLUGIN_CLASS 1
#define ENABLE_ICGRAVITY_MODIFIER_PLUGIN_CLASS 1
#define ENABLE_ICWIND_OBJECT_PLUGIN_CLASS 1
#define ENABLE_ICWIND_MODIFIER_PLUGIN_CLASS 1
#define ENABLE_ICDEFLECTOR_OBJECT_PLUGIN_CLASS 1
#define ENABLE_ICDEFLECTOR_MODIFIER_PLUGIN_CLASS 1
#define ENABLE_IC_MODULAR_SKIN_MASTER 1
#define ENABLE_IC_MODULAR_SKIN_CATEGORY 1
#define ENABLE_ICOSM_BILLBOARD 1
#define ENABLE_ICWSM_BILLBOARD 1

#define OVERRIDE_SIMPLE_MATERIAL_NAME_WITH_DAE_ID 0
#define EXPORT_MODULAR_SKIN_CATEGORY_AS_NODE 0



// ----------------------------------------------------------------------
// To build exporter to recover files corrupted by writing 
// event group version in the wrong place
// ----------------------------------------------------------------------
#define EVENT_GROUP_RECOVERY_BUILD 0

// ----------------------------------------------------------------------
// Debug/Release dependant config
// ----------------------------------------------------------------------
#ifdef _DEBUG
#define DEBUG_BUILD ( 1 )
#define RELEASE_BUILD ( 0 )
#else 
#define DEBUG_BUILD ( 0 )
#define RELEASE_BUILD ( 1 )
#endif

#define ENABLE_DBG_ICPCLOUD_PLUGIN_CLASS ( DEBUG_BUILD )

#define ENABLE_IC_BILLBOARD_CONTROLLER 0
#define ENABLE_IC_BILLBOARD_HELPER 0
#define ENABLE_COLLADA_EFFECT 0
#define ENABLE_IC_EFFECT 0
#define ENABLE_IC_MATERIAL ( DEBUG_BUILD )

#endif // _CONFIG_H_