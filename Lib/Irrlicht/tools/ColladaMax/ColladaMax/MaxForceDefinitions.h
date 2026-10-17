/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _MAX_FORCE_DEFINITIONS_H_
#define _MAX_FORCE_DEFINITIONS_H_

// From maxsdk/samples/modifiers/gravity.cpp sample.
namespace FSForceGravity
{
	const Class_ID IC_OBJECT_CLASSID = Class_ID( 0x64257266, 0 );
	const Class_ID IC_MODIFIER_CLASSID = Class_ID( 0x44345d43, 0 );
	const Class_ID OBJECT_CLASS_ID = Class_ID(GRAVITYOBJECT_CLASS_ID, 0);
	const Class_ID MODIFIER_CLASS_ID = Class_ID(GRAVITYMOD_CLASS_ID, 0);

	const uint32 PBLOCK_REF_NO = 0;

	// param block IDs
	enum { gravity_params, };

	// geo_param param IDs
	enum { PB_STRENGTH, PB_DECAY, PB_TYPE, PB_DISPLENGTH, PB_HOOPSON };
};

// From maxsdk/samples/modifiers/deflect.cpp sample.
namespace FSDeflector
{
	const Class_ID IC_OBJECT_CLASSID = Class_ID( 0x389f3357, 0 );
	const Class_ID IC_MODIFIER_CLASSID = Class_ID( 0x68e573a1, 0 );
	const Class_ID OBJECT_CLASS_ID = Class_ID(DEFLECTOBJECT_CLASS_ID,0);
	const Class_ID MODIFIER_CLASS_ID = Class_ID(DEFLECTMOD_CLASS_ID,0);

	const uint32 PBLOCK_REF_NO = 0;

	// block IDs
	enum { deflect_params, };

	// geo_param param IDs
	enum { deflect_bounce, deflect_width, deflect_height, deflect_variation, deflect_chaos, deflect_friction, 
		   deflect_inherit_vel,deflect_quality,deflect_collider};
};

// From maxsdk/samples/objects/sphered.cpp sample.
namespace FSForceSDeflector
{
	const Class_ID OBJECT_CLASS_ID = Class_ID(0x6cbd289d, 0x3fef6656);

	const uint32 PBLOCK_REF_NO = 0;

	// block IDs
	enum { deflect_params };

	enum 
	{	sdeflectrobj_bounce, 
		sdeflectrobj_bouncevar,
		sdeflectrobj_chaos,
		sdeflectrobj_diameter, // wrongly called "radius" in the ParamBlock definition.
		sdeflectrobj_velocity,
		sdeflectrobj_friction,
		sdeflectrobj_collider
	};
};

// From maxsdk/samples/modifiers/wind.cpp sample.
namespace FSForceWind
{
	const Class_ID IC_OBJECT_CLASSID = Class_ID( 0x42e3006d, 0 );
	const Class_ID IC_MODIFIER_CLASSID = Class_ID( 0x655d2738, 0 );
	const Class_ID OBJECT_CLASS_ID = Class_ID(WINDOBJECT_CLASS_ID,0);
	const Class_ID MODIFIER_CLASS_ID = Class_ID(WINDMOD_CLASS_ID,0);

	const uint32 PBLOCK_REF_NO = 0;

	// block IDs
	enum { wind_params, };

	// geo_param param IDs
	enum { PB_STRENGTH,PB_DECAY,PB_TYPE,PB_DISPLENGTH,PB_TURBULENCE,PB_FREQUENCY,PB_SCALE,PB_HOOPSON };
};

// From maxsdk/samples/objects/particles/pbomb.cpp sample.
namespace FSForcePBomb
{
	const Class_ID OBJECT_CLASS_ID = Class_ID(0x4c200df3, 0x1a347a77);

	// Reference number
	const uint32 PBLOCK_REF_NO = 0;

	// Param IDs
	// IMPORTANT: uses the old IParamBlock* object.
	enum { PB_SYMMETRY, PB_CHAOS, PB_STARTTIME, PB_LASTSFOR, PB_DELTA_V, PB_DECAY, PB_DECAYTYPE, PB_ICONSIZE, PB_RANGEON };

	// Forms
	enum { SPHERE, CYLIND, PLANAR };
};

// Information retrieved using the parameter block information
namespace FSForceDrag
{
	const Class_ID OBJECT_CLASS_ID = Class_ID(0x45F47BC1, 0x283B4E1A);

	// Reference number
	const uint32 PBLOCK_REF_NO = 0;

	// Param IDs
	enum { PB_TIMEON, PB_TIMEOFF, PB_SYMMETRY,
		PB_DAMPING_X, PB_RANGE_X, PB_FALLOFF_X,
		PB_DAMPING_Y, PB_RANGE_Y, PB_FALLOFF_Y,
		PB_DAMPING_Z, PB_RANGE_Z, PB_FALLOFF_Z,
		PB_DAMPING_R, PB_RANGE_R, PB_FALLOFF_R,
		PB_DAMPING_GC, PB_RANGE_GC, PB_FALLOFF_GC,
		PB_DAMPING_RC, PB_RANGE_RC, PB_FALLOFF_RC,
		PB_DAMPING_C, PB_RANGE_C, PB_FALLOFF_C,
		PB_DAMPING_AX, PB_RANGE_AX, PB_FALLOFF_AX,
		PB_RANGELESS, PB_ICONSIZE };

};

#endif // _MAX_FORCE_DEFINITIONS_H_