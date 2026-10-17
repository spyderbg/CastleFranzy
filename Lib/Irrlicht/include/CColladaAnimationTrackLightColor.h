#ifndef __CCOLLADAANIMATIONTRACKLIGHTCOLOR_H__
#define __CCOLLADAANIMATIONTRACKLIGHTCOLOR_H__

#include "CColladaAnimationTrack.h"
#include "SColor.h"
#include "SMaterial.h"

namespace irr
{
namespace collada
{
namespace animation_track
{
class CLightColorEx
	: public collada::CAnimationTrackEx
{
public:
///////////////////////////////////////////////////////////////////////////////////////////////////
// static - used in CWeight and CWeightEx
///////////////////////////////////////////////////////////////////////////////////////////////////
	static inline int getValueSizeEx()
	{
		return 4 * sizeof(float);
	}

	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		video::SColorf* pColors = (video::SColorf*)pInputsArray;
		video::SColorf &output = *(video::SColorf*)pOutputPtr;
		if (iSize > 2)
		{
			video::SColorf result = pColors[0];
			float resultWeight = pWeightArray[0];
			for(int i = 1; i < iSize; i++)
			{
				video::SColorf &newColor = pColors[i];
				if (pWeightArray[i] != 0) {
					resultWeight += pWeightArray[i];
					float ratio = pWeightArray[i] / resultWeight;
					result = newColor.getInterpolated(result, ratio);
					//result = core::lerp(result, newColor, ratio);
				}
			}

			output = result;
		}
		else if (iSize == 2)
		{
			video::SColorf &k0 = pColors[0];
			video::SColorf &k1 = pColors[1];

			if (pWeightArray[0] == 0) 
			{
				output = k1;
			}
			else if(pWeightArray[1] == 0) 
			{
				output = k0;
			}
			else
			{
				float ratio = pWeightArray[1] / (pWeightArray[0] + pWeightArray[1]);
				output = k1.getInterpolated(k0, ratio);
				//output = core::lerp(k0, k1, ratio);
			}
		}
		else if (iSize == 1)
		{
			video::SColorf &k0 = pColors[0];
			output = k0;
		}
		else
		{
			_IRR_DEBUG_BREAK_IF("Not implemented");
		}
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		res::vector< core::vector3d<unsigned char> > &color = *(res::vector< core::vector3d<unsigned char> > *)&animation.getOutput();
		video::SColorf &output = *(video::SColorf*)pOutputPtr;
		core::vector3d<unsigned char> &k0 = color[iKey0];
		core::vector3d<unsigned char> &k1 = color[iKey1];
		output.R = core::lerp( k0.X / 255.0f, k1.X / 255.0f, ratio );
		output.G = core::lerp( k0.Y / 255.0f, k1.Y / 255.0f, ratio );
		output.B = core::lerp( k0.Z / 255.0f, k1.Z / 255.0f, ratio );
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		res::vector< core::vector3d<unsigned char> > &color = *(res::vector< core::vector3d<unsigned char> > *)&animation.getOutput();
		video::SColorf &output = *(video::SColorf*)pOutputPtr;
		core::vector3d<unsigned char> &test = color[iKey0];
		output.R = test.X / 255.0f;
		output.G = test.Y / 255.0f;
		output.B = test.Z / 255.0f;		
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

public:
	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
	{
		memcpy(pOutputPtr, pDataPtr, getValueSize());
	}

	virtual int getValueSize() const
	{
		return getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const
	{
		getKeyBasedValueEx(animation, iKey0, pOutputPtr);
	}

	static const CLightColorEx s_Instance;
};

class CLightColor
	: public CAnimationTrack
{
public:
	CLightColor(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return CLightColorEx::getValueSizeEx();
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		CLightColorEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		CLightColorEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		CLightColorEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &CLightColorEx::s_Instance;
	}
protected:

};


class CColorSetComponent
{
public:
	static inline void setComponent(video::SColor &color, video::SColor &value) { color = value; }
};

class CColorAlphaSetComponent
{
public:
	static inline void setComponent(video::SColor &color, unsigned char value) { color.A = value; }
};

class CColorRedSetComponent
{
public:
	static inline void setComponent(video::SColor &color, unsigned char value) { color.R = value; }
};

class CColorGreenSetComponent
{
public:
	static inline void setComponent(video::SColor &color, unsigned char value) { color.G = value; }
};

class CColorBlueSetComponent
{
public:
	static inline void setComponent(video::SColor &color, unsigned char value) { color.B = value; }
};

class CColorMaterialAmbientApplyValueEx
{
public:
	static inline void applyValueEx(void *material, void *color) 
	{
		video::SMaterial &mat = *(video::SMaterial*)material;
		mat.setAmbientColor(*(video::SColor*)color);
	}
};

class CColorMaterialDiffuseApplyValueEx
{
public:
	static inline void applyValueEx(void *material, void *color) 
	{
		video::SMaterial &mat = *(video::SMaterial*)material;
		mat.setDiffuseColor(*(video::SColor*)color);
	}
};

class CColorMaterialEmissiveApplyValueEx
{
public:
	static inline void applyValueEx(void *material, void *color) 
	{
		video::SMaterial &mat = *(video::SMaterial*)material;
		mat.setEmissiveColor(*(video::SColor*)color);
	}
};

class CColorMaterialSpecularApplyValueEx
{
public:
	static inline void applyValueEx(void *material, void *color) 
	{
		video::SMaterial &mat = *(video::SMaterial*)material;
		mat.setSpecularColor(*(video::SColor*)color);
	}
};

template <class CApplyValue, class CGetKeyBaseValue>
class CColorKeyBasedValueEx
	: public CApplyValue
	, public CGetKeyBaseValue
{
public:
	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		video::SColor output;
		CGetKeyBaseValue::getKeyBasedValueEx(animation, iKey0, iKey1, ratio, &output);
		CApplyValue::applyValueEx(pOutputPtr, &output);
	}

	static inline void applyKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		video::SColor output;
		CGetKeyBaseValue::getKeyBasedValueEx(animation, iKey0, &output);
		CApplyValue::applyValueEx(pOutputPtr, &output);
	}
};

template <class CSetComponent>
class CColorGetKeyBaseValueEx
	: public CSetComponent
{
public:
	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		res::vector< video::SColor > &color = *(res::vector< video::SColor > *)&animation.getOutput();
		video::SColor &output = *(video::SColor*)pOutputPtr;
		if( animation.pAnimationExtra != 0  )
		{
			output = *(video::SColor*)animation.pAnimationExtra; // DefaultValue
		}
		video::SColor &k0 = color[iKey0];
		video::SColor &k1 = color[iKey1];
		video::SColor lerp = k0.getInterpolated(k1, 1.0 - ratio);
		CSetComponent::setComponent(output, lerp);
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		res::vector< video::SColor > &color = *(res::vector< video::SColor > *)&animation.getOutput();
		video::SColor &output = *(video::SColor*)pOutputPtr;
		if( animation.pAnimationExtra != 0  )
		{
			output = *(video::SColor*)animation.pAnimationExtra; // DefaultValue
		}
		CSetComponent::setComponent(output, color[iKey0]);
	}
};

template <class CSetComponent>
class CColorComponentGetKeyBaseValueEx
	: public CSetComponent
{
public:
	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr)
	{
		res::vector< unsigned char > &color = *(res::vector< unsigned char > *)&animation.getOutput();
		video::SColor &output = *(video::SColor*)pOutputPtr;
		if(animation.pAnimationExtra)
		{
			output = *(video::SColor*)animation.pAnimationExtra; // DefaultValue
		}
		unsigned char &k0 = color[iKey0];
		unsigned char &k1 = color[iKey1];
		CSetComponent::setComponent(output, core::lerp( k0, k1, ratio ));
	}

