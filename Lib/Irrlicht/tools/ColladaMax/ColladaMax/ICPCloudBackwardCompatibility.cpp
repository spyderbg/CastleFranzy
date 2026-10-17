/**********************************************************************
 *<                             

   FILE: PCloud.CPP
   DESCRIPTION: Pcloud main code


   CREATED BY: Audrey Peterson   

   HISTORY: created February 1997

 *>   Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "StdAfx.h"
#include "ICPCloudBackwardCompatibility.h"
#include "Common/MaxParticleDefinitions.h"

ParamBlockDescID ICPCloudBackwardCompatiblity::k_previousVersion_00 [] = 
{
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_CREATEIN },  // distribution method
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPEED },    // speed
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPEEDVAR },    // speed var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPEEDDIR },  // vel dir
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRX },    // x
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRY },    // y
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRZ },    // z
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRVAR },    // var

	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_PBIRTHRATE },   // Birth rate
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PTOTALNUMBER },    // total number
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_LIFE },  // life
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_LIFEVAR },  // life var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_CUSTOMMTL },  // Custom Mtl
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SIZE },   // size
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SIZEVAR },   // size var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_GROWTIME },    // grow time
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_FADETIME },    // fade time
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_RNDSEED }, // rnd seed
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITRWID },  // emitter width
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITRHEIGHT },  // emitter height
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITRDEPTH },  // emitter depth
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_EMITRHID },    // emitter hidden

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PARTICLECLASS },  // particle class
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PARTICLETYPE },  // particle type
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_METATENSION },  // meta tension
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_METATENSIONVAR },  // metatension var
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_METACOURSE }, // meta course
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_METAAUTOCOARSE }, // auto coarseness
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_USESUBTREE }, // use subtree
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD }, // animation offset
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_ANIMATION_OFFSET }, // offset amount

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_VIEWPORTSHOWS },     // viewport shows
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_DISPLAYPORTION }, // display portion
	{ TYPE_INT, NULL, FALSE, -1 },     // mapping type
	{ TYPE_INT, NULL, TRUE, -1 },   // mapping time
	{ TYPE_FLOAT, NULL, TRUE, -1 },    // mapping dist

	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_SPINTIME }, // spin time
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINTIMEVAR },  // spin time var
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINPHASE },  // spin phase
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINPHASEVAR },  // spin phase var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPINAXISTYPE },  // spin axis choice
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISX }, // spin axis x
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISY }, // spin axis y
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISZ }, // spin axis z
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISVAR }, // spin axis var

	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITVINFL },  // emit influence
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITVMULT },  // emit multiplier
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITVMULTVAR },  // emit mult var

	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLAMP },  // bubble amp
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLAMPVAR },  // bubble amp var
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_BUBLPER },  // bubble period
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLPERVAR },  // bubble period var
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLPHAS },  // bubble phase
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLPHASVAR },  // bubble phase var

	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_STRETCH },  // Stretch
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNTYPE }, // spawn type
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNGENS }, // number of gens
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNCOUNT }, // number of spawns
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNDIRCHAOS }, // direction chaos
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNSPEEDCHAOS }, // speed chaos
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSPEEDSIGN }, // speed chaos sign
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNINHERITV }, // inherit old particle velocity
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNSCALECHAOS }, // scale chaos 
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSCALESIGN }, // scale chaos sign
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNLIFEVLUE }, // lifespan entry field
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSPEEDFIXED }, // constant spawn speed
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSCALEFIXED }, // constant spawn scale
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_METACOURSEV }, // Meta courseness viewport
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNPERCENT }, // Spawn percent
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_SPAWNMULTVAR }, // Spawn mult var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PCNOTDRAFT }, // Not Draft
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_SPAWNDIEAFTER }, // die after
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNDIEAFTERVAR }, // die after var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PCIPCOLLIDE_ON },  // IPC Enable
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PCIPCOLLIDE_STEPS },  // IPC Steps
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCE },  // IPC Bounce
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCEVAR },  // IPC Bounce Var
};

// FLAW : See FSParticlePCloud parameter map enum for description
ParamBlockDescID ICPCloudBackwardCompatiblity::k_currentVersion [] = 
{
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_CREATEIN },  // distribution method
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPEED },    // speed
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPEEDVAR },    // speed var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPEEDDIR },  // vel dir
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRX },    // x
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRY },    // y
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRZ },    // z
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_DIRVAR },    // var

	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_PBIRTHRATE },   // Birth rate
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PTOTALNUMBER },    // total number
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_LIFE },  // life
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_LIFEVAR },  // life var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_CUSTOMMTL },  // Custom Mtl
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SIZE },   // size
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SIZEVAR },   // size var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_GROWTIME },    // grow time
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_FADETIME },    // fade time
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_RNDSEED }, // rnd seed
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITRWID },  // emitter width
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITRHEIGHT },  // emitter height
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITRDEPTH },  // emitter depth
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_EMITRHID },    // emitter hidden

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PARTICLECLASS },  // particle class
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PARTICLETYPE },  // particle type
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_METATENSION },  // meta tension
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_METATENSIONVAR },  // metatension var
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_METACOURSE }, // meta course
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_METAAUTOCOARSE }, // auto coarseness
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_USESUBTREE }, // use subtree

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_VIEWPORTSHOWS },     // viewport shows
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_DISPLAYPORTION }, // display portion

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD },
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_ANIMATION_OFFSET },
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_ANIMATION_OFFSET_VARIATION },

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_ANIMATION_SCALING_METHOD },
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_ANIMATION_SCALING_SRC_LENGTH },
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_ANIMATION_SCALING_SRC_VARIANCE },
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_ANIMATION_SCALING_DST_MULTIPLIER },
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_ANIMATION_SCALING_DST_VARIANCE },

	//{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_MAPPINGTYPE },     // mapping type
	//{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_MAPPINGTIME },   // mapping time
	//{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_MAPPINGDIST },    // mapping dist

	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_SPINTIME }, // spin time
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINTIMEVAR },  // spin time var
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINPHASE },  // spin phase
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINPHASEVAR },  // spin phase var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPINAXISTYPE },  // spin axis choice
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISX }, // spin axis x
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISY }, // spin axis y
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISZ }, // spin axis z
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPINAXISVAR }, // spin axis var

	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITVINFL },  // emit influence
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITVMULT },  // emit multiplier
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_EMITVMULTVAR },  // emit mult var

	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLAMP },  // bubble amp
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLAMPVAR },  // bubble amp var
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_BUBLPER },  // bubble period
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLPERVAR },  // bubble period var
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLPHAS },  // bubble phase
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_BUBLPHASVAR },  // bubble phase var

	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_STRETCH },  // Stretch
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNTYPE }, // spawn type
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNGENS }, // number of gens
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNCOUNT }, // number of spawns
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNDIRCHAOS }, // direction chaos
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNSPEEDCHAOS }, // speed chaos
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSPEEDSIGN }, // speed chaos sign
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNINHERITV }, // inherit old particle velocity
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNSCALECHAOS }, // scale chaos 
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSCALESIGN }, // scale chaos sign
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNLIFEVLUE }, // lifespan entry field
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSPEEDFIXED }, // constant spawn speed
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNSCALEFIXED }, // constant spawn scale
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_METACOURSEV }, // Meta courseness viewport
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_SPAWNPERCENT }, // Spawn percent
	{ TYPE_FLOAT, NULL, FALSE, FSParticlePCloud::PB_SPAWNMULTVAR }, // Spawn mult var
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PCNOTDRAFT }, // Not Draft
	{ TYPE_INT, NULL, TRUE, FSParticlePCloud::PB_SPAWNDIEAFTER }, // die after
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_SPAWNDIEAFTERVAR }, // die after var

	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PCIPCOLLIDE_ON },  // IPC Enable
	{ TYPE_INT, NULL, FALSE, FSParticlePCloud::PB_PCIPCOLLIDE_STEPS },  // IPC Steps
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCE },  // IPC Bounce
	{ TYPE_FLOAT, NULL, TRUE, FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCEVAR },  // IPC Bounce Var
};

ParamVersionDesc ICPCloudBackwardCompatiblity::k_previousVersionDescriptions[] = 
{
	ParamVersionDesc( k_previousVersion_00, ARRAY_SIZE( k_previousVersion_00 ), 0 )
};

const int ICPCloudBackwardCompatiblity::k_previousVersionCount = ARRAY_SIZE( k_previousVersionDescriptions );

//const int ICPCloudBackwardCompatiblity::k_currentVersionNumber = 0;
const int ICPCloudBackwardCompatiblity::k_currentVersionNumber = k_previousVersionCount;
const int ICPCloudBackwardCompatiblity::k_currentVersionParamCount = ARRAY_SIZE( k_currentVersion );

ParamVersionDesc ICPCloudBackwardCompatiblity::k_currentVersionDescription( k_currentVersion, k_currentVersionParamCount, k_currentVersionNumber );
