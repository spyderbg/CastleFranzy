#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaLightSceneNode.h"

#include "SLight.h"
#include "SDatabaseCollada.h"

namespace irr
{
namespace collada
{

CLightSceneNode::CLightSceneNode(const CColladaDatabase& database, SLight &light)
: scene::CLightSceneNode(-1, core::vector3df(0,0,0), video::SColor(255,255,255,255), 1 )
, IObject(database)
, m_Light(light)
{
	video::SLight ldata;

	float fIntensityFactor = m_Light.intensity / 255.f;

	//Shut off the light specular component
	ldata.SpecularColor.A = m_Light.color.a * fIntensityFactor;
	ldata.SpecularColor.R = m_Light.color.r * fIntensityFactor;
	ldata.SpecularColor.G = m_Light.color.g * fIntensityFactor;
	ldata.SpecularColor.B = m_Light.color.b * fIntensityFactor;

	switch(m_Light.type)
	{
	case SLight::LT_POINT:
		ldata.Type = video::ELT_POINT;
		ldata.DiffuseColor.A = m_Light.color.a * fIntensityFactor;
		ldata.DiffuseColor.R = m_Light.color.r * fIntensityFactor;
		ldata.DiffuseColor.G = m_Light.color.g * fIntensityFactor;
		ldata.DiffuseColor.B = m_Light.color.b * fIntensityFactor;
		ldata.Attenuation.X = m_Light.pPoint->constantAttenuation;
		ldata.Attenuation.Y = m_Light.pPoint->linearAttenuation;
		ldata.Attenuation.Z = m_Light.pPoint->quadraticAttenuation;
		break;
	case SLight::LT_SPOT:
		ldata.Type = video::ELT_SPOT;
		ldata.DiffuseColor.A = m_Light.color.a * fIntensityFactor;
		ldata.DiffuseColor.R = m_Light.color.r * fIntensityFactor;
		ldata.DiffuseColor.G = m_Light.color.g * fIntensityFactor;
		ldata.DiffuseColor.B = m_Light.color.b * fIntensityFactor;
		ldata.Attenuation.X = m_Light.pSpot->constantAttenuation;
		ldata.Attenuation.Y = m_Light.pSpot->linearAttenuation;
		ldata.Attenuation.Z = m_Light.pSpot->quadraticAttenuation;
		ldata.OuterCone = m_Light.pSpot->falloffAngle;
		ldata.Falloff = m_Light.pSpot->falloffExponent;
		break;
	case SLight::LT_DIRECTIONNAL:
		ldata.Type = video::ELT_DIRECTIONAL;
		ldata.DiffuseColor.A = m_Light.color.a * fIntensityFactor;
		ldata.DiffuseColor.R = m_Light.color.r * fIntensityFactor;
		ldata.DiffuseColor.G = m_Light.color.g * fIntensityFactor;
		ldata.DiffuseColor.B = m_Light.color.b * fIntensityFactor;
		break;
	case SLight::LT_AMBIENT:
		ldata.Type = video::ELT_AMBIENT;
		ldata.AmbientColor.A = m_Light.color.a * fIntensityFactor;
		ldata.AmbientColor.R = m_Light.color.r * fIntensityFactor;
		ldata.AmbientColor.G = m_Light.color.g * fIntensityFactor;
		ldata.AmbientColor.B = m_Light.color.b * fIntensityFactor;
		ldata.DiffuseColor.A = 0.f;
		ldata.DiffuseColor.R = 0.f;
		ldata.DiffuseColor.G = 0.f;
		ldata.DiffuseColor.B = 0.f;
		ldata.SpecularColor.A = 0.f;
		ldata.SpecularColor.R = 0.f;
		ldata.SpecularColor.G = 0.f;
		ldata.SpecularColor.B = 0.f;
		break;
	}
	setLightData(ldata);
	doLightRecalc();
}

res::String	&
CLightSceneNode::getLightID() const
{
	return m_Light.id;
}

const char * 
CLightSceneNode::getUID() const
{
	return m_Light.id;
}

}; // namespace collada
}; // namespace irr

#endif //#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