	static inline void getKeyBasedValueEx(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr)
	{
		res::vector< unsigned char > &color = *(res::vector< unsigned char > *)&animation.getOutput();
		video::SColor &output = *(video::SColor*)pOutputPtr;
		if(animation.pAnimationExtra)
		{
			output = *(video::SColor*)animation.pAnimationExtra; // DefaultValue`<
		}
		CSetComponent::setComponent(output, color[iKey0]);
	}
};

#define TYPEDEF_CCGKBVEX(color, mat_component) \
	typedef CColorKeyBasedValueEx< \
			CColorMaterial##mat_component##ApplyValueEx, \
			CColorComponentGetKeyBaseValueEx<CColor##color##SetComponent> > \
				CColorMaterial##mat_component##color##GetKeyBaseValueEx

#define TYPEDEF_CCGKBVEX_FULLCOMPONENT(mat_component) \
	typedef CColorKeyBasedValueEx< \
			CColorMaterial##mat_component##ApplyValueEx, \
			CColorGetKeyBaseValueEx<CColorSetComponent> > \
				CColorMaterial##mat_component##GetKeyBaseValueEx

#define TYPEDEF_ALL_CCGKBVEX(mat_component) \
	TYPEDEF_CCGKBVEX_FULLCOMPONENT(mat_component); \
	TYPEDEF_CCGKBVEX(Alpha, mat_component); \
	TYPEDEF_CCGKBVEX(Red, mat_component);	\
	TYPEDEF_CCGKBVEX(Green, mat_component); \
	TYPEDEF_CCGKBVEX(Blue, mat_component);

