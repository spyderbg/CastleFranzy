
#include "stdafx.h"

#include "CColladaAnimationTrackLightColor.h"
#include "SColor.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

const CLightColorEx CLightColorEx::s_Instance;

//const CColorMaterialAmbientEx		CColorMaterialAmbientEx::s_Instance;
/*#define DECL_MAT(mat_component) \
	const CColorMaterial##mat_component##Ex		CColorMaterial##mat_component##Ex::s_Instance;

#define DECL_COLOR_MAT(color, mat_component) \
	const CColorMaterial##mat_component##color##Ex		CColorMaterial##mat_component##color##Ex::s_Instance;

#define DECL_ALL_COLOR_MAT(mat_component) \
	DECL_MAT(mat_component); \
	DECL_COLOR_MAT(Alpha, mat_component); \
	DECL_COLOR_MAT(Red, mat_component); \
	DECL_COLOR_MAT(Green, mat_component); \
	DECL_COLOR_MAT(Blue, mat_component);

DECL_ALL_COLOR_MAT(Ambient);
DECL_ALL_COLOR_MAT(Diffuse);
DECL_ALL_COLOR_MAT(Specular);
DECL_ALL_COLOR_MAT(Emissive);*/

}; // animation_track
}; // collada
}; // irr