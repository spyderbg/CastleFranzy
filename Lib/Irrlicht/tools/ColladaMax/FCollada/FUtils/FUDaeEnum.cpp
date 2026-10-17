/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the FS Import classes:
	Copyright (C) 2005-2006 Feeling Software Inc
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FUDaeEnum.h"
#include "FUDaeEnumSyntax.h"
#include "FUDaeSyntax.h"

namespace FUDaeCommon
{
	template <class EnumType>
	static EnumType FromString( const EnumMapping enumMap[], size_t mappingCount, EnumType unknown, const char *value )
	{
		int enumCount = ARRAY_SIZE( enumMap );
		for ( int enumId = 0; enumId < enumCount; ++enumId )
		{
			if ( IsEquivalent( value, enumMap[enumId].GetName() ) )
			{
				return ( EnumType )enumMap[enumId].GetValue();
			}
		}

		return unknown;
	}

	template <class EnumType>
	static const char *ToString( const EnumMapping enumMap[], size_t mappingCount, const char *unknown, EnumType value )
	{
		for ( int enumId = 0; enumId < mappingCount; ++enumId )
		{
			if ( ( EnumType )enumMap[enumId].GetValue() == value )
			{
				return enumMap[enumId].GetName();
			}
		}

		return unknown;
	}
}

namespace FUDaeAccessor
{
	const char* XY[3] = { "X", "Y", 0 };
	const char* XYZW[5] = { "X", "Y", "Z", "W", 0 };
	const char* RGBA[5] = { "R", "G", "B", "A", 0 };
	const char* ARGB[5] = { "A", "R", "G", "B", 0 };
	const char* STPQ[5] = { "S", "T", "P", "Q", 0 };
	const char* XYX_ANGLE[5] = { "X", "Y", "Z", "ANGLE", 0 };
};

namespace FUDaeInterpolation
{
	Interpolation FromString(const fm::string& value)
	{
		if ( value.empty() )
		{
			return BEZIER; // COLLADA 1.4.1, p4.92: application defined
		}
		else
		{
			return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value.c_str() );
		}
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};

namespace FUDaeSplineType
{
	Type FromString(const fm::string& value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value.c_str() );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAE_UNKNOWN_SPLINE_TYPE, value );
	}
};

namespace FUDaeSplineForm
{
	Form FromString(const fm::string& value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value.c_str() );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAE_UNKNOWN_SPLINE_FORM, value );
	}
};


namespace FUDaeTextureChannel
{
	Channel FromString(const fm::string& value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value.c_str() );
	}
};

namespace FUDaeTextureWrapMode
{

	WrapMode FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value );
	}
	
	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAE_TEXTURE_WRAP_UNKNOWN, value );
	}
};

/** Contains the texture filter functions.*/
namespace FUDaeTextureFilterFunction
{
	FilterFunction FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAE_TEXTURE_FILTER_UNKNOWN, value );
	}
};

namespace FUDaeMorphMethod
{
	Method FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), DEFAULT, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_MORPH_METHOD, value );
	}
};

namespace FUDaeInfinity
{

	Infinity FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), DEFAULT, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEMAYA_CONSTANT_INFINITY, value );
	}
};

namespace FUDaeBlendMode
{
	Mode FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
}

// START HERE
namespace FUDaeGeometryInput
{
	Semantic FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value );
	}

    const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
}

namespace FUDaeProfileType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), UNKNOWN, value );
	}

    const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
}

namespace FUDaePassStateFunction
{
	Function FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateStencilOperation
{
	Operation FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateBlendType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

/** The render state face types. */
namespace FUDaePassStateFaceType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateBlendEquation
{
	Equation FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateMaterialType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateFogType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}
	
	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
}

namespace FUDaePassStateFogCoordinateType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateFrontFaceType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};


/** The render state logic operations. */
namespace FUDaePassStateLogicOperation
{
	Operation FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

/** The render state polygon modes. */
namespace FUDaePassStatePolygonMode
{
	Mode FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

/** The render state shading model. */
namespace FUDaePassStateShadeModel
{
	Model FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassStateLightModelColorControlType
{
	Type FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaePassState
{
	State FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};

namespace FUDaeTextureOperator
{
	Operator FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaeTextureCombinerArgumentSource
{
	Source FromString(const char* value)
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), INVALID, value );
	}

	const char* ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_INPUT, value );
	}
};

namespace FUDaeBool
{
	Bool FromString( const char *value )
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), B_INVALID, value );
	}
	
	const char *ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};

namespace FUDaeBoolAbbreviated
{
	Bool FromString( const char *value )
	{
		return FUDaeCommon::FromString( enumMap, ARRAY_SIZE( enumMap ), B_INVALID, value );
	}
	