TYPEDEF_ALL_CCGKBVEX(Ambient);
TYPEDEF_ALL_CCGKBVEX(Diffuse);
TYPEDEF_ALL_CCGKBVEX(Specular);
TYPEDEF_ALL_CCGKBVEX(Emissive);

#undef TYPEDEF_ALL_CCGKBVEX
#undef TYPEDEF_CCGKBVEX_FULLCOMPONENT
#undef TYPEDEF_CCGKBVEX

class CColorGetValueSizeEx
{
public:
	static inline int getValueSizeEx()
	{
		return sizeof(video::SColor);
	}
};

template <class TColorApplyValueEx>
class CColorGetBlendingValueEx
	: public TColorApplyValueEx
{
public:
	static inline void applyBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		SColor output;
		getBlendedValueEx(pInputsArray, pWeightArray, iSize, &output);
		TColorApplyValueEx::applyValueEx(pOutputPtr, &output);
	}
	
	static inline void getBlendedValueEx(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr)
	{
		video::SColor* pColors = (video::SColor*)pInputsArray;
		video::SColor &output = *(video::SColor*)pOutputPtr;
		if (iSize > 2)
		{
			video::SColor result = pColors[0];
			float resultWeight = pWeightArray[0];
			for(int i = 1; i < iSize; i++)
			{
				video::SColor &newColor = pColors[i];
				if (pWeightArray[i] != 0) {
					resultWeight += pWeightArray[i];
					float ratio = pWeightArray[i] / resultWeight;
					result = newColor.getInterpolated(result, ratio);
					//result = core::lerp(result, newColor, ratio);
				}
			}

			output = result;
		}
		else if (iSize == 2)
		{
			video::SColor &k0 = pColors[0];
			video::SColor &k1 = pColors[1];

			if (pWeightArray[0] == 0) 
			{
				output = k1;
			}
			else if(pWeightArray[1] == 0) 
			{
				output = k0;
			}
			else
			{
				float ratio = pWeightArray[1] / (pWeightArray[0] + pWeightArray[1]);
				output = k0.getInterpolated(k1, ratio);
			}
		}
		else if (iSize == 1)
		{
			video::SColor &k0 = pColors[0];
			output = k0;
		}
		else
		{
			_IRR_DEBUG_BREAK_IF("Not implemented");
		}
	}
};

template<class StaticBase>
class CColorCommonVirtualEx
	: public StaticBase
	, public CAnimationTrackEx
{
public:
	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
	{
		StaticBase::applyValueEx(pDataPtr, pOutputPtr);
	}

	virtual int getValueSize() const
	{
		return StaticBase::getValueSizeEx();
	}

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		StaticBase::applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		StaticBase::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		StaticBase::getKeyBasedValueEx(animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(const collada::SAnimation &animation, 
		int iKey0, void *pOutputPtr) const
	{
		StaticBase::getKeyBasedValueEx(animation, iKey0, pOutputPtr);
	}

	static const CColorCommonVirtualEx &getInstance() 
	{
		static const CColorCommonVirtualEx s_Instance;
		return s_Instance;
	}
	//static const CColorCommonVirtualEx<StaticBase> s_Instance;
};

template <	class GetBlendingValueEx, 
			class GetValueSizeEx, 
			class GetKeyBaseValueEx >
class CAnimationTrackExMixin
	: public GetBlendingValueEx
	, public GetValueSizeEx
	, public GetKeyBaseValueEx
{
};

template <	class TGetKeyBaseValueEx, 
			class TApplyValueEx >
class CColorCommonAnimationTrackExMixin
	: public CAnimationTrackExMixin<
				CColorGetBlendingValueEx<TApplyValueEx>,
				CColorGetValueSizeEx,
				TGetKeyBaseValueEx>
{
};

#define TYPEDEF_CCVE(color, mat_component) \
typedef CColorCommonVirtualEx< \
			CColorCommonAnimationTrackExMixin< \
				CColorMaterial##mat_component##color##GetKeyBaseValueEx, \
				CColorMaterial##mat_component##ApplyValueEx> > CColorMaterial##mat_component##color##Ex;

#define TYPEDEF_CCVE_FULLCOMPONENT(mat_component) \
typedef CColorCommonVirtualEx< \
			CColorCommonAnimationTrackExMixin< \
				CColorMaterial##mat_component##GetKeyBaseValueEx, \
				CColorMaterial##mat_component##ApplyValueEx> > CColorMaterial##mat_component##Ex;

#define TYPEDEF_ALL_CCVE(mat_component) \
	TYPEDEF_CCVE_FULLCOMPONENT(mat_component); \
	TYPEDEF_CCVE(Alpha, mat_component); \
	TYPEDEF_CCVE(Blue, mat_component); \
	TYPEDEF_CCVE(Green, mat_component); \
	TYPEDEF_CCVE(Red, mat_component)

TYPEDEF_ALL_CCVE(Ambient);
TYPEDEF_ALL_CCVE(Diffuse);
TYPEDEF_ALL_CCVE(Specular);
TYPEDEF_ALL_CCVE(Emissive);

#undef TYPEDEF_ALL_CCVE
#undef TYPEDEF_CCVE

template < class TColorCommonVirtualEx >
class CColorCommonVirtual
	: public CAnimationTrack
{
public:
	CColorCommonVirtual(collada::SAnimation &animation)
	: CAnimationTrack(animation)
	{
		
	}

	virtual int getValueSize() const
	{
		return TColorCommonVirtualEx::getValueSizeEx();
	}

	virtual void applyValue(void *pDataPtr, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::applyValueEx(pDataPtr, pOutputPtr);
	}

	virtual void applyBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::applyBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void getBlendedValue(void *pInputsArray, float *pWeightArray, int iSize, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::getBlendedValueEx(pInputsArray, pWeightArray, iSize, pOutputPtr);
	}

	virtual void applyKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::applyKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::getKeyBasedValueEx(m_Animation, iKey0, iKey1, ratio, pOutputPtr);
	}

	virtual void applyKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::applyKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual void getKeyBasedValue(int iKey0, void *pOutputPtr) const
	{
		TColorCommonVirtualEx::getKeyBasedValueEx(m_Animation, iKey0, pOutputPtr);
	}

	virtual const collada::CAnimationTrackEx* getAnimationTrackEx() const
	{
		return &TColorCommonVirtualEx::getInstance();
	}
protected:

};

#define TYPEDEF_CM(color, mat_component) \
	typedef CColorCommonVirtual<CColorMaterial##mat_component##color##Ex>	CColorMaterial##mat_component##color

#define TYPEDEF_ALL_CM(mat_component) \
	typedef CColorCommonVirtual<CColorMaterial##mat_component##Ex> CColorMaterial##mat_component; \
	TYPEDEF_CM(Alpha, mat_component); \
	TYPEDEF_CM(Red, mat_component); \
	TYPEDEF_CM(Green, mat_component); \
	TYPEDEF_CM(Blue, mat_component)

TYPEDEF_ALL_CM(Ambient);
TYPEDEF_ALL_CM(Diffuse);
TYPEDEF_ALL_CM(Specular);
TYPEDEF_ALL_CM(Emissive);


}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif // __CCOLLADAANIMATIONTRACKLIGHTCOLOR_H__