	const char *ToString( int value )
	{
		return FUDaeCommon::ToString( enumMap, ARRAY_SIZE( enumMap ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};

namespace FUDaeEmitter
{
	fstring GetParamVarianceName( fstring paramName )
	{
		FUStringBuilder builder;

		builder.append( paramName );
		builder.append( "_" );
		builder.append( DAE_EMIT_VARIANCE );

		return builder.ToString();
	}

	Shape FromStringShape( const char *value )
	{
		return FUDaeCommon::FromString( enumMapShape, ARRAY_SIZE( enumMapShape ), SHAPE_INVALID, value );
	}
	
	const char *ToStringShape( int value )
	{
		return FUDaeCommon::ToString( enumMapShape, ARRAY_SIZE( enumMapShape ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	ParticleDirectionMethod FromStringParticleDirectionMethod( const char *value )
	{
		return FUDaeCommon::FromString( enumMapParticleDirectionMethod, ARRAY_SIZE( enumMapParticleDirectionMethod ), PDM_INVALID, value );
	}
	
	const char *ToStringParticleDirectionMethod( int value )
	{
		return FUDaeCommon::ToString( enumMapParticleDirectionMethod, ARRAY_SIZE( enumMapParticleDirectionMethod ), DAEERR_UNKNOWN_ELEMENT, value );
	}
	ParticleType FromStringParticleType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapParticleType, ARRAY_SIZE( enumMapParticleType ), PT_INVALID, value );
	}
	
	const char *ToStringParticleType( int value )
	{
		return FUDaeCommon::ToString( enumMapParticleType, ARRAY_SIZE( enumMapParticleType ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	StdParticleType FromStringStdParticleType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapStdParticleType, ARRAY_SIZE( enumMapStdParticleType ), STD_PT_INVALID, value );
	}
	
	const char *ToStringStdParticleType( int value )
	{
		return FUDaeCommon::ToString( enumMapStdParticleType, ARRAY_SIZE( enumMapStdParticleType ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	ParticleSpinAxisType FromStringParticleSpinAxisType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapParticleSpinAxisType, ARRAY_SIZE( enumMapParticleSpinAxisType ), PSAT_INVALID, value );
	}
	
	const char *ToStringParticleSpinAxisType( int value )
	{
		return FUDaeCommon::ToString( enumMapParticleSpinAxisType, ARRAY_SIZE( enumMapParticleSpinAxisType ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	AnimationOffsetMethod FromStringAnimationOffsetMethod( const char *value )
	{
		return FUDaeCommon::FromString( enumMapAnimationOffsetMethod, ARRAY_SIZE( enumMapAnimationOffsetMethod ), AOM_INVALID, value );
	}
	
	const char *ToStringAnimationOffsetMethod( int value )
	{
		return FUDaeCommon::ToString( enumMapAnimationOffsetMethod, ARRAY_SIZE( enumMapAnimationOffsetMethod ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	AnimationScalingMethod FromStringAnimationScalingMethod( const char *value )
	{
		return FUDaeCommon::FromString( enumMapAnimationScalingMethod, ARRAY_SIZE( enumMapAnimationScalingMethod ), ASM_INVALID, value );
	}
	
	const char *ToStringAnimationScalingMethod( int value )
	{
		return FUDaeCommon::ToString( enumMapAnimationScalingMethod, ARRAY_SIZE( enumMapAnimationScalingMethod ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};

namespace FUDaeForce
{
	ForceType FromStringForceType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapForceType, ARRAY_SIZE( enumMapForceType ), FORCE_INVALID, value );
	}
	
	const char *ToStringForceType( int value )
	{
		return FUDaeCommon::ToString( enumMapForceType, ARRAY_SIZE( enumMapForceType ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	ForceParamsType FromStringForceParamsType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapForceParamsType, ARRAY_SIZE( enumMapForceParamsType ), FORCE_PARAMS_INVALID, value );
	}
	
	const char *ToStringForceParamsType( int value )
	{
		return FUDaeCommon::ToString( enumMapForceParamsType, ARRAY_SIZE( enumMapForceParamsType ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};

namespace FUDaeBillboard
{
	BillboardType FromStringBillboardType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapBillboardType, ARRAY_SIZE( enumMapBillboardType ), BT_INVALID, value );
	}
	
	const char *ToStringBillboardType( int value )
	{
		return FUDaeCommon::ToString( enumMapBillboardType, ARRAY_SIZE( enumMapBillboardType ), DAEERR_UNKNOWN_ELEMENT, value );
	}

	OrientationType FromStringOrientationType( const char *value )
	{
		return FUDaeCommon::FromString( enumMapOrientationType, ARRAY_SIZE( enumMapOrientationType ), OT_INVALID, value );
	}
	
	const char *ToStringOrientationType( int value )
	{
		return FUDaeCommon::ToString( enumMapOrientationType, ARRAY_SIZE( enumMapOrientationType ), DAEERR_UNKNOWN_ELEMENT, value );
	}
};