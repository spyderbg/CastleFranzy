/**********************************************************************
 *<                             

   FILE: PCloud.CPP
   DESCRIPTION: Pcloud main code


   CREATED BY: Audrey Peterson   

   HISTORY: created February 1997

 *>   Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "StdAfx.h"
#include "ICPCloud.h"
#include "ICParticleSystemBridge.h"
#include "FUtils/FUStringBuilder.h"
#include "Common/ConversionFunctions.h"
#include "Common/MaxParticleDefinitions.h"
#include "ICPCloudBackwardCompatibility.h"

#include <io.h>
#include "iparamm.h"
#include "interpik.h"
#include "texutil.h"
#include "stdmat.h"
#include "macrorec.h"
#include "maxscrpt/MAXScrpt.h"

#include "UIHelpers.h"
#include "AnimationExporter.h"
#include "EmitterExporter.h"
#include "ColladaModifiers.h"

#define EPSILON 0.0001f

static ICPCloudClassDesc PCloudDesc;
ClassDesc* GetICPCloudDesc() {return &PCloudDesc;}

void ICPCloudClassDesc::ResetClassParams(BOOL fileReset)
{
   ICPCloudParticle::createmeth=0;
}

//--- ClassDescriptor and class vars ---------------------------------
IParamMap *ICPCloudParticle::pmapParam = NULL;
IParamMap *ICPCloudParticle::pmapPGen = NULL;
IParamMap *ICPCloudParticle::pmapPType = NULL;
IParamMap *ICPCloudParticle::pPMapAnimationKeying = NULL;
IParamMap *ICPCloudParticle::pmapPSpin = NULL;
IParamMap *ICPCloudParticle::pmapEmitV = NULL;
IParamMap *ICPCloudParticle::pmapBubl = NULL;
IParamMap *ICPCloudParticle::pmapSpawn = NULL;

IObjParam *ICPCloudParticle::ip    = NULL;
BOOL ICPCloudParticle::creating    = FALSE;
ICPCPickOperand ICPCloudParticle::pickCB;
ICPCPickNorm ICPCloudParticle::pickNorm;
ICustEdit *ICPCloudParticle::custCtrlEdit=NULL;
int ICPCloudParticle::custsettings=0;
AName *ICPCloudParticle::NameLst=NULL;
HWND ICPCloudParticle::hexts = NULL;
HWND ICPCloudParticle::hParams2 = NULL;
HWND ICPCloudParticle::hgen = NULL;
HWND ICPCloudParticle::hptype = NULL;
HWND ICPCloudParticle::hAnimationKeying = NULL;
HWND ICPCloudParticle::hparam = NULL;
HWND ICPCloudParticle::hrot = NULL;
HWND ICPCloudParticle::hbubl = NULL;
HWND ICPCloudParticle::spawn = NULL;
int ICPCloudParticle::createmeth=0;


#define SIZEFACTOR (float(TIME_TICKSPERSEC)/120.0f)

// render types
#define RENDMETA    8
#define RENDGEOM  9

//
//
// Parameters

#define ISSTD 0
#define METABALLS 1
#define INSTGEOM 2

static int createIDs[] = {IDC_AP_FILLBOX,IDC_AP_FILLSPHERE,IDC_AP_FILLCYLINDER,IDC_AP_FILLOBJECT};

static int particleclassIDs[] = {IDC_SP_TYPESTD,IDC_SP_TYPEMET,IDC_SP_TYPEINSTANCE};

static int particletypeIDs[] = {IDC_SP_TYPETRI,IDC_SP_TYPECUB,IDC_SP_TYPESPC,IDC_SP_TYPEFAC,
                        IDC_SP_TYPEPIX,IDC_SP_TYPETET,IDC_SP_TYPE6PNT,IDC_SP_TYPESPHERE};

static int viewportoptionIDs[] = {IDC_SP_VIEWDISPDOT,IDC_SP_VIEWDISPTIK,IDC_SP_VIEWDISPMESH,IDC_SP_VIEWDISPBOX};

static int spindirectionIDs[] = {IDC_AP_PARTICLEDIRRND,IDC_AP_PARTICLEDIRTRAVL,IDC_AP_PARTICLEDIRUSER};
static int particleaxisIDs[] = {IDC_AP_PCLOUDDIRRND,IDC_AP_PCLOUDDIRENTER,IDC_AP_PCLOUDDIROBJ};

#define DIRTRAVEL 1

static int animateoffsetIDs[] = {IDC_AP_NOANIOFF,IDC_AP_ANIOFFBIRTH,IDC_AP_ANIOFFRND};
static int animationScalingMethodIDs[] = {IDC_ANIMATION_SCALING_NONE_RADIOBUTTON,IDC_ANIMATION_SCALING_LIFE_RADIOBUTTON,IDC_ANIMATION_SCALING_SPECIFIC_RADIOBUTTON};
static int custmtlIDs[] = {IDC_SP_MAPCUSTOMEMIT,IDC_SP_MAPCUSTOMINST};

// Dialog Unique to Particle Array
static ParamUIDesc descParamPCloud[] = {

   // Distribution Method
   ParamUIDesc(FSParticlePCloud::PB_CREATEIN,TYPE_RADIO,createIDs,4),

   // Emitter Width
   ParamUIDesc(
      FSParticlePCloud::PB_EMITRWID,
      EDITTYPE_UNIVERSE,
      IDC_AP_EMITRADWID,IDC_AP_EMITRADWIDSPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),  

   // Emitter Height
   ParamUIDesc(
      FSParticlePCloud::PB_EMITRHEIGHT,
      EDITTYPE_UNIVERSE,
      IDC_AP_EMITHGT,IDC_AP_EMITHGTSPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),  

   // Emitter Depth
   ParamUIDesc(
      FSParticlePCloud::PB_EMITRDEPTH,
      EDITTYPE_UNIVERSE,
      IDC_AP_EMITDEP,IDC_AP_EMITDEPSPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),  

   // Hide Emitter
   ParamUIDesc(FSParticlePCloud::PB_EMITRHID,TYPE_SINGLECHEKBOX,IDC_SP_EMITHID),

   // Viewport Shows
   ParamUIDesc(FSParticlePCloud::PB_VIEWPORTSHOWS,TYPE_RADIO,viewportoptionIDs,4),

   // Particle Display Portion
   ParamUIDesc(
      FSParticlePCloud::PB_DISPLAYPORTION,
      EDITTYPE_FLOAT,
      IDC_SP_GENDISP,IDC_SP_GENDISPSPIN,
      0.0f,100.0f,
      0.1f,
      stdPercentDim),
};

// Common Dialog for Particle Generation
static ParamUIDesc descParamPGen[] = {

   // Particle Birth Rate
   ParamUIDesc(
      FSParticlePCloud::PB_PBIRTHRATE,
      EDITTYPE_FLOAT,
      IDC_SP_GENRATE,IDC_SP_GENRATESPIN,
      0.0f,65000.0f,
      1.0f),

   // Particle Total Count
   ParamUIDesc(
      FSParticlePCloud::PB_PTOTALNUMBER,
      EDITTYPE_INT,
      IDC_SP_GENTTL,IDC_SP_GENTTLSPIN,
      0.0f,65000.0f,
      1.0f),

   // Particle Speed
   ParamUIDesc(
      FSParticlePCloud::PB_SPEED,
      EDITTYPE_UNIVERSE,
      IDC_SP_SPEED,IDC_SP_SPEEDSPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Particle Speed Var
   ParamUIDesc(
      FSParticlePCloud::PB_SPEEDVAR,
      EDITTYPE_FLOAT,
      IDC_SP_SPEEDVAR,IDC_SP_SPEEDVARSPIN,
      0.0f,100.0f,
      SPIN_AUTOSCALE,stdPercentDim),

   // Enter Axis Control
   ParamUIDesc(FSParticlePCloud::PB_SPEEDDIR,TYPE_RADIO,particleaxisIDs,3),

   // X-Axis
   ParamUIDesc(
      FSParticlePCloud::PB_DIRX,
      EDITTYPE_FLOAT,
      IDC_AP_MOVEDIRX,IDC_AP_MOVEDIRXSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Y-Axis
   ParamUIDesc(
      FSParticlePCloud::PB_DIRY,
      EDITTYPE_FLOAT,
      IDC_AP_MOVEDIRY,IDC_AP_MOVEDIRYSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Z-Axis
   ParamUIDesc(
      FSParticlePCloud::PB_DIRZ,
      EDITTYPE_FLOAT,
      IDC_AP_MOVEDIRZ,IDC_AP_MOVEDIRZSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Particle Move Var
   ParamUIDesc(
      FSParticlePCloud::PB_DIRVAR,
      EDITTYPE_FLOAT,
      IDC_AP_MOVEDIRVAR,IDC_AP_MOVEDIRVARSPIN,
      0.0f,100.0f,
      SPIN_AUTOSCALE,stdPercentDim),

   // Particle Life
   ParamUIDesc(
      FSParticlePCloud::PB_LIFE, 
      EDITTYPE_TIME,
      IDC_SP_GENLIFE,IDC_SP_GENLIFESPIN,
      0.0f,999999999.0f,
      10.0f),

   // Particle Life Var
   ParamUIDesc(
      FSParticlePCloud::PB_LIFEVAR,
      EDITTYPE_TIME,
      IDC_SP_GENLIFEVAR,IDC_SP_GENLIFEVARSPIN,
      0.0f,999999999.0f,
      10.0f),

   // Particle Size
   ParamUIDesc(
      FSParticlePCloud::PB_SIZE,
      EDITTYPE_UNIVERSE,
      IDC_SP_GENSIZE,IDC_SP_GENSIZESPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Particle Size Var
   ParamUIDesc(
      FSParticlePCloud::PB_SIZEVAR,
      EDITTYPE_FLOAT,
      IDC_SP_GENSIZEVAR,IDC_SP_GENSIZEVARSPIN,
      0.0f,100.0f,
      SPIN_AUTOSCALE,stdPercentDim),

   // Particle Grow Time
   ParamUIDesc(
      FSParticlePCloud::PB_GROWTIME,
      EDITTYPE_TIME,
      IDC_SP_GENGRO,IDC_SP_GENGROSPIN,
      0.0f,999999999.0f,
      10.0f),

   // Particle Fade Time
   ParamUIDesc(
      FSParticlePCloud::PB_FADETIME,
      EDITTYPE_TIME,
      IDC_SP_GENFAD,IDC_SP_GENFADSPIN,
      0.0f,999999999.0f,
      10.0f),

   // Random Number Seed
   ParamUIDesc(
      FSParticlePCloud::PB_RNDSEED,
      EDITTYPE_INT,
      IDC_SP_GENSEED,IDC_SP_GENSEEDSPIN,
      0.0f,25000.0f,
      1.0f),
   };

// Particle Type for PCloud
static ParamUIDesc descParamPType[] = {

   // Particle Class
   ParamUIDesc(FSParticlePCloud::PB_PARTICLECLASS,TYPE_RADIO,particleclassIDs,3),

   // Particle Type
   ParamUIDesc(FSParticlePCloud::PB_PARTICLETYPE,TYPE_RADIO,particletypeIDs,8),

   // Metaball Tension
   ParamUIDesc(
      FSParticlePCloud::PB_METATENSION,
      EDITTYPE_FLOAT,
      IDC_SP_METTENS,IDC_SP_METTENSSPIN,
      0.1f,10.0f,
      SPIN_AUTOSCALE),

   // Metaball Tension Variation
   ParamUIDesc(
      FSParticlePCloud::PB_METATENSIONVAR,
      EDITTYPE_FLOAT,
      IDC_SP_METTENSVAR,IDC_SP_METTENSVARSPIN,
      0.0f,100.0f,
      SPIN_AUTOSCALE,stdPercentDim),

   // Metaball Courseness
   ParamUIDesc(
      FSParticlePCloud::PB_METACOURSE,
      EDITTYPE_UNIVERSE,
      IDC_SP_METCOURSE,IDC_SP_METCOURSESPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Metaball Courseness ViewPort
   ParamUIDesc(
      FSParticlePCloud::PB_METACOURSEV,
      EDITTYPE_UNIVERSE,
      IDC_SP_METCOURSEV,IDC_SP_METCOURSEVSPIN,
      0.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Metaball Auto Coarseness
   ParamUIDesc(FSParticlePCloud::PB_METAAUTOCOARSE,TYPE_SINGLECHEKBOX,IDC_SP_AUTOCOARSE),

   // Display type
   ParamUIDesc(FSParticlePCloud::PB_PCNOTDRAFT,TYPE_SINGLECHEKBOX,IDC_SP_DRAFTMODE),

   // Use Subtree Checkbox
   ParamUIDesc(FSParticlePCloud::PB_USESUBTREE,TYPE_SINGLECHEKBOX,IDC_AP_USESUBTREE),

   // Use Custom Mtl
   ParamUIDesc(FSParticlePCloud::PB_CUSTOMMTL,TYPE_RADIO,custmtlIDs,2),
};

// Animation Keying parameters for PCloud
static ParamUIDesc descParamAnimationKeying[] = 
{
	// Animation Offset
	ParamUIDesc( FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD, TYPE_RADIO, animateoffsetIDs, 3 ),

	ParamUIDesc(
		FSParticlePCloud::PB_ANIMATION_OFFSET,
		EDITTYPE_TIME,
		IDC_AP_ANIRNDFR,IDC_AP_ANIRNDFRSPIN,
		INT_MIN,INT_MAX,
		10.0f 
		),

	ParamUIDesc(
		FSParticlePCloud::PB_ANIMATION_OFFSET_VARIATION,
		EDITTYPE_FLOAT,
		IDC_ANIMATION_OFFSET_VARIATION_EDITTEXT,IDC_ANIMATION_OFFSET_VARIATION_SPINNER,
		0.0f,FLT_MAX,
		0.01f,
		stdPercentDim 
		),

	// Animation Scaling
	ParamUIDesc( FSParticlePCloud::PB_ANIMATION_SCALING_METHOD, TYPE_RADIO, animationScalingMethodIDs, 3 ),

	ParamUIDesc(
		FSParticlePCloud::PB_ANIMATION_SCALING_SRC_LENGTH,
		EDITTYPE_TIME,
		IDC_ANIMATION_SCALING_SRC_LENGTH_EDITTEXT,IDC_ANIMATION_SCALING_SRC_LENGTH_SPINNER,
		0.f,INT_MAX,
		10.0f 
		),

	ParamUIDesc(
		FSParticlePCloud::PB_ANIMATION_SCALING_SRC_VARIANCE,
		EDITTYPE_FLOAT,
		IDC_ANIMATION_SCALING_SRC_VARIATION_EDITTEXT,IDC_ANIMATION_SCALING_SRC_VARIATION_SPINNER,
		0.0f,FLT_MAX,
		0.01f,
		stdPercentDim 
		),

	ParamUIDesc(
		FSParticlePCloud::PB_ANIMATION_SCALING_DST_MULTIPLIER,
		EDITTYPE_FLOAT,
		IDC_ANIMATION_SCALING_DST_MULTIPLIER_EDITTEXT,IDC_ANIMATION_SCALING_DST_MULTIPLIER_SPINNER,
		0.f,FLT_MAX,
		1.f 
		),

	ParamUIDesc(
		FSParticlePCloud::PB_ANIMATION_SCALING_DST_VARIANCE,
		EDITTYPE_FLOAT,
		IDC_ANIMATION_SCALING_DST_VARIATION_EDITTEXT,IDC_ANIMATION_SCALING_DST_VARIATION_SPINNER,
		0.0f,FLT_MAX,
		0.01f,
		stdPercentDim 
		),

};

// Common Dialog for Particle Spin
static ParamUIDesc descParamPSpin[] = {

   // Spin Time
   ParamUIDesc(
      FSParticlePCloud::PB_SPINTIME,
      EDITTYPE_TIME,
      IDC_SP_SPIN,IDC_SP_SPINSPIN,
      0.0f,999999999.0f,
      10.0f),

   // Spin Time Var
   ParamUIDesc(
      FSParticlePCloud::PB_SPINTIMEVAR,
      EDITTYPE_FLOAT,
      IDC_SP_SPINVAR,IDC_SP_SPINVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),

   // Spin Phase
   ParamUIDesc(
      FSParticlePCloud::PB_SPINPHASE,
      EDITTYPE_FLOAT,
      IDC_SP_SPINPHA,IDC_SP_SPINPHASPIN,
      -360.0f,360.0f,
      0.01f,
      stdAngleDim),

   // Spin Phase Variation
   ParamUIDesc(
      FSParticlePCloud::PB_SPINPHASEVAR,
      EDITTYPE_FLOAT,
      IDC_SP_SPINPHAVAR,IDC_SP_SPINPHAVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),

   // Enter Axis Control
   ParamUIDesc(FSParticlePCloud::PB_SPINAXISTYPE,TYPE_RADIO,spindirectionIDs,3),

   // X-Axis
   ParamUIDesc(
      FSParticlePCloud::PB_SPINAXISX,
      EDITTYPE_FLOAT,
      IDC_SP_SPINAXISX,IDC_SP_SPINAXISXSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Y-Axis
   ParamUIDesc(
      FSParticlePCloud::PB_SPINAXISY,
      EDITTYPE_FLOAT,
      IDC_SP_SPINAXISY,IDC_SP_SPINAXISYSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Z-Axis
   ParamUIDesc(
      FSParticlePCloud::PB_SPINAXISZ,
      EDITTYPE_FLOAT,
      IDC_SP_SPINAXISZ,IDC_SP_SPINAXISZSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Spin Direction Variation
   ParamUIDesc(
      FSParticlePCloud::PB_SPINAXISVAR,
      EDITTYPE_FLOAT,
      IDC_SP_SPINAXISVAR,IDC_SP_SPINAXISVARSPIN,
      -180.0f,180.0f,
      0.01f,
      stdAngleDim),

   // Stretch
   ParamUIDesc(
      FSParticlePCloud::PB_STRETCH,
      EDITTYPE_INT,
      IDC_AP_STRETCH,IDC_AP_STRETCHSPIN,
      -1000.0f,1000.0f,
      1.0f),

   // IPC Enable
   ParamUIDesc(FSParticlePCloud::PB_PCIPCOLLIDE_ON,TYPE_SINGLECHEKBOX,IDC_INTERP_BOUNCEON),

   // IPC Steps
   ParamUIDesc(
      FSParticlePCloud::PB_PCIPCOLLIDE_STEPS,
      EDITTYPE_INT,
      IDC_INTERP_NSTEPS,IDC_INTERP_NSTEPSSPIN,
      1.0f,1000.0f,
      1.0f),

   // IPC Bounce
   ParamUIDesc(
      FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCE,
      EDITTYPE_FLOAT,
      IDC_INTERP_BOUNCE,IDC_INTERP_BOUNCESPIN,
      0.0f,10000.0f,
      1.0f,
      stdPercentDim),

   // IPC Bounce
   ParamUIDesc(
      FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCEVAR,
      EDITTYPE_FLOAT,
      IDC_INTERP_BOUNCEVAR,IDC_INTERP_BOUNCEVARSPIN,
      0.0f,100.0f,
      1.0f,
      stdPercentDim),

   };

  // Common Dialog for Secondary Motion Data
static ParamUIDesc descParamEmitV[] = {

   // Emitter Velocity Inheritance Influence Portion
   ParamUIDesc(
      FSParticlePCloud::PB_EMITVINFL,
      EDITTYPE_FLOAT,
      IDC_SP_EMVI,IDC_SP_EMVISPIN,
      0.0f,100.0f,
      0.01f),

   // Secondary Motion Multiplier
   ParamUIDesc(
      FSParticlePCloud::PB_EMITVMULT,
      EDITTYPE_FLOAT,
      IDC_SP_EMVIMULT,IDC_SP_EMVIMULTSPIN,
      -999999999.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Multiplier Variation
   ParamUIDesc(
      FSParticlePCloud::PB_EMITVMULTVAR,
      EDITTYPE_FLOAT,
      IDC_SP_EMVIMULTVAR,IDC_SP_EMVIMULTVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),
   };

// Common Dialog for Bubble Motion
static ParamUIDesc descParamBubl[] = {

   // Bubble Amplitude
   ParamUIDesc(
      FSParticlePCloud::PB_BUBLAMP,
      EDITTYPE_UNIVERSE,
      IDC_SP_BUBL_AMP,IDC_SP_BUBL_AMPSPIN,
      -.0f,999999999.0f,
      SPIN_AUTOSCALE),

   // Bubble Amp Var
   ParamUIDesc(
      FSParticlePCloud::PB_BUBLAMPVAR,
      EDITTYPE_FLOAT,
      IDC_SP_BUBL_AMPVAR,IDC_SP_BUBL_AMPVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),

   // Bubble Period
   ParamUIDesc(
      FSParticlePCloud::PB_BUBLPER,
      EDITTYPE_TIME,
      IDC_SP_BUBL_PER,IDC_SP_BUBL_PERSPIN,
      0.0f,999999999.0f,
      10.0f),

   // Bubble Period Var
   ParamUIDesc(
      FSParticlePCloud::PB_BUBLPERVAR,
      EDITTYPE_FLOAT,
      IDC_SP_BUBL_PERVAR,IDC_SP_BUBL_PERVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),

   // Bubble Phase
   ParamUIDesc(
      FSParticlePCloud::PB_BUBLPHAS,
      EDITTYPE_FLOAT,
      IDC_SP_BUBL_PERPHA,IDC_SP_BUBL_PERPHASPIN,
      -360.0f,360.0f,
      0.01f,
      stdAngleDim),

   // Bubble Phase Var
   ParamUIDesc(
      FSParticlePCloud::PB_BUBLPHASVAR,
      EDITTYPE_FLOAT,
      IDC_SP_BUBL_PERPHAVAR,IDC_SP_BUBL_PERPHAVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),
};

static int SpawnTypeIDs[] = {IDC_AP_NOSPAWN,IDC_AP_COLLIDEDIE,IDC_AP_COLLIDESPAWN,IDC_AP_DEATHSPAWN,IDC_AP_SPAWNTRAILS};
static int SpeedChaosIDs[] = {IDC_AP_SPEEDLESS,IDC_AP_SPEEDMORE,IDC_AP_SPEEDBOTH};
static int ScaleChaosIDs[] = {IDC_AP_SCALEDOWN,IDC_AP_SCALEUP,IDC_AP_SCALEBOTH};

// Dialog for Particle Spawning
static ParamUIDesc descPSpawning[] = {
   
   // Spawing Effects Type
   ParamUIDesc(FSParticlePCloud::PB_SPAWNTYPE,TYPE_RADIO,SpawnTypeIDs,5),

   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNDIEAFTER,
      EDITTYPE_TIME,
      IDC_AP_MAXSPAWNDIEAFTER,IDC_AP_MAXSPAWNDIEAFTERSPIN,
      0.0f,999999999.0f,
      10.0f),

   // Spawn Generations
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNDIEAFTERVAR,
      EDITTYPE_FLOAT,
      IDC_AP_MAXSPAWNDIEAFTERVAR,IDC_AP_MAXSPAWNDIEAFTERVARSPIN,
      0.0f,100.0f,
      0.01f,
      stdPercentDim),

   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNGENS,
      EDITTYPE_INT,
      IDC_AP_MAXSPAWNGENS,IDC_AP_MAXSPAWNGENSSPIN,
      1.0f,65000.0f,
      1.0f),

   // Spawn Percent
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNPERCENT,
      EDITTYPE_INT,
      IDC_AP_PARENTPERCENT,IDC_AP_PARENTPERCENTSPIN,
      1.0f,100.0f,
      1.0f),

   // Spawn Spawncount
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNCOUNT,
      EDITTYPE_INT,
      IDC_AP_NUMBERVAR,IDC_AP_NUMBERVARSPIN,
      1.0f,65000.0f,
      1.0f),

   // Spawn Mult Percent
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNMULTVAR,
      EDITTYPE_FLOAT,
      IDC_AP_NUMBERVARVAR,IDC_AP_NUMBERVARVARSPIN,
      0.0f,100.0f,
      0.1f,stdPercentDim),

   // Spawn Direction Chaos
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNDIRCHAOS,
      EDITTYPE_FLOAT,
      IDC_AP_CHAOSANGLE,IDC_AP_CHAOSANGLESPIN,
      0.0f,100.0f,
      0.1f,stdPercentDim),

   // Spawn Speed Chaos
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNSPEEDCHAOS,
      EDITTYPE_FLOAT,
      IDC_AP_CHAOSSPEED,IDC_AP_CHAOSSPEEDSPIN,
      0.0f,100.0f,
      0.1f),

   // Spawing Speed Sign
   ParamUIDesc(FSParticlePCloud::PB_SPAWNSPEEDSIGN,TYPE_RADIO,SpeedChaosIDs,3),

   // Spawning Inherit Parent V
   ParamUIDesc(FSParticlePCloud::PB_SPAWNINHERITV,TYPE_SINGLECHEKBOX,IDC_AP_SPAWNSUMV),

   // Spawning Speed Fixed
   ParamUIDesc(FSParticlePCloud::PB_SPAWNSPEEDFIXED,TYPE_SINGLECHEKBOX,IDC_AP_SPAWNSPEEDFIXED),

   // Spawn Scale Chaos
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNSCALECHAOS,
      EDITTYPE_FLOAT,
      IDC_AP_CHAOSSCALE,IDC_AP_CHAOSSCALESPIN,
      0.0f,100.0f,
      0.1f),

   // Spawning Scale Sign
   ParamUIDesc(FSParticlePCloud::PB_SPAWNSCALESIGN,TYPE_RADIO,ScaleChaosIDs,3),

   // Spawning Speed Fixed
   ParamUIDesc(FSParticlePCloud::PB_SPAWNSCALEFIXED,TYPE_SINGLECHEKBOX,IDC_AP_SPAWNSCALEFIXED),

   // Spawn Lifespan Entry Field
   ParamUIDesc(
      FSParticlePCloud::PB_SPAWNLIFEVLUE,
      EDITTYPE_INT,
      IDC_AP_QUEUELIFESPAN,IDC_AP_QUEUELIFESPANSPIN,
      0.0f,65000.0f,
      1.0f),
};

//-- ParticleDlgProc ------------------------------------------------
static ICCreatePCloudMode theCreatePCloudMode;

RefResult ICCreatePCloudProc::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message ) 
{
	switch (message)
	{
		case REFMSG_PRENOTIFY_PASTE:
		case REFMSG_TARGET_SELECTIONCHANGE:
		{
			if ( ignoreSelectionChange )
			{
				break;
			}
			if ( CloudObject && CloudNode == hTarget )
			{
				// this will set camNode== NULL;
				theHold.Suspend();
				DeleteReference( 0 );
				theHold.Resume();
				goto endEdit;
			}
		}
		// fall through
		case REFMSG_TARGET_DELETED:
		{
			if ( CloudObject && CloudNode == hTarget )
			{
endEdit:
				if ( createInterface->GetCommandMode()->ID() == CID_STDPICK ) 
				{ 
					if ( CloudObject->creating )
					{  
						theCreatePCloudMode.JumpStart( CloudObject->ip, CloudObject );
						createInterface->SetCommandMode( &theCreatePCloudMode );
					} 
					else 
					{
						createInterface->SetStdCommandMode( CID_OBJMOVE );
					}
				}
#ifdef _OSNAP
				CloudObject->ClearAFlag( A_OBJ_LONG_CREATE );
#endif
				CloudObject->EndEditParams( ( IObjParam* )createInterface, 0, NULL );
				CloudObject = NULL;
				CloudNode = NULL;
				CreateNewObject();
				attachedToNode = FALSE;
			}
			else if ( CloudNode == hTarget )
			{
				CloudNode = NULL;
			}
			break;
		}
	}

	return REF_SUCCEED;
}

void ICCreatePCloudProc::Begin( IObjCreate *ioc, ClassDesc *desc )
{
	createInterface = ioc;
	cDesc           = desc;
	attachedToNode  = FALSE;
	createCB        = NULL;
	CloudNode         = NULL;
	CloudObject       = NULL;
	CreateNewObject();
}

void ICCreatePCloudProc::CreateNewObject()
{
	SuspendSetKeyMode();
	createInterface->GetMacroRecorder()->BeginCreate( cDesc );
	lastPutCount  = theHold.GetGlobalPutCount();
	CloudObject = ( ICPCloudParticle* )cDesc->Create();

	// Start the edit params process
	if ( CloudObject )
	{
		CloudObject->BeginEditParams( ( IObjParam * )createInterface, BEGIN_EDIT_CREATE, NULL );
#ifdef _OSNAP
		CloudObject->SetAFlag( A_OBJ_LONG_CREATE );
#endif
	}
	ResumeSetKeyMode();
}

//LACamCreationManager::~LACamCreationManager
void ICCreatePCloudProc::End()
{ 
	if ( CloudObject ) 
	{ 
#ifdef _OSNAP
		CloudObject->ClearAFlag(A_OBJ_LONG_CREATE);
#endif
		CloudObject->EndEditParams( (IObjParam*)createInterface, END_EDIT_REMOVEUI, NULL );
		if ( !attachedToNode ) 
		{  
			// RB 4-9-96: Normally the hold isn't holding when this 
			// happens, but it can be in certain situations (like a track view paste)
			// Things get confused if it ends up with undo...
			theHold.Suspend();
			createInterface->GetMacroRecorder()->Cancel();
			delete CloudObject;
			CloudObject = NULL;
			theHold.Resume();
			if ( theHold.GetGlobalPutCount() != lastPutCount )
			{
				GetSystemSetting(SYSSET_CLEAR_UNDO);
			}
		}  
		else if ( CloudNode ) 
		{
			theHold.Suspend();
			DeleteReference(0);  // sets cloudNode = NULL
			theHold.Resume();
		}
	}
}

void ICEnableFillPickButton( IParamBlock *pblock, HWND hWnd )
{
	int emitterShape = 0;
	pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,emitterShape,FOREVER);
	UIHelpers::SetButtonEnabled(hWnd,IDC_AP_FILLPICKBUTTON,emitterShape == 3);
}

void ICCreatePCloudMode::JumpStart(IObjParam *i,ICPCloudParticle *o)
   {
   ip  = i;
   obj = o;
   obj->BeginEditParams(i,BEGIN_EDIT_CREATE,NULL);
   }

int ICPCloudClassDesc::BeginCreate(Interface *i)
   {  
   SuspendSetKeyMode();
   IObjCreate *iob = i->GetIObjCreate();
   theCreatePCloudMode.Begin(iob,this);
   iob->PushCommandMode(&theCreatePCloudMode);
   return TRUE;
   }

int ICPCloudClassDesc::EndCreate(Interface *i)
   {
   ResumeSetKeyMode();
   theCreatePCloudMode.End();
   i->RemoveMode(&theCreatePCloudMode);
   macroRec->EmitScript();  // 10/00
   return TRUE;
   }

int ICCreatePCloudProc::proc(HWND hwnd,int msg,int point,int flag,
            IPoint2 m )
{  int res=TRUE;  
   ViewExp *vpx = createInterface->GetViewport(hwnd); 
   assert( vpx );

   INode *l_CloudNode = NULL;

#ifdef _3D_CREATE
   DWORD snapdim = SNAP_IN_3D;
#else
   DWORD snapdim = SNAP_IN_PLANE;
#endif

   switch ( msg ) {
      case MOUSE_POINT:
         switch ( point ) {
            case 0:
               assert( CloudObject );              
               vpx->CommitImplicitGrid(m, flag );
               if ( createInterface->SetActiveViewport(hwnd) ) {
                  return FALSE;
                  }

               if (createInterface->IsCPEdgeOnInView()) { 
                  res = FALSE;
                  goto done;
                  }

               if ( attachedToNode ) {
                     // send this one on its way
#ifdef _OSNAP
                  CloudObject->ClearAFlag(A_OBJ_LONG_CREATE);
#endif
                     CloudObject->EndEditParams( (IObjParam*)createInterface, 0, NULL);
              createInterface->GetMacroRecorder()->EmitScript();
                  // Get rid of the reference.
                  if (CloudNode) {
                     theHold.Suspend();
                     DeleteReference(0);
                     theHold.Resume();
                     }
                  // new object
                  CreateNewObject();   // creates CloudObject
                  }

                  theHold.Begin();   // begin hold for undo
               mat.IdentityMatrix();

               // link it up
               l_CloudNode = createInterface->CreateObjectNode( CloudObject);
               attachedToNode = TRUE;
               assert( l_CloudNode );             
               createCB = CloudObject->GetCreateMouseCallBack();
               createInterface->SelectNode( l_CloudNode );
               
               // Reference the new node so we'll get notifications.
               theHold.Suspend();
               ReplaceReference( 0, l_CloudNode);
               theHold.Resume();
               mat.IdentityMatrix();
            default:          
               if (createCB) {                  
                  res = createCB->proc(vpx,msg,point,flag,m,mat);
//                macroRec->Disable();   // 10/00
                  createInterface->SetNodeTMRelConstPlane(CloudNode,mat);
//                macroRec->Enable();

                  if (res==CREATE_ABORT)
                     goto abort;
                  if (res==CREATE_STOP){
#ifdef _OSNAP
                         CloudObject->ClearAFlag(A_OBJ_LONG_CREATE);
#endif
                      theHold.Accept(GetString(IDS_DS_CREATE));    
                  }
                  createInterface->RedrawViews(createInterface->GetTime());   //DS
                  }

               break;
               
            }        
         break;

      case MOUSE_MOVE:
         if (createCB) {            
            res = createCB->proc(vpx,msg,point,flag,m,mat);
//          macroRec->Disable();   // 10/00
            createInterface->SetNodeTMRelConstPlane(CloudNode,mat);
//          macroRec->Enable();
            if (res==CREATE_ABORT) 
               goto abort;
            if (res==CREATE_STOP){
#ifdef _OSNAP
            CloudObject->ClearAFlag(A_OBJ_LONG_CREATE);
#endif
               theHold.Accept(GetString(IDS_DS_CREATE)); // TH
            }
            createInterface->RedrawViews(createInterface->GetTime(),REDRAW_INTERACTIVE);     //DS     
            }
         break;

   case MOUSE_PROPCLICK:
      createInterface->SetStdCommandMode(CID_OBJMOVE);
      break;
      case MOUSE_ABORT: 
   if (createCB)
   { res = createCB->proc(vpx,msg,point,flag,m,mat);
//    macroRec->Disable();   // 10/00
     createInterface->SetNodeTMRelConstPlane(CloudNode, mat);
//    macroRec->Enable();
     if (res==CREATE_ABORT)
         goto abort;
     if (res==CREATE_STOP){
#ifdef _OSNAP
         CloudObject->ClearAFlag(A_OBJ_LONG_CREATE);
#endif
       theHold.Accept(GetString(IDS_DS_CREATE));   
     }
     createInterface->RedrawViews(createInterface->GetTime()); 
      break;
   }
   abort:
      assert( CloudObject );
#ifdef _OSNAP
      CloudObject->ClearAFlag(A_OBJ_LONG_CREATE);
#endif
      CloudObject->EndEditParams( (IObjParam*)createInterface,0,NULL);
      // Toss the Undo stack if any parameters have changed.
      theHold.Cancel();  // deletes both the Cloudera and target.
      if (theHold.GetGlobalPutCount()!=lastPutCount) 
         GetSystemSetting(SYSSET_CLEAR_UNDO);
      CloudObject=NULL;
      createInterface->RedrawViews(createInterface->GetTime());
      CreateNewObject();   
      attachedToNode = FALSE;
      res = FALSE;
      break;
   
      case MOUSE_FREEMOVE:
         SetCursor(LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CROSS_HAIR)));
#ifdef _OSNAP  //PREVIEW SNAP
       if (createCB) res =createCB->proc(vpx,msg,point,flag,m,mat);
#endif
      vpx->TrackImplicitGrid(m);
         break;
   }

   done:
   if ((res == CREATE_STOP)||(res==CREATE_ABORT))
      vpx->ReleaseImplicitGrid();
   createInterface->ReleaseViewport(vpx); 
   return res;
}

void ICInstStuff(HWND hWnd,BOOL ison,HWND hparam,HWND spawn,BOOL dist)
{ 
	EnableWindow(GetDlgItem(hWnd,IDC_AP_INSTANCESRCNAME),ison);
	UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJECTPICK,ison);
	UIHelpers::SetButtonEnabled(hWnd,IDC_AP_TREEPICK,ison);
	EnableWindow(GetDlgItem(hWnd,IDC_AP_USESUBTREE),ison);
	EnableWindow(GetDlgItem(hWnd,IDC_SP_MAPCUSTOMINST),ison);
	//  if (!dist) EnableWindow(GetDlgItem(hWnd,IDC_AP_CUSTOMMTLL2),ison);
	EnableWindow(GetDlgItem(hparam,IDC_SP_VIEWDISPBOX),ison);
	EnableWindow(GetDlgItem(spawn,IDC_AP_OBJECTQUEUE),ison);
	EnableWindow(GetDlgItem(hWnd,IDC_ANIMATION_OFFSET_LABEL),ison);
}

void ICEnterDir(HWND hWnd,BOOL ison,BOOL pickon=FALSE)
{ if (ison)
  { UIHelpers::SpinnerOn(hWnd,IDC_AP_MOVEDIRXSPIN,IDC_AP_MOVEDIRX);
   UIHelpers::SpinnerOn(hWnd,IDC_AP_MOVEDIRYSPIN,IDC_AP_MOVEDIRY);
   UIHelpers::SpinnerOn(hWnd,IDC_AP_MOVEDIRZSPIN,IDC_AP_MOVEDIRZ);
  }
  else
  { UIHelpers::SpinnerOff(hWnd,IDC_AP_MOVEDIRXSPIN,IDC_AP_MOVEDIRX);
   UIHelpers::SpinnerOff(hWnd,IDC_AP_MOVEDIRYSPIN,IDC_AP_MOVEDIRY);
   UIHelpers::SpinnerOff(hWnd,IDC_AP_MOVEDIRZSPIN,IDC_AP_MOVEDIRZ);
  }
  EnableWindow(GetDlgItem(hWnd,IDC_AP_MOVEDIRX_TXT),ison);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_MOVEDIRY_TXT),ison);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_MOVEDIRZ_TXT),ison);
  if ((ison)||(pickon)) 
   UIHelpers::SpinnerOn(hWnd,IDC_AP_MOVEDIRVARSPIN,IDC_AP_MOVEDIRVAR);
  else
   UIHelpers::SpinnerOff(hWnd,IDC_AP_MOVEDIRVARSPIN,IDC_AP_MOVEDIRVAR);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_MOVEDIRVAR_TXT),(ison)||(pickon));
  EnableWindow(GetDlgItem(hWnd,IDC_AP_MOVEDIRVAR_PCNT),(ison)||(pickon));
  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJECTDIRPICK,pickon);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_PCLOUDOBJDIRNAME),pickon);
}

void ICPCCheckInstButtons(IParamBlock *pblock,HWND hptype)
{ int isinst;
  pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,isinst,FOREVER);
  float width;
  pblock->GetValue(FSParticlePCloud::PB_EMITRWID,0,width,FOREVER);
  UIHelpers::SetButtonEnabled(hptype,IDC_AP_OBJECTPICK,((isinst==INSTGEOM)&&(width>=0.01f)));
}
void ICPCCheckLifeButtons(int stype,HWND spawn)
{ int rep;
  rep = SendMessage(GetDlgItem(spawn,IDC_AP_LIFEQUEUE),LB_GETCURSEL,0,0);
  UIHelpers::SetButtonEnabled(spawn,IDC_AP_LIFEQUEUEADD,(stype>1));
  UIHelpers::SetButtonEnabled(spawn,IDC_AP_LIFEQUEUEREPL,(stype>1)&&(rep>-1));
  UIHelpers::SetButtonEnabled(spawn,IDC_AP_LIFEQUEUEDEL,(stype>1)&&(rep>-1));
}
void ICPCCheckSpawnButtons(IParamBlock *pblock,HWND spawn,int repi)
{ int stype;
  pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
  EnableWindow(GetDlgItem(spawn,IDC_AP_OBJECTQUEUE),(stype>1));
  UIHelpers::SetButtonEnabled(spawn,IDC_AP_OBJECTQUEUEPICK,(stype>1));
  UIHelpers::SetButtonEnabled(spawn,IDC_AP_OBJQUEUEREPLACE,(stype>1)&&(repi>-1));
  UIHelpers::SetButtonEnabled(spawn,IDC_AP_OBJQUEUEDELETE,(stype>1)&&(repi>-1));
  ICPCCheckLifeButtons(stype,spawn);
}

void ICPCCheckPickButtons(IParamBlock *pblock,HWND hptype,HWND spawn,int repi)
{ ICPCCheckInstButtons(pblock,hptype);
  ICPCCheckSpawnButtons(pblock,spawn,repi);
}

int ICPCloudEmitterCreateCallback::proc(
      ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
{
#ifdef _3D_CREATE
   DWORD snapdim = SNAP_IN_3D;
#else
   DWORD snapdim = SNAP_IN_PLANE;
#endif

#ifdef _OSNAP
   if (msg == MOUSE_FREEMOVE)
   { vpt->SnapPreview(m,m,NULL, snapdim);
   }
#endif
   Point3 d;
   if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
      switch(point)  {
         case 0:
            // if hidden by category, re-display particles and objects
            GetCOREInterface()->SetHideByCategoryFlags(
                  GetCOREInterface()->GetHideByCategoryFlags() & ~(HIDE_OBJECTS|HIDE_PARTICLES));
            sp0 = m;
            p0 = vpt->SnapPoint(m,m,NULL,snapdim);
            mat.SetTrans(p0);
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRWID,0,0.01f);
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRHEIGHT,0,0.01f);
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRDEPTH,0,0.01f);
            p0 = vpt->SnapPoint(m,m,NULL,snapdim);
            p1 = p0 + Point3(.01,.01,.01);
            mat.SetTrans(float(.5)*(p0+p1));          
            po->pmapParam->Invalidate();
            break;

         case 1: 
            sp1 = m;
            p1 = vpt->SnapPoint(m,m,NULL,snapdim);
            if ((po->createmeth==1)||(po->createmeth==2))
            { d = p1-p0;}
            else
            {p1.z = p0.z +(float).01; 
            if((flags&MOUSE_CTRL) ||(po->createmeth==3))
            { mat.SetTrans(p0);  } 
            else mat.SetTrans(float(.5)*(p0+p1));
            d = p1-p0;
            square = FALSE;
            if (flags&MOUSE_CTRL) {
               // Constrain to square base
               float len;
               if (fabs(d.x) > fabs(d.y)) len = d.x;
               else len = d.y;
               d.x = d.y = 2.0f * len;
               square = TRUE;
               }
            }
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRWID,0,float(fabs(d.y)));
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRHEIGHT,0,float(fabs(d.x)));

            if (msg==MOUSE_POINT)
            { 
				if (Length(sp1-sp0)<3 || Length(d)<0.1f) 
				{
					return CREATE_ABORT; 
				}
				else if ( ( po->createmeth == 1 ) || ( po->createmeth == 3 ) )
				{
					ICEnableFillPickButton( po->pblock, po->hparam );
					UIHelpers::SetButtonEnabled(po->hptype,IDC_AP_OBJECTPICK,TRUE);
					UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJECTQUEUEPICK,TRUE);
					if ( po->pickCB.repi > -1 )
					{
						UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJQUEUEREPLACE,TRUE);
					}

					po->pblock->SetValue(FSParticlePCloud::PB_EMITRDEPTH,0,float(fabs(d.y)));
					po->pmapParam->Invalidate();
					
					return CREATE_STOP;
				}
            }
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRDEPTH,0,float(fabs(d.z)));
            po->pmapParam->Invalidate();
            break;
         case 2:
            p1.z = p0.z + vpt->SnapLength(vpt->GetCPDisp(p1,Point3(0,0,1),sp1,m));
            if (po->createmeth==2)
            { Point3 tmp=p0;tmp.z=float(.5)*(p0.z+p1.z);
              mat.SetTrans(tmp);
            }
            else if (!square)
            { mat.SetTrans(float(.5)*(p0+p1)); }

            d = p1-p0;
            if (square) {
               // Constrain to square base
               float len;
               if (fabs(d.x) > fabs(d.y)) len = d.x;
               else len = d.y;
               d.x = d.y = 2.0f * len;             
               }

            po->pblock->SetValue(FSParticlePCloud::PB_EMITRWID,0,float(fabs(d.y)));
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRHEIGHT,0,float(fabs(d.x)));
            po->pblock->SetValue(FSParticlePCloud::PB_EMITRDEPTH,0,float(fabs(d.z)));
            po->pmapParam->Invalidate();           
               
            if (msg==MOUSE_POINT)
			{ 
                ICPCCheckPickButtons(po->pblock,po->hptype,po->spawn,po->pickCB.repi);
				ICEnableFillPickButton( po->pblock, po->hparam );
				return CREATE_STOP;  
			}
            break;

         }
   } else {
      if (msg == MOUSE_ABORT)
         return CREATE_ABORT;
      }
   return 1;
   }

static ICPCloudEmitterCreateCallback pcloudemitterCallback;

CreateMouseCallBack* ICPCloudParticle::GetCreateMouseCallBack() 
   {
   pcloudemitterCallback.po = this;
   return &pcloudemitterCallback;
   }

void ICPCParticleGenDlgProc::Update(TimeValue t)
{  if (!po->editOb) return;
   int ison; 
   po->pblock->GetValue(FSParticlePCloud::PB_SPEEDDIR,0,ison,FOREVER);
   ICEnterDir(po->hgen,ison==1,ison==2);
   po->ShowName(shownormname);
}

INT_PTR ICPCParticleGenDlgProc::DlgProc( TimeValue t, IParamMap *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{  
	int acourse;
	float size;
	switch (msg) 
	{ 
		case WM_INITDIALOG: 
		{
			iBut = GetICustButton( GetDlgItem( hWnd, IDC_AP_OBJECTDIRPICK ) );
			iBut->SetType( CBT_CHECK );
			iBut->SetHighlightColor( GREEN_WASH );
			Update( t );

			// Disable uniqueness group box
			EnableWindow( GetDlgItem( hWnd, IDC_UNIQUENESS_GROUPBOX ), IC_PCLOUD_ENABLE_UNIQUENESS_GROUPBOX );
			UIHelpers::SetButtonEnabled(hWnd,IDC_AP_NEWSEED, IC_PCLOUD_ENABLE_UNIQUENESS_GROUPBOX );
			UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_SP_GENSEEDSPIN, IDC_SP_GENSEED, IDC_SP_GENSEED_LABEL, IC_PCLOUD_ENABLE_UNIQUENESS_GROUPBOX );

			// Disable invalid speed directions
			EnableWindow( GetDlgItem( hWnd, IDC_AP_PCLOUDDIRRND ), IC_PCLOUD_ENABLE_SPEEDDIR_RANDOM );
			EnableWindow( GetDlgItem( hWnd, IDC_AP_PCLOUDDIRENTER ), IC_PCLOUD_ENABLE_SPEEDDIR_VECTOR );
			EnableWindow( GetDlgItem( hWnd, IDC_AP_PCLOUDDIROBJ ), IC_PCLOUD_ENABLE_SPEEDDIR_REFERENCE_OBJECT );

			break;
		}
      case WM_DESTROY:
         // Release all our Custom Controls
         ReleaseICustButton(iBut);
         return FALSE;
      case WM_COMMAND:
         case CC_SPINNER_CHANGE:
         switch ( LOWORD(wParam) ) 
         { case IDC_SP_GENSIZESPIN:
               { po->pblock->GetValue(FSParticlePCloud::PB_METAAUTOCOARSE,t,acourse,FOREVER);
               int mpart;
                po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,t,mpart,FOREVER);
                if ((mpart==METABALLS)&&(acourse) )
                { po->pblock->GetValue(FSParticlePCloud::PB_SIZE,t,size,FOREVER);
                  po->pblock->SetValue(FSParticlePCloud::PB_METACOURSE,t,size/coursedivider);
                  po->pblock->SetValue(FSParticlePCloud::PB_METACOURSEV,t,size/3.0f);
                  po->pmapPType->Invalidate();
                }
                 return TRUE;
               }  
            case IDC_AP_NEWSEED:
               { srand( (unsigned)time( NULL ) );
                 int newseed=rand() % 25001;
                 po->pblock->SetValue(FSParticlePCloud::PB_RNDSEED,0,newseed);
                 po->pmapPGen->Invalidate();
                 break;
               }
            case IDC_AP_PCLOUDDIRRND:
            case IDC_AP_PCLOUDDIRENTER:
            case IDC_AP_PCLOUDDIROBJ:
               { int ison; 
               po->pblock->GetValue(FSParticlePCloud::PB_SPEEDDIR,0,ison,FOREVER);
               ICEnterDir(hWnd,ison==1,ison==2);
                 return TRUE;
               }
            case IDC_AP_OBJECTDIRPICK:
                if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
               { if (po->creating) 
                  {  theCreatePCloudMode.JumpStart(po->ip,po);
                     po->ip->SetCommandMode(&theCreatePCloudMode);
                  } else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
               } else 
                  { po->pickNorm.po = po;                
                    po->ip->SetPickMode(&po->pickNorm);
                  }
               break;
         }
     }
   return FALSE;
   }

void DbgPCCheckStretchBox(HWND hWnd,ICPCloudParticle *po)
{ if (IsStdMtl(po->cnode))
  { EnableWindow(GetDlgItem(hWnd,IDC_AP_PBLURON),TRUE);
  } else EnableWindow(GetDlgItem(hWnd,IDC_AP_PBLURON),FALSE);
  UIHelpers::SpinnerOn(hWnd,IDC_AP_STRETCHSPIN,IDC_AP_STRETCH);
  EnableWindow(GetDlgItem(hWnd,IDC_AP_STRETCH_TXT),TRUE);
  if ( po->pmapPSpin )
  {
	po->pmapPSpin->Invalidate();
  }
}

void DbgPCStretchStuff(int dir,int fragflags,HWND hWnd,ICPCloudParticle *po)
{ if (dir==0)
  { EnableWindow(GetDlgItem(hWnd,IDC_AP_PBLURON),FALSE);
   UIHelpers::SpinnerOff(hWnd,IDC_AP_STRETCHSPIN,IDC_AP_STRETCH);
   EnableWindow(GetDlgItem(hWnd,IDC_AP_STRETCH_TXT),FALSE);
   SpinStuff(hWnd,FALSE,fragflags!=METABALLS);
  }
  else if (dir==1)
  { DbgPCCheckStretchBox(hWnd,po);
   SpinStuff(hWnd,FALSE,fragflags!=METABALLS);
  }  
  else if (dir==2)
  {   EnableWindow(GetDlgItem(hWnd,IDC_AP_PBLURON),FALSE);
   UIHelpers::SpinnerOff(hWnd,IDC_AP_STRETCHSPIN,IDC_AP_STRETCH);
   EnableWindow(GetDlgItem(hWnd,IDC_AP_STRETCH_TXT),FALSE);
   SpinStuff(hWnd,TRUE,fragflags!=METABALLS);
  }
}

void DbgCourseCheck(ICPCloudParticle *po,HWND hWnd,TimeValue t)
{ int acourse;
  float size;
  po->pblock->GetValue(FSParticlePCloud::PB_METAAUTOCOARSE,t,acourse,FOREVER);
  if (acourse) 
  { float mc1,mc2,mc,mcv;
   po->pblock->GetValue(FSParticlePCloud::PB_SIZE,t,size,FOREVER);mc=size/coursedivider;mcv=size/3.0f;
   po->pblock->GetValue(FSParticlePCloud::PB_METACOURSE,t,mc1,FOREVER);
   po->pblock->GetValue(FSParticlePCloud::PB_METACOURSEV,t,mc2,FOREVER);
   if ((mc1!=mc)||(mc2!=mcv))
   { po->pblock->SetValue(FSParticlePCloud::PB_METACOURSE,t,mc);
     po->pblock->SetValue(FSParticlePCloud::PB_METACOURSEV,t,mcv);
     po->pmapPType->Invalidate();
   }
   UIHelpers::SpinnerOff(hWnd,IDC_SP_METCOURSESPIN,IDC_SP_METCOURSE);
   UIHelpers::SpinnerOff(hWnd,IDC_SP_METCOURSEVSPIN,IDC_SP_METCOURSEV);
   EnableWindow(GetDlgItem(hWnd,IDC_SP_COARSENESS_TXT),FALSE);
   EnableWindow(GetDlgItem(hWnd,IDC_SP_METCOURSE_TXT),FALSE);
   EnableWindow(GetDlgItem(hWnd,IDC_SP_METCOURSEV_TXT),FALSE);
  }
  else 
  { UIHelpers::SpinnerOn(hWnd,IDC_SP_METCOURSESPIN,IDC_SP_METCOURSE);
   UIHelpers::SpinnerOn(hWnd,IDC_SP_METCOURSEVSPIN,IDC_SP_METCOURSEV);
   EnableWindow(GetDlgItem(hWnd,IDC_SP_COARSENESS_TXT),TRUE);
   EnableWindow(GetDlgItem(hWnd,IDC_SP_METCOURSE_TXT),TRUE);
   EnableWindow(GetDlgItem(hWnd,IDC_SP_METCOURSEV_TXT),TRUE);
  }
}

void ICPCAniFr(HWND hWnd,IParamBlock *pblock)
{ 
	int anitype;
	pblock->GetValue( FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD, 0, anitype, FOREVER );
	
	if ( anitype > 1 )
		UIHelpers::SpinnerOn( hWnd, IDC_AP_ANIRNDFRSPIN, IDC_AP_ANIRNDFR );
	else
		UIHelpers::SpinnerOff( hWnd, IDC_AP_ANIRNDFRSPIN, IDC_AP_ANIRNDFR );

	EnableWindow( GetDlgItem( hWnd, IDC_ANIMATION_OFFSET_LABEL ), anitype > 1 );
}

void DbgPCSetRateSpinner(IParamBlock *pblock,TimeValue t,HWND hWnd)
{ 
  //EnableWindow(GetDlgItem(hWnd,IDC_SP_GENUSERATE),TRUE);
  //EnableWindow(GetDlgItem(hWnd,IDC_SP_GENUSETTL),TRUE);
}

void ICStdStuff(HWND hWnd,BOOL ison)
{
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPETRI),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_TRIANGLE );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPECUB),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_CUBE );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPESPC),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SPECIAL );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPEFAC),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_FACING );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPEPIX),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_CONSTANT );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPE6PNT),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SIXPOINT );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPETET),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_TETRA );
	EnableWindow(GetDlgItem(hWnd,IDC_SP_TYPESPHERE),ison && IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SPHERE );
}

void ICInstOn(ICPCloudParticle *po,HWND hWnd,TimeValue t)
{ if ((po->fragflags==METABALLS)||(po->fragflags<0))
  { MetaOff(hWnd);
  }
  if (po->fragflags<=0)
  { ICStdStuff(hWnd,FALSE);  }
  po->fragflags=INSTGEOM;
  ICInstStuff(hWnd,TRUE,po->hparam,po->spawn,FALSE);
  //ICPCAniFr(hWnd,po->pblock);
  int sptype;
  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJECTPICK,TRUE);
  EnableWindow(GetDlgItem(po->hrot,IDC_AP_PARTICLEDIRTRAVL),TRUE);
  EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPBOX),TRUE);
  po->pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,sptype,FOREVER);
  DbgPCStretchStuff(sptype,INSTGEOM,po->hrot,po);
  SpinMainStuff(po->hrot,TRUE);
  DbgPCSetRateSpinner(po->pblock,t,po->hgen);
  int stype;po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
  ObjectMutQueOn(stype,po->spawn,po->pickCB.repi);
  if ( po->pmapPSpin )
  {
	po->pmapPSpin->Invalidate();
  }
}

void DbgMetaIn(ICPCloudParticle *po,HWND hWnd,TimeValue t)
{ int lastflag=po->fragflags;
  po->fragflags=METABALLS;
  UIHelpers::SpinnerOn(hWnd,IDC_SP_METTENSSPIN,IDC_SP_METTENS);
  UIHelpers::SpinnerOn(hWnd,IDC_SP_METTENSVARSPIN,IDC_SP_METTENSVAR);
  EnableWindow(GetDlgItem(hWnd,IDC_SP_METTENS_TXT),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_SP_METTENSVAR_TXT),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_SP_METTENSVAR_PCNT),TRUE);
  EnableWindow(GetDlgItem(hWnd,IDC_SP_AUTOCOARSE),TRUE);
  DbgCourseCheck(po,hWnd,t);
  EnableWindow(GetDlgItem(hWnd,IDC_SP_DRAFTMODE),TRUE);
  EnableWindow(GetDlgItem(po->hrot,IDC_AP_PARTICLEDIRTRAVL),FALSE);
  DbgPCStretchStuff(0,METABALLS,po->hrot,po);
  SpinMainStuff(po->hrot,FALSE);
  EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPMESH),TRUE);
  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJECTPICK,FALSE);
  EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPBOX),FALSE);
  ICStdStuff(hWnd,FALSE);
  DbgPCSetRateSpinner(po->pblock,t,po->hgen);
  if (lastflag<=0)
  { ICStdStuff(hWnd,FALSE); }
  if ((lastflag==INSTGEOM)||(lastflag<0))
  {  ICInstStuff(hWnd,FALSE,po->hparam,po->spawn,FALSE);
     int maptype,viewpt;
     po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,maptype,FOREVER);
     if (maptype==2) po->pblock->SetValue(FSParticlePCloud::PB_CUSTOMMTL,0,0);
     po->pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,viewpt,FOREVER);
    if (viewpt==3) po->pblock->SetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,1);
    po->pmapParam->Invalidate();
  }
  ObjectMutQueOff(po->spawn);
  int stype;
  po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
//  if (stype>1) po->pblock->SetValue(FSParticlePCloud::PB_SPAWNTYPE,0,0);
//  AllSpawnBad(po->spawn,0,FALSE);
  int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,0,ison,FOREVER);
  if (ison) stype=0;
  SpawnStuff(po->spawn,stype);

  if ( po->pmapPSpin )
  {
	po->pmapPSpin->Invalidate();
  }
}

void ICStdOn(ICPCloudParticle *po,HWND hWnd,TimeValue t)
{ if ((po->fragflags==METABALLS)||(po->fragflags<0))
  { MetaOff(hWnd);
  }
  if ((po->fragflags==INSTGEOM)||(po->fragflags<0))
  {  ICInstStuff(hWnd,FALSE,po->hparam,po->spawn,FALSE);
     int maptype,viewpt;
     po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,maptype,FOREVER);
     if (maptype==2) po->pblock->SetValue(FSParticlePCloud::PB_CUSTOMMTL,0,0);
     po->pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,viewpt,FOREVER);
    if (viewpt==3) po->pblock->SetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,1);
    po->pmapParam->Invalidate();
  }
  ICStdStuff(hWnd,TRUE);
  po->fragflags=0;
  EnableWindow(GetDlgItem(po->hrot,IDC_AP_PARTICLEDIRTRAVL),TRUE);
 EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPBOX),FALSE);
  int sptype;
  po->pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,sptype,FOREVER);
  if ( po->hrot )
  {
	  DbgPCStretchStuff(sptype,0,po->hrot,po);
	  SpinMainStuff(po->hrot,TRUE);
  }
  EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPMESH),TRUE);
  if ( po->spawn )
  {
	ObjectMutQueOff(po->spawn);
  }
  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJECTPICK,FALSE);
  UIHelpers::SpinnerOff(hWnd,IDC_SP_METTENSSPIN,IDC_SP_METTENS);
}

void ICPCParticleSpinDlgProc::Update(TimeValue t)
{   int axis;
   po->pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,t,axis,FOREVER);
   DbgPCStretchStuff(axis,po->fragflags,po->hrot,po);
   int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,t,ison,FOREVER);
   int stype; po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,t,stype,FOREVER);
   IPCControls(po->hrot,po->spawn,stype,ison);
}

INT_PTR ICPCParticleSpinDlgProc::DlgProc(
      TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{  switch (msg) {
      case WM_INITDIALOG: 
      {break;
      }
      case WM_COMMAND:
         switch (LOWORD(wParam)) 
         { case IDC_AP_PARTICLEDIRTRAVL:
             DbgPCStretchStuff(1,po->fragflags,hWnd,po);
           break;
           case IDC_AP_PARTICLEDIRRND:
            DbgPCStretchStuff(0,po->fragflags,hWnd,po);
           break;
           case IDC_AP_PARTICLEDIRUSER:
            DbgPCStretchStuff(2,po->fragflags,hWnd,po);
            break;
           case IDC_INTERP_BOUNCEON:
            {  int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,t,ison,FOREVER);
               int stype; po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,t,stype,FOREVER);
               IPCControls(hWnd,po->spawn,stype,ison);
            }
            break;
         }
      default:
         return FALSE;
   }
   return TRUE;
}

void DbgAddMtl(ICPCloudParticle *po,TimeValue t)
{ 
	if (po->cnode)
	{ 
		int subtree,frag,custmtl=0,submtl=0;
		po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
		po->pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
		po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,frag,FOREVER);
		if ((po->custnode)&&(frag==INSTGEOM)&& custmtl) 
		{
			po->AssignMtl(po->cnode,po->custnode,subtree,t);
		}
		po->valid=FALSE;
		po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	}
}

void ICPCParticleDisableDlgProc::Update(TimeValue t)
{
	if (!po->editOb) return;
	po->ShowName(0);
	int chunky,stype;
	po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
	po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,chunky,FOREVER);
	// AllSpawnBad(po->spawn,stype,chunky!=METABALLS);
	int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,0,ison,FOREVER);
	if (ison) stype=0;
	if ( po->spawn )
	{
		SpawnStuff(po->spawn,stype);
		ICPCCheckSpawnButtons(po->pblock,po->spawn,po->pickCB.repi=0);
	}
	if (chunky==METABALLS) DbgMetaIn(po,po->hptype,t);
	else if (chunky==0) ICStdOn(po,po->hptype,t);
	else ICInstOn(po,po->hptype,t);
	ICPCCheckInstButtons(po->pblock,po->hptype);
}

INT_PTR ICPCParticleDisableDlgProc::DlgProc( TimeValue t, IParamMap *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{  
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_OBJECTPICK));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(GREEN_WASH);
			
			Update(t);

			// Force disable ui parameters
#if !IC_PCLOUD_ENABLE_META_PARTICLES
			EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPEMET),FALSE);
#endif

#if !IC_PCLOUD_ENABLE_INSTANCED_GEOMETRY_PARTICLES
			EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPEINSTANCE),FALSE);
#endif
			break;
		}
      case WM_DESTROY:
         // Release all our Custom Controls
         ReleaseICustButton(iBut);
         return FALSE;
      case WM_COMMAND:
         switch (LOWORD(wParam)) 
         {  case IDC_AP_OBJECTPICK:
               { po->flags=0;
               if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
               { if (po->creating) 
                  {  theCreatePCloudMode.JumpStart(po->ip,po);
                     po->ip->SetCommandMode(&theCreatePCloudMode);
                  } else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
               } else 
                  { po->pickCB.po = po;
                     po->pickCB.dodist=0;
                    po->ip->SetPickMode(&po->pickCB);
                  }
               break;
            }
         case IDC_AP_UPDATEMTL:
            { DbgAddMtl(po,t);
              int custmtl;
              po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
              if ((po->cnode)&&(custmtl))
              { EnableWindow(GetDlgItem(hWnd,IDC_SP_MAPCUSTOMEMIT),FALSE);
                po->origmtl=po->cnode->GetMtl();
              }
              break;
            }
            case IDC_SP_TYPESTD:
            { po->pblock->SetValue(FSParticlePCloud::PB_CUSTOMMTL,0,0);
              ICStdOn(po,hWnd,t);
              po->valid=FALSE;
              if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
              { if (po->creating) 
               {  theCreatePCloudMode.JumpStart(po->ip,po);
                  po->ip->SetCommandMode(&theCreatePCloudMode);
               } else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
              }
             break;
            }
           case IDC_SP_TYPETRI:
           case IDC_SP_TYPECUB:
           case IDC_SP_TYPESPC:
           case IDC_SP_TYPE6PNT:
           case IDC_SP_TYPESPHERE:
            { 
              EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPMESH),TRUE);
             break;
            }
           case IDC_SP_TYPETET:
            {            
              EnableWindow(GetDlgItem(po->hparam,IDC_SP_VIEWDISPMESH),TRUE);
             break;
            }
           case IDC_SP_MAPCUSTOMEMIT:
           case IDC_SP_MAPCUSTOMINST:
            { 
              int dir;
              po->pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,dir,FOREVER);
              if (dir==1)  DbgPCCheckStretchBox(po->hrot,po);
             break;
            }
           case IDC_SP_TYPEMET:
              {po->pblock->SetValue(FSParticlePCloud::PB_CUSTOMMTL,0,0);
               DbgMetaIn(po,hWnd,t);
                po->valid=FALSE;
              if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
              { if (po->creating) 
               {  theCreatePCloudMode.JumpStart(po->ip,po);
                  po->ip->SetCommandMode(&theCreatePCloudMode);
               } else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
              }
              break;
              }
           case IDC_SP_TYPEINSTANCE:
              { int custmtl,vshow,anioff;
                po->pblock->SetValue(FSParticlePCloud::PB_CUSTOMMTL,0,1);
               ICInstOn(po,hWnd,t);
               po->pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,vshow,FOREVER);
               if (vshow>1)
               {int subtree;
                po->pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
                po->pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
                TimeValue aniend=GetAnimEnd();
                int anifr=aniend+GetTicksPerFrame();
                po->GetTimes(po->times,t,anifr,anioff);
                if (vshow==2)
                { po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
                   po->GetMesh(t,subtree,custmtl);
                }
               else po->GetallBB(po->custnode,subtree,t);
               }
                po->valid=FALSE;
              break;
              }
           case IDC_SP_AUTOCOARSE:
            { int chunky;
              po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,chunky,FOREVER);
              if (chunky==METABALLS)
                DbgCourseCheck(po,hWnd,t);
              break;
            }
         }
         break;   
      default:
         return FALSE;
      }
   return TRUE;
   }

void ICPCAnimationKeyingDlgProc::SetAnimationOffsetWidgetsEnabled( HWND hWnd, BOOL bEnabled )
{
	UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_AP_ANIRNDFRSPIN, IDC_AP_ANIRNDFR, IDC_ANIMATION_OFFSET_LABEL, bEnabled );
	UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_ANIMATION_OFFSET_VARIATION_SPINNER, IDC_ANIMATION_OFFSET_VARIATION_EDITTEXT, IDC_ANIMATION_OFFSET_VARIATION_LABEL, bEnabled );
}

void ICPCAnimationKeyingDlgProc::SetAnimationScalingWidgetsEnabled( HWND hWnd, BOOL bEnabled )
{
	UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_ANIMATION_SCALING_SRC_LENGTH_SPINNER, IDC_ANIMATION_SCALING_SRC_LENGTH_EDITTEXT, IDC_ANIMATION_SCALING_SRC_LENGTH_LABEL, bEnabled );
	UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_ANIMATION_SCALING_SRC_VARIATION_SPINNER, IDC_ANIMATION_SCALING_SRC_VARIATION_EDITTEXT, IDC_ANIMATION_SCALING_SRC_VARIATION_LABEL, bEnabled );
	UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_ANIMATION_SCALING_DST_MULTIPLIER_SPINNER, IDC_ANIMATION_SCALING_DST_MULTIPLIER_EDITTEXT, IDC_ANIMATION_SCALING_DST_MULTIPLIER_LABEL, bEnabled );
	UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_ANIMATION_SCALING_DST_VARIATION_SPINNER, IDC_ANIMATION_SCALING_DST_VARIATION_EDITTEXT, IDC_ANIMATION_SCALING_DST_VARIATION_LABEL, bEnabled );
}

void ICPCAnimationKeyingDlgProc::Update(TimeValue t)
{
	if (!po->editOb) return;
	//po->ShowName(0);
}

INT_PTR ICPCAnimationKeyingDlgProc::DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			Update(t);
	
			int animationOffsetMethod = 0;
			po->pblock->GetValue( FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD, t, animationOffsetMethod, FOREVER );
			SetAnimationOffsetWidgetsEnabled( hWnd, animationOffsetMethod > 0 );

			int animationScalingMethod = 0;
			po->pblock->GetValue( FSParticlePCloud::PB_ANIMATION_SCALING_METHOD, t, animationScalingMethod, FOREVER );
			SetAnimationScalingWidgetsEnabled( hWnd, animationScalingMethod > 0 );
			break;
		}
		case WM_DESTROY:
		{
			// Release all our Custom Controls
			//ReleaseICustButton(iBut);
			return FALSE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{  
				case IDC_AP_NOANIOFF:
				{
					SetAnimationOffsetWidgetsEnabled( hWnd, FALSE );
					break;
				}
				case IDC_AP_ANIOFFBIRTH:
				case IDC_AP_ANIOFFRND:
				{
					SetAnimationOffsetWidgetsEnabled( hWnd, TRUE );
					break;
				}
				case IDC_ANIMATION_SCALING_NONE_RADIOBUTTON:
				{
					SetAnimationScalingWidgetsEnabled( hWnd, FALSE );
					break;
				}
				case IDC_ANIMATION_SCALING_LIFE_RADIOBUTTON:
				case IDC_ANIMATION_SCALING_SPECIFIC_RADIOBUTTON:
				{
					SetAnimationScalingWidgetsEnabled( hWnd, TRUE );
					break;
				}
			}
			break;
		}
		default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

void ICPCParticleParmDlgProc::Update(TimeValue t)
{
	if (!po->editOb) return;
	float width;
	po->pblock->GetValue(FSParticlePCloud::PB_EMITRWID,0,width,FOREVER);
	
	if (width<0.01f) 
		iBut->Disable(); 
	else 
		iBut->Enable();
	
	po->ShowName(1);
	po->pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,po->createmeth,FOREVER);
	
	if (po->createmeth>0)
		UIHelpers::SpinnerOff(po->hparam,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
	else 
		UIHelpers::SpinnerOn(po->hparam,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
	
	EnableWindow(GetDlgItem(po->hparam,IDC_AP_EMITHGT_TXT),po->createmeth==0);
	
	if ((po->createmeth==0)||(po->createmeth==2))
		UIHelpers::SpinnerOn(po->hparam,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);
	else 
		UIHelpers::SpinnerOff(po->hparam,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);

	EnableWindow(GetDlgItem(po->hparam,IDC_AP_EMITDEP_TXT),(po->createmeth==0)||(po->createmeth==2));

	ICEnableFillPickButton( po->pblock, po->hparam );
	EnableWindow(GetDlgItem(po->hparam,IDC_AP_FILLPICKOBJECT),po->createmeth==3);
	//EnableWindow(GetDlgItem(po->hparam,IDC_AP_FILLPICKGROUPBOX),po->createmeth==3);
}
   
INT_PTR ICPCParticleParmDlgProc::DlgProc( TimeValue t, IParamMap *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg)
	{
		case WM_INITDIALOG:
		{
			iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_FILLPICKBUTTON));
			iBut->SetType(CBT_CHECK);
			iBut->SetHighlightColor(GREEN_WASH);
			Update(t);

			// Enabled/Disabling Particles Displayed
			UIHelpers::SetSpinnerEditTextLabelEnabled( hWnd, IDC_SP_GENDISPSPIN, IDC_SP_GENDISP, IDC_SP_GENDISP_LABEL, IC_PCLOUD_ENABLE_PERCENTAGE_PARTICLES_DISPLAYED );
			EnableWindow( GetDlgItem( hWnd, IDC_SP_GENDISP_PERCENTAGE_LABEL ), IC_PCLOUD_ENABLE_PERCENTAGE_PARTICLES_DISPLAYED );

			// Particle Display Type
			EnableWindow( GetDlgItem( hWnd, IDC_SP_VIEWDISPDOT ), IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_DOTS );
			EnableWindow( GetDlgItem( hWnd, IDC_SP_VIEWDISPTIK ), IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_TICKS );
			EnableWindow( GetDlgItem( hWnd, IDC_SP_VIEWDISPMESH ), IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_MESH );
			EnableWindow( GetDlgItem( hWnd, IDC_SP_VIEWDISPBOX ), IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_BOUNDING_BOX );

			EnableWindow( GetDlgItem( hWnd, IDC_AP_FILLBOX ), IC_PCLOUD_ENABLE_EMITTER_SHAPE_BOX );
			EnableWindow( GetDlgItem( hWnd, IDC_AP_FILLSPHERE ), IC_PCLOUD_ENABLE_EMITTER_SHAPE_SPHERE );
			EnableWindow( GetDlgItem( hWnd, IDC_AP_FILLCYLINDER ), IC_PCLOUD_ENABLE_EMITTER_SHAPE_CYLINDER );
			EnableWindow( GetDlgItem( hWnd, IDC_AP_FILLOBJECT ), IC_PCLOUD_ENABLE_EMITTER_SHAPE_OBJECT );

			break;
		}
		case WM_DESTROY:
		{
			// Release all our Custom Controls
			ReleaseICustButton(iBut);
			return FALSE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{   
				case IDC_AP_FILLPICKBUTTON:
				{
					if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
					{ 
						if (po->creating) 
						{
							theCreatePCloudMode.JumpStart(po->ip,po);
							po->ip->SetCommandMode(&theCreatePCloudMode);
						} 
						else 
						{
							po->ip->SetStdCommandMode(CID_OBJMOVE);
						}
					} 
					else 
					{
						po->pickCB.po = po;
						po->pickCB.dodist=1;
						po->ip->SetPickMode(&po->pickCB);
					}
					break;
				}	
				case IDC_AP_FILLBOX:
				{
					po->createmeth=0;
					UIHelpers::SpinnerOn(hWnd,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
					UIHelpers::SpinnerOn(hWnd,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITHGT_TXT),TRUE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITDEP_TXT),TRUE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKOBJECT),FALSE);
					//EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKGROUPBOX),FALSE);
					break;
				}
				case IDC_AP_FILLSPHERE:
				{
					po->createmeth=1;
					UIHelpers::SpinnerOff(hWnd,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
					UIHelpers::SpinnerOff(hWnd,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITHGT_TXT),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITDEP_TXT),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKOBJECT),FALSE);
					//EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKGROUPBOX),FALSE);
					break;
				}
				case IDC_AP_FILLCYLINDER:
				{
					po->createmeth=2;
					UIHelpers::SpinnerOff(hWnd,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
					UIHelpers::SpinnerOn(hWnd,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITHGT_TXT),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITDEP_TXT),TRUE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKOBJECT),FALSE);
					//EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKGROUPBOX),FALSE);
					break;
				}
				case IDC_AP_FILLOBJECT:
				{
					po->createmeth=3;
					UIHelpers::SpinnerOff(hWnd,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
					UIHelpers::SpinnerOff(hWnd,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITHGT_TXT),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_EMITDEP_TXT),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKOBJECT),TRUE);
					//EnableWindow(GetDlgItem(hWnd,IDC_AP_FILLPICKGROUPBOX),TRUE);
					break;
				}
				case IDC_SP_VIEWDISPMESH:
				{
					po->valid=FALSE;
					int subtree,custmtl;
					po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
					po->pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
					int anioff;
					po->pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
					TimeValue aniend=GetAnimEnd();
					int anifr=aniend+GetTicksPerFrame();
					po->GetTimes(po->times,t,anifr,anioff);
					po->GetMesh(t,subtree,custmtl);
					break;
				}
				case IDC_SP_VIEWDISPBOX:         
				{
					po->valid=FALSE;
					int subtree;
					po->pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
					int anioff;
					po->pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
					TimeValue aniend=GetAnimEnd();
					int anifr=aniend+GetTicksPerFrame();
					po->GetTimes(po->times,t,anifr,anioff);
					po->GetallBB(po->custnode,subtree,t);
					break;
				}
			}
			break;   
		}
		default:
		{
			return FALSE;
		}
	}
	return TRUE;
}

void ICPCParticleSpawnDlgProc::Update(TimeValue t)
{ if (!po->editOb) return;
   po->SetUpList();
   float width;
   po->pblock->GetValue(FSParticlePCloud::PB_EMITRWID,0,width,FOREVER);
   if (width<0.01f) iBut->Disable();else iBut->Enable();
   po->SetUpLifeList();
   if (width<0.01f) iButrep->Disable();
   else
   { po->pickCB.repi= SendMessage(GetDlgItem(po->spawn,IDC_AP_OBJECTQUEUE),
               LB_GETCURSEL,0,0);
     if (po->pickCB.repi<0) iButrep->Disable();
     else iButrep->Enable();
   }
   int stype,chunky;
   po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
   po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,chunky,FOREVER);
// AllSpawnBad(po->spawn,stype,chunky!=METABALLS);
    int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,0,ison,FOREVER);
    if (ison) stype=0;
   SpawnStuff(po->spawn,stype);
// ICPCCheckSpawnButtons(po->pblock,po->spawn,po->pickCB.repi);
   if (chunky==INSTGEOM)
     ICPCCheckSpawnButtons(po->pblock,po->spawn,po->pickCB.repi);
    else 
    { ObjectMutQueOff(po->spawn);
      ICPCCheckLifeButtons(stype,po->spawn);
    }
   if (stype==EMIT) 
   {  UIHelpers::SpinnerOff(po->spawn,IDC_AP_MAXSPAWNGENSSPIN,IDC_AP_MAXSPAWNGENS);
      EnableWindow(GetDlgItem(po->spawn,IDC_AP_MAXSPAWNDIEAFTER_TXT),FALSE);
   }
}

INT_PTR ICPCParticleSpawnDlgProc::DlgProc(
      TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{  int dtype=2,stype,rep;  
   switch (msg) {
      case WM_INITDIALOG: 
      { iBut = GetICustButton(GetDlgItem(hWnd,IDC_AP_OBJECTQUEUEPICK));
        iBut->SetType(CBT_CHECK);
        iBut->SetHighlightColor(GREEN_WASH);
        iButrep = GetICustButton(GetDlgItem(hWnd,IDC_AP_OBJQUEUEREPLACE));
        iButrep->SetType(CBT_CHECK);
        iButrep->SetHighlightColor(GREEN_WASH);
        Update(t);
        break;
      }
      case WM_DESTROY:
         // Release all our Custom Controls
         ReleaseICustButton(iBut);
         ReleaseICustButton(iButrep);
         return FALSE;
      case WM_COMMAND:
         switch (LOWORD(wParam)) 
         {   case IDC_AP_NOSPAWN:stype=0;goto spawnradio;
           case IDC_AP_COLLIDEDIE:
              stype=1;goto spawnradio;
           case IDC_AP_SPAWNTRAILS:stype=EMIT;goto spawnradio;
           case IDC_AP_COLLIDESPAWN:
           case IDC_AP_DEATHSPAWN: stype=2;
         spawnradio: 
            int chunky;
         po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,chunky,FOREVER);
//       AllSpawnBad(po->spawn,stype,chunky!=METABALLS);
            int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,0,ison,FOREVER);
            if (ison) stype=0;
            SpawnStuff(po->spawn,stype);     
             if (chunky==INSTGEOM)
               ICPCCheckSpawnButtons(po->pblock,hWnd,po->pickCB.repi);
             else 
             {  ObjectMutQueOff(po->spawn);
                ICPCCheckLifeButtons(stype,po->spawn);
             }
             if (stype==EMIT) 
             { UIHelpers::SpinnerOff(po->spawn,IDC_AP_MAXSPAWNGENSSPIN,IDC_AP_MAXSPAWNGENS);
               EnableWindow(GetDlgItem(hWnd,IDC_AP_MAXSPAWNDIEAFTER_TXT),FALSE);
             }
            break;
         case IDC_AP_OBJECTQUEUE:
            { po->pickCB.repi= SendMessage(GetDlgItem(hWnd,IDC_AP_OBJECTQUEUE),
                     LB_GETCURSEL,0,0);
             UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJQUEUEREPLACE,po->pickCB.repi>-1);
             UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJQUEUEDELETE,po->pickCB.repi>-1);
            break;
            }
            case IDC_AP_OBJECTQUEUEPICK:
               dtype=2;goto dopick;
            case IDC_AP_OBJQUEUEREPLACE:
              dtype=3;
              dopick:
               {if (po->ip->GetCommandMode()->ID() == CID_STDPICK) 
               { if (po->creating) 
                  {  theCreatePCloudMode.JumpStart(po->ip,po);
                     po->ip->SetCommandMode(&theCreatePCloudMode);
                  } else {po->ip->SetStdCommandMode(CID_OBJMOVE);}
               } else 
                  { po->pickCB.po = po;
                     po->pickCB.dodist=dtype;
                    po->ip->SetPickMode(&po->pickCB);
                  }
               break;
            }
            case IDC_AP_OBJQUEUEDELETE:
            {  int i = SendMessage(GetDlgItem(hWnd,IDC_AP_OBJECTQUEUE),
                     LB_GETCURSEL,0,0);
               if ((po->nlist.Count()>0)&&(i>-1))
               {  theHold.Begin();
                  po->DeleteFromList(i,0);
                  theHold.Accept(GetString(IDS_AP_OBJDEL));}
                  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJQUEUEREPLACE,0);
                  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_OBJQUEUEDELETE,0);
               break;
            }
            case IDC_AP_LIFEQUEUEDEL:
            {  int i = SendMessage(GetDlgItem(hWnd,IDC_AP_LIFEQUEUE),
                     LB_GETCURSEL,0,0);
               int Lcnt=po->llist.Count();
               if ((Lcnt>0)&&(i>-1)&&(i<Lcnt))
               {  theHold.Begin();
                  theHold.Put(new ICPCLifeListRestore(po));
                  po->DeleteFromLifeList(i);
                  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEREPL,0);
                  UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEDEL,0);
                  theHold.Accept(GetString(IDS_AP_LIFEDEL));
               }
               po->valid=FALSE;
               po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
               break;
            }
            case IDC_AP_LIFEQUEUEADD:
            {   int i;
               po->pblock->GetValue(FSParticlePCloud::PB_SPAWNLIFEVLUE,t,i,FOREVER);
               theHold.Begin();
               theHold.Put(new ICPCLifeListRestore(po));
               po->AddToLifeList(i);
                UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEREPL,0);
               UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEDEL,0);
               theHold.Accept(GetString(IDS_AP_LIFEADD));
               po->valid=FALSE;
               po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
               break;
            }
            case IDC_AP_LIFEQUEUE:
            { rep = SendMessage(GetDlgItem(hWnd,IDC_AP_LIFEQUEUE),LB_GETCURSEL,0,0);
              UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEREPL,rep>-1);
              UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEDEL,rep>-1);
              break;
            }
            case IDC_AP_LIFEQUEUEREPL:
            { int i;
            rep = SendMessage(GetDlgItem(hWnd,IDC_AP_LIFEQUEUE),
                     LB_GETCURSEL,0,0);
              if (rep>-1)
              {   po->pblock->GetValue(FSParticlePCloud::PB_SPAWNLIFEVLUE,t,i,FOREVER);
               theHold.Begin();
               theHold.Put(new ICPCLifeListRestore(po));
               po->llist[rep]=i;
               po->SetUpLifeList();
                UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEREPL,0);
               UIHelpers::SetButtonEnabled(hWnd,IDC_AP_LIFEQUEUEDEL,0);
               theHold.Accept(GetString(IDS_AP_LIFEREP));
               po->valid=FALSE;
               po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
              }
               break;
            }
            
         }
         break;   
      default:
         return FALSE;
      }
   return TRUE;
}

void ICPCloudParticle::GetFilename(TCHAR *filename)
{   
	_tcscpy( filename, ip->GetDir( APP_PLUGCFG_DIR ) );
	int len = static_cast<int>( _tcslen( filename ) ); // SR DCAST64: Downcast to 2G limit.
	if ( len )
	{  
		if ( filename[len-1]!=_T('\\'))
		{
			_tcscat(filename,_T("\\"));
		}
	}
	_tcscat(filename,GetString(IDS_AP_ICPCLOUDCST));
}

void ICPCloudParticle::SetupTargetList()    
{
	TCHAR filename[MAX_PATH];
	FILE *f;
	int vers,i,Namelen,osize;

	custsettings = i = 0; // LAM - 6/9/02 - initialize i so if fileok == false, will display error in onerr.
	GetFilename( filename );
	BOOL fileok = TRUE;
	if ( ( f = _tfopen( filename, _T( "rb" )) ) == NULL )
	{
		return;
	}

	if((FReadInt(&custsettings,f))&&(FReadInt(&vers,f))&&(FReadInt(&osize,f))&&(FReadInt(&Namelen,f)))
	{
		if ( ((Namelen!=NLEN)&& (fileok=GenNewNameLen(size,custsettings,f,filename,ICPCloudBackwardCompatiblity::k_currentVersionNumber)))||
			((vers!=ICPCloudBackwardCompatiblity::k_currentVersionNumber)&&(fileok=GenNewSaveFile(osize,size,custsettings,f,filename,ICPCloudBackwardCompatiblity::k_currentVersionNumber))))
		{ 
			fileok=((FReadInt(&custsettings,f))&&(FReadInt(&vers,f))&&(FReadInt(&osize,f))&&(FReadInt(&Namelen,f)));
		}
		if (fileok&&(vers==ICPCloudBackwardCompatiblity::k_currentVersionNumber))
		{ 
			NameLst = new AName[custsettings]; 
			for (i=0;i<custsettings;i++)
			{
				if (fread(NameLst[i],NLEN,1,f)==1)
					fseek(f,size,SEEK_CUR);
				else 
					goto onerr;
			}
		}
	}
onerr:
	if (i<custsettings) 
	{ 
		custsettings=0;
		MessageBox (NULL,GetString(IDS_RB_BADFILE),"", MB_ICONINFORMATION);
	}
	if (fileok) fclose(f);
	UpdatePresetListBox(GetDlgItem(hParams2, IDC_SP_SETLIST), custsettings, NameLst);
}

int ICPCloudParticle::RemSettings(int overwrite,TCHAR *newname)
{ TCHAR filename[MAX_PATH];
  FILE *f;
  long startpt;
  int vers,newsets,baselen=size+NLEN;
  GetFilename(filename);
  vers=ICPCloudBackwardCompatiblity::k_currentVersionNumber;
  newsets=custsettings-1;
  if ((f = _tfopen(filename,_T("r+b"))) == NULL) 
  { MessageBox(NULL,GetString(IDS_AP_WRITEPRO),"", MB_ICONINFORMATION);
   return 0;
  }
  if (custsettings==1) 
  {fclose(f);remove(filename);custsettings=0;delete[] NameLst;NameLst=NULL;
    SendMessage(GetDlgItem(hParams2,IDC_SP_SETLIST),LB_RESETCONTENT,0,0);
   return (1);}
 if (!FWriteInt(&newsets,f)) {fclose(f);return 0;}
  startpt=overwrite*(baselen)+HLEN;
  fseek(f,startpt,SEEK_SET); 
  int i;
  BYTE *buf;
  buf=new BYTE[baselen];
  assert(buf);
  long cpos=startpt;
  for (i=overwrite+1;i<custsettings;i++)
  {   fseek(f,cpos+baselen,SEEK_SET);
   if (fread(buf,baselen,1,f)!=1) {delete[] buf;fclose(f);return 0;}
   fseek(f,cpos,SEEK_SET);
   if (fwrite(buf,baselen,1,f)!=1) {delete[] buf;fclose(f);return 0;}
   cpos+=baselen;
  }
  delete[] buf;
  _chsize(_fileno(f),ftell(f));
  fclose(f);                          
  AName *Tmp=new AName[custsettings-1];
  int newi=0;
  for (i=0;i<custsettings;i++)
  { if (i!=overwrite) 
      _tcscpy(Tmp[newi++],NameLst[i]);}
  delete []NameLst;NameLst=Tmp;
  custsettings--;
  UpdatePresetListBox(GetDlgItem(hParams2, IDC_SP_SETLIST), custsettings, NameLst);
  return(1);
}
int ICPCloudParticle::SaveSettings(int overwrite,TCHAR *newname)
{ TCHAR filename[MAX_PATH];
  FILE *f;
  int vers,newsets,Namelen=NLEN;

  if ((overwrite>-1)&&(MessageBox (NULL,GetString(IDS_AP_SETEXISTS),GetString(IDS_AP_WARNING), MB_ICONQUESTION | MB_YESNO ) == IDNO))
    return 0;
  GetFilename(filename);
  vers=ICPCloudBackwardCompatiblity::k_currentVersionNumber;
  newsets=custsettings+1;
  if ((f = _tfopen(filename,(custsettings==0?_T("wb"):_T("r+b")))) == NULL)
  { MessageBox(NULL,GetString(IDS_AP_WRITEPRO),"", MB_ICONINFORMATION);
   return 0;
  }
  // longest one
  ParamBlockDescID *descVer=ICPCloudBackwardCompatiblity::k_currentVersion;
  int plength=(ICPCloudBackwardCompatiblity::k_currentVersionParamCount);
  if (custsettings==0)
  { if (!(FWriteInt(&newsets,f)&&FWriteInt(&vers,f)&&FWriteInt(&size,f)&&FWriteInt(&Namelen,f))) goto errend;
  } 
  else 
  { if (overwrite>=0) 
    { overwrite=overwrite*(size+NLEN)+HLEN;
      fseek(f,overwrite,SEEK_SET); }
    else 
   { if (!FWriteInt(&newsets,f)) goto errend;
     fseek(f,0,SEEK_END);
   }
  }    
  int ival,i;
  float fval;
  if (fwrite(newname,1,NLEN,f)!=NLEN) goto errend;
  for (i=0;i<plength;i++)
  {   if (descVer[i].type==TYPE_INT) 
    { pblock->GetValue(i,0,ival,FOREVER);
      if (fwrite(&ival,isize,1,f)!=1) goto errend;
   }
    else
    { pblock->GetValue(i,0,fval,FOREVER);
      if (fwrite(&fval,fsize,1,f)!=1) goto errend;
   }
  }
  fclose(f);
  if (overwrite<0) 
  { AName *Tmp=new AName[custsettings+1];
    memcpy(Tmp,NameLst,sizeof(AName)*custsettings);
   delete []NameLst;NameLst=Tmp;
    _tcscpy(NameLst[custsettings],newname);
   custsettings++;
   UpdatePresetListBox(GetDlgItem(hParams2, IDC_SP_SETLIST), custsettings, NameLst);
  }  
  return(1);
errend: fclose(f);return(0);
}
int ICPCloudParticle::GetSettings(int setnum,TCHAR *newname)
{ TCHAR filename[MAX_PATH];
  FILE *f;
  GetFilename(filename);
  if ((f = _tfopen(filename,_T("rb"))) == NULL) return 0;
  { setnum=setnum*(size+NLEN)+HLEN;
    fseek(f,setnum,SEEK_SET); 
  }    
  int ival,i;
  float fval;
  // longest one
  ParamBlockDescID *descVer=ICPCloudBackwardCompatiblity::k_currentVersion;
  int plength=(ICPCloudBackwardCompatiblity::k_currentVersionParamCount);
  if (fread(newname,1,NLEN,f)!=NLEN) goto errend;
  for (i=0;i<plength;i++)
  {   if (descVer[i].type==TYPE_INT) 
    { if (fread(&ival,isize,1,f)!=1) goto errend;
     pblock->SetValue(i,0,ival);
   }
    else
    { if (fread(&fval,fsize,1,f)!=1) goto errend;
//   if (i!=FSParticlePCloud::PB_EMITRWID) 
        pblock->SetValue(i,0,fval);
   }
  }
  fclose(f);
  InvalidateUI();                   
  return(1);
  errend: fclose(f);return(0);
}

static INT_PTR CALLBACK CustomSettingParamDlgProc( 
      HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{ TCHAR newname[NLEN];
  int i,save=0;
  ICPCloudParticle *po = DLGetWindowLongPtr<ICPCloudParticle*>(hWnd);
  if (!po && message!=WM_INITDIALOG) return FALSE;

  switch (message) {
      case WM_INITDIALOG: {
         po = (ICPCloudParticle*)lParam;
         DLSetWindowLongPtr(hWnd, lParam);
         po->custCtrlEdit = GetICustEdit(GetDlgItem(hWnd,IDC_SP_SETEDIT));
         po->custCtrlEdit->SetText(_T(""));
         po->hParams2 = hWnd;
         po->SetupTargetList();
         break;
         }
      case WM_COMMAND:
         { switch(LOWORD(wParam)) {
            case IDC_SP_SETLIST:  
               if (HIWORD(wParam)==LBN_DBLCLK)
                 goto doload;
               break;
            case IDC_SP_DELETE:  
               i = SendMessage(GetDlgItem(po->hParams2,IDC_SP_SETLIST),
                     LB_GETCURSEL,0,0);
                  if ((i>-1)&&(po->custsettings>0))
                  { po->custCtrlEdit->GetText(newname,NLEN);
                   if (_tcscmp(newname,po->NameLst[i])==0)
                     po->custCtrlEdit->SetText(_T(""));
                    po->RemSettings(i,po->NameLst[i]);
                  }
               break;
            case IDC_SP_SAVE:
               save=1;
            case IDC_SP_LOAD:
               doload:
               if (!save)
               { i = SendMessage(GetDlgItem(po->hParams2,IDC_SP_SETLIST),
                     LB_GETCURSEL,0,0);
                  if ((i>-1)&&(po->custsettings>0))
                   po->custCtrlEdit->SetText(po->NameLst[i]);
               }
                po->custCtrlEdit->GetText(newname,NLEN);
               if (save) 
               {int tstblk=0,tstlen=static_cast<int>(_tcslen(newname)); // SR DCAST64: Downcast to 2G limit.
                 while ((tstblk<tstlen)&&(newname[tstblk]==' ')) tstblk++;
                 if (tstblk>=tstlen) 
                  MessageBox (NULL,GetString(IDS_RB_NONAME),
            "", MB_ICONINFORMATION);
                 else 
                 { i=0;
                   while ((i<po->custsettings)&&(_tcscmp(newname,po->NameLst[i])))
                    i++;
                  if (i>=po->custsettings) i=-1;
                   po->SaveSettings(i,newname);
                 }
               }
               else if ((i>-1)&&(i<po->custsettings))
               { po->GetSettings(i,newname);
                 int chunky;
                   po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,chunky,FOREVER);
                 if (chunky==METABALLS) DbgMetaIn(po,po->hptype,po->ip->GetTime());
                  else 
                 { int stype;
                   po->pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
//                AllSpawnBad(po->spawn,stype,TRUE);
                  int ison; po->pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,0,ison,FOREVER);
                  if (ison) stype=0;
                  SpawnStuff(po->spawn,stype);
                  if (chunky==0) ICStdOn(po,po->hptype,po->ip->GetTime());
                   else ICInstOn(po,po->hptype,po->ip->GetTime());
                   if (stype==EMIT)
                  {  UIHelpers::SpinnerOff(po->spawn,IDC_AP_MAXSPAWNGENSSPIN,IDC_AP_MAXSPAWNGENS);
                     EnableWindow(GetDlgItem(hWnd,IDC_AP_MAXSPAWNDIEAFTER_TXT),FALSE);
                  }
                 }
                 ICPCCheckSpawnButtons(po->pblock,po->spawn,po->pickCB.repi=0);
             if (chunky==INSTGEOM)
             { int onscreen;
               po->pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,onscreen,FOREVER);
               if (onscreen>1)
               { int subtree,custmtl=0,anioff;
               TimeValue t=po->ip->GetTime();
                po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
                po->pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
                po->pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
                TimeValue aniend=GetAnimEnd();
                int anifr=aniend+GetTicksPerFrame();
                po->GetTimes(po->times,t,anifr,anioff);
                if (onscreen==2)
                   po->GetMesh(po->ip->GetTime(),subtree,custmtl);
                  else po->GetallBB(po->custnode,subtree,po->ip->GetTime());
               }
             }
                 po->ip->RedrawViews(po->ip->GetTime()); 
               }
               else MessageBox (NULL,GetString(IDS_RB_BADNAME),
            "", MB_ICONINFORMATION);
               break;
            }
         break;
         }

      case WM_DESTROY:
         // Release all our Custom Controls
         ReleaseICustEdit(po->custCtrlEdit);
         if (po->NameLst) {delete []po->NameLst;po->NameLst=NULL;}
         return FALSE;
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MOUSEMOVE:
         po->ip->RollupMouseMessage(hWnd,message,wParam,lParam);
         return FALSE;
      
      default: return FALSE;     
      }
   return TRUE;
   }  


//--- ICPCloudParticle Methods--------------------------------------------
TimeValue ICPCloudParticle::ParticleLife(TimeValue t, int i)
{   
	int pcount=parts.Count();
	if (!(i<pcount)) return 0;
	return sdata[i].L;
}

void ICPCloudParticle::ResetSystem(TimeValue t,BOOL full)
{  
	lc.lastmin=-1;
	lc.lastcollide=-1;

	rcounter=0;
	vcounter=0;
	
	if (full)
	{
		tvalid = t;
		valid  = TRUE;
	}
}

Point3 ICPCloudParticle::ParticlePosition(TimeValue t,int i)
{  
	int pcount=parts.points.Count();
	if (!(i<pcount)) return Zero;
	Point3 FinalP=parts.points[i];
	// figure out particle class
	int isinst;
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,isinst,FOREVER);
	// if we have custom geometry, find the center of the bounding box and velocity normal
	if ((isinst==INSTGEOM)&&(custnode))
	{  
		int anioff;
		float dlgsize;
		dlgsize=parts.radius[i];
		float zoffset=0.0f;
		Point3 OffsetV=Zero;;
		TimeValue aniend=GetAnimEnd();
		int anifr=aniend+GetTicksPerFrame();
		pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
		TimeValue Ctime=(anioff?GetCurTime(sdata[i].showframe,(anioff>1?t:parts.ages[i]),anifr):t);
		int nCount=nlist.Count();
		int mnum=TimeFound(times,Ctime,(sdata[i].gennum>nCount?nCount:sdata[i].gennum));
		if (mnum>=0) zoffset=cmbb[mnum].Center().z;
		// if we're using DoT/MBlur, account for scaling due to stretch
		int axisentered;
		pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,axisentered,FOREVER);
		if (axisentered==DIRTRAVEL)
		{  
			int K;
			pblock->GetValue(FSParticlePCloud::PB_STRETCH,t,K,FOREVER);
			float strlen=GetLen(parts.vels[i],K);
			zoffset*=strlen;
		}
		float lenV=Length(parts.vels[i]);
		if (lenV>EPSILON) OffsetV=parts.vels[i]/lenV;
		FinalP+=zoffset*dlgsize*OffsetV;
	}
	return FinalP;
}

Point3 ICPCloudParticle::ParticleVelocity(TimeValue t,int i)
{  
	Point3 retvel=Zero;
	int pcount=parts.vels.Count();
	if (i<pcount)
		retvel=parts.vels[i];
	return retvel;
}

float ICPCloudParticle::ParticleSize(TimeValue t,int i)
{  
	float strlen=1.0f;
	float boxlen=1.0f;
	float dlgsize;
	int axisentered,K,isinst,ptype;
	// get the size/scale from the dialog box processed for this particle...
	dlgsize=parts.radius[i];
	// figure out particle type and class
	pblock->GetValue(FSParticlePCloud::PB_PARTICLETYPE,0,ptype,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,isinst,FOREVER);
	// if there are custom particles, find bounding box params...
	if ((isinst==INSTGEOM)&&(custnode))
	{  
		int anioff;
		TimeValue aniend=GetAnimEnd();
		int anifr=aniend+GetTicksPerFrame();
		pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
		TimeValue Ctime=(anioff?GetCurTime(sdata[i].showframe,(anioff>1?t:parts.ages[i]),anifr):t);
		int nCount=nlist.Count();
		int mnum=TimeFound(times,Ctime,(sdata[i].gennum>nCount?nCount:sdata[i].gennum));
		if (mnum>=0) boxlen=cmbb[mnum].Width().z;
	}

	// if we're using MBlur and DoT then account for scaling along DoT
	pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,axisentered,FOREVER);
	if (axisentered==DIRTRAVEL)
	{ 
		pblock->GetValue(FSParticlePCloud::PB_STRETCH,t,K,FOREVER);
		strlen=GetLen(parts.vels[i],K);
	}
	float templen=boxlen*strlen;
	if ((isinst!=FSParticlePCloud::PB_PARTICLECLASS)&&(ptype==RENDTET)) templen*=1.3f;
	return templen*dlgsize; 
}

int ICPCloudParticle::ParticleCenter(TimeValue t,int i)
{  
	int ptype,isinst;
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,isinst,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_PARTICLETYPE,0,ptype,FOREVER);
	if (isinst==INSTGEOM) return PARTCENTER_CENTER;
	if (ptype==RENDTET) return PARTCENTER_HEAD;
	return PARTCENTER_CENTER;  
}

int ICPCloudParticle::RenderBegin(TimeValue t, ULONG flags)
{  
	SetAFlag(A_RENDER);
	ParticleInvalid();      
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	cancelled=FALSE;
	int isinst;
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,isinst,FOREVER);
	if ((isinst==INSTGEOM) &&(custnode))
		custnode->SetProperty(PROPID_FORCE_RENDER_MESH_COPY,(void *)1);
	return 0;
}

int ICPCloudParticle::RenderEnd(TimeValue t)
{
	ClearAFlag(A_RENDER);
	ParticleInvalid();      
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	int isinst;
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,isinst,FOREVER);
	if ((isinst==INSTGEOM) &&(custnode))
		custnode->SetProperty(PROPID_FORCE_RENDER_MESH_COPY,(void *)0);
	/*    TimeValue aniend=GetAnimEnd();
	int subtree,onscreen,custmtl=0;
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,frag,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,onscreen,FOREVER);
	if ((frag==INSTGEOM)&&(onscreen>1)&&(custnode))
	{ pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
	thePCloudDraw.anioff=anioff;
	thePCloudDraw.anifr=aniend+GetTicksPerFrame();
	thePCloudDraw.t=t;
	int anioff;
	pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
	if (anioff>0)
	GetTimes(times,thePCloudDraw.t,thePCloudDraw.anifr,anioff);
	else times.tl.ZeroCount();
	if (onscreen==2)
	GetMesh(t,subtree,custmtl);
	else GetallBB(custnode,subtree,t);
	}*/
	return 0;
}

void ICPCloudParticle::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	SimpleParticle::BeginEditParams(ip,flags,prev);
	editOb = this;
	this->ip = ip;

	if ( flags & BEGIN_EDIT_CREATE )
	{
		creating = TRUE;
	} 
	else 
	{ 
		creating = FALSE; 
	}

	if ( !pmapParam )
	{
		fragflags=-1;
	}

	// Init Phase 1
	{

#define ADD_PMAP_ROLLOUT( pmap, descParam, rollupIdd, rollupNameIds, rollupIdFlag, rollupHWnd ) \
{ \
	if ( pmap ) \
	{ \
		pmap->SetParamBlock( pblock ); \
	} \
	else \
	{ \
		pmap = CreateCPParamMap ( \
			descParam,ARRAY_SIZE( descParam ), \
			pblock, \
			ip, \
			hInstance, \
			MAKEINTRESOURCE(rollupIdd), \
			GetString(rollupNameIds), \
			dflags&rollupIdFlag?0:APPENDROLL_CLOSED \
		); \
	} \
	 \
	rollupHWnd = pmap->GetHWnd(); \
}

		ADD_PMAP_ROLLOUT( pmapParam, descParamPCloud, IDD_SUPRPRTS_PARTICLECLOUD, IDS_RB_PARAMETERS, APRTS_ROLLUP1_OPEN, hparam );
		ADD_PMAP_ROLLOUT( pmapPGen, descParamPGen, IDD_SUPRPRTS_IC_GEN_PC, IDS_RB_PGEN, APRTS_ROLLUP2_OPEN, hgen );

#if IC_PCLOUD_ENABLE_ROLLOUT_TYPE
		ADD_PMAP_ROLLOUT( pmapPType, descParamPType, IDD_SUPRPRTS_IC_PARTTYPE_PC, IDS_RB_PTYPE, APRTS_ROLLUP3_OPEN, hptype );
#endif

		ADD_PMAP_ROLLOUT( pPMapAnimationKeying, descParamAnimationKeying, IDD_SUPRPRTS_IC_ANIMATION_KEYING, IDS_RB_ANIMATION_KEYING, APRTS_ROLLUP4_OPEN, hAnimationKeying );

#if IC_PCLOUD_ENABLE_ROLLOUT_SPIN
		ADD_PMAP_ROLLOUT( pmapPSpin, descParamPSpin, IDD_SUPRPRTS_ROT, IDS_RB_SPIN, APRTS_ROLLUP5_OPEN, hrot );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_OBJECT_MOTION_INHERITANCE
		ADD_PMAP_ROLLOUT( pmapEmitV, descParamEmitV, IDD_SUPRPRTS_EMITV, IDS_RB_EMITV, APRTS_ROLLUP6_OPEN, hexts );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_BUBBLE_MOTION
		ADD_PMAP_ROLLOUT( pmapBubl, descParamBubl, IDD_SUPRPRTS_BUBL, IDS_RB_BUBL, APRTS_ROLLUP7_OPEN, hbubl );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_PARTICLE_SPAWN
		ADD_PMAP_ROLLOUT( pmapSpawn, descPSpawning, IDD_SUPRPRTS_SPAWNING, IDS_AP_PSPAWN, APRTS_ROLLUP8_OPEN, spawn );
#endif

		#undef ADD_PMAP_ROLLOUT
	}

	// Init Phase 2
	{
		pmapParam->SetUserDlgProc( new ICPCParticleParmDlgProc( this ) );
		pmapPGen->SetUserDlgProc( new ICPCParticleGenDlgProc( this ) );

#if IC_PCLOUD_ENABLE_ROLLOUT_TYPE
		pmapPType->SetUserDlgProc( new ICPCParticleDisableDlgProc(this) );
#endif

		pPMapAnimationKeying->SetUserDlgProc( new ICPCAnimationKeyingDlgProc(this) );

#if IC_PCLOUD_ENABLE_ROLLOUT_SPIN
		pmapPSpin->SetUserDlgProc( new ICPCParticleSpinDlgProc(this) );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_OBJECT_MOTION_INHERITANCE
		pmapEmitV->SetUserDlgProc( NULL );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_BUBBLE_MOTION
		pmapBubl->SetUserDlgProc( NULL );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_PARTICLE_SPAWN
		pmapSpawn->SetUserDlgProc( new ICPCParticleSpawnDlgProc(this) );
#endif

#if IC_PCLOUD_ENABLE_ROLLOUT_PRESETS
		if ( hParams2 )
		{
			DLSetWindowLongPtr(hParams2, this);
		}
		else
		{
			hParams2 = ip->AddRollupPage( 
				hInstance, 
				MAKEINTRESOURCE(IDD_SUPRPRTS_SAVE),
				CustomSettingParamDlgProc, 
				GetString(IDS_RB_LOADSAVE), 
				(LPARAM)this,dflags&APRTS_ROLLUP9_OPEN?0:APPENDROLL_CLOSED 
			);    

			ip->RegisterDlgWnd(hParams2);
		}
#endif // IC_PCLOUD_ENABLE_ROLLOUT_PRESETS
	}
}  

void ICPCloudParticle::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	SimpleParticle::EndEditParams(ip,flags,next);

#define REMOVE_PMAP_ROLLOUT( pmap, rollupHWnd, rollupIdFlag ) \
{ \
	if ( rollupHWnd ) \
	{ \
		SetFlag( dflags, rollupIdFlag, IsRollupPanelOpen( rollupHWnd ) ); \
	} \
	 \
	if ( pmap && flags & END_EDIT_REMOVEUI ) \
	{ \
		DestroyCPParamMap( pmap ); \
		pmap  = NULL; \
	} \
}

	REMOVE_PMAP_ROLLOUT( pmapParam, hparam, APRTS_ROLLUP1_OPEN );
	REMOVE_PMAP_ROLLOUT( pmapPGen, hgen, APRTS_ROLLUP2_OPEN );
	REMOVE_PMAP_ROLLOUT( pmapPType, hptype, APRTS_ROLLUP3_OPEN );
	REMOVE_PMAP_ROLLOUT( pPMapAnimationKeying, hAnimationKeying, APRTS_ROLLUP4_OPEN );
	REMOVE_PMAP_ROLLOUT( pmapPSpin, hrot, APRTS_ROLLUP5_OPEN );
	REMOVE_PMAP_ROLLOUT( pmapEmitV, hexts, APRTS_ROLLUP6_OPEN );
	REMOVE_PMAP_ROLLOUT( pmapBubl, hbubl, APRTS_ROLLUP7_OPEN );
	REMOVE_PMAP_ROLLOUT( pmapSpawn, spawn, APRTS_ROLLUP8_OPEN );

#undef REMOVE_PMAP_ROLLOUT

	if ( hParams2 )
	{
		SetFlag(dflags,APRTS_ROLLUP9_OPEN,IsRollupPanelOpen(hParams2));

		if (flags&END_EDIT_REMOVEUI)
		{
			ip->UnRegisterDlgWnd(hParams2);
			ip->DeleteRollupPage(hParams2);
			hParams2 = NULL;

			//hgen=NULL;
		}
		else
		{
			DLSetWindowLongPtr(hParams2, NULL);
		}
	}

	ip->ClearPickMode();
	ip= NULL;
	creating = FALSE;
}

// TODO ?
void ICPCloudParticle::MapKeys(TimeMap *map,DWORD flags)
{  
	Animatable::MapKeys(map,flags);
	TimeValue TempTime;

	// scaled values
	pblock->GetValue(FSParticlePCloud::PB_LIFE,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(FSParticlePCloud::PB_LIFE,0,TempTime);
	pblock->GetValue(FSParticlePCloud::PB_LIFEVAR,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(FSParticlePCloud::PB_LIFEVAR,0,TempTime);
	pblock->GetValue(FSParticlePCloud::PB_GROWTIME,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(FSParticlePCloud::PB_GROWTIME,0,TempTime);
	pblock->GetValue(FSParticlePCloud::PB_FADETIME,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(FSParticlePCloud::PB_FADETIME,0,TempTime);
	TempTime=map->map(TempTime);
	pblock->GetValue(FSParticlePCloud::PB_SPINTIME,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(FSParticlePCloud::PB_SPINTIME,0,TempTime);
	pblock->GetValue(FSParticlePCloud::PB_BUBLPER,0,TempTime,FOREVER);
	TempTime=map->map(TempTime);
	pblock->SetValue(FSParticlePCloud::PB_BUBLPER,0,TempTime);

	// pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET,0,TempTime,FOREVER);
	// TempTime=map->map(TempTime);
	// pblock->SetValue(FSParticlePCloud::PB_ANIMATION_OFFSET,0,TempTime);
}  

Interval ICPCloudParticle::GetValidity(TimeValue t)
   {
   // For now...
   return Interval(t,t);
   }

void ICPCloudParticle::BuildEmitter(TimeValue t, Mesh& amesh)
   {
   float width,height,depth,wholewid;
   int sym,csymv=20,csymf=18,startv,startf;
   mvalid = FOREVER;
    pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,sym,FOREVER);
   pblock->GetValue(FSParticlePCloud::PB_EMITRWID,t,height,mvalid);
   pblock->GetValue(FSParticlePCloud::PB_EMITRHEIGHT,t,width,mvalid);
   pblock->GetValue(FSParticlePCloud::PB_EMITRDEPTH,t,depth,mvalid);
   wholewid=width;
   width  *= 0.5f;
   height  *= 0.5f;
   depth  *= 0.5f;

   if (sym==0)
   { mesh.setNumVerts(9+csymv);startv=9;
     mesh.setNumFaces(12+csymf);startf=12;
     mesh.setVert(0, Point3(-width,-height, depth));
     mesh.setVert(1, Point3( width,-height, depth));
     mesh.setVert(2, Point3( width, height, depth));
     mesh.setVert(3, Point3(-width, height, depth));
     mesh.setVert(4, Point3(-width,-height, -depth));
     mesh.setVert(5, Point3( width,-height, -depth));
     mesh.setVert(6, Point3( width, height, -depth));
     mesh.setVert(7, Point3(-width, height, -depth));
     mesh.setVert(8, Point3(0,0,0));

     mesh.faces[0].setEdgeVisFlags(1,0,1);
     mesh.faces[0].setSmGroup(0);
     mesh.faces[0].setVerts(0,1,3);

     mesh.faces[1].setEdgeVisFlags(1,1,0);
     mesh.faces[1].setSmGroup(0);
     mesh.faces[1].setVerts(1,2,3);

     mesh.faces[2].setEdgeVisFlags(1,1,0);
     mesh.faces[2].setSmGroup(0);
     mesh.faces[2].setVerts(1,0,4);
 
     mesh.faces[3].setEdgeVisFlags(1,0,1);
     mesh.faces[3].setSmGroup(0);
     mesh.faces[3].setVerts(5,1,4);

     mesh.faces[4].setEdgeVisFlags(0,1,1);
     mesh.faces[4].setSmGroup(0);
     mesh.faces[4].setVerts(5,2,1);

     mesh.faces[5].setEdgeVisFlags(1,1,0);
     mesh.faces[5].setSmGroup(0);
     mesh.faces[5].setVerts(5,6,2);

     mesh.faces[6].setEdgeVisFlags(1,0,1);
     mesh.faces[6].setSmGroup(0);
     mesh.faces[6].setVerts(2,6,3);

     mesh.faces[7].setEdgeVisFlags(1,1,0);
     mesh.faces[7].setSmGroup(0);
     mesh.faces[7].setVerts(6,7,3);

     mesh.faces[8].setEdgeVisFlags(1,1,0);
     mesh.faces[8].setSmGroup(0);
     mesh.faces[8].setVerts(0,3,7);

     mesh.faces[9].setEdgeVisFlags(1,0,1);
     mesh.faces[9].setSmGroup(0);
     mesh.faces[9].setVerts(4,0,7);

     mesh.faces[10].setEdgeVisFlags(0,1,1);
     mesh.faces[10].setSmGroup(0);
     mesh.faces[10].setVerts(6,4,7);

     mesh.faces[11].setEdgeVisFlags(1,1,0);
     mesh.faces[11].setSmGroup(0);
     mesh.faces[11].setVerts(6,5,4);
   }
   else if (sym==1)
   { 
      height *= 2.0f;
      float u,cu,su;
     #define NUM_SEGS 12
     startf=3*NUM_SEGS;
     startv=startf+1;
     mesh.setNumVerts(startv+csymv);
     mesh.setNumFaces(startf+csymf);

     for (int i=0; i<NUM_SEGS; i++) {
         u = float(i)/float(NUM_SEGS) * TWOPI;
         cu=(float)cos(u)*height;su=(float)sin(u)*height;
         mesh.setVert(i, Point3(cu, su, 0.0f));
         mesh.setVert(i+NUM_SEGS, Point3(0.0f, cu, su));
         mesh.setVert(i+2*NUM_SEGS, Point3(cu, 0.0f, su));
         }
     mesh.setVert(3*NUM_SEGS, Point3(0.0f, 0.0f, 0.0f));
      
     for (int i=0; i<3*NUM_SEGS; i++) {
         int i1 = i+1;
         if (i1%NUM_SEGS==0) i1 -= NUM_SEGS;
         mesh.faces[i].setEdgeVisFlags(1,0,0);
         mesh.faces[i].setSmGroup(0);
         mesh.faces[i].setVerts(i,i1,3*NUM_SEGS);
         }
   }
   else if (sym==2)
   { float u,cu,su;
     #define NUM_SEGS 12
     startv=(startf=2*NUM_SEGS);
     mesh.setNumVerts(startv+csymv);
     mesh.setNumFaces(startf+csymf);
      
     for (int i=0; i<NUM_SEGS; i++) {
         u = float(i)/float(NUM_SEGS) * TWOPI;
         cu=(float)cos(u)*height;su=(float)sin(u)*height;
         mesh.setVert(i, Point3(cu, su, depth));
         mesh.setVert(i+NUM_SEGS, Point3(cu,su, -depth));
         }
     int il=NUM_SEGS;
     for (int f=0,i=0; f<2*NUM_SEGS; f+=2) {
         int i1 = i+1,i2=il+1;
         if (i1%NUM_SEGS==0) {i1 -= NUM_SEGS;i2-=NUM_SEGS;}
         mesh.faces[f].setEdgeVisFlags(1,0,1);
         mesh.faces[f].setSmGroup(0);
         mesh.faces[f].setVerts(i,il,i1);
         mesh.faces[f+1].setEdgeVisFlags(1,1,0);
         mesh.faces[f+1].setSmGroup(0);
         mesh.faces[f+1].setVerts(il,i2,i1);
         il++;i++;
         }
   }
   else
   { mesh.setNumVerts(35);
     mesh.setNumFaces(28);
/*   if (distnode)
     { Object *cobj=distnode->EvalWorldState(t).obj;
      TriObject *triOb=TriIsUseable(cobj,t);
      if (triOb)
      { Box3 bbox=triOb->mesh.getBoundingBox();
        Point3 lens=(bbox.pmax-bbox.pmin)/2.0f;
        width=lens.x;
         height=lens.y;
         depth=lens.z;
      }
     }*/
     width=2*height;depth=height;wholewid=width*2;
     mesh.setVert(0, Point3(-width,-height, depth));
     mesh.setVert(1, Point3( width,-height, depth));
     mesh.setVert(2, Point3( width, height, depth));
     mesh.setVert(3, Point3(-width, height, depth));
     mesh.setVert(4, Point3(-width,-height, -depth));
     mesh.setVert(5, Point3( width,-height, -depth));
     mesh.setVert(6, Point3( width, height, -depth));
     mesh.setVert(7, Point3(-width, height, -depth));
     mesh.setVert(8, Point3(0,0,0));

     mesh.faces[0].setEdgeVisFlags(1,0,1);
     mesh.faces[0].setSmGroup(0);
     mesh.faces[0].setVerts(0,1,3);

     mesh.faces[1].setEdgeVisFlags(1,1,0);
     mesh.faces[1].setSmGroup(0);
     mesh.faces[1].setVerts(1,2,3);

     mesh.faces[2].setEdgeVisFlags(1,1,0);
     mesh.faces[2].setSmGroup(0);
     mesh.faces[2].setVerts(1,0,4);
 
     mesh.faces[3].setEdgeVisFlags(1,0,1);
     mesh.faces[3].setSmGroup(0);
     mesh.faces[3].setVerts(5,1,4);

     mesh.faces[4].setEdgeVisFlags(0,1,1);
     mesh.faces[4].setSmGroup(0);
     mesh.faces[4].setVerts(5,2,1);

     mesh.faces[5].setEdgeVisFlags(1,1,0);
     mesh.faces[5].setSmGroup(0);
     mesh.faces[5].setVerts(5,6,2);

     mesh.faces[6].setEdgeVisFlags(1,0,1);
     mesh.faces[6].setSmGroup(0);
     mesh.faces[6].setVerts(2,6,3);

     mesh.faces[7].setEdgeVisFlags(1,1,0);
     mesh.faces[7].setSmGroup(0);
     mesh.faces[7].setVerts(6,7,3);

     mesh.faces[8].setEdgeVisFlags(1,1,0);
     mesh.faces[8].setSmGroup(0);
     mesh.faces[8].setVerts(0,3,7);

     mesh.faces[9].setEdgeVisFlags(1,0,1);
     mesh.faces[9].setSmGroup(0);
     mesh.faces[9].setVerts(4,0,7);

     mesh.faces[10].setEdgeVisFlags(0,1,1);
     mesh.faces[10].setSmGroup(0);
     mesh.faces[10].setVerts(6,4,7);

     mesh.faces[11].setEdgeVisFlags(1,1,0);
     mesh.faces[11].setSmGroup(0);
     mesh.faces[11].setVerts(6,5,4);

     float h6=height*0.625f,h1=height*0.125f,h3=height*0.375f;
     float w4=0.4f*wholewid,w3=0.3f*wholewid,w2=0.2f*wholewid,w1=0.1f*wholewid;

     mesh.setVert(9, Point3(-w4,-h6, 0.0f));
     mesh.setVert(10, Point3(-w3,-h6, 0.0f));
     mesh.setVert(11, Point3(-w3,-h1, 0.0f));
     mesh.setVert(12, Point3(-w2,-h1, 0.0f));
     mesh.setVert(13, Point3(-w2,h1, 0.0f));
     mesh.setVert(14, Point3(-w3,h1, 0.0f));
     mesh.setVert(15, Point3(-w3,h3, 0.0f));
     mesh.setVert(16, Point3(-w2,h3, 0.0f));
     mesh.setVert(17, Point3(-w2,h6, 0.0f));
     mesh.setVert(18, Point3(-w4,h6, 0.0f));
     mesh.setVert(19, Point3(-w1,-h6, 0.0f));
     mesh.setVert(20, Point3(0.0f,-h6, 0.0f));
     mesh.setVert(21, Point3(0.0f,-h1, 0.0f));
     mesh.setVert(22, Point3(-w1,-h1, 0.0f));
     mesh.setVert(23, Point3(-w1,h1, 0.0f));
     mesh.setVert(24, Point3(0.0f,h1, 0.0f));
     mesh.setVert(25, Point3(0.0f,h3, 0.0f));
     mesh.setVert(26, Point3(-w1,h3, 0.0f));
     mesh.setVert(27, Point3(w1,-h6, 0.0f));
     mesh.setVert(28, Point3(w2,-h6, 0.0f));
     mesh.setVert(29, Point3(w2,h6, 0.0f));
     mesh.setVert(30, Point3(w1,h6, 0.0f));
     mesh.setVert(31, Point3(w3,-h6, 0.0f));
     mesh.setVert(32, Point3(w4,-h6, 0.0f));
     mesh.setVert(33, Point3(w4,h6, 0.0f));
     mesh.setVert(34, Point3(w3,h6, 0.0f));

     mesh.faces[12].setEdgeVisFlags(1,1,0);
     mesh.faces[12].setSmGroup(0);
     mesh.faces[12].setVerts(9,10,11);

     mesh.faces[13].setEdgeVisFlags(1,1,0);
     mesh.faces[13].setSmGroup(0);
     mesh.faces[13].setVerts(11,12,13);

     mesh.faces[14].setEdgeVisFlags(1,0,0);
     mesh.faces[14].setSmGroup(0);
     mesh.faces[14].setVerts(13,14,11);

     mesh.faces[15].setEdgeVisFlags(1,0,0);
     mesh.faces[15].setSmGroup(0);
     mesh.faces[15].setVerts(14,15,18);

     mesh.faces[16].setEdgeVisFlags(1,1,0);
     mesh.faces[16].setSmGroup(0);
     mesh.faces[16].setVerts(15,16,17);

     mesh.faces[17].setEdgeVisFlags(1,0,0);
     mesh.faces[17].setSmGroup(0);
     mesh.faces[17].setVerts(17,18,15);

     mesh.faces[18].setEdgeVisFlags(1,0,0);
     mesh.faces[18].setSmGroup(0);
     mesh.faces[18].setVerts(18,9,14);

     mesh.faces[27].setEdgeVisFlags(0,0,0);
     mesh.faces[27].setSmGroup(0);
     mesh.faces[27].setVerts(9,11,14);

     mesh.faces[19].setEdgeVisFlags(1,1,0);
     mesh.faces[19].setSmGroup(0);
     mesh.faces[19].setVerts(19,20,21);

     mesh.faces[20].setEdgeVisFlags(1,1,0);
     mesh.faces[20].setSmGroup(0);
     mesh.faces[20].setVerts(21,22,19);

     mesh.faces[21].setEdgeVisFlags(1,1,0);
     mesh.faces[21].setSmGroup(0);
     mesh.faces[21].setVerts(27,28,29);

     mesh.faces[22].setEdgeVisFlags(1,1,0);
     mesh.faces[22].setSmGroup(0);
     mesh.faces[22].setVerts(29,30,27);

     mesh.faces[23].setEdgeVisFlags(1,1,0);
     mesh.faces[23].setSmGroup(0);
     mesh.faces[23].setVerts(31,32,33);

     mesh.faces[24].setEdgeVisFlags(1,1,0);
     mesh.faces[24].setSmGroup(0);
     mesh.faces[24].setVerts(33,34,31);

     mesh.faces[25].setEdgeVisFlags(1,1,0);
     mesh.faces[25].setSmGroup(0);
     mesh.faces[25].setVerts(23,24,25);

     mesh.faces[26].setEdgeVisFlags(1,1,0);
     mesh.faces[26].setSmGroup(0);
     mesh.faces[26].setVerts(25,26,23);
   }
   if (sym<3)
   {int s0=startv;
    int f0=startf;
     float r=height;
     float r4=0.4f*r,r2=0.2f*r,r6=0.6f*r,r3=0.3f*r;

     mesh.setVert(startv++, Point3(r6,r2, 0.0f));
     mesh.setVert(startv++, Point3(r6,r4, 0.0f));
     mesh.setVert(startv++, Point3(r4,r6, 0.0f));
     mesh.setVert(startv++, Point3(-r4,r6, 0.0f));
     mesh.setVert(startv++, Point3(-r6,r4, 0.0f));
     mesh.setVert(startv++, Point3(-r6,-r4, 0.0f));
     mesh.setVert(startv++, Point3(-r4,-r6, 0.0f));
     mesh.setVert(startv++, Point3(r4,-r6, 0.0f));
     mesh.setVert(startv++, Point3(r6,-r4, 0.0f));
     mesh.setVert(startv++, Point3(r6,-r2, 0.0f));
     mesh.setVert(startv++, Point3(r4,-r2, 0.0f));
     mesh.setVert(startv++, Point3(r4,-r3, 0.0f));
     mesh.setVert(startv++, Point3(r3,-r4, 0.0f));
     mesh.setVert(startv++, Point3(-r3,-r4, 0.0f));
     mesh.setVert(startv++, Point3(-r4,-r3, 0.0f));
     mesh.setVert(startv++, Point3(-r4,r3, 0.0f));
     mesh.setVert(startv++, Point3(-r3,r4, 0.0f));
     mesh.setVert(startv++, Point3(r3,r4, 0.0f));
     mesh.setVert(startv++, Point3(r4,r3, 0.0f));
     mesh.setVert(startv++, Point3(r4,r2, 0.0f));

     mesh.faces[startf].setEdgeVisFlags(1,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+19,s0,s0+1);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+1,s0+18,s0+19);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+18,s0+1,s0+2);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+2,s0+17,s0+18);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+17,s0+2,s0+3);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+3,s0+16,s0+17);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+16,s0+3,s0+4);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+4,s0+15,s0+16);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+15,s0+4,s0+5);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+5,s0+14,s0+15);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+14,s0+5,s0+6);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+6,s0+13,s0+14);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+13,s0+6,s0+7);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+7,s0+12,s0+13);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+12,s0+7,s0+8);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+8,s0+11,s0+12);

     mesh.faces[startf].setEdgeVisFlags(0,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+11,s0+8,s0+9);

     mesh.faces[startf].setEdgeVisFlags(1,1,0);
     mesh.faces[startf].setSmGroup(0);
     mesh.faces[startf++].setVerts(s0+9,s0+10,s0+11);

   }
   mesh.InvalidateGeomCache();
   }

int ICPCloudParticle::CountLive()
{  int c=0;
   for (int i=0; i<parts.Count(); i++)
     {if (parts.Alive(i)) c++;}
   return c;
}

#define DUMSHINE .20f  //.25f
#define DUMSPEC .20f

void ICPCloudParticle::RetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t)
{ int nc;
  Mtl *submtl=node->GetMtl();
  if (subtree)
  { if ((nc=node->NumberOfChildren())>0)
   for (int i=0;i<nc;i++)
     RetrieveMtls(node->GetChildNode(i),subtree,numsubs,numtabs,tabmax,t);
  }
  if (IsGEOM(node->EvalWorldState(t).obj)!=NULL)
  { if (*numtabs>=*tabmax)
   { mttab.Resize((*tabmax)+=256);}
   if (submtl!=NULL)
   { mttab[(*numtabs)++]=submtl;
     int subs;
     if ((subs=submtl->NumSubMtls())>0)
       (*numsubs)+=subs;
     else (*numsubs)++;
   }
   else 
   {DWORD tc=node->GetWireColor();
   StdMat *m = NewDefaultStdMat();
   m->SetName(TSTR(_T("C"))+node->GetName());
   Color bcolor(tc);
   m->SetDiffuse(bcolor,0);
   mttab[(*numtabs)++]=(Mtl*)m;
   (*numsubs)++;
   }
  }
}

void ICPCloudParticle::DoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t)
{ int nc;
  if ((nc=node->NumberOfChildren())>0)
   for (int i=0;i<nc;i++)
   { INode *nxtnode=node->GetChildNode(i);
     if (nxtnode->IsGroupHead()) DoGroupMtls(nxtnode,subtree,numsubs,numtabs,tabmax,t);
     else if ((subtree)||(nxtnode->IsGroupMember())) RetrieveMtls(nxtnode,subtree,numsubs,numtabs,tabmax,t);
  }
}

void ICPCloudParticle::AssignMtl(INode *node,INode *topnode,int subtree,TimeValue t) 
{  
	Mtl *submtl;
	MultiMtl *newmat=NULL;
	Mtl *nmtl=NULL;
	TSTR newname;
	MtlBaseLib glib;
	int tabmax=256;
	newname=TSTR(_T("CMat"))+node->GetName();
	if (_tcslen(newname)>16) newname[16]='\0';
	int numsubs=0,numtabs=0;
	nmtls.SetCount(nlist.Count());
	mttab.SetCount(tabmax);
	submtl=custnode->GetMtl();
	INode *tmpnode=custnode;
	backpatch=FALSE;
	int nCount=nlist.Count();
	for (int mut=0;mut<=nCount;mut++)
	{ 
		if (tmpnode->IsGroupHead())
			DoGroupMtls(tmpnode,subtree,&numsubs,&numtabs,&tabmax,t);
		else
			RetrieveMtls(tmpnode,subtree,&numsubs,&numtabs,&tabmax,t);
		if (mut!=nCount)
		{
			nmtls[mut]=numsubs;
			tmpnode=nlist[mut];
		}
	}
	mttab.Shrink();
	if (!((numtabs==1)&&(submtl!=NULL)))
	{ 
		TSTR oldname=TSTR(_T(" "));
		if (nmtl=node->GetMtl())
			oldname=nmtl->GetName();
		if (_tcscmp(oldname,newname)!=0) 
		{ 
			newmat=NewDefaultMultiMtl();
			newmat->SetName(newname); 
		}
		else newmat=(MultiMtl*)nmtl;
		//   nmtl->FindMtl(nmtl);
		newmat->SetNumSubMtls(numsubs);
		int k=0,nt=0;
		for (int i=0;i<numtabs;i++)
		{   
			if ((nt=mttab[i]->NumSubMtls())>0)
				for (int j=0;j<nt;j++) 
				{ 
					newmat->SetSubMtl(k,mttab[i]->GetSubMtl(j));
					k++;
				}
			else 
			{ 
				newmat->SetSubMtl(k,mttab[i]);
				k++;
			}
		}
	}
	mttab.SetCount(0); mttab.Shrink();
	node->SetMtl(newmat!=NULL?newmat:submtl);  
}
void ICPCloudParticle::CntRetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t)
{ int nc;
  Mtl *submtl=node->GetMtl();
  if (subtree)
  { if ((nc=node->NumberOfChildren())>0)
   for (int i=0;i<nc;i++)
     CntRetrieveMtls(node->GetChildNode(i),subtree,numsubs,numtabs,tabmax,t);
  }
  if (IsGEOM(node->EvalWorldState(t).obj)!=NULL)
  { if (submtl!=NULL)
   { int subs;
     if ((subs=submtl->NumSubMtls())>0)
       (*numsubs)+=subs;
     else (*numsubs)++;
   }
   else 
   (*numsubs)++;
  }
}

void ICPCloudParticle::CntDoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t)
{ int nc;
  if ((nc=node->NumberOfChildren())>0)
   for (int i=0;i<nc;i++)
   { INode *nxtnode=node->GetChildNode(i);
     if (nxtnode->IsGroupHead()) CntDoGroupMtls(nxtnode,subtree,numsubs,numtabs,tabmax,t);
     else if ((subtree)||(nxtnode->IsGroupMember())) CntRetrieveMtls(nxtnode,subtree,numsubs,numtabs,tabmax,t);
  }
}

void ICPCloudParticle::GetSubs(INode *node,INode *topnode,int subtree,TimeValue t) 
{  Mtl *submtl;
   int tabmax=256;
   int numsubs=0,numtabs=0;
   int mstart=0,mcnt=nlist.Count();INode *onode;
   onode=custnode;
   nmtls.SetCount(mcnt);
    submtl=onode->GetMtl();
   INode *tmpnode=onode;
   for (int mut=mstart;mut<=mcnt;mut++)
   { if (tmpnode->IsGroupHead())
       CntDoGroupMtls(tmpnode,subtree,&numsubs,&numtabs,&tabmax,t);
     else
     CntRetrieveMtls(tmpnode,subtree,&numsubs,&numtabs,&tabmax,t);
     if (mut!=mcnt)
     {nmtls[mut]=numsubs;
      tmpnode=nlist[mut];
     }
   }
}

void ICPCloudParticle::GetNextBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode)
{int nc;
   if (subtree)
  { if ((nc=node->NumberOfChildren())>0)
   for (int i=0;i<nc;i++)
     GetNextBB(node->GetChildNode(i),subtree,count,tabmax,boxcenter,t,tcount,onode);
  }
  Object *cobj=node->EvalWorldState(t).obj;
  TriObject *triOb=TriIsUseable(cobj,t);
  if (triOb)
  {    if (*count>=*tabmax)
   { thePCloudDraw.bboxpt[tcount].bpts.Resize((*tabmax)+=256);}
    Matrix3 ctm = node->GetObjectTM(t); //change
   if (node!=custnode)
  { thePCloudDraw.bboxpt[tcount].bpts[*count].Suboffset=Point3(0.0f,0.0f,0.0f)*ctm-boxcenter;
  }   
  else thePCloudDraw.bboxpt[tcount].bpts[*count].Suboffset=Point3(0.0f,0.0f,0.0f);
   ctm.NoTrans();
   Box3 bbox=triOb->GetMesh().getBoundingBox();
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[0]=Point3(bbox.pmax[0],bbox.pmax[1],bbox.pmax[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[1]=Point3(bbox.pmax[0],bbox.pmax[1],bbox.pmin[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[2]=Point3(bbox.pmax[0],bbox.pmin[1],bbox.pmin[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[3]=Point3(bbox.pmax[0],bbox.pmin[1],bbox.pmax[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[4]=Point3(bbox.pmin[0],bbox.pmax[1],bbox.pmax[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[5]=Point3(bbox.pmin[0],bbox.pmax[1],bbox.pmin[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[6]=Point3(bbox.pmin[0],bbox.pmin[1],bbox.pmin[2])*ctm;
  thePCloudDraw.bboxpt[tcount].bpts[*count].pts[7]=Point3(bbox.pmin[0],bbox.pmin[1],bbox.pmax[2])*ctm;
  (*count)++;
     if (triOb!=cobj) triOb->DeleteThis();
  }
}

void ICPCloudParticle::DoGroupBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode)
{ int nc;
  if ((nc=node->NumberOfChildren())>0)
   for (int j=0;j<nc;j++)
   { INode *nxtnode=node->GetChildNode(j);
     if (nxtnode->IsGroupHead()) DoGroupBB(nxtnode,subtree,count,tabmax,boxcenter,t,tcount,onode);
     else if((subtree)||(nxtnode->IsGroupMember())) GetNextBB(nxtnode,subtree,count,tabmax,boxcenter,t,tcount,onode);
   }
}

void ICPCloudParticle::GetallBB(INode *custnode,int subtree,TimeValue t)
{ int tabmax=256;
  int count=1,ocount=times.tl.Count();
  if (ocount>0) count=ocount;
  if (thePCloudDraw.bboxpt) delete[] thePCloudDraw.bboxpt;
  thePCloudDraw.bboxpt=NULL;
  INode *tmpnode;
  if (custnode!=NULL)
  { thePCloudDraw.bboxpt=new boxlst[count];
    int cgen;
    for (int tcount=0;tcount<count;tcount++)
    { TimeValue tofs=(ocount>0?times.tl[tcount].tl:t);
     cgen=(times.tl.Count()>0?times.tl[tcount].gennum-1:-1);
     if ((cgen>-1)&&(cgen<nlist.Count()))
     { if (!(tmpnode=nlist[cgen])) tmpnode=custnode;
     } else tmpnode=custnode;
     thePCloudDraw.bboxpt[tcount].bpts.SetCount(tabmax);
      thePCloudDraw.bboxpt[tcount].numboxes=0;
      Matrix3 ctm = tmpnode->GetObjectTM(tofs); //change
      boxcenter=Zero*ctm;
     if (tmpnode->IsGroupHead())
       DoGroupBB(tmpnode,subtree,&(thePCloudDraw.bboxpt[tcount].numboxes),&tabmax,boxcenter,tofs,tcount,tmpnode);
     else
      GetNextBB(tmpnode,subtree,&(thePCloudDraw.bboxpt[tcount].numboxes),&tabmax,boxcenter,tofs,tcount,tmpnode);
     thePCloudDraw.bboxpt[tcount].bpts.SetCount(thePCloudDraw.bboxpt[tcount].numboxes);
    thePCloudDraw.bboxpt[tcount].bpts.Shrink();
    }
  }
}

void ICPCloudParticle::CheckTree(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl)
{ Object *pobj;     
  TriObject *triOb;
  Point3 deftex=Point3(0.5f,0.5f,0.0f);
  TVFace Zerod;
  Zerod.t[0]=0;Zerod.t[1]=0;Zerod.t[2]=0;
  int nc,subv=0,subf=0,subtvnum=0,tface,tvert;

  if (subtree)
  { if ((nc=node->NumberOfChildren())>0)
   for (int i=0;i<nc;i++)
     CheckTree(node->GetChildNode(i),tspace,cmesh,cmbb,numV,numF,tvnum,ismapped,t,subtree,custmtl);
  }
  if ((triOb=TriIsUseable(pobj = node->EvalWorldState(t).obj,t))!=NULL)
  {   Point3 Suboffset;
    Matrix3 ctm = node->GetObjectTM(t); //change
/* if (node!=custnode)
     Suboffset=Point3(0.0f,0.0f,0.0f)*ctm-boxcenter;
   else 
   Suboffset=Point3(0.0f,0.0f,0.0f);
   ctm.NoTrans();*/
   int submtls=0;
   Mtl* m=node->GetMtl();
   if (!((m!=NULL)&&(submtls=m->NumSubMtls()))) 
     submtls=-1;
   int tottv;
   subv=triOb->GetMesh().getNumVerts();
    subf=triOb->GetMesh().getNumFaces();
    subtvnum=triOb->GetMesh().getNumTVerts();
   (*cmbb)+=(triOb->GetMesh().getBoundingBox()*ctm);
    cmesh->setNumFaces(tface=(*numF+subf),(*numF>0?TRUE:FALSE));
    cmesh->setNumVerts(tvert=(*numV+subv),(*numV>0?TRUE:FALSE));
    cmesh->setNumTVerts(tottv=(*tvnum+subtvnum),(*tvnum>0?TRUE:FALSE));
    if ((subtvnum>0)||(*ismapped))
    { if ((!(*ismapped))&&(*numF>0))
     { cmesh->setNumTVFaces(tface);
       *tvnum=1;
      cmesh->setNumTVerts(tottv+1);
      cmesh->tVerts[0]=deftex;
      for (int k=0;k<*numF;k++)
        memcpy(&(cmesh->tvFace[k]),&Zerod,sizeof(TVFace));
      }
     else 
      cmesh->setNumTVFaces(tface,(*numF>0?TRUE:FALSE),*numF);
     *ismapped=1;
   }
   if (subf>0) 
   { memcpy(&(cmesh->faces[*numF]),triOb->GetMesh().faces,sizeof(Face)*subf);
     if (subtvnum>0)
       memcpy(&(cmesh->tvFace[*numF]),triOb->GetMesh().tvFace,sizeof(TVFace)*subf);
   }
   BOOL mirror=DotProd(ctm.GetRow(0)^ctm.GetRow(1),ctm.GetRow(2))<0.0f;
   for (int j=(*numF);j<tface;j++)
   { cmesh->faces[j].v[0]+=*numV;
     cmesh->faces[j].v[1]+=*numV;
     cmesh->faces[j].v[2]+=*numV;
     if (mirror) MirrorFace(&cmesh->faces[j]);
     if (custmtl)
     { if (submtls<0) cmesh->faces[j].setMatID(CustMtls);
     else cmesh->faces[j].setMatID(cmesh->faces[j].getMatID()+CustMtls);
     }
     if (subtvnum>0)
     { cmesh->tvFace[j].t[0]+=*tvnum;
       cmesh->tvFace[j].t[1]+=*tvnum;
       cmesh->tvFace[j].t[2]+=*tvnum;
      if (mirror) MirrorTVs(&cmesh->tvFace[j]);
     }
     else if (*ismapped)
       memcpy(&(cmesh->tvFace[j]),&Zerod,sizeof(TVFace));
   }
   if (subtvnum>0) 
      memcpy(&(cmesh->tVerts[*tvnum]),triOb->GetMesh().tVerts,sizeof(UVVert)*subtvnum);
// if (subv) memcpy(&(clst->v[*numV]),triOb->mesh.verts,sizeof(Point3)*subv);
   int k=0;
   ctm=ctm*tspace;
   for (int j=*numV;j<tvert;j++)
// { cmesh->verts[j]=triOb->GetMesh().verts[k]*ctm+Suboffset;
   { cmesh->verts[j]=triOb->GetMesh().verts[k]*ctm;
     k++;
   }

// Multiple Map Support (Bayboro)
   CopyMultipleMapping(cmesh, triOb, numF, tface, deftex, Zerod, subf, mirror);
// end of Multiple Map Support

   *numV+=subv;
   *numF+=subf;
   *tvnum+=subtvnum;
   if (submtls>0)
   CustMtls+=submtls;
   else CustMtls++;
     if (triOb!=pobj) triOb->DeleteThis();
  }
}

void ICPCloudParticle::TreeDoGroup(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl)
{ int nc;
  if ((nc=node->NumberOfChildren())>0)
   for (int j=0;j<nc;j++)
   { INode *nxtnode=node->GetChildNode(j);
     if (nxtnode->IsGroupHead()) TreeDoGroup(nxtnode,tspace,cmesh,cmbb,numV,numF,tvnum,ismapped,t,subtree,custmtl);
     else if ((subtree)||(nxtnode->IsGroupMember())) CheckTree(nxtnode,tspace,cmesh,cmbb,numV,numF,tvnum,ismapped,t,subtree,custmtl);
   }
}

void ICPCloudParticle::GetMesh(TimeValue t,int subtree,int custmtl)
{  int tnums,numV,numF,tvnum,ismapped,msubtree=subtree;
    INode *tmpnode;
   tnums=times.tl.Count();
   if (tnums==0) tnums=1;
   if (cmesh) delete[] cmesh;cmesh=NULL;
   if (cmbb) delete[] cmbb;cmbb=NULL;
   if (custnode!=NULL)
   { cmesh=new Mesh[tnums];
     cmbb=new Box3[tnums];
//   for (int cm=0;cm<tnums;cm++) cmbb[cm].SetEmpty();
     int cgen;
    for (int i=0;i<tnums;i++)
   { TimeValue tofs=(tnums>1?times.tl[i].tl:t);
     cgen=(times.tl.Count()>0?times.tl[i].gennum-1:-1);
     if ((cgen>-1)&&(cgen<nlist.Count()))
     { if (!(tmpnode=nlist[cgen])) tmpnode=custnode;
     } else tmpnode=custnode;
     Matrix3 ptm = tmpnode->GetObjectTM(tofs); //change
     ptm=Inverse(ptm);
     numV=numF=tvnum=CustMtls=ismapped=0;
     if (tmpnode->IsGroupHead())
      TreeDoGroup(tmpnode,ptm,&cmesh[i],&cmbb[i],&numV,&numF,&tvnum,&ismapped,tofs,subtree,custmtl);
     else
      CheckTree(tmpnode,ptm,&cmesh[i],&cmbb[i],&numV,&numF,&tvnum,&ismapped,tofs,subtree,custmtl);
   }
   }
}

#define VEL_SCALE (0.01f*1200.0f/float(TIME_TICKSPERSEC))
#define VAR_SCALE (0.01f*1200.0f/float(TIME_TICKSPERSEC))

BOOL ICPCloudParticle::ComputeParticleStart( TimeValue t0, int c )
{
	int seed,anioff,tani,sym;
	TimeValue anifr;

	if (c > gCountUpperLimit)
	{
		c = gCountUpperLimit;
	}

	pblock->GetValue(FSParticlePCloud::PB_RNDSEED,t0,seed,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET,t0,anifr,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,t0,anioff,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,sym,FOREVER);

	srand(seed);               
	parts.SetCount(c,PARTICLE_VELS|PARTICLE_AGES|PARTICLE_RADIUS|PARTICLE_TENSION);
	int pcount=parts.Count();
	if (sdata){delete[] sdata;sdata=NULL;} if (pcount) sdata=new PCSavePt[pcount];
	if ((pcount<c)||(c>0 && (!sdata)))
	{   
		parts.FreeAll();if (sdata) delete sdata;sdata=NULL;  maincount=0;
		BOOL playing=GetCOREInterface()->IsAnimPlaying();
		if (playing) GetCOREInterface()->EndAnimPlayback();
		TSTR name;name=(cnode ? cnode->GetName() : TSTR(GetString(IDS_AP_PCLOUD)));
		TSTR buf; buf=TSTR(GetString(IDS_OFM_PART));
		GetCOREInterface()->Log()->LogEntry(SYSLOG_ERROR,DISPLAY_DIALOG, GetString(IDS_OFM_ERROR),_T("%s: \n\n%s\n"),buf,name);
		return (0);
	}
	int oneframe=GetTicksPerFrame();
	for (int i=0; i<parts.Count(); i++) 
	{
		parts.ages[i] = -1;
		sdata[i].themtl=0;
		sdata[i].L=RND0x(99);sdata[i].DL=-1;sdata[i].pvar=RND11();
		tani=RND0x(anifr/oneframe);
		sdata[i].showframe=(anioff==2?tani*oneframe:0);
		sdata[i].gennum=0;
		sdata[i].V.x=RND11();
		sdata[i].V.y=RND11();
		sdata[i].V.z=RND11();
		sdata[i].Ts0=RND11();
		sdata[i].Ts=0.0f;
		sdata[i].LamTs=RND11();
		sdata[i].A=RND11();
		sdata[i].LamA=RND11();
		// Martell 4/14/01: Fix for order of ops bug.
		float ztmp=RND11(); float ytmp=RND11(); float xtmp=RND11();
		parts.vels[i]=Point3(xtmp,ytmp,ztmp);
		sdata[i].To=RND11();
		sdata[i].Vsz=RND11();
		// Martell 4/14/01: Fix for order of ops bug.
		ztmp=RND11(); ytmp=RND11(); xtmp=RND11();
		sdata[i].W=Point3(xtmp,ytmp,ztmp);
		// Martell 4/14/01: Fix for order of ops bug.
		ztmp=RND01(); ytmp=RND01(); xtmp=RND01();
		sdata[i].RV=Point3(xtmp,ytmp,ztmp);
		//    sdata[i].RV=Point3(RND11(),RND11(),RND11());
		parts.radius[i]=0.0f;
		parts.tension[i]=RND11();
		sdata[i].Mltvar=RND11();
		sdata[i].SpVar=RND0x(99);
		// Martell 4/14/01: Fix for order of ops bug.
		//parts.points[i]=(sym<3?Point3((float)RNDSign(),(float)RNDSign(),(float)RNDSign()):Point3(RND01(),RND01(),RND01()));
		if ( sym<3 )
		{
			ztmp=(float)RNDSign(); ytmp=(float)RNDSign(); xtmp=(float)RNDSign();
			parts.points[i]=Point3( xtmp,ytmp,ztmp );
		}
		else
		{
			ztmp=RND01(); ytmp=RND01(); xtmp=RND01();
			parts.points[i]=Point3( xtmp,ytmp,ztmp );
		}
	}

	tvalid = t0-1;
	valid  = TRUE;
	// rseed=rand();
	return (1);
}

 #define EPSILON  0.0001f

BOOL DbgInsideMesh(Ray& ray, Mesh *amesh,Point3 *fnorms)
{  Face *face; 
   Point3 n,p, bry;
   float d, rn, a,at;
   BOOL notfirst = FALSE,inside=FALSE,savein=FALSE;
   int raycount=0;

   while ((!savein)&&(raycount<3))
   { notfirst=FALSE;
     if (raycount==0) ray.dir=Point3(1.0f,0.0f,0.0f);
    else if (raycount==1) ray.dir=Point3(0.0f,1.0f,0.0f);
    else ray.dir=Point3(0.0f,0.0f,1.0f);
   face=amesh->faces;
   for (int i=0; i<amesh->getNumFaces(); i++,face++)
   {  n = fnorms[i];
      // See if the ray intersects the plane (backfaced)
      rn = DotProd(ray.dir,n);
      if (fabs(rn) < EPSILON) continue; //is parallel, so bail
      // Use a point on the plane to find d
      d = DotProd(amesh->verts[face->v[0]],n);
      // Find the point on the ray that intersects the plane
      a = (d - DotProd(ray.p,n)) / rn;
      // The point on the ray and in the plane.
      p = ray.p + a*ray.dir;
      inside=(((a > 0.0f)&&(rn>0.0f))||((a < 0.0f)&&(rn<0.0f)));
      // Compute barycentric coords.
      bry = amesh->BaryCoords(i,p);
      // barycentric coordinates must sum to 1 and each component must
      // be in the range 0-1
      if (bry.x<0.0f || bry.x>1.0f || bry.y<0.0f || bry.y>1.0f || bry.z<0.0f || bry.z>1.0f) continue;
      if (fabs(bry.x + bry.y + bry.z - 1.0f) > EPSILON) continue;
      // Must be closer than the closest at so far
      if (notfirst)
         if (fabs(a) > at) continue;

      // Hit!
      notfirst = TRUE;     
      at    = (float)fabs(a);
      savein=inside;
     } raycount++;
   }
   return savein;

   }

#define TESTLIMIT 100

void ICPCloudParticle::BirthParticle(INode *node,TimeValue bt,int num,VelDir *ptvel,Mesh* amesh,Point3* fnorms,Matrix3 disttm)
{
	float PI = 3.1415926535f;

// Matrix3 atm = (amesh?distnode->GetObjTMBeforeWSM(bt):node->GetObjTMBeforeWSM(bt));
// Matrix3 tm = (amesh?distnode->GetObjTMAfterWSM(bt):node->GetObjTMAfterWSM(bt));
// tm.SetRow(3,atm.GetRow(3));
   Matrix3 tm =  (amesh?distnode->GetObjTMAfterWSM(bt):node->GetObjTMAfterWSM(bt)); //change

   Point3 vel;
   float Ie,Em,Vm;
   pblock->GetValue(FSParticlePCloud::PB_EMITVINFL,bt,Ie,FOREVER);
   pblock->GetValue(FSParticlePCloud::PB_EMITVMULT,bt,Em,FOREVER);
   pblock->GetValue(FSParticlePCloud::PB_EMITVMULTVAR,bt,Vm,FOREVER);
   sdata[num].Ts0 = (1.0f + sdata[num].Ts0*ptvel->VSpin)/TWOPI;
   sdata[num].Ts = (float)ptvel->Spin*sdata[num].Ts0;
   parts.tension[num] = ptvel->bstr*(1.0f + parts.tension[num]*ptvel->bstrvar);
// ok, so I'm using L for M and .z for L.  They were unused float and ints
// srand(rseed);

   float inheremit,tmp = RND11();
   inheremit = (sdata[num].L<Ie?Em*(1 + tmp*Vm):0);  
   sdata[num].L = ptvel->Life + (int)(parts.vels[num].z*ptvel->Vl);
   sdata[num].Vsz *= ptvel->VSz;
   sdata[num].LamTs = ptvel->Phase*(1.0f + sdata[num].LamTs*ptvel->VPhase);
   sdata[num].A = ptvel->ToAmp*(1.0f + sdata[num].A*ptvel->VToAmp);
   sdata[num].LamA = ptvel->ToPhase*(1.0f + sdata[num].LamA*ptvel->VToPhase);
   sdata[num].To = ptvel->ToPeriod*(1 + sdata[num].To*ptvel->VToPeriod);
   sdata[num].persist = (TimeValue)(ptvel->persist*(1.0f + sdata[num].pvar*ptvel->pvar));

	if ( ptvel->axisentered == 2 )
	{  
		sdata[num].W = Normalize( ptvel->Axis );
		if ( ptvel->axisvar > 0.0f ) 
		{
			VectorVar( &sdata[num].W, ptvel->axisvar, PI );
		}
	}
	else
	{
		sdata[num].W = Normalize( sdata[num].W );
	}

   parts.ages[num] = 0;
   float theta = RND01(),
        gamma = (ptvel->sym==2?RND11():RND01()),r=RND01();

   if (ptvel->sym==0) //box emitter emitter
   {  parts.points[num].x *= ptvel->width*r;
      parts.points[num].y *= ptvel->len*theta;
      parts.points[num].z *= ptvel->depth*gamma;
   }
   else if (ptvel->sym<3)
   {  float thetapi = theta*TWOPI;
      float lec = ptvel->len*r;
      if (ptvel->sym==2) //cylinder emitter
      {  parts.points[num].x = lec*(float)cos(thetapi);
         parts.points[num].y = lec*(float)sin(thetapi);
         parts.points[num].z *= gamma*ptvel->depth;
      } 
      else if (ptvel->sym==1) //spherical emitter
      {  float hgamma = gamma*HalfPI;
         lec *= 2.0f;
         parts.points[num].x = lec*(float)cos(thetapi)*(float)cos(hgamma);
         parts.points[num].y = lec*(float)sin(thetapi)*(float)cos(hgamma);
         parts.points[num].z *= lec*(float)sin(hgamma);
      }
   }
   else //custom emitter
   {  Ray ray;
      Box3 bbox = amesh->getBoundingBox();
      Point3 box = bbox.pmax - bbox.pmin;

//    if (box.x<EPSILON) {box.x=0.2f;bbox.pmin.x-=0.1f;}
//    if (box.y<EPSILON) {box.y=0.2f;bbox.pmin.y-=0.1f;}
//    if (box.z<EPSILON) {box.z=0.2f;bbox.pmin.z-=0.1f;}

      if ((box.x<EPSILON)||(box.y<EPSILON)||(box.z<EPSILON)) //emitter volume isn't
      {  
         // Martell 4/14/01: Fix for order of ops bug.
         float ztmp=RND01(); float ytmp=RND01(); float xtmp=RND01();
         ray.p = bbox.pmin + Point3(xtmp,ytmp,ztmp)*box;
      }
      else //assume we have a logically arranged mesh
      {  
         int testcount=0;
         ray.p = bbox.pmin + parts.points[num]*box;
         while ((!DbgInsideMesh(ray,amesh,fnorms))&&(testcount<TESTLIMIT))
         {  testcount++;
            // Martell 4/14/01: Fix for order of ops bug.
            float ztmp=RND01(); float ytmp=RND01(); float xtmp=RND01();
            ray.p = bbox.pmin + Point3(xtmp,ytmp,ztmp)*box;
         }
      }
      parts.points[num] = ray.p;
   }

   if ((ptvel->direntered==2)&&(mbase))
   {  Interval ivalid;
      Matrix3 ntm = mbase->GetObjectTM(bt,&ivalid);
      vel = ntm.GetRow(2);
      if (ptvel->DirVar>0.0f)
         VectorVar(&vel,ptvel->DirVar,PI);
   }
   else if (ptvel->direntered==1)
   {  vel = Normalize(ptvel->Dir);
      if (ptvel->DirVar>0.0f) 
         VectorVar(&vel,ptvel->DirVar,PI);
   }
   else
      vel = parts.vels[num];

   vel = VectorTransform(tm,vel);

   vel.x *= ptvel->Speed*(1.0f + sdata[num].V.x*ptvel->VSpeed);
   vel.y *= ptvel->Speed*(1.0f + sdata[num].V.y*ptvel->VSpeed);
   vel.z *= ptvel->Speed*(1.0f + sdata[num].V.z*ptvel->VSpeed);

// parts[num] = (parts[num] * tm) + vel*(0.5f*sdata[num].RV.x);
   parts[num] = parts[num] * tm;

   parts[num] += ptvel->bps.Position(1.0f - sdata[num].RV.x) - tm.GetTrans();
   parts[num] += vel*(sdata[num].RV.x);

   if ((float)fabs(inheremit)>0.0f)
   {
      Point3 inhVel = ptvel->bps.Speed(1.0f - sdata[num].RV.x);
      parts[num] += inhVel*inheremit*sdata[num].RV.x;
      vel += inhVel*inheremit; // motion inheritance
   }

   vel /= (float)GetTicksPerFrame();

   parts.vels[num] = vel;  
   sdata[num].V = parts[num];
// rseed=rand();
}

void ICPCloudParticle::DoSpawn(int j,int spmult,SpawnVars spvars,TimeValue lvar,BOOL emits)
{ if (!emits) spmult--;
  int oldcount,newcount=((oldcount=parts.Count())+spmult); 
//  srand(rseed);
  if (spmult)
  { parts.SetCount(newcount,PARTICLE_VELS|PARTICLE_AGES|PARTICLE_RADIUS|PARTICLE_TENSION);
   PCSavePt *tmp=sdata;
   sdata=new PCSavePt[newcount];
   if (tmp)
   { for (int j=0;j<oldcount;j++) sdata[j]=tmp[j];
     delete[] tmp;
   }
  }
  int Lcnt=llist.Count();
  int baselife=(Lcnt==0?deftime:(sdata[j].gennum<Lcnt?llist[sdata[j].gennum]:llist[Lcnt-1]))*GetTicksPerFrame();
  if (!emits)
  { sdata[j].gennum++;
   parts.ages[j]=0;}
  Point3 holdv=(emits?-parts.vels[j]:parts.vels[j]);
  for (int i=oldcount;i<newcount;i++)
  { parts.points[i]=parts.points[j];
    parts.vels[i]=holdv;
   parts.radius[i]=parts.radius[j];
   parts.tension[i]=parts.tension[j];
   parts.ages[i]=0;
   memcpy(&sdata[i],&sdata[j],sizeof(PCSavePt));
   if (emits) sdata[i].gennum++;
    parts.vels[i]=DoSpawnVars(spvars,parts.vels[j],holdv,&parts.radius[i],&sdata[i].W);
   sdata[i].Vsz=parts.radius[i];
   sdata[i].L=baselife+(int)RND11()*lvar;
   sdata[i].Mltvar=RND11();
   sdata[i].SpVar=RND0x(99);
   sdata[i].DL=-1;
 }
  if (!emits)
  {parts.vels[j]=DoSpawnVars(spvars,holdv,holdv,&parts.radius[j],&sdata[j].W);
  sdata[j].Vsz=parts.radius[j];
  sdata[j].L=baselife+(int)RND11()*lvar;
   sdata[j].Mltvar=RND11();
   sdata[j].SpVar=RND0x(99);
   sdata[j].DL=-1;
}
/*  int tmprseed=rand();
  rseed=(tmprseed==rseed?rand():tmprseed);*/
//  rseed=rand();
}
void ICPCloudParticle::MovePart(int j,TimeValue dt,BOOL fullframe,int tpf)
{  float curdt=(float)dt;
   parts[j] = sdata[j].V+parts.vels[j] * (float)dt;
   // add transverse oscillation
   if (fullframe) sdata[j].V=parts[j];
   Point3 DotPt=(FGT0(parts.vels[j]*(float)tpf)?parts.vels[j]:sdata[j].W);
   DotPt=sdata[j].RV^DotPt;            // Cross Product 
   DotPt=Normalize(DotPt);        
   float oscil=(sdata[j].To>0.0f?parts.ages[j]*TWOPI/sdata[j].To:0.0f);
   oscil=sdata[j].A*(float)sin(oscil+sdata[j].LamA);
   parts[j] +=DotPt*oscil;
}

void ICPCloudParticle::InvalidateUI()
   {
   if (pmapParam) pmapParam->Invalidate();
   if (pmapPGen) pmapPGen->Invalidate();
   if (pmapPType) pmapPType->Invalidate();
   if (pPMapAnimationKeying) pPMapAnimationKeying->Invalidate();
   if (pmapPSpin) pmapPSpin->Invalidate();
   if (pmapEmitV) pmapEmitV->Invalidate();
   if (pmapBubl) pmapBubl->Invalidate();
   if (pmapSpawn) pmapSpawn->Invalidate();
   }

BOOL ICPCloudParticle::EmitterVisible()
   {
   int hide;
   pblock->GetValue(FSParticlePCloud::PB_EMITRHID,0,hide,FOREVER);
   return !hide;
   }

ParamDimension *ICPCloudParticle::GetParameterDim(int pbIndex) 
{
	IICBridgeableParticleSystemUIParam *pParam = m_inParams.GetParamByParamBlockId( pbIndex );
	if ( pParam )
	{
		return pParam->GetDimensionPtr();
	}
	return defaultDim;
}

TSTR ICPCloudParticle::GetParameterName(int pbIndex)
{
	IICBridgeableParticleSystemUIParam *pParam = m_inParams.GetParamByParamBlockId( pbIndex );
	if ( pParam )
	{
		int nameIds = pParam->GetNameIds();
		if ( nameIds >= 0 )
		{
			return GetString( nameIds );
		}
	}
	return _T("");
}


void ICPCloudParticle::GetWorldBoundBox( TimeValue t, INode *inode, ViewExp* vpt, Box3& box )
{
	// particles may require update (bayboro|march.25.2002)
	if (!OKtoDisplay(t)) return;
	if (t!=tvalid) cancelled=FALSE;
	BOOL doupdate=((!cancelled)&&((t!=tvalid)||!valid));
	if (doupdate) Update(t,inode);
	// end of particles may require update (bayboro|march.25.2002)

	cnode=inode;
	int type,ptype,disptype,createin;
	pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,type,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,disptype,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_PARTICLETYPE,0,ptype,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,createin,FOREVER);
	if ((createin==3) && (!distnode)) type=0;
	// if ((type>2)&&((ptype==RENDTYPE5)||(ptype==RENDTYPE6)||((disptype>0)&&(!custnode))  )) type=1;
	if ((type>2)&&(((disptype>0)&&(!custnode))  )) type=1;
	if ((type==3)&&((disptype!=INSTGEOM)||(!custnode))) type=1;
	if (type==2) 
	{ 
		Box3 pbox;
		Matrix3 mat;
		mat = inode->GetObjTMBeforeWSM(t);
		UpdateMesh(t);
		box  = mesh.getBoundingBox();
		box=box*mat;
		for (int i=0; i<parts.points.Count(); i++) 
		{
			if (!parts.Alive(i)) { continue; }
			Point3 pt = parts.points[i];
			float r=parts.radius[i];
			int axisentered,K;
			pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,axisentered,FOREVER);
			if (axisentered==DIRTRAVEL)
			{
				pblock->GetValue(FSParticlePCloud::PB_STRETCH,t,K,FOREVER);
				float len=GetLen(parts.vels[i],K);
				r*=len;
			}
			if ((disptype==INSTGEOM)&&(custnode))
			{ 
				int tlst=times.tl.Count();
				if (tlst==0) tlst=1;
				pbox += pt;
				for (int level=0;level<tlst;level++)
				{
					Box3 cbb=cmbb[level];
					Point3 dist=cbb.pmax-cbb.pmin;
					float pdist=Largest(dist)*r*SQR2;
					pbox += pt + Point3( pdist, 0.0f, 0.0f);
					pbox += pt + Point3(-pdist, 0.0f, 0.0f);
					pbox += pt + Point3( 0.0f, pdist, 0.0f);
					pbox += pt + Point3( 0.0f,-pdist, 0.0f);
					pbox += pt + Point3( 0.0f, 0.0f, pdist);
					pbox += pt + Point3( 0.0f, 0.0f,-pdist);
				}
			}
			else
			{ 
				if (ptype==RENDTET) r*=8.0f;
				pbox += pt;
				pbox += pt + Point3( r, 0.0f, 0.0f);
				pbox += pt + Point3(-r, 0.0f, 0.0f);
				pbox += pt + Point3( 0.0f, r, 0.0f);
				pbox += pt + Point3( 0.0f,-r, 0.0f);
				pbox += pt + Point3( 0.0f, 0.0f, r);
				pbox += pt + Point3( 0.0f, 0.0f,-r);
			}
		}
		if (!pbox.IsEmpty()) box += pbox;
	}
	else if (type==3) 
	{ 
		Box3 pbox;
		Matrix3 mat;
		/*    if (createin)
		{ mat=distnode->GetObjTMAfterWSM(t);
		box.pmin=box.pmax=Zero;
		} else */
		{ 
			UpdateMesh(t);
			mat = inode->GetObjTMBeforeWSM(t);
			box  = mesh.getBoundingBox();
		}
		box  = box * mat;
		for (int i=0; i<parts.points.Count(); i++)
		{
			if (!parts.Alive(i)) { continue; }
			Point3 pt;
			float radius=parts.radius[i];
			int axisentered,K;
			pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,axisentered,FOREVER);
			if ((ptype!=RENDTYPE5)&&(ptype!=RENDTYPE6))
			{
				if (axisentered==DIRTRAVEL)
				{ 
					pblock->GetValue(FSParticlePCloud::PB_STRETCH,t,K,FOREVER);
					float strlen=GetLen(parts.vels[i],K);
					radius*=strlen;
				}
			}
			int n=0,found=0;
			TimeValue Ctime=(thePCloudDraw.anioff?GetCurTime(sdata[i].showframe,(thePCloudDraw.anioff>1?thePCloudDraw.t:parts.ages[i]),thePCloudDraw.anifr):thePCloudDraw.t);
			int nCount=nlist.Count();
			found=((n=TimeFound(times,Ctime,(sdata[i].gennum>nCount?nCount:sdata[i].gennum)))>-1);
			for (int nb=0;nb<thePCloudDraw.bboxpt[n].numboxes;nb++)
			{ 
				for (int j=0;j<8;j++)
				{ 
					pt=(radius*(thePCloudDraw.bboxpt[n].bpts[nb].pts[j]+thePCloudDraw.bboxpt[n].bpts[nb].Suboffset))+parts[i];
					pbox += pt;
				}
			}
		}
		if (!pbox.IsEmpty()) box += pbox;
	}
	else 
	{
		SimpleParticle::GetWorldBoundBox(t,inode,vpt,box);
	}
}

BOOL ICPCloudParticleDraw::DrawParticle( GraphicsWindow *gw, ParticleSys &parts, int i )
{
	HCURSOR hCur;
	BOOL chcur=FALSE;
	if ( !(( disptype == INSTGEOM ) && ( bb )) ) return TRUE;
	if ( obj->custnode )
	{
		int n = 0
			, found = 0;
		
		TimeValue Ctime = anioff 
			? GetCurTime( obj->sdata[i].showframe, ( anioff > 1 ? t : parts.ages[i] ), anifr ) 
			: t;

		int nCount = obj->nlist.Count();
		found = ( n = TimeFound( obj->times, Ctime, ( obj->sdata[i].gennum > nCount ? nCount:obj->sdata[i].gennum ))) > -1;
		float radius = parts.radius[i];
		float elapsedtime = (float)parts.ages[i];
		float Angle = obj->sdata[i].LamTs;
		if ( indir.inaxis )
		{
			indir.vel = parts.vels[i];
		}

		Point3 nextpt;
		if ( found ) 
		{
			Point3 pt[9];
			if ( bboxpt[n].numboxes * 8 > LOTSOFACES )
			{
				hCur = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
				chcur = TRUE;
			}
			for (int nb = 0; nb < bboxpt[n].numboxes; nb++ )
			{
				for ( int j = 0; j < 8; j++ )
				{
					if ( indir.inaxis )
					{
						pt[j] = RotateAboutAxis( Angle
							, parts.points[i]
							, radius * ( bboxpt[n].bpts[nb].pts[j] + bboxpt[n].bpts[nb].Suboffset )
							, obj->sdata[i].W
							, indir 
							);
					}
					else
					{ 
						nextpt = ( radius * ( bboxpt[n].bpts[nb].pts[j] + bboxpt[n].bpts[nb].Suboffset )) + parts[i];
						RotateOnePoint( &nextpt.x, &parts.points[i].x, &(obj->sdata[i].W.x), Angle );
						pt[j] = nextpt;
					}
				}
				gw->polyline( 4, pt, NULL, NULL, TRUE, NULL );
				Point3 tmppt[5]; 
				for ( int k=0; k < 4; k++ )
				{
					tmppt[k] = pt[4+k];
				}

				gw->polyline( 4, tmppt, NULL, NULL, TRUE, NULL );
				tmppt[0] = pt[0];
				tmppt[1] = pt[4];
				gw->polyline( 2, tmppt, NULL, NULL, FALSE, NULL );
				tmppt[0] = pt[1];
				tmppt[1] = pt[5];
				gw->polyline( 2, tmppt, NULL, NULL, FALSE, NULL );
				tmppt[0] = pt[2];
				tmppt[1] = pt[6];
				gw->polyline( 2, tmppt, NULL, NULL, FALSE, NULL );
				tmppt[0] = pt[3];
				tmppt[1] = pt[7];
				gw->polyline( 2, tmppt, NULL, NULL, FALSE, NULL );

				if ( GetAsyncKeyState( VK_ESCAPE ))
				{
					return TRUE;
				}
			}
		}
	}
	if ( GetAsyncKeyState( VK_ESCAPE ) ) 
	{ 
		if ( chcur )
		{
			SetCursor( hCur ); 
		}
		return TRUE;
	}
	return 0;
}

int DbgGetDrawType( ICPCloudParticle *po, int &ptype, int &disptype )
{
	int type;
	po->pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,type,FOREVER);
	po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,disptype,FOREVER);
	po->pblock->GetValue(FSParticlePCloud::PB_PARTICLETYPE,0,ptype,FOREVER);
	TimeValue aniend;

	// if ((type>2)&&((ptype==RENDTYPE5)||(ptype==RENDTYPE6)||((disptype>0)&&(!po->custnode))  )) type=1;
	if ((type>2)&&(((disptype>0)&&(!po->custnode))  )) type=1;
	if ((type==3)&&((disptype!=INSTGEOM)||(!po->custnode))) type=1;
	if (type>1)
	{
		aniend=GetAnimEnd();
		int oneframe=GetTicksPerFrame();
		po->thePCloudDraw.anifr=aniend+oneframe;
		int axisentered;
		po->pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,axisentered,FOREVER);
		if (po->thePCloudDraw.indir.inaxis=(axisentered==DIRTRAVEL))
		{
			po->pblock->GetValue(FSParticlePCloud::PB_STRETCH,0,po->thePCloudDraw.indir.K,FOREVER);
			po->thePCloudDraw.indir.oneframe=oneframe;
		}
	}
	return type;
}


MarkerType ICPCloudParticle::GetMarkerType() 
{
	int ptype, disptype, type = DbgGetDrawType( this, ptype, disptype );
	switch (type)
	{
		case 0:
		{
			parts.SetCustomDraw(NULL);
			return POINT_MRKR;
		}
		case 1:
		{
			parts.SetCustomDraw(NULL);
			return PLUS_SIGN_MRKR;
		}
		case 2:
		{        
			thePCloudDraw.obj = this;
			thePCloudDraw.firstpart=TRUE;
			thePCloudDraw.disptype=disptype;
			thePCloudDraw.ptype=ptype;
			parts.SetCustomDraw(&thePCloudDraw);         
			thePCloudDraw.bb=FALSE;
			return POINT_MRKR;
		}   
		case 3:
		{
			thePCloudDraw.obj = this;
			thePCloudDraw.firstpart=TRUE;
			thePCloudDraw.disptype=disptype;
			thePCloudDraw.ptype=ptype;
			thePCloudDraw.bb=TRUE;
			parts.SetCustomDraw(&thePCloudDraw);         
			return POINT_MRKR;
		}
		default:
		{
			return PLUS_SIGN_MRKR;
		}
	}
}



//--- PCloud particle -----------------------------------------------

// TODO : Add BridgeableParticleSystem clone code
RefTargetHandle ICPCloudParticle::Clone(RemapDir& remap) 
{
	ICPCloudParticle* newob = new ICPCloudParticle();   
	newob->ReplaceReference(0,remap.CloneRef(pblock)); 
	if (custnode) newob->ReplaceReference(CUSTNODE,custnode);
	if (distnode) newob->ReplaceReference(DISTNODE,distnode);
	if (mbase) newob->ReplaceReference(MBASE,mbase);
	newob->nlist.SetCount(nlist.Count());
	newob->llist.SetCount(llist.Count());
	for (int ix=0;ix<nlist.Count();ix++) 
	{ newob->nlist[ix]=NULL;
	 newob->ReplaceReference(ix+BASER,nlist[ix]);
	}
	for (int ix=0;ix<llist.Count();ix++) newob->llist[ix]=llist[ix];
	newob->createmeth=createmeth;
	newob->custname = custname;
	newob->distnode = distnode;
	newob->distname = distname;
	newob->normname = normname;
	newob->mvalid.SetEmpty();  
	newob->tvalid = FALSE;
	newob->dflags=dflags;
	newob->dispmesh=NULL;
	newob->times.tl.SetCount(0);
	newob->nmtls.ZeroCount();
	newob->cancelled=FALSE;
	newob->wasmulti=FALSE;
	newob->stepSize=stepSize;
	int vshow;
	pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,vshow,FOREVER);
	if (vshow>1)
	{int subtree,anioff,custmtl;
	pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
	TimeValue aniend=GetAnimEnd();
	int anifr=aniend+GetTicksPerFrame();
	TimeValue t=GetCOREInterface()->GetTime();
	newob->GetTimes(newob->times,t,anifr,anioff);
	if (vshow==2)
	{ pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
	  newob->GetMesh(t,subtree,custmtl);
	}
	else newob->GetallBB(newob->custnode,subtree,t);
	}
	BaseClone(this, newob, remap);
	return newob;
}

void DbgCacheSpin(float *holddata,PCSavePt *sdata,int pcount,BOOL issave)
{ 
	for (int i=0;i<pcount;i++)
	{
		if (issave) holddata[i]=sdata[i].LamTs;
		else sdata[i].LamTs=holddata[i];
	}
}

static float Dbgfindmappos(float curpos)
{
	return curpos < 0 ? 0 : ( curpos > 1 ? 1 : curpos );
}

void ICGetMeshInfo(int type,int count,Mesh *pm,int *numF,int *numV)
{  if (type==RENDTYPE1) 
   {   pm->setNumFaces(count);
      pm->setNumVerts(count*3);
      pm->setNumTVerts(count);
      pm->setNumTVFaces(count);
      *numV=3;*numF=1;
   }
   else if (type==RENDTYPE2)
   {  pm->setNumFaces(count*12);
      pm->setNumVerts(count*8);
      pm->setNumTVerts(count);
      pm->setNumTVFaces(count*12);
      *numV=8;*numF=12;
   }
   else if (type==RENDTYPE3)
   {  pm->setNumFaces(count*6);
      pm->setNumVerts(count*12);
      pm->setNumTVerts(count);
      pm->setNumTVFaces(count*6);
      *numV=12;*numF=6;
   }
   else if (type==RENDTYPE5)
   {  pm->setNumFaces(count*2);
      pm->setNumVerts(count*4);
      pm->setNumTVerts(4);
      //pm->setNumTVerts(count);
      pm->setNumTVFaces(count*2);
      *numV=4;*numF=2;
   }
   else if (type==RENDTYPE6)
   {  pm->setNumFaces(count*10);
      pm->setNumVerts(count*11);
      pm->setNumTVerts(count);
      pm->setNumTVFaces(count*10);
      *numV=11;*numF=10;
   }
   else if (type==RENDTET)
   { pm->setNumFaces(count*4);
     pm->setNumVerts(count*4);
     pm->setNumTVerts(2);
     pm->setNumTVFaces(count*4);
     pm->tVerts[0]=Point3(0.0f,0.5f,0.0f);
     pm->tVerts[1]=Point3(1.0f,0.5f,0.0f);
      *numV=4;*numF=4;
   }
   else if (type==REND6PT)
   { pm->setNumFaces(count*2);
     pm->setNumVerts(count*6);
     pm->setNumTVerts(count);
     pm->setNumTVFaces(count*2);
      *numV=6;*numF=2;
   }
   else if (type==RENDSPHERE)
   { int segs=10,rows=(segs/2-1);
     int nverts = rows * segs + 2;
     int tnf,nfaces = rows * segs * 2;
     pm->setNumVerts(count*nverts);
     pm->setNumFaces(tnf=count*nfaces);
     pm->setNumTVerts(count);
     pm->setNumTVFaces(tnf);
      *numV=nverts;*numF=nfaces;
   }
}

Mesh *ICPCloudParticle::GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete)
{  
	float Uval, Vval,Wval,Angle,elapsedtime;
	int type, count,anioff,anifr;
	float width;
	Point3 Center;
	TVFace defface;
	Tab<int> tVertsOffset;
	MultipleChannelMapMerger mcmm;
	BOOL mirror=FALSE;
	TimeValue aniend=GetAnimEnd();
	Mesh *pm = new Mesh;
	if (cancelled) {ZeroMesh(pm);mesh.InvalidateGeomCache();
	needDelete = TRUE;   return pm;}
	anifr=aniend+GetTicksPerFrame();
	dispt=t;
	int nummtls=0,curmtl=0,multi=0,pc=0,custmtl=0;
	pblock->GetValue(FSParticlePCloud::PB_PARTICLETYPE,0,type,FOREVER);
	pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,pc,FOREVER);
	if (pc==METABALLS) type=RENDMETA;
	else if (pc==INSTGEOM)
	{
		if (custnode==NULL) {type=0;pc=ISSTD;} else type=RENDGEOM;
		pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
	}

	float FperT=GetFrameRate()/(float)TIME_TICKSPERSEC;
	pblock->GetValue(FSParticlePCloud::PB_EMITRHEIGHT,t,width,FOREVER);
	int createin;
	pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,createin,FOREVER);
	BOOL weirdCenter=(createin==3)&&(distnode);

	int isrend=!TestAFlag(A_NOTREND);
	//my comment out below - ecp
	// if ((!isrend)&&((type==RENDTYPE5)||(type==RENDTYPE6))) type=RENDTYPE1;
	Matrix3 wcam,cam=ident;
	Point3 v, v0,v1, camUpV=Zero,camFrontV=Zero,camPosition=Zero;
	if (isrend)
	{ 
		cam = Inverse(wcam=view.worldToView);
	}
	else
	{
		// this is new active viewport stuff here
		ViewExp *VE = GetCOREInterface()->GetActiveViewport();
		VE->GetAffineTM(wcam);
		GetCOREInterface()->ReleaseViewport(VE);
		cam = Inverse(wcam);
	}

	camUpV = cam.GetRow(1);
	camFrontV = cam.GetRow(2);
	camPosition = cam.GetRow(3);

	// Matrix3 tm = inode->GetObjTMAfterWSM(t);
	Matrix3 tm = inode->GetObjectTM(t); //change
	Matrix3 itm = Inverse(tm);
	// if (weirdCenter) tm=distnode->GetObjTMAfterWSM(t);
	if (weirdCenter) tm=distnode->GetObjectTM(t); //change
	int vertexnum=0, face=0,numV=0,numF=0,j=0,tvnum=0,ismapped=0;
	ParticleSys lastparts;
	TimeValue offtime=t%GetTicksPerFrame();
	BOOL midframe;
	midframe=offtime>0;
	float *holddata=NULL;int pcount=0;
	if (isrend)
	{ 
		if (midframe) 
		{ 
			Update(t-offtime,inode);
			if ((pcount=parts.Count())>0)
			{   
				DL_CacheData(&parts,&lastparts);
				holddata=new float[pcount];
				DbgCacheSpin(holddata,sdata,pcount,TRUE);
			}
		}
		Update(t,inode);
	}
	count = CountLive();
	Center = Zero* tm;
	float Thetah;
	Point3 r1;
	if (count==0)
	{
		ZeroMesh(pm);
		goto quit;
	}
	else
	{
		mirror=DotProd(tm.GetRow(0)^tm.GetRow(1),tm.GetRow(2))<0.0f;
		if (type==RENDTYPE6)
		{ 
			if (view.projType) type=RENDTYPE5;
			else
			{
				Thetah=view.fov;
				r1=cam.GetRow(1);
			}
		}
		int gtvnum=0;
		if (pc<METABALLS) ICGetMeshInfo(type,count,pm,&numF,&numV);
		else if (type==RENDGEOM)
		{ 
			int subtree,onscreen;
			pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
			pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,onscreen,FOREVER);
			pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,0,anioff,FOREVER);
			//   if (onscreen!=2) { 
			thePCloudDraw.t=t;
			GetTimes(times,t,anifr,anioff);
			GetMesh(t,subtree,custmtl);
			//   }
			if (backpatch)
			{ 
				int custmtl,frag;
				pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
				pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,frag,FOREVER);
				if ((custnode)&&(frag==INSTGEOM)&& custmtl) 
				GetSubs(inode,custnode,subtree,t);
			}
			TimeValue Ctime;
			int mnum,tmptvs=0;
			numV=0;numF=0;
			BOOL alltex=TRUE;
			for (int pcnt=0;pcnt<parts.Count();pcnt++)
			{ 
				if (GetAsyncKeyState (VK_ESCAPE)) 
				{ 
					ZeroMesh(pm);cancelled=TRUE;goto quit;
				}
				if (!parts.Alive(pcnt)) continue;
				Ctime=(anioff?GetCurTime(sdata[pcnt].showframe,(anioff>1?t:parts.ages[pcnt]),anifr):t);
				int nCount=nlist.Count();
				mnum=TimeFound(times,Ctime,(sdata[pcnt].gennum>nCount?nCount:sdata[pcnt].gennum));
				numV+=cmesh[mnum].getNumVerts();
				numF+=cmesh[mnum].getNumFaces();
			}
			pm->setNumFaces(numF);
			pm->setNumVerts(numV);
			int mcnt=0;
			if (!custmtl) gtvnum=count;
			else 
			{
				mcnt=times.tl.Count();if (mcnt==0) mcnt=1;
				for (int mc=0;mc<mcnt;mc++)
				{ 
					tmptvs=cmesh[mc].getNumTVerts();
					if (tmptvs==0) alltex=FALSE;
					else gtvnum+=tmptvs;
				}
				if ((!alltex)&&(gtvnum>0))
				{ 
					defface.setTVerts(gtvnum,gtvnum,gtvnum);
					gtvnum++;
				}
				tVertsOffset.SetCount(mcnt);
				tVertsOffset[0]=0;
			}

			int gtv;
			//   pm->setNumTVerts(gtv=(custmtl?gtvnum:count*numV));
			pm->setNumTVerts(gtv=gtvnum);
			if ((custmtl)&&(gtvnum>0))
			{ 
				int tvs=0,mtvs=0;
				for (int mc=0;mc<mcnt;mc++)
				{ 
					if (mc) tVertsOffset[mc]=tVertsOffset[mc-1]+mtvs;
					mtvs=cmesh[mc].getNumTVerts();
					if (mtvs>0)
					{ 
						for (int imtv=0;imtv<mtvs;imtv++)
							pm->tVerts[tvs++]=cmesh[mc].tVerts[imtv];
					} 
				}
				if (!alltex) pm->tVerts[tvs]=deftex;
			}
			pm->setNumTVFaces(gtv>0?numF:0);

			// Multiple Channel Map Support (single line)
			if (custmtl) mcmm.setNumTVertsTVFaces(pm, cmesh, mcnt);
		}

		Uval=0.5f;
		Vval=0.5f;
		Wval=0.5f;
		Point3 ipt;
		if (count>0)
		{
			if (type==RENDMETA)
			{
				float res,bstr,thres=0.6f;int notdraft;
				pblock->GetValue(FSParticlePCloud::PB_METATENSION,t,bstr,FOREVER);
				pblock->GetValue(FSParticlePCloud::PB_PCNOTDRAFT,t,notdraft,FOREVER);
				notdraft = (notdraft?0:1);
				if (isrend)
					pblock->GetValue(FSParticlePCloud::PB_METACOURSE,0,res,FOREVER);
				else pblock->GetValue(FSParticlePCloud::PB_METACOURSEV,0,res,FOREVER);
					metap.CreateMetas(parts,pm,thres,res,bstr,notdraft);
				for (int j=0;j<pm->getNumVerts();j++)
					pm->verts[j] = itm * pm->verts[j];
			}
			else
			{
				Mtl *mtl;
				mtl=inode->GetMtl();
				if (mtl)
				{
					Class_ID mc=Class_ID(MULTI_CLASS_ID,0);
					Class_ID oc=mtl->ClassID();
					if (multi=(oc==mc))
					{
						nummtls=mtl->NumSubMtls();
						if (nummtls==0) 
							multi=0;
					}
				}

				wasmulti=multi;
				InDirInfo indir;indir.vel=Zero;indir.K=0;indir.oneframe=GetTicksPerFrame();
				int axisentered;
				pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,axisentered,FOREVER);
				if (indir.inaxis=(axisentered==DIRTRAVEL))
				{
					pblock->GetValue(FSParticlePCloud::PB_STRETCH,0,indir.K,FOREVER);
				}

				MtlID cm;
				for (int i=0; i<parts.Count(); i++) 
				{ 
					if (GetAsyncKeyState (VK_ESCAPE)) 
					{ 
						ZeroMesh(pm);
						cancelled=TRUE;
						goto quit;
					}
					if (!parts.Alive(i)) 
						continue;
					if (multi) 
					{
						curmtl=sdata[i].themtl;
						if (curmtl>=nummtls) 
							curmtl=curmtl % nummtls;
					} else curmtl=i;

					if (indir.inaxis)
						indir.vel=parts.vels[i];

					float x;
					elapsedtime=(float)parts.ages[i];
					//Vval = 0.f;
					Angle=sdata[i].LamTs;
					if (type==RENDTYPE1) PlotTriangle(parts.radius[i],vertexnum,face,pm,Angle,&sdata[i].W.x,curmtl,&parts.points[i],indir);
					else if (type==RENDTYPE2) PlotCube8(parts.radius[i],vertexnum,face,pm,Angle,&sdata[i].W.x,curmtl,&parts.points[i],indir);
					else if (type==RENDTYPE3) PlotSpecial(parts.radius[i],vertexnum,face,pm,Angle,&sdata[i].W.x,curmtl,&parts.points[i],indir);
					else if (type==RENDTET) PlotTet(parts.radius[i],vertexnum,face,pm,Angle,sdata[i].W,curmtl,&parts.points[i],indir);
					else if (type==REND6PT) Plot6PT(parts.radius[i],vertexnum,face,pm,Angle,&sdata[i].W.x,curmtl,&parts.points[i],indir);
					else if (type==RENDSPHERE) PlotSphere(parts.radius[i],vertexnum,face,pm,Angle,&sdata[i].W.x,curmtl,&parts.points[i],indir);
					else if (type==RENDTYPE5)
					{
						// Standard Particles ==> Facing
						v0 = Normalize( camFrontV ^ camUpV ) * parts.radius[i];
						v1 = Normalize( camUpV ) * parts.radius[i];

						pm->verts[vertexnum] = (parts[i]+v0+v1);
						pm->verts[vertexnum+1] = (parts[i]-v0+v1);
						pm->verts[vertexnum+2] = (parts[i]-v0-v1);
						pm->verts[vertexnum+3] = (parts[i]+v0-v1); 

						// Spin
						Matrix3 rotMat;
						rotMat.SetAngleAxis( camFrontV, Angle );

						for ( int l = 0; l < 4; l++ )
						{
							Point3 currentVertex = pm->verts[vertexnum + l];
							currentVertex -= parts[i];
							currentVertex = rotMat * currentVertex;
							pm->verts[vertexnum + l] = currentVertex + parts[i];
						}

						AddFace(vertexnum+3,vertexnum+2,vertexnum+1,face,pm,curmtl);
						AddFace(vertexnum+1,vertexnum,vertexnum+3,face+1,pm,curmtl);
					}
					else if (type==RENDTYPE6)
					{    
					#pragma warning (disable : 4288)
						Point3 a,b,Rv;
						float Theta,R=Length(Rv=(parts[i]-camPosition)),angle=parts.radius[i]*R*Thetah/view.screenW;
						a=angle*Normalize(r1^Rv);
						b=angle*Normalize(a^Rv);
						pm->verts[vertexnum]=parts[i];
						for (int l=0;l<numV;l++)
						{
							Theta=(l-1)*PIOver5;
							pm->verts[vertexnum+l]=parts[i]+(float)cos(Theta)*a+(float)sin(Theta)*b;
						}
						int l;
						for (l=0;l<numF-1;l++)
						{
							AddFace(vertexnum,vertexnum+1+l,vertexnum+2+l,face+l,pm,curmtl);
						}
						AddFace(vertexnum,vertexnum+1+l,vertexnum+1,face+l,pm,curmtl); // should this be included in the for loop?
					#pragma warning (default : 4288)
					}
					else if (type==RENDGEOM)
					{ 
						TimeValue Ctime=(anioff?GetCurTime(sdata[i].showframe,(anioff>1?t:parts.ages[i]),anifr):t);
						int nCount=nlist.Count();
						int mnum=TimeFound(times,Ctime,(sdata[i].gennum>nCount?nCount:sdata[i].gennum));
						if (mnum<0) continue;
						numF=cmesh[mnum].getNumFaces();
						numV=cmesh[mnum].getNumVerts();
						if ((gtvnum>0)&&(custmtl))
						{ 
							for (int j=0;j<numF;j++)
							if (cmesh[mnum].getNumTVerts()>0)
								pm->tvFace[j+face].setTVerts(cmesh[mnum].tvFace[j].t[0]+tVertsOffset[mnum],
									cmesh[mnum].tvFace[j].t[1]+tVertsOffset[mnum],
									cmesh[mnum].tvFace[j].t[2]+tVertsOffset[mnum]);
							else pm->tvFace[j+face]=defface;
						}

						// Multiple Channel Map Support (single line)
						if (custmtl) mcmm.setTVFaces(pm, cmesh, face, mnum);

						int jf=face;
						for (int j=0;j<numF;j++)
						{
							Face cface=cmesh[mnum].faces[j];
							pm->faces[jf].flags=cface.flags;
							pm->faces[jf].setSmGroup(cface.smGroup);
							pm->faces[jf].setVerts(vertexnum+cface.v[0],vertexnum+cface.v[1],vertexnum+cface.v[2]);
							if (!custmtl)
								pm->faces[jf].setMatID((MtlID)curmtl); 
							else
							{
								cm=cface.getMatID();
								int mtlgen=times.tl[mnum].gennum-1,maxmtl=nmtls.Count();
								if (mtlgen>=maxmtl) mtlgen=maxmtl-1;
								if ((mtlgen>-1)&&((times.tl.Count()>0)&&(times.tl[mnum].gennum>0)))
								cm+=nmtls[mtlgen];
								pm->faces[jf].setMatID(cm);
							}
							pm->faces[jf].setEdgeVisFlags(cface.getEdgeVis(0),cface.getEdgeVis(1),cface.getEdgeVis(2));
							jf++;
							//      pm->faces[face].setEdgeVisFlags(f[j].flags&EDGE_A,f[j].flags&EDGE_Bf[j].flags&EDGE_A);
						}
						PlotCustom(parts.radius[i],i,vertexnum,pm,Angle,&sdata[i].W.x,&cmesh[mnum],&parts.points[i],numV,indir);
					}

					// Convert to object coords
					for (int j=0;j<numV;j++)
					{
						pm->verts[vertexnum] = itm * pm->verts[vertexnum];
						vertexnum++;
					}


					if ( ( type != RENDTET ) && ( !custmtl ) )
					{
						if ( pc == ISSTD && type == RENDTYPE5 )
						{
							pm->tVerts[0] = Point3( 1.0f, 1.0f, 0.f );
							pm->tVerts[1] = Point3( 0.0f, 1.0f, 0.f );
							pm->tVerts[2] = Point3( 0.0f, 0.0f, 0.f );
							pm->tVerts[3] = Point3( 1.0f, 0.0f, 0.f );

							pm->tvFace[face++].setTVerts( 2, 3, 0 ); 
							pm->tvFace[face++].setTVerts( 0, 1, 2 ); 
						}
						else
						{
							pm->tVerts[tvnum]=Point3(Dbgfindmappos(Uval),Dbgfindmappos(Vval),Dbgfindmappos(Wval));
							for (int j = 0; j < numF; j++ )
							{
								pm->tvFace[face++].setTVerts( tvnum, tvnum, tvnum ); 
							}
							tvnum++;
						}
					} 
					else 
					{
						face+=numF;
					}
				}
			}
		}
	}

	if ((isrend)&&(midframe))
	{ 
		if (pcount>0)
		{
			DL_CacheData(&lastparts,&parts);
			DbgCacheSpin(holddata,sdata,pcount,FALSE);
			delete[] holddata;
		}
		tvalid=t-offtime;
	}
	if (mirror) SwitchVerts(pm);
	quit:mesh.InvalidateGeomCache();
	needDelete = TRUE;
	return pm;
}

const TCHAR *ICPCloudClassDesc::ClassName ()   {return GetString(IDS_AP_PCLOUD);}
const TCHAR *ICPCloudClassDesc::Category () {return GetString(IDS_RB_PARTICLESYSTEMS);}
TCHAR *ICPCloudParticle::GetObjectName() {return GetString(IDS_AP_PCLOUDGC);}


void ICPCloudParticle::GetTimes(TimeLst &times,TimeValue t,int anifr,int ltype)
{ 
	int n,found,tmax,tnums=0,tgen;
	TimeValue tframe;
	times.tl.SetCount(0);times.tl.Shrink();
	times.tl.Resize(100);tmax=100;times.tl.SetCount(tmax);
	for (int m=0;m<parts.Count();m++)
	{ 
		if (!parts.Alive(m)) continue;
		if (ltype)
		{ 
			if (ltype==1) tframe=sdata[m].showframe+parts.ages[m];
			else tframe=sdata[m].showframe+t;
			//      if ((tframe>=anifr)&&(anifr!=0)) tframe=tframe % anifr;
		} 
		else 
			tframe=t;
		found=n=0;
		int nCount=nlist.Count();
		tgen=(sdata[m].gennum>nCount?nCount:sdata[m].gennum);
		while ((n<tnums)&&(!found))
		{
			found=((times.tl[n].gennum==tgen)&&(tframe==times.tl[n].tl));
			n++;
		}
		if (!found) 
		{
			if (tnums>=tmax) 
			{
				times.tl.Resize(tmax+=100);times.tl.SetCount(tmax);
			}
			times.tl[tnums].gennum=tgen;times.tl[tnums++].tl=tframe;
		}
	}
	times.tl.SetCount(tnums);
	times.tl.Shrink();
}

BOOL ICPCPickOperand::Filter(INode *node)
   {
   if ((node)&&(!node->IsGroupHead())) {
      ObjectState os = node->GetObjectRef()->Eval(po->ip->GetTime());
      if (!IsGEOM(os.obj)) {
         node = NULL;
         return FALSE;
         }
      }

   return node ? TRUE : FALSE;
   }

BOOL ICPCPickOperand::HitTest(
      IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
   {  
   INode *node = ip->PickNode(hWnd,m,this);
   
   if ((node)&&(!node->IsGroupHead())) {
      ObjectState os = node->GetObjectRef()->Eval(ip->GetTime());
      if (!IsGEOM(os.obj)) {
         node = NULL;
         return FALSE;
         }
      }

   return node ? TRUE : FALSE;
   }

void ICPCloudParticle::ShowName(int dist)
{ 
	TSTR name;
	if ( dist == shownormname )
	{
		FormatName( name = TSTR( GetString( IDS_AP_OBJECTSTR ) ) + ( mbase ? normname : TSTR( GetString( IDS_AP_NONE ) ) ) );
		if ( hgen )
		{
			SetWindowText(GetDlgItem(hgen, IDC_AP_PCLOUDOBJDIRNAME), name);
		}
	}
	else if (dist) 
	{
		FormatName( name = TSTR( GetString( IDS_AP_OBJECTSTR ) ) + ( distnode ? distname : TSTR( GetString( IDS_AP_NONE ) ) ) );
		if ( hparam )
		{
			SetWindowText( GetDlgItem( hparam, IDC_AP_FILLPICKOBJECT ), name );
		}
	}
	else
	{ 
		FormatName( name = TSTR( GetString( IDS_AP_OBJECTSTR ) ) + ( custnode ? custname : TSTR( GetString( IDS_AP_NONE ) ) ) );
		if ( hptype )
		{
			SetWindowText(GetDlgItem(hptype, IDC_AP_INSTANCESRCNAME), name);
		}
	}
}

BOOL ICPCPickOperand::Pick(IObjParam *ip,ViewExp *vpt)
   {
   INode *node = vpt->GetClosestHit();
   assert(node);

   INodeTab nodes;
   int subtree;
// if (dodist) {nodes.SetCount(1);nodes[0]=node;}
    if (dodist==1)
     {   theHold.Begin();
      theHold.Put(new ICCreatePCPartPickNode(po,po->distname,node->GetName(),1));
      po->ReplaceReference(DISTNODE,node);
      theHold.Accept(GetString(IDS_AP_COMPICK));
       po->distname = TSTR(node->GetName());
      po->createmeth=3;
      po->pblock->SetValue(FSParticlePCloud::PB_CREATEIN,0,po->createmeth);
      UIHelpers::SpinnerOff(po->hparam,IDC_AP_EMITHGTSPIN,IDC_AP_EMITHGT);
      UIHelpers::SpinnerOff(po->hparam,IDC_AP_EMITDEPSPIN,IDC_AP_EMITDEP);
      EnableWindow(GetDlgItem(po->hparam,IDC_AP_EMITHGT_TXT),FALSE);
      EnableWindow(GetDlgItem(po->hparam,IDC_AP_EMITDEP_TXT),FALSE);
       po->ShowName(dodist);  
     }
     else
     { po->pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
     if (node->IsGroupMember()) 
      while (node->IsGroupMember()) node=node->GetParentNode();
     nodes.SetCount(0);
     if (po->flags=(node->IsGroupHead()?1:0)) MakeGroupNodeList(node,&nodes,subtree,ip->GetTime());
     else MakeNodeList(node,&nodes,subtree,ip->GetTime());
     if (dodist==0)
     {   theHold.Begin();
      theHold.Put(new ICCreatePCPartPickNode(po,po->custname,node->GetName(),0));
      po->ReplaceReference(CUSTNODE,node);
      theHold.Accept(GetString(IDS_AP_COMPICK));
       po->custname = TSTR(node->GetName());
      po->ShowName(dodist);   
     }
     else if (dodist==2)
    {  theHold.Begin();
      theHold.Put(new ICPCObjectListRestore(po));
      po->AddToList(node,po->nlist.Count(),TRUE);
      theHold.Accept(GetString(IDS_AP_OBJADD));
     }
     else 
     { theHold.Begin();
      theHold.Put(new ICPCObjectListRestore(po));
       po->AddToList(node,repi,FALSE);
      theHold.Accept(GetString(IDS_AP_OBJADD));
     }
     // Automatically check show result and do one update
      int frag,custmtl,onscreen;
      po->pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
      po->pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,frag,FOREVER);
      po->pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,onscreen,FOREVER);
      if ((frag==INSTGEOM)&&(onscreen>1))
        if (onscreen==2)
         po->GetMesh(ip->GetTime(),subtree,custmtl);
        else po->GetallBB(node,subtree,ip->GetTime());
   } 
   ip->FlashNodes(&nodes);
   nodes.ZeroCount();nodes.Shrink();
   po->valid=FALSE;
   po->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
   if (po->creating) {
      theCreatePCloudMode.JumpStart(ip,po);
      ip->SetCommandMode(&theCreatePCloudMode);
      ip->RedrawViews(ip->GetTime());
      return FALSE;
   } else {
      return TRUE;
      }
   }

void ICPCPickOperand::EnterMode(IObjParam *ip)
   {
   ICustButton *iBut;
   if (dodist>=2)
   { iBut=GetICustButton(GetDlgItem(po->spawn,(dodist==3?IDC_AP_OBJQUEUEREPLACE:IDC_AP_OBJECTQUEUEPICK)));
     UIHelpers::SetButtonEnabled(po->spawn,(dodist==2?IDC_AP_OBJQUEUEREPLACE:IDC_AP_OBJECTQUEUEPICK),FALSE);
     UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJQUEUEDELETE,FALSE);
     UIHelpers::SetButtonEnabled(po->hptype,IDC_AP_OBJECTPICK,FALSE);
     UIHelpers::SetButtonEnabled(po->hparam,IDC_AP_FILLPICKBUTTON,FALSE);
   }
    else if (dodist)
   { iBut=GetICustButton(GetDlgItem(po->hparam,IDC_AP_FILLPICKBUTTON));
     UIHelpers::SetButtonEnabled(po->hptype,IDC_AP_OBJECTPICK,FALSE);
     UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJECTQUEUEPICK,FALSE);
     UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJQUEUEREPLACE,FALSE);
     EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPESTD),FALSE);
     EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPEMET),FALSE);
     EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPEINSTANCE),FALSE);
   }
   else
   {iBut=GetICustButton(GetDlgItem(po->hptype,IDC_AP_OBJECTPICK));
     UIHelpers::SetButtonEnabled(po->hparam,IDC_AP_FILLPICKBUTTON,FALSE);
     UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJECTQUEUEPICK,FALSE);
     UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJQUEUEREPLACE,FALSE);
   }
   if (iBut) iBut->SetCheck(TRUE);
   ReleaseICustButton(iBut);
   GetCOREInterface()->PushPrompt(GetString(IDS_AP_PICKMODE));
   }

void ICPCPickOperand::ExitMode(IObjParam *ip)
{
	if (!po->ip)
	{
		return;
	}

	ICustButton *iBut;
	if (dodist>=2)
	{ 
		iBut=GetICustButton(GetDlgItem(po->spawn,(dodist==3?IDC_AP_OBJQUEUEREPLACE:IDC_AP_OBJECTQUEUEPICK)));
		ICPCCheckInstButtons(po->pblock,po->hptype);
		ICEnableFillPickButton( po->pblock, po->hparam );
		if (dodist==3)
		{
			UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJECTQUEUEPICK,TRUE);
		}
		UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJQUEUEDELETE,FALSE);
		UIHelpers::SetButtonEnabled(po->spawn,IDC_AP_OBJQUEUEREPLACE,FALSE);
	}
	else if (dodist) 
	{ 
		iBut=GetICustButton(GetDlgItem(po->hparam,IDC_AP_FILLPICKBUTTON));
		ICPCCheckInstButtons(po->pblock,po->hptype);
		ICPCCheckSpawnButtons(po->pblock,po->spawn,repi);
		EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPECHUNKS),TRUE);

		// Particle class
		{
			EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPESTD),TRUE);
#if IC_PCLOUD_ENABLE_META_PARTICLES
			EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPEMET),TRUE);
#endif

#if IC_PCLOUD_ENABLE_INSTANCED_GEOMETRY_PARTICLES
			EnableWindow(GetDlgItem(po->hptype,IDC_SP_TYPEINSTANCE),TRUE);
#endif
		}
	}
	else
	{ 
		iBut=GetICustButton(GetDlgItem(po->hptype,IDC_AP_OBJECTPICK));
		ICEnableFillPickButton( po->pblock, po->hparam );
		ICPCCheckSpawnButtons(po->pblock,po->spawn,repi);
	}

	if (iBut)
	{
		iBut->SetCheck(FALSE);
	}
	
	ReleaseICustButton(iBut);
	GetCOREInterface()->PopPrompt();
}

BOOL ICPCPickNorm::Filter(INode *node)
   {
   return node ? TRUE : FALSE;
   }

BOOL ICPCPickNorm::HitTest(
      IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
   {  
   INode *node = ip->PickNode(hWnd,m,this);
   
   return node ? TRUE : FALSE;
   }

BOOL ICPCPickNorm::Pick(IObjParam *ip,ViewExp *vpt)
   {
   INode *node = vpt->GetClosestHit();
   assert(node);
   INodeTab nodes;
   nodes.SetCount(1);nodes[0]=node;
   ip->FlashNodes(&nodes);
   theHold.Begin();
   theHold.Put(new ICCreatePCPartPickNode(po,po->normname,node->GetName(),shownormname));
   po->ReplaceReference(MBASE,node);
   theHold.Accept(GetString(IDS_AP_COMPICK));
   po->normname = TSTR(node->GetName());
   po->ShowName(shownormname);
   po->valid=FALSE;
   
   if (po->creating) {
      theCreatePCloudMode.JumpStart(ip,po);
      ip->SetCommandMode(&theCreatePCloudMode);
      ip->RedrawViews(ip->GetTime());
      return FALSE;
   } else {
      return TRUE;
      }
   }

void ICPCPickNorm::EnterMode(IObjParam *ip)
   {
   ICustButton *iBut = GetICustButton(GetDlgItem(po->hgen,IDC_AP_OBJECTDIRPICK));
   if (iBut) iBut->SetCheck(TRUE);
   ReleaseICustButton(iBut);
   }

void ICPCPickNorm::ExitMode(IObjParam *ip)
{
	if (po->hgen)
	{  
		ICustButton *iBut = GetICustButton(GetDlgItem(po->hgen,IDC_AP_OBJECTDIRPICK));
		if (iBut) iBut->SetCheck(FALSE);
		ReleaseICustButton(iBut);
	}
}

int ICPCloudParticle::GetNListBaser()
{
	return nlist.Count() + BASER;
}

int ICPCloudParticle::NumRefs()
{
	return BASER + nlist.Count() + m_modifierReferences.size();
}

RefTargetHandle ICPCloudParticle::GetReference(int i)
{
	int nListBaser = GetNListBaser();
	if ( i == PBLK )
	{
		return (RefTargetHandle)pblock;
	}
	else if ( i == DISTNODE )
	{
		return (RefTargetHandle)distnode;
	}
	else if ( i == CUSTNODE )
	{
		return (RefTargetHandle)custnode;
	}
	else if ( i == MBASE )
	{
		return (RefTargetHandle)mbase;
	}
	else if ( i < nListBaser )
	{
		return (RefTargetHandle)nlist[i-BASER];
	}
	else 
	{
		return (RefTargetHandle)m_modifierReferences[i - nListBaser];
	}
}

void ICPCloudParticle::SetReference(int i, RefTargetHandle rtarg)
{
	int nListBaser = GetNListBaser();
	if ( i == PBLK )
	{
		pblock=(IParamBlock*)rtarg;
		for ( InParamList::iterator paramIter = m_inParams.m_uiParams.begin(); paramIter != m_inParams.m_uiParams.end(); ++paramIter )
		{
			(*paramIter)->SetPBlockPtr( pblock );
		}
	}
	else if ( i == DISTNODE )
	{
		distnode = (INode *)rtarg;
	}
	else if ( i == CUSTNODE )
	{
		custnode = (INode *)rtarg;
	}
	else if ( i == MBASE )
	{
		mbase = (INode *)rtarg;
	}
	else if ( i < nListBaser )
	{
		nlist[i-BASER] = (INode *)rtarg;
	}
	else 
	{
		m_modifierReferences[i - nListBaser] = ( Modifier * )rtarg;
	}
}

RefResult ICPCloudParticle::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message )
{           
	switch (message)
	{      
		case REFMSG_TARGET_DELETED:   
		{ 
			if (hTarget==custnode) 
			{
				if (theHold.Holding()) theHold.Put(new ICCreatePCPartDelNode(this,custnode->GetName(),0));
				custnode=NULL;custname=TSTR(_T(" "));cancelled=FALSE;
			}
			if ( hTarget == distnode )
			{
				if ( theHold.Holding() )
				{
					theHold.Put( new ICCreatePCPartDelNode( this, distnode->GetName(), 1 ) );
				}

				distnode = NULL;
				distname = TSTR(_T(" "));
				cancelled = FALSE;
				
				createmeth = 0;
				pblock->SetValue( FSParticlePCloud::PB_CREATEIN, 0, createmeth );
			}
			if (hTarget==mbase)
			{
				if (theHold.Holding()) theHold.Put(new ICCreatePCPartDelNode(this,mbase->GetName(),shownormname));
				mbase=NULL;normname=TSTR(_T(" "));cancelled=FALSE;
			}
			BOOL notfound=TRUE; 
			for (int i=0;(i<nlist.Count())&&(notfound);i++)
			{
				if (hTarget==nlist[i]) 
				{ 
					DeleteFromList(i,TRUE);
					notfound=FALSE;cancelled=FALSE;
				}
			}
			for (int i=0;(i<m_modifierReferences.size())&&(notfound);i++)
			{
				if (hTarget==m_modifierReferences[i]) 
				{ 
					// TODO : TheHold ? cancelled ?
					DeleteReference( GetNListBaser() + i );
					m_modifierReferences.erase( i );
					notfound=FALSE;
					//cancelled=FALSE;
				}
			}
			break;
		}
		case REFMSG_NODE_NAMECHANGE:
		{ 
			if (hTarget==custnode) 
			{ 
				custname = TSTR(custnode->GetName());
				ShowName(0);cancelled=FALSE;
			}
			if (hTarget==distnode) 
			{ 
				distname = TSTR(distnode->GetName());
				ShowName(1);cancelled=FALSE;
			}
			if (hTarget==mbase) 
			{ 
				normname = TSTR(mbase->GetName());
				ShowName(shownormname);cancelled=FALSE;
			}
			BOOL notfound=TRUE;
			for (int i=0;(i<nlist.Count())&&(notfound);i++)
			{
				if (hTarget==nlist[i]) 
				{
					notfound=FALSE;SetUpList();cancelled=FALSE;
				}
			}
			break;
		}
		case REFMSG_NODE_LINK:     
		case REFMSG_CHANGE:
		{ 
			int pblst=0;
			if (pblock && (pblock==hTarget))
			{ 
				pblst=pblock->LastNotifyParamNum();
				if (pblst==FSParticlePCloud::PB_METACOURSE)
					return REF_STOP;
				if (pblst==FSParticlePCloud::PB_SPAWNLIFEVLUE)
					return REF_STOP;
			}
			MeshInvalid();
			ParticleInvalid();
			if (editOb==this) InvalidateUI();
			cancelled=FALSE;
			break;
		}
		case REFMSG_SUBANIM_STRUCTURE_CHANGED:
		{
			EnableWindow(GetDlgItem(hptype,IDC_SP_MAPCUSTOMEMIT),TRUE);
			if (editOb==this) InvalidateUI();
			//       NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
			break;
		}
		default: 
		{
			SimpleParticle::NotifyRefChanged(changeInt,hTarget,partID,message);
			break;
		}
	}

	return REF_SUCCEED;
}

void ICPCloudParticle::RegisterModifierReference( Modifier *pModifier )
{
	if ( !pModifier )
	{
		return;
	}

	ModifierReferenceList::iterator it = m_modifierReferences.find( pModifier );
	if ( it == m_modifierReferences.end() )
	{
		m_modifierReferences.push_back( pModifier );
		ReplaceReference( GetNListBaser() + m_modifierReferences.size() - 1, pModifier );
	}
}

// replace the pblock value if it is invalid with the first valid replacement found
void ReplacePBlockValueConditional( IParamBlock *pblock, int pbid, const fm::map<int,bool> &validatedParamValues )
{
	if ( !pblock 
		|| pbid < 0 )
	{
		return;
	}

	int __pBlockValue = 0;
	pblock->GetValue( pbid, 0, __pBlockValue, FOREVER );
	fm::map<int,bool>::const_iterator validatedParamValueIter = validatedParamValues.find( __pBlockValue );
	
	if ( validatedParamValueIter != validatedParamValues.end() &&
		 !validatedParamValueIter->second )
	{
		for ( validatedParamValueIter = validatedParamValues.begin(); validatedParamValueIter != validatedParamValues.end(); ++validatedParamValueIter )
		{
			if ( validatedParamValueIter->second )
			{
				pblock->SetValue( pbid, 0, validatedParamValueIter->first );
			}
		}
	}
}

// --------------------------------------------------------
// Forcing parameters to certain values if necessary
// --------------------------------------------------------
void ICPCloudParticle::ForceValidParameterBlockValues()
{
	// Particle Class
	{
		fm::map<int,bool> validatedParamValues;

		validatedParamValues.insert( ISSTD, true );
		validatedParamValues.insert( METABALLS, IC_PCLOUD_ENABLE_META_PARTICLES );
		validatedParamValues.insert( INSTGEOM, IC_PCLOUD_ENABLE_INSTANCED_GEOMETRY_PARTICLES );

		ReplacePBlockValueConditional( pblock, FSParticlePCloud::PB_PARTICLECLASS, validatedParamValues );
	}

	// Standard Particle Types
	{
		fm::map<int,bool> validatedParamValues;

		validatedParamValues.insert( RENDTYPE1, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_TRIANGLE );
		validatedParamValues.insert( RENDTYPE2, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_CUBE );
		validatedParamValues.insert( RENDTYPE3, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SPECIAL );
		validatedParamValues.insert( RENDTYPE5, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_FACING );
		validatedParamValues.insert( RENDTYPE6, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_CONSTANT );
		validatedParamValues.insert( RENDTET, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_TETRA );
		validatedParamValues.insert( REND6PT, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SIXPOINT );
		validatedParamValues.insert( RENDSPHERE, IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SPHERE );

		ReplacePBlockValueConditional( pblock, FSParticlePCloud::PB_PARTICLETYPE, validatedParamValues );
	}

	// Speed Directions
	{
		fm::map<int,bool> validatedParamValues;

		validatedParamValues.insert( 0, IC_PCLOUD_ENABLE_SPEEDDIR_RANDOM );
		validatedParamValues.insert( 1, IC_PCLOUD_ENABLE_SPEEDDIR_VECTOR );
		validatedParamValues.insert( 2, IC_PCLOUD_ENABLE_SPEEDDIR_REFERENCE_OBJECT );

		ReplacePBlockValueConditional( pblock, FSParticlePCloud::PB_SPEEDDIR, validatedParamValues );
	}

	// Particle Display Type
	{
		fm::map<int,bool> validatedParamValues;

		validatedParamValues.insert( 0, IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_DOTS );
		validatedParamValues.insert( 1, IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_TICKS );
		validatedParamValues.insert( 2, IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_MESH );
		validatedParamValues.insert( 3, IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_BOUNDING_BOX );

		ReplacePBlockValueConditional( pblock, FSParticlePCloud::PB_SPEEDDIR, validatedParamValues );
	}

	// Emitter Shape
	{
		fm::map<int,bool> validatedParamValues;

		validatedParamValues.insert( 0, IC_PCLOUD_ENABLE_EMITTER_SHAPE_BOX );
		validatedParamValues.insert( 1, IC_PCLOUD_ENABLE_EMITTER_SHAPE_SPHERE );
		validatedParamValues.insert( 2, IC_PCLOUD_ENABLE_EMITTER_SHAPE_CYLINDER );
		validatedParamValues.insert( 3, IC_PCLOUD_ENABLE_EMITTER_SHAPE_OBJECT );

		ReplacePBlockValueConditional( pblock, FSParticlePCloud::PB_SPEEDDIR, validatedParamValues );
	}
}

void ICPCloudPostLoadCallback::proc(ILoad *iload)
{
	//int mc;
	//ReferenceTarget *targ = cb->targ;
	DWORD oldVer = ((ICPCloudParticle*)(cb->targ))->pblock->GetVersion();
	cb->proc(iload);
	
	if ( oldVer < 1 )
	{   
		
	}

	((ICPCloudParticle*)(cb->targ))->ForceValidParameterBlockValues();
	delete this;
}

#define PC_CUSTNAME_CHUNK  0x0100
#define PC_DISTNAME_CHUNK  0x0200
#define PC_CUSTFLAGS_CHUNK 0x0300
#define PC_SPAWNC_CHUNK    0x0400
#define PC_LCNT_CHUNK      0x0500
#define PC_LIFE_CHUNK      0x0600
#define PC_NORMNAME_CHUNK  0x0700

IOResult ICPCloudParticle::Save(ISave *isave)
{  
	ULONG nb;

	isave->BeginChunk(PC_CUSTNAME_CHUNK);     
	isave->WriteWString(custname);
	isave->EndChunk();

	isave->BeginChunk(PC_DISTNAME_CHUNK);     
	isave->WriteWString(distname);
	isave->EndChunk();

	isave->BeginChunk(PC_CUSTFLAGS_CHUNK);    
	isave->Write(&flags,sizeof(flags),&nb);
	isave->EndChunk();

	int nCount=nlist.Count();
	isave->BeginChunk(PC_SPAWNC_CHUNK);    
	isave->Write(&nCount,sizeof(nCount),&nb);
	isave->EndChunk();

	int Lcnt=llist.Count();
	isave->BeginChunk(PC_LCNT_CHUNK);      
	isave->Write(&Lcnt,sizeof(Lcnt),&nb);
	isave->EndChunk();

	isave->BeginChunk(PC_LIFE_CHUNK);
	for (int i=0;i<Lcnt;i++)
	{ 
		isave->Write(&llist[i],sizeof(int),&nb);
	}
	isave->EndChunk();

	isave->BeginChunk(PC_NORMNAME_CHUNK);     
	isave->WriteWString(normname);
	isave->EndChunk();

	return IO_OK;
}

IOResult ICPCloudParticle::Load(ILoad *iload)
{
	ULONG nb;
	IOResult res = IO_OK;
	iload->RegisterPostLoadCallback( new ICPCloudPostLoadCallback( new ParamBlockPLCB(
		ICPCloudBackwardCompatiblity::k_previousVersionDescriptions,
		ICPCloudBackwardCompatiblity::k_previousVersionCount,
		&ICPCloudBackwardCompatiblity::k_currentVersionDescription, this, 0 
		)));

	int cnmtl=0;
	// Default names
	custname = TSTR(_T(" "));
	while (IO_OK==(res=iload->OpenChunk()))
	{
		switch (iload->CurChunkID())
		{
			case PC_CUSTNAME_CHUNK: 
			{
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				custname = TSTR(buf);
				break;
			}
			case PC_DISTNAME_CHUNK: 
			{
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				distname = TSTR(buf);
				break;
			}
			case PC_CUSTFLAGS_CHUNK:
			{
				res=iload->Read(&flags,sizeof(flags),&nb);
				break;
			}
			case PC_SPAWNC_CHUNK:
			{  
				int nCount;
				res=iload->Read(&nCount,sizeof(nCount),&nb);
				nlist.SetCount(nCount);
				for (int i=0; i<nCount; i++) nlist[i] = NULL;
				break;
			}
			case PC_LCNT_CHUNK:
			{  
				int Lcnt;
				res=iload->Read(&Lcnt,sizeof(Lcnt),&nb);
				llist.SetCount(Lcnt);
				for (int i=0; i<Lcnt; i++) llist[i] = NULL;
				break;
			}
			case PC_LIFE_CHUNK:
			{  
				for (int i=0;i<llist.Count();i++)
				res=iload->Read(&llist[i],sizeof(int),&nb);
				break;
			}
			case PC_NORMNAME_CHUNK: 
			{
				TCHAR *buf;
				res=iload->ReadWStringChunk(&buf);
				normname = TSTR(buf);
				break;
			}
		}
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
	}
	return IO_OK;
}

void ICPCloudParticle::SetUpList()
{ 
	SendMessage(GetDlgItem(spawn,IDC_AP_OBJECTQUEUE),LB_RESETCONTENT,0,0);
	for (int i=0;i<nlist.Count(); i++) 
	{
		SendMessage(GetDlgItem(spawn,IDC_AP_OBJECTQUEUE), LB_ADDSTRING,0,(LPARAM)(TCHAR*)(nlist[i]->GetName()));
	}
}

void ICPCloudParticle::AddToList(INode *newnode,int i,BOOL add)
{
	if (add)
	{ 
		INode *null_ptr = NULL;
		nlist.Insert(i,1,&null_ptr);
	}    
	ReplaceReference(i+BASER,newnode);
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	SetUpList();
}

void ICPCloudParticle::DeleteFromList( int nnum, BOOL all)
{
	int nCount=nlist.Count();
	INode *cnode=nlist[nnum];
	DeleteReference(nnum+BASER);

	if (theHold.Holding() && !TestAFlag(A_HELD)) 
	{
		theHold.Put(new ICPCObjectListRestore(this));
	}
	
	nlist.Delete(nnum,1);
	if (all) 
	{
		for (int di=nnum;di<nlist.Count();di++)
		{
			if (nlist[di]==cnode)
			{
				DeleteReference(di+BASER);
				nlist.Delete(di,1);
			}
		}
	}
	
	NotifyDependents(FOREVER,0,REFMSG_CHANGE);
	if (ip)
	{
		SetUpList();
	}
	
	valid=FALSE;
}

void ICPCloudParticle::SetUpLifeList()
{ TCHAR buffer[20];
  SendMessage(GetDlgItem(spawn,IDC_AP_LIFEQUEUE),LB_RESETCONTENT,0,0);
  for (int i=0;i<llist.Count(); i++) 
  {   _itoa(llist[i], buffer, 10 );
   SendMessage(GetDlgItem(spawn,IDC_AP_LIFEQUEUE),LB_ADDSTRING,0,(LPARAM)(TCHAR*)buffer);
  }
} 

void ICPCloudParticle::AddToLifeList(int newlife)
{  llist.Insert(llist.Count(),1,&newlife);
   SetUpLifeList();
}

void ICPCloudParticle::DeleteFromLifeList(int nnum)
{  llist.Delete(nnum,1);
    if (ip) SetUpLifeList();
}
int ICPCloudParticle::HitTest( TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt ) 
{  
	BOOL doupdate=((!cancelled)&&((t!=tvalid)||!valid));
	if (doupdate) Update(t,inode);
	Point2 pt( (float)p[0].x, (float)p[0].y );
	HitRegion hitRegion;
	GraphicsWindow *gw = vpt->getGW();  
	MakeHitRegion(hitRegion, type, crossing, 4, p);
	DWORD rlim  = gw->getRndLimits();
	int res;

	gw->setTransform(ident);
	int ptype,disptype,dtype=DbgGetDrawType(this,ptype,disptype);
	if ((dtype<2)||(dtype==3))
	{  
		if (parts.HitTest(gw,&hitRegion,flags&HIT_ABORTONHIT,GetMarkerType()))
		{
			return TRUE;
		}
	}
	else
	{ 
		if ((t!=dispt)||doupdate||!dispmesh)
		{
			NullView nullView;
			BOOL needdel;
			if (dispmesh) delete dispmesh;
			SetAFlag(A_NOTREND);
			dispmesh=GetRenderMesh(t,inode,nullView,needdel);
			ClearAFlag(A_NOTREND);
		}
		gw->setRndLimits(rlim);
		gw->setTransform(inode->GetObjTMBeforeWSM(t));
		res = dispmesh->select(gw, &particleMtl, &hitRegion, flags & HIT_ABORTONHIT);
		if (res) return TRUE;
	}

	if (EmitterVisible()) 
	{
		gw->setRndLimits((rlim|GW_PICK|GW_WIREFRAME) 
		 & ~(GW_ILLUM|GW_BACKCULL|GW_FLAT|GW_SPECULAR));
		gw->setTransform(inode->GetObjTMBeforeWSM(t));
		res = mesh.select(gw, &particleMtl, &hitRegion, flags & HIT_ABORTONHIT);
		gw->setRndLimits(rlim);
	} 
	else 
	{
		res = 0;
	}
	
	return res;
}

int ICPCloudParticle::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) 
{
	if (!OKtoDisplay(t)) return 0;
	if (t!=tvalid) cancelled=FALSE;
	if ((ip && origmtl) &&(origmtl!=inode->GetMtl()))
	{ 
		EnableWindow(GetDlgItem(hptype,IDC_SP_MAPCUSTOMEMIT),TRUE);
		origmtl=NULL;
	}

	GraphicsWindow *gw = vpt->getGW();
	DWORD rlim  = gw->getRndLimits();
	BOOL doupdate=((!cancelled)&&((t!=tvalid)||!valid));
	if (!doupdate) doupdate=CheckMtlChange(inode->GetMtl(),wasmulti);
	if (doupdate) Update(t,inode);

	// Draw emitter
	gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|/*GW_BACKCULL|*/ (rlim&GW_Z_BUFFER) );  // removed BC on 4/29/99
	if (inode->Selected()) 
		gw->setColor( LINE_COLOR, GetSelColor());
	else if(!inode->IsFrozen())
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_PARTICLE_EM));

	if (EmitterVisible())
	{
		gw->setTransform(inode->GetObjTMBeforeWSM(t));  
		mesh.render(gw, &particleMtl, (flags&USE_DAMAGE_RECT) ? &vpt->GetDammageRect() : NULL, COMP_ALL);
	}
	  
	Material *mtl = gw->getMaterial();   
	if (!inode->Selected() && !inode->IsFrozen())
	gw->setColor( LINE_COLOR, mtl->Kd[0], mtl->Kd[1], mtl->Kd[2]);
	int ptype,disptype,type=DbgGetDrawType(this,ptype,disptype);

	// bounding box
	if (type==3)
	{ 
		thePCloudDraw.obj = this;
		thePCloudDraw.firstpart=TRUE;
		thePCloudDraw.disptype=disptype;
		thePCloudDraw.ptype=ptype;
		thePCloudDraw.bb=TRUE;
		parts.SetCustomDraw(&thePCloudDraw);         
	}

	// ticks, dots
	if ( type < 2 )
	{ 
		parts.SetCustomDraw(NULL);
		MarkerType mt=(type==0?POINT_MRKR:PLUS_SIGN_MRKR);
		gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|GW_BACKCULL| (rlim&(GW_Z_BUFFER|GW_BOX_MODE)) );
		gw->setTransform(ident);
		parts.Render(gw,mt);
		gw->setRndLimits(rlim);
	}
	// geometry
	else
	{
		parts.SetCustomDraw(NULL);       
		//parts.SetCustomDraw(&thePCloudDraw);
		NullView nullView;
		BOOL needdel;
		gw->setRndLimits(rlim);
		if ( ( t != dispt ) || doupdate || !dispmesh )
		{
			if (dispmesh) delete dispmesh;
			SetAFlag( A_NOTREND );
			dispmesh = GetRenderMesh( t, inode, nullView, needdel );
			ClearAFlag( A_NOTREND );
		}

		//   Matrix3 mat = inode->GetObjTMAfterWSM(t);
		Matrix3 mat = inode->GetObjectTM( t ); //change
		gw->setTransform( mat );
		dispmesh->render( gw, inode->Mtls(),
			( flags & USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL
			, COMP_ALL
			, inode->NumMtls()
			);
	}
	return(0);
}

ICPCloudParticle::ICPCloudParticle()
{
	sdata=NULL;
	cnode=NULL;
	custnode=NULL;
	distnode=NULL;
	mbase=NULL;
	cmesh=NULL;
	cmbb=NULL;
	dispmesh=NULL;
	origmtl=NULL;

	createmeth=0;
	custname=TSTR(_T(" "));
	normname=TSTR(_T(" "));
	nlist.ZeroCount();
	llist.ZeroCount();
	distname=TSTR(_T(" "));
	ResetSystem(0,FALSE);
	size=42*isize+fsize*39;
	times.tl.SetCount(0);
	dispt=-99999;
	thePCloudDraw.bboxpt=NULL;
	nmtls.ZeroCount();
	parts.points.ZeroCount();
	cancelled=FALSE;
	wasmulti=FALSE;
	maincount=0;
	fragflags=-1;
	dflags=APRTS_ROLLUP_FLAGS;
	backpatch=TRUE;
	ClearAFlag(A_NOTREND);
	stepSize=GetTicksPerFrame();

	pblock = NULL;
	ReplaceReference( PBLK, CreateParameterBlock( 
		ICPCloudBackwardCompatiblity::k_currentVersion, 
		ICPCloudBackwardCompatiblity::k_currentVersionParamCount, 
		ICPCloudBackwardCompatiblity::k_currentVersionNumber 
		));
	
	InitParams();
}

ICPCloudParticle::~ICPCloudParticle()
{
	if (sdata) {delete[] sdata;sdata=NULL;}
	DeleteAllRefsFromMe();
	pblock=NULL;
	parts.FreeAll();
	times.tl.Resize(0);
	nmtls.Resize(0);
	llist.ZeroCount();llist.Shrink();
	nlist.ZeroCount();nlist.Shrink();
	if (cmesh) delete[] cmesh;if (dispmesh) delete dispmesh;
	if (cmbb) delete[] cmbb;
	if (thePCloudDraw.bboxpt) delete[] thePCloudDraw.bboxpt;
	ReleaseParams();
}
void ICPCloudParticle::InitUIParams()
{
	int FToTick=(int)((float)TIME_TICKSPERSEC/(float)GetFrameRate());

#define ADD_INPARAM( pbid, uiType, uiDefaultVal, sysType, sysPtr, convFuncPtr, nameIds, pDimension ) m_inParams.m_uiParams.push_back( new ICBridgeableParticleSystemUIParam<uiType,sysType>( pblock, pbid, uiDefaultVal, (sysType *)(sysPtr), convFuncPtr, nameIds, pDimension ) )

	ADD_INPARAM( FSParticlePCloud::PB_CREATEIN, int, 0, int, m_pParticleSystem->getParameter( EMITTER_TYPE_N ), NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPEED, float, 0.f, float, m_pParticleSystem->getParameter( SPEED_N ), &FSConvert::PerFrameToPerSec, IDS_RB_SPEED, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPEEDVAR, float, 0.f, float, m_pParticleSystem->getParameter( SPEED_VARIATION_N ), NULL, IDS_RB_SPEEDVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPEEDDIR, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_DIRX, float, 1.f, float, &(( irr::core::vector3df * )m_pParticleSystem->getParameter( DIRECTION_N ))->X, NULL, IDS_AP_SPDIRX, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_DIRY, float, 0.f, float, &(( irr::core::vector3df * )m_pParticleSystem->getParameter( DIRECTION_N ))->Y, NULL, IDS_AP_SPDIRY, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_DIRZ, float, 0.f, float, &(( irr::core::vector3df * )m_pParticleSystem->getParameter( DIRECTION_N ))->Z, NULL, IDS_AP_SPDIRZ, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_DIRVAR, float, 0.f, float, m_pParticleSystem->getParameter( DIR_VARIATION_N ), NULL, IDS_AP_SPDIRVAR, defaultDim );

	ADD_INPARAM( FSParticlePCloud::PB_PBIRTHRATE, float, 10, float, m_pParticleSystem->getParameter( BIRTH_RATE_N ), &FSConvert::PerFrameToPerSec, IDS_RB_PBIRTHRATE, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_PTOTALNUMBER, int, 100, int, m_pParticleSystem->getParameter( MAX_PARTICLES_N ), NULL, IDS_RB_PTOTALNUMBER, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_LIFE, int, 101*FToTick, float, m_pParticleSystem->getParameter( LIFE_N ), &FSConvert::MaxTickToSecs, IDS_RB_LIFE, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_LIFEVAR, int, 0, float, m_pParticleSystem->getParameter( LIFE_VARIATION_N ), &FSConvert::MaxTickToSecs, IDS_RB_LIFEVAR, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_SIZE, float, 1.f, float, m_pParticleSystem->getParameter( TARGET_SIZE_N ), NULL, IDS_RB_SIZE, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_SIZEVAR, float, 0.f, float, m_pParticleSystem->getParameter( SIZE_VARIATION_N ), NULL, IDS_RB_SIZEVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_GROWTIME, int, 0, float, m_pParticleSystem->getParameter( SIZE_GROWTH_TIME_N ), &FSConvert::MaxTickToSecs, IDS_RB_GROWTIME, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_FADETIME, int, 0, float, m_pParticleSystem->getParameter( SIZE_FADE_TIME_N ), &FSConvert::MaxTickToSecs, IDS_RB_FADETIME, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_RNDSEED, int, 12345, int, NULL, NULL, IDS_RB_RNDSEED, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_EMITRWID, float, 0.f, float, m_pParticleSystem->getParameter( HEIGHT_N ), NULL, IDS_AP_RADLEN, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_EMITRHEIGHT, float, 0.f, float, m_pParticleSystem->getParameter( WIDTH_N ), NULL, IDS_RB_EMITRWID, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_EMITRDEPTH, float, 0.f, float, m_pParticleSystem->getParameter( RADIUS_LENGTH_N ), NULL, IDS_AP_EMITRHEIGHT, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_EMITRHID, int, 0, int, NULL, NULL, -1, defaultDim );

	ADD_INPARAM( FSParticlePCloud::PB_PARTICLECLASS, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_PARTICLETYPE, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_METATENSION, float, 1.0f, float, NULL, NULL, IDS_RB_METATENSION, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_METATENSIONVAR, float, 0.f, float, NULL, NULL, IDS_RB_METATENSIONVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_METACOURSE, float, 0.5f, float, NULL, NULL, IDS_RB_METACOURSE, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_METACOURSEV, float, 1.0f, float, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_METAAUTOCOARSE, int, 1, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_PCNOTDRAFT, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_USESUBTREE, int, 0, int, NULL, NULL, -1, defaultDim );

	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_OFFSET, int, 0, float, NULL, &FSConvert::MaxTickToSecs, IDS_AP_OFFSETAMT, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_OFFSET_VARIATION, float, 0.f, float, NULL, NULL, IDS_ANIMATION_OFFSET_VARIATION, stdPercentDim );

	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_SCALING_METHOD, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_SCALING_SRC_LENGTH, int, 0, float, NULL, &FSConvert::MaxTickToSecs, IDS_ANIMATION_SCALING_SRC_LENGTH, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_SCALING_SRC_VARIANCE, float, 0.f, float, NULL, NULL, IDS_ANIMATION_SCALING_SRC_VARIATION, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_SCALING_DST_MULTIPLIER, float, 1.f, float, NULL, NULL, IDS_ANIMATION_SCALING_DST_MULTIPLIER, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_ANIMATION_SCALING_DST_VARIANCE, float, 0.f, float, NULL, NULL, IDS_ANIMATION_SCALING_DST_VARIATION, stdPercentDim );

	ADD_INPARAM( FSParticlePCloud::PB_VIEWPORTSHOWS, int, 1, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_DISPLAYPORTION, float, 1.f, float, NULL, NULL, IDS_RB_DISPLAYPORTION, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_CUSTOMMTL, int, 0, int, NULL, NULL, -1, defaultDim );

	ADD_INPARAM( FSParticlePCloud::PB_SPINTIME, int, 10000*FToTick, int, m_pParticleSystem->getParameter( SPIN_TIME_N ), &FSConvert::MaxTickToSecs, IDS_RB_SPINTIME, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINTIMEVAR, float, 0.f, float, m_pParticleSystem->getParameter( SPIN_VARIATION_N ), NULL, IDS_RB_SPINTIMEVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINPHASE, float, 0.f, float, m_pParticleSystem->getParameter( SPIN_PHASE_N ), NULL, IDS_RB_SPINPHASE, stdAngleDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINPHASEVAR, float, 0.f, float, m_pParticleSystem->getParameter( SPIN_PHASE_VARIATION_N ), NULL, IDS_RB_SPINPHASEVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINAXISTYPE, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINAXISX, float, 1.f, float, &(( irr::core::vector3df * )m_pParticleSystem->getParameter( SPIN_AXIS_N ))->X, NULL, IDS_RB_SPINAXISX, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINAXISY, float, 0.f, float, &(( irr::core::vector3df * )m_pParticleSystem->getParameter( SPIN_AXIS_N ))->Y, NULL, IDS_RB_SPINAXISY, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINAXISZ, float, 0.f, float, &(( irr::core::vector3df * )m_pParticleSystem->getParameter( SPIN_AXIS_N ))->Z, NULL, IDS_RB_SPINAXISZ, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPINAXISVAR, float, 0.f, float, NULL, NULL, IDS_RB_SPINAXISVAR, stdAngleDim );

	ADD_INPARAM( FSParticlePCloud::PB_EMITVINFL, float, 100.f, float, NULL, &FSConvert::FromPercent, IDS_RB_EMITVINFL, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_EMITVMULT, float, 1.f, float, NULL, NULL, IDS_RB_EMITVMULT, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_EMITVMULTVAR, float, 0.f, float, NULL, NULL, IDS_RB_EMITVMULTVAR, stdPercentDim );

	ADD_INPARAM( FSParticlePCloud::PB_BUBLAMP, float, 0.f, float, NULL, NULL, IDS_RB_BUBLAMP, stdWorldDim );
	ADD_INPARAM( FSParticlePCloud::PB_BUBLAMPVAR, float, 0.f, float, NULL, NULL, IDS_RB_BUBLAMPVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_BUBLPER, int, 100000*FToTick, float, NULL, &FSConvert::MaxTickToSecs, IDS_RB_BUBLPER, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_BUBLPERVAR, float, 0.f, float, NULL, NULL, IDS_RB_BUBLPERVAR, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_BUBLPHAS, float, 0.f, float, NULL, NULL, IDS_RB_BUBLPHAS, stdAngleDim );
	ADD_INPARAM( FSParticlePCloud::PB_BUBLPHASVAR, float, 0.f, float, NULL, NULL, IDS_RB_BUBLPHASVAR, stdPercentDim );

	ADD_INPARAM( FSParticlePCloud::PB_STRETCH, int, 0, int, NULL, NULL, IDS_AP_STRETCH, defaultDim );

	ADD_INPARAM( FSParticlePCloud::PB_SPAWNTYPE, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNGENS, int, 1, int, NULL, NULL, IDS_AP_SPAWNGENS, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNCOUNT, int, 1, int, NULL, NULL, IDS_AP_SPAWNCOUNT, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNDIRCHAOS, float, 0.f, float, NULL, NULL, IDS_AP_SPAWNDIRCHAOS, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNSPEEDCHAOS, float, 0.f, float, NULL, &FSConvert::FromPercent, IDS_AP_SPAWNSPEEDCHAOS, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNSPEEDSIGN, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNINHERITV, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNSCALECHAOS, float, 0.f, float, NULL, &FSConvert::FromPercent, IDS_AP_SPAWNSCALECHAOS, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNSCALESIGN, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNLIFEVLUE, int, 0, int, NULL, NULL, IDS_AP_SPAWNLIFEVLUE, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNSPEEDFIXED, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNSCALEFIXED, int, 0, int, NULL, NULL, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNPERCENT, int, 100, float, NULL, &FSConvert::FromPercent, -1, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNMULTVAR, float, 0.f, float, NULL, NULL, -1, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNDIEAFTER, int, 0, float, NULL, &FSConvert::MaxTickToSecs, IDS_AP_SPAWNDIEAFTER, stdTimeDim );
	ADD_INPARAM( FSParticlePCloud::PB_SPAWNDIEAFTERVAR, float, 0.f, float, NULL, NULL, IDS_AP_SPAWNDIEAFTERVAR, stdPercentDim );

	ADD_INPARAM( FSParticlePCloud::PB_PCIPCOLLIDE_ON, int, 0, int, NULL, NULL, IDS_AP_IPCOLLIDE_ON, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_PCIPCOLLIDE_STEPS, int, 2, int, NULL, NULL, IDS_AP_IPCOLLIDE_STEPS, defaultDim );
	ADD_INPARAM( FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCE, float, 1.f, float, NULL, NULL, IDS_AP_IPCOLLIDE_BOUNCE, stdPercentDim );
	ADD_INPARAM( FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCEVAR, float, 0.f, float, NULL, NULL, IDS_AP_IPCOLLIDE_BOUNCEVAR, stdPercentDim );

#undef ADD_INPARAM

	// hack init
	{
		m_pRadiusLengthSystemValue = m_pParticleSystem->getParameter( RADIUS_LENGTH_N );
		m_pHeightSystemValue = m_pParticleSystem->getParameter( HEIGHT_N );
		m_pCreationMethodParam = m_inParams.GetParamByParamBlockId( FSParticlePCloud::PB_CREATEIN );
		m_pEmitterLengthParam = m_inParams.GetParamByParamBlockId( FSParticlePCloud::PB_EMITRWID );
		m_pEmitterHeightParam = m_inParams.GetParamByParamBlockId( FSParticlePCloud::PB_EMITRDEPTH );
	}
}

void ICPCloudParticle::InitParams()
{
	ICBridgeableParticleSystem::InitParams();
	InitUIParams();
	ForceValidParameterBlockValues();
	m_pParticleSystem->init();
}


void ICPCloudParticle::FetchInParams( ICBridgeableParticleSystemInParams &inParams, TimeValue t, INode *pNode )
{
	Matrix3 maxTM = pNode->GetObjectTM( t );
	inParams.m_emitterWorldTM = ICParticleSystemBridge::MaxMatrix3ToIrrCoreMatrix4( maxTM );
}

void ICPCloudParticle::ApplyOutParams( const ICBridgeableParticleSystemOutParams &outParams )
{
	if ( outParams.m_particleCount <= 0 )
	{
		parts.FreeAll();
		return;
	}

	parts.SetCount( outParams.m_particleCount, PARTICLE_VELS | PARTICLE_AGES | PARTICLE_RADIUS | PARTICLE_TENSION );

	SAFE_DELETE( sdata );
	sdata = new PCSavePt[outParams.m_particleCount];

	for ( size_t particleId = 0; particleId < outParams.m_particleCount; ++particleId )
	{
		//const ICBridgeableParticleParamsOut &outParticle = outParams.m_particles[particleId];

		//Position
		memcpy( &parts.points[particleId], &outParams.m_pParticlePositions[ outParams.m_particleStride * particleId ], 3 * sizeof(float) );
		//Velocity
		memcpy( &parts.vels[particleId], &outParams.m_pParticleVelocities[ outParams.m_particleStride * particleId ], 3 * sizeof(float) );
		//Age
		//memcpy( &parts.ages[particleId], &outParams.m_pParticleAges[ outParams.m_particleStride * particleId ], sizeof(float) );
		parts.ages[particleId] = outParams.m_pParticleAges[ outParams.m_particleStride * particleId ] * TIME_TICKSPERSEC; //<-- Convert to ticks
		//Size
		memcpy( &parts.radius[particleId], &outParams.m_pParticleSizes[ outParams.m_particleStride * particleId ], sizeof(float) );

		//parts.points[particleId][0] = outParams.m_pParticlePositions[ m_particlePositionStride * particleId ];
		//parts.points[particleId][1] = outParticle.m_position.Y;
		//parts.points[particleId][2] = outParticle.m_position.Z;

		//parts.vels[particleId][0] = 0.f;
		//parts.vels[particleId][1] = 0.f;
		//parts.vels[particleId][2] = 0.f;

		//parts.ages[particleId] = 1.f;
		//parts.radius[particleId] = 1.f;
		parts.tension[particleId] = 0.f;

		//parts.vels[particleId][0] = outParticle.m_velocity.X;
		//parts.vels[particleId][1] = outParticle.m_velocity.Y;
		//parts.vels[particleId][2] = outParticle.m_velocity.Z;

		//parts.ages[particleId] = FSConvert::MaxSecToTicks( outParticle.m_age );
		//parts.radius[particleId] = outParticle.m_radius;
		//parts.tension[particleId] = outParticle.m_tension;

		sdata[particleId].LamTs = 0.f;
		sdata[particleId].showframe = 0;
		sdata[particleId].W = Point3( 0.f, 0.f, 0.f );
		sdata[particleId].themtl = 0;
		sdata[particleId].gennum = 0;

		//float Ts // Determines LamTs
		//	, Ts0 // Determines Ts
		//	, LamTs // Related to spin, used for rendering <==
				// LamTs is used to rotate particle bounding boxes ==> Probably current spin angle

		//	sdata[particleId].LamTs = RND11();
		//	sdata[particleId].LamTs = ptvel->Phase*(1.0f + RND11()*ptvel->VPhase);

		//TimeValue showframe // ?, used for rendering, boundingbox, GetTimes <==

		//Point3 W // partcicle spin axis, used for rendering <==

		//int themtl // used in GetRenderMesh <==
		//, gennum // probably related to particle spawn, used for rendering <==

		// 
		//parts.ages[i] = -1;
		//sdata[i].themtl=0;
		//sdata[i].L=RND0x(99);sdata[i].DL=-1;sdata[i].pvar=RND11();
		//tani=RND0x(anifr/oneframe);
		//sdata[i].showframe=(anioff==2?tani*oneframe:0);
		//sdata[i].gennum=0;
		//sdata[i].V.x=RND11();
		//sdata[i].V.y=RND11();
		//sdata[i].V.z=RND11();
		//sdata[i].Ts0=RND11();
		//sdata[i].Ts=0.0f;
		//sdata[i].LamTs=RND11();
		//sdata[i].A=RND11();
		//sdata[i].LamA=RND11();
		//// Martell 4/14/01: Fix for order of ops bug.
		//float ztmp=RND11(); float ytmp=RND11(); float xtmp=RND11();
		//parts.vels[i]=Point3(xtmp,ytmp,ztmp);
		//sdata[i].To=RND11();
		//sdata[i].Vsz=RND11();
		//// Martell 4/14/01: Fix for order of ops bug.
		//ztmp=RND11(); ytmp=RND11(); xtmp=RND11();
		//sdata[i].W=Point3(xtmp,ytmp,ztmp);
		//// Martell 4/14/01: Fix for order of ops bug.
		//ztmp=RND01(); ytmp=RND01(); xtmp=RND01();
		//sdata[i].RV=Point3(xtmp,ytmp,ztmp);
		////    sdata[i].RV=Point3(RND11(),RND11(),RND11());
		//parts.radius[i]=0.0f;
		//parts.tension[i]=RND11();
		//sdata[i].Mltvar=RND11();
		//sdata[i].SpVar=RND0x(99);
		//// Martell 4/14/01: Fix for order of ops bug.
		////parts.points[i]=(sym<3?Point3((float)RNDSign(),(float)RNDSign(),(float)RNDSign()):Point3(RND01(),RND01(),RND01()));
		//if ( sym<3 )
		//{
		//	ztmp=(float)RNDSign(); ytmp=(float)RNDSign(); xtmp=(float)RNDSign();
		//}
		//else
		//{
		//	ztmp=RND01(); ytmp=RND01(); xtmp=RND01();
		//}
		//parts.points[i]=Point3( xtmp,ytmp,ztmp );
	}

	parts.size = 10.f; //outParams.m_particleSize;

	//if ( parts.Count() <= 0 )
	//{
	//	pCount = FMath::Clamp( pCount, 0, gCountUpperLimit );
	//	parts.SetCount( pCount, PARTICLE_VELS | PARTICLE_AGES | PARTICLE_RADIUS | PARTICLE_TENSION );

	//	if ( pCount )
	//	{
	//		sdata = new PCSavePt[pCount];
	//	}
	//	
	//	{
	//		parts.points[0].x = 0;
	//		parts.points[0].y = 0;
	//		parts.points[0].z = 0;

	//		parts.ages[0] = 0;

	//		parts.radius[0] = params.m_particleSize;

	//		parts.vels[0].x = 0;
	//		parts.vels[0].y = 0;
	//		parts.vels[0].z = 0;

	//		parts.tension[0] = 1.f;

	//		parts.size = params.m_particleSize;

	//		memset( &sdata[0], 0, sizeof( PCSavePt ));
	//	}
	//}
	//else
	//{
	//	parts.radius[0] = params.m_particleSize;
	//	parts.size = params.m_particleSize;

	//	parts.vels[0].x = params.m_particleDirectionVector[0] * params.m_particleSpeed;
	//	parts.vels[0].y = params.m_particleDirectionVector[1] * params.m_particleSpeed;
	//	parts.vels[0].z = params.m_particleDirectionVector[2] * params.m_particleSpeed;
	//}

	//tvalid = t0-1;
	//valid  = TRUE;
	//// rseed=rand();
	//return (1);

	//srand(seed);               
	//parts.SetCount(c,PARTICLE_VELS|PARTICLE_AGES|PARTICLE_RADIUS|PARTICLE_TENSION);
	//int pcount=parts.Count();
	//if (sdata){delete[] sdata;sdata=NULL;} 
	//if (pcount) sdata=new PCSavePt[pcount];
	//if ((pcount<c)||(c>0 && (!sdata)))
	//{   
	//	parts.FreeAll();if (sdata) delete sdata;sdata=NULL;  maincount=0;
	//	BOOL playing=GetCOREInterface()->IsAnimPlaying();
	//	if (playing) GetCOREInterface()->EndAnimPlayback();
	//	TSTR name;name=(cnode ? cnode->GetName() : TSTR(GetString(IDS_AP_PCLOUD)));
	//	TSTR buf; buf=TSTR(GetString(IDS_OFM_PART));
	//	GetCOREInterface()->Log()->LogEntry(SYSLOG_ERROR,DISPLAY_DIALOG, GetString(IDS_OFM_ERROR),_T("%s: \n\n%s\n"),buf,name);
	//	return (0);
	//}
	//int oneframe=GetTicksPerFrame();
	//for (int i=0; i<parts.Count(); i++) 
	//{
	//	parts.ages[i] = -1;
	//	sdata[i].themtl=0;
	//	sdata[i].L=RND0x(99);sdata[i].DL=-1;sdata[i].pvar=RND11();
	//	tani=RND0x(anifr/oneframe);
	//	sdata[i].showframe=(anioff==2?tani*oneframe:0);
	//	sdata[i].gennum=0;
	//	sdata[i].V.x=RND11();
	//	sdata[i].V.y=RND11();
	//	sdata[i].V.z=RND11();
	//	sdata[i].Ts0=RND11();
	//	sdata[i].Ts=0.0f;
	//	sdata[i].LamTs=RND11();
	//	sdata[i].A=RND11();
	//	sdata[i].LamA=RND11();
	//	// Martell 4/14/01: Fix for order of ops bug.
	//	float ztmp=RND11(); float ytmp=RND11(); float xtmp=RND11();
	//	parts.vels[i]=Point3(xtmp,ytmp,ztmp);
	//	sdata[i].To=RND11();
	//	sdata[i].Vsz=RND11();
	//	// Martell 4/14/01: Fix for order of ops bug.
	//	ztmp=RND11(); ytmp=RND11(); xtmp=RND11();
	//	sdata[i].W=Point3(xtmp,ytmp,ztmp);
	//	// Martell 4/14/01: Fix for order of ops bug.
	//	ztmp=RND01(); ytmp=RND01(); xtmp=RND01();
	//	sdata[i].RV=Point3(xtmp,ytmp,ztmp);
	//	//    sdata[i].RV=Point3(RND11(),RND11(),RND11());
	//	parts.radius[i]=0.0f;
	//	parts.tension[i]=RND11();
	//	sdata[i].Mltvar=RND11();
	//	sdata[i].SpVar=RND0x(99);
	//	// Martell 4/14/01: Fix for order of ops bug.
	//	//parts.points[i]=(sym<3?Point3((float)RNDSign(),(float)RNDSign(),(float)RNDSign()):Point3(RND01(),RND01(),RND01()));
	//	if ( sym<3 )
	//	{
	//		ztmp=(float)RNDSign(); ytmp=(float)RNDSign(); xtmp=(float)RNDSign();
	//		parts.points[i]=Point3( xtmp,ytmp,ztmp );
	//	}
	//	else
	//	{
	//		ztmp=RND01(); ytmp=RND01(); xtmp=RND01();
	//		parts.points[i]=Point3( xtmp,ytmp,ztmp );
	//	}
	//}
}

void ICPCloudParticle::GetICForceModifiers( INode *pNode, ModifierList &modifiers )
{
	if ( !pNode )
	{
		return;
	}

	IDerivedObject *pDerivedObject = pNode->GetWSMDerivedObject();
	if ( pDerivedObject )
	{
		EmitterExporter exporter( NULL );
		ColladaModifierStack colladaModifiers;
		exporter.ResolveModifiers( pNode, pDerivedObject, colladaModifiers );
		for ( ColladaModifierStack::iterator itM = colladaModifiers.begin(); itM != colladaModifiers.end(); ++itM )
		{
			ColladaModifier* mod = ( *itM );
			if ( mod->GetClassID() == FSForceGravity::IC_MODIFIER_CLASSID )
			{
				modifiers.push_back( ((ICGravityModifier*)mod)->GetInterface() );
			}
			else 
			if ( mod->GetClassID() == FSForceWind::IC_MODIFIER_CLASSID )
			{
				modifiers.push_back( ((ICWindModifier*)mod)->GetInterface() );
			}
			else 
			if ( mod->GetClassID() == FSDeflector::IC_MODIFIER_CLASSID )
			{
				modifiers.push_back( ((ICDeflectorModifier*)mod)->GetInterface() );
			}
		}
		CLEAR_POINTER_VECTOR( colladaModifiers );
	}
}

void ICPCloudParticle::UpdateICForceModifierBindedParameterValues( Modifier *pModifier, TimeValue t )
{
	if ( pModifier->ClassID() == FSForceGravity::IC_MODIFIER_CLASSID )
	{
		ICGravityMod *pMod = ( ICGravityMod * )pModifier;
		pMod->force.UpdateBindedParameters( pMod->m_params, pMod->tm, t );
	}
	else if ( pModifier->ClassID() == FSForceWind::IC_MODIFIER_CLASSID )
	{
		ICWindMod *pMod = ( ICWindMod * )pModifier;
		pMod->force.UpdateBindedParameters( pMod->m_params, pMod->tm, t );
	}
	else if ( pModifier->ClassID() == FSDeflector::IC_MODIFIER_CLASSID )
	{
		ICDeflectMod *pMod = ( ICDeflectMod * )pModifier;
		pMod->deflect.UpdateBindedParameters( pMod->m_params, pMod->tm, t );
	}
}

void ICPCloudParticle::UpdateICForceModifiers( TimeValue t,INode *node )
{
	ModifierList modifiers;
	GetICForceModifiers( node, modifiers );
	
	// unbind no longer applied modifiers
	ModifierList modifiersToErase;
	for ( ICForceModifierMap::iterator iter = m_icForceModifiers.begin(); iter != m_icForceModifiers.end(); ++iter )
	{
		Modifier *pModifier = iter->first;
		if ( modifiers.find( pModifier ) == modifiers.end() )
		{
			if ( pModifier->ClassID() == FSForceGravity::IC_MODIFIER_CLASSID )
			{
				ICGravityMod *pMod = ( ICGravityMod * )pModifier;
				m_pParticleSystem->unbindForce( iter->second );
			}
			else if ( pModifier->ClassID() == FSForceWind::IC_MODIFIER_CLASSID )
			{
				ICWindMod *pMod = ( ICWindMod * )pModifier;
				m_pParticleSystem->unbindForce( iter->second );
			}
			else if ( pModifier->ClassID() == FSDeflector::IC_MODIFIER_CLASSID )
			{
				ICDeflectMod *pMod = ( ICDeflectMod * )pModifier;
				m_pParticleSystem->unbindForce( iter->second );
			}

			modifiersToErase.push_back( pModifier );
		}
	}

	for ( ModifierList::iterator iter = modifiersToErase.begin(); iter != modifiersToErase.end(); ++iter )
	{
		Modifier *pModifier = *iter;
		m_icForceModifiers.erase( pModifier );
	}


	// bind newly applied modifiers
	for ( ModifierList::iterator iter = modifiers.begin(); iter != modifiers.end(); ++iter )
	{
		Modifier *pModifier = *iter;
		if ( m_icForceModifiers.find( pModifier ) == m_icForceModifiers.end() )
		{
			int forceId = -1;
			if ( pModifier->ClassID() == FSForceGravity::IC_MODIFIER_CLASSID )
			{
				ICGravityMod *pMod = ( ICGravityMod * )pModifier;
				pMod->m_params.transformation = &pMod->tm;
				forceId = m_pParticleSystem->bindForce<irr::ps::PGravity>( pMod->m_params );
			}
			else if ( pModifier->ClassID() == FSForceWind::IC_MODIFIER_CLASSID )
			{
				ICWindMod *pMod = ( ICWindMod * )pModifier;
				pMod->m_params.transformation = &pMod->tm;
				forceId = m_pParticleSystem->bindForce<irr::ps::PWind>( pMod->m_params );
			}
			else if ( pModifier->ClassID() == FSDeflector::IC_MODIFIER_CLASSID )
			{
				ICDeflectMod *pMod = ( ICDeflectMod * )pModifier;
				pMod->m_params.transformation = &pMod->tm;
				forceId = m_pParticleSystem->bindForce<irr::ps::PDeflector>( pMod->m_params );
			}

			if ( forceId >= 0 )
			{
				m_icForceModifiers.insert( pModifier, forceId );
			}
		}
	}

	// update binded parameter values
	for ( ICForceModifierMap::iterator iter = m_icForceModifiers.begin(); iter != m_icForceModifiers.end(); ++iter )
	{
		UpdateICForceModifierBindedParameterValues( iter->first, t );	
	}
}

// Box length and height are switched to accommodate for different up axises ( Z-Up in Max, Y-Up in Irrlicht )
void ICPCloudParticle::UpdateParameterMapping()
{
	if ( !m_pCreationMethodParam
		|| !m_pEmitterLengthParam
		|| !m_pEmitterHeightParam 
		|| !m_pRadiusLengthSystemValue
		|| !m_pHeightSystemValue )
	{
		return;
	}

	int creationMethod = 0;
	m_pCreationMethodParam->GetPBlockValue( &creationMethod, 0, FOREVER );
	void *pCurrentLength = m_pEmitterLengthParam->GetSystemValuePtr();

	// Box
	if ( creationMethod == 0 )
	{
		if ( pCurrentLength == m_pRadiusLengthSystemValue )
		{
			m_pEmitterLengthParam->SetSystemValuePtr( m_pHeightSystemValue );
			m_pEmitterHeightParam->SetSystemValuePtr( m_pRadiusLengthSystemValue );
		}
	}
	else
	{
		if ( pCurrentLength != m_pRadiusLengthSystemValue )
		{
			m_pEmitterLengthParam->SetSystemValuePtr( m_pRadiusLengthSystemValue );
			m_pEmitterHeightParam->SetSystemValuePtr( m_pHeightSystemValue );
		}		
	}
}

// Fill ICBridgeableParticleSystemInParams with info from Max side
// Set Irrlicht Particle System in params
// Update Irrlicht Particle System
// Get Irrlicht Particle System out params
// Use ICBridgeableParticleSystemOutParams to set relevant fields in Max side
void ICPCloudParticle::UpdateParticles(TimeValue t,INode *node)
{
	UpdateICForceModifiers( t, node );
	UpdateParameterMapping();

	// get all params
	FetchInParams( m_inParams, t, node );
	ICParticleSystemBridge::GetInst()->Update( t, m_pParticleSystem, m_inParams, m_outParams );
	ApplyOutParams( m_outParams );

	//// original
	//TimeValue t0,dt,t2,grow,fade;
	//int c, birth,counter,tpf=GetTicksPerFrame(),count=0,anioff;
	//VelDir ptvel;
	//Matrix3 tm,lasttm; 
	//int isrend=TestAFlag(A_RENDER),bmethod,onscreen,oneframe;
	//// variable for new collision scheme (Bayboro 2/5/01)
	//CollisionCollection cc;

	//// fix for "Abort by Escape key" problem (Bayboro 1/31/01)
	//if (cancelled && (tvalid == t)) return;
	//if (cancelled && valid) tvalid = TIME_NegInfinity;
	//cancelled = FALSE;   

	//// initialization for new collision scheme (Bayboro 2/5/01)
	//cc.Init(cobjs);

	//// The size of steps we take to integrate will be frame size steps.
	//oneframe=GetTicksPerFrame();
	//if (stepSize!=oneframe) 
	//{  
	//	stepSize = oneframe;
	//	valid = FALSE;
	//}

	//float FperT=GetFrameRate()/(float)TIME_TICKSPERSEC;
	//pblock->GetValue(FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD,t,anioff,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_EMITSTART,t,t0,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SIZE,t,parts.size,FOREVER); 
	//pblock->GetValue(FSParticlePCloud::PB_BIRTHMETHOD,0,bmethod,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_VIEWPORTSHOWS,0,onscreen,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_EMITSTOP,0,t2,FOREVER);

	//// t0-=oneframe;t2-=oneframe;
	//if (bmethod)
	//{
	//	pblock->GetValue(FSParticlePCloud::PB_PTOTALNUMBER,0,c,FOREVER);
	//}

	//int subtree,frag,custmtl=0;
	//pblock->GetValue(FSParticlePCloud::PB_CUSTOMMTL,0,custmtl,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_USESUBTREE,0,subtree,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_PARTICLECLASS,0,frag,FOREVER);
	//if (t < t0)
	//{
	//	// Before the start time, nothing is happening
	//	parts.FreeAll();
	//	if (sdata) {delete[] sdata;sdata=NULL;}
	//	ResetSystem(t);
	//	return;
	//}

	//int pkind;
	//float dpercent;
	//pblock->GetValue(FSParticlePCloud::PB_DISPLAYPORTION,0,dpercent,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_PARTICLETYPE,0,pkind,FOREVER);

	//if (!valid || t<tvalid || tvalid<t0) 
	//{  
	//	// Set the particle system to the initial condition
	//	TimeValue cincr; 
	//	dispt=-99999;
	//	if (!bmethod)
	//	{ 
	//		c=0;
	//		for (TimeValue ti=t0;ti<=t2;ti+=oneframe)
	//		{ 
	//			pblock->GetValue(FSParticlePCloud::PB_PBIRTHRATE,ti,cincr,FOREVER);
	//			if (cincr<0) cincr=0;
	//			c+=cincr;
	//		}
	//	}
	//	if (!isrend) c=(int)(dpercent*(float)c+FTOIEPS);
	//	if (!ComputeParticleStart(t0,c))
	//	{ 
	//		ResetSystem(t);
	//		return;
	//	}
	//	dispt=t-1;
	//	maincount=parts.Count();
	//	ResetSystem(t,FALSE);
	//}

	//int total=maincount;
	//valid = TRUE;
	//int stype,maxgens,spmultb;
	//float spawnbvar;
	//TimeValue dis;

	//pblock->GetValue(FSParticlePCloud::PB_SPAWNTYPE,0,stype,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPAWNGENS,0,maxgens,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPAWNCOUNT,0,spmultb,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPINAXISTYPE,0,ptvel.axisentered,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPEEDDIR,0,ptvel.direntered,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_DIRX,0,ptvel.Dir.x,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_DIRY,0,ptvel.Dir.y,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_DIRZ,0,ptvel.Dir.z,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_DIRVAR,0,ptvel.DirVar,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_DISPUNTIL,0,dis,FOREVER);

	//SpawnVars spawnvars;
	//spawnvars.axisentered=ptvel.axisentered;

	//if (t2<t0) 
	//{
	//	t2=t0;
	//}
	//// TimeValue fstep=oneframe;

 //  //t2+=fstep;
	//TimeValue createover;
	//createover=t2-t0+oneframe;
	//counter=(isrend?rcounter:vcounter);
	//float frate,grate;
	//pblock->GetValue(FSParticlePCloud::PB_GROWTIME,0,grow,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_FADETIME,0,fade,FOREVER);
	//frate=(fade>0.0f?(1-M)/fade:0.0f);
	//grate=(grow>0.0f?(1-M)/grow:0.0f);
	//float basesize;
	//BOOL fullframe;
	//if (!isrend)
	//{ 
	//	int offby=t%oneframe;
	//	if (offby!=0) t-=offby;
	//}

	//pblock->GetValue(FSParticlePCloud::PB_SPAWNSCALESIGN,0,spawnvars.scsign,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPAWNSPEEDSIGN,0,spawnvars.spsign,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPAWNINHERITV,0,spawnvars.invel,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPAWNSPEEDFIXED,0,spawnvars.spconst,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_SPAWNSCALEFIXED,0,spawnvars.scconst,FOREVER);
	//pblock->GetValue(FSParticlePCloud::PB_CREATEIN,0,ptvel.sym,FOREVER);
	//
	//if ((ptvel.sym==3)&&(!distnode))
	//{
	//	ptvel.sym=0;
	//}

	//int sper,spmult;
	//float smper;
	//BOOL first=(tvalid<t0);
	//
	//while ((tvalid < t)&&(tvalid<=dis))
	//{  
	//	int born = 0;
	//	TimeValue atvalid=abs(tvalid);
	//	count=0;
	//	if (first)
	//	{
	//		tvalid=t0;
	//	}

	//	// Compute our step size
	//	if (tvalid%stepSize !=0) 
	//	{
	//		dt = stepSize - atvalid%stepSize;
	//	}
	//	else 
	//	{ 
	//		dt = stepSize; 
	//	}
	//	
	//	if (tvalid + dt > t)
	//	{
	//		dt = t-tvalid;
	//	}

	//	// Increment time
	//	if (!first) tvalid +=dt;
	//	if (tvalid>dis)
	//	{ 
	//		for (int j=0; j<parts.Count(); j++)
	//		{ 
	//			parts.ages[j] = -1;  
	//		}
	//		tvalid=t;
	//		continue;
	//	}

	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNDIEAFTER,tvalid,ptvel.persist,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNDIEAFTERVAR,tvalid,ptvel.pvar,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPEED,tvalid,ptvel.Speed,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPEEDVAR,tvalid,ptvel.VSpeed,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SIZE,tvalid,ptvel.Size,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_LIFE,tvalid,ptvel.Life,FOREVER);
	//	if (llist.Count()==0) deftime=ptvel.Life/oneframe;
	//	pblock->GetValue(FSParticlePCloud::PB_LIFEVAR,tvalid,ptvel.Vl,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPINTIME,tvalid,ptvel.Spin,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPINTIMEVAR,tvalid,ptvel.VSpin,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPINPHASE,tvalid,ptvel.Phase,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPINPHASEVAR,tvalid,ptvel.VPhase,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SIZEVAR,tvalid,ptvel.VSz,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_METATENSION,tvalid,ptvel.bstr,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_METATENSIONVAR,tvalid,ptvel.bstrvar,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_BUBLAMP,tvalid,ptvel.ToAmp,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_BUBLAMPVAR,tvalid,ptvel.VToAmp,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_BUBLPHAS,tvalid,ptvel.ToPhase,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_BUBLPHASVAR,tvalid,ptvel.VToPhase,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_BUBLPER,tvalid,ptvel.ToPeriod,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_BUBLPERVAR,tvalid,ptvel.VToPeriod,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_EMITRHEIGHT,tvalid,ptvel.width,FOREVER);ptvel.width*=0.5f;
	//	pblock->GetValue(FSParticlePCloud::PB_EMITRWID,tvalid,ptvel.len,FOREVER);ptvel.len*=0.5f;
	//	pblock->GetValue(FSParticlePCloud::PB_EMITRDEPTH,tvalid,ptvel.depth,FOREVER);ptvel.depth*=0.5f;
	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNDIRCHAOS,tvalid,spawnvars.dirchaos,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNPERCENT,tvalid,sper,FOREVER);   
	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNMULTVAR,tvalid,smper,FOREVER);  
	//	spawnbvar=smper*spmultb;
	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNSPEEDCHAOS,tvalid,spawnvars.spchaos,FOREVER);
	//	spawnvars.spchaos/=100.0f;
	//	pblock->GetValue(FSParticlePCloud::PB_SPAWNSCALECHAOS,tvalid,spawnvars.scchaos,FOREVER);
	//	spawnvars.scchaos/=100.0f;
	//	pblock->GetValue(FSParticlePCloud::PB_SPINAXISX,tvalid,ptvel.Axis.x,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPINAXISY,tvalid,ptvel.Axis.y,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_SPINAXISZ,tvalid,ptvel.Axis.z,FOREVER);
	//	if (Length(ptvel.Axis)==0.0f) ptvel.Axis.x=0.001f;
	//	pblock->GetValue(FSParticlePCloud::PB_SPINAXISVAR,t,ptvel.axisvar,FOREVER);
	//	spawnvars.Axis=ptvel.Axis;spawnvars.axisvar=ptvel.axisvar;
	//	basesize=M*ptvel.Size;
	//	// Compute the number of particles that should be born LAST FRAME!
	//	birth=0;
	//	fullframe=(tvalid%tpf==0);
	//	if (fullframe)
	//	{ 
	//		if (bmethod)
	//		{ 
	//			int tdelta;
	//			if (tvalid>=t2) birth=total-counter;
	//			else
	//			{ 
	//				tdelta=int((float)total*(tvalid-t0+oneframe)/createover);
	//				birth=tdelta-counter;
	//			}
	//		}
	//		else if (tvalid<=t2)
	//		{ 
	//			pblock->GetValue(FSParticlePCloud::PB_PBIRTHRATE,tvalid,total,FOREVER);
	//			if (!isrend) total=(int)(dpercent*(float)total+FTOIEPS);
	//			birth=total;
	//			if (birth+counter>maincount) birth=0;
	//		}
	//	}

	//	// First increment age and kill off old particles
	//	for (int j=0;j<parts.Count();j++)
	//	{ 
	//		if ((!isrend)&&(GetAsyncKeyState (VK_ESCAPE)) )
	//		{ 
	//			valid=FALSE;tvalid=t;cancelled=TRUE;parts.FreeAll();return;
	//		}
	//		if (!parts.Alive(j)) continue;
	//		parts.ages[j] += dt;
	//		if (parts.ages[j] >= sdata[j].L) 
	//		{  
	//			if ((stype!=ONDEATH)||(sdata[j].gennum>=maxgens)||(sdata[j].SpVar>=sper)) parts.ages[j] = -1; 
	//			else if (fullframe)
	//			{
	//				spmult=(int)(sdata[j].Mltvar*spawnbvar)+spmultb;
	//				if (spmult!=0) DoSpawn(j,spmult,spawnvars,ptvel.Vl,FALSE);
	//				else parts.ages[j] = -1;
	//			}
	//		} else if (sdata[j].DL>-1)
	//		{
	//			sdata[j].DL+=dt;
	//			if (sdata[j].DL>sdata[j].persist) parts.ages[j]=-1;
	//		}
	//		
	//		if (parts.ages[j]>-1)
	//		{ 
	//			if (fullframe &&((stype==EMIT)&&(sdata[j].gennum==0)&&(sdata[j].SpVar<sper)))
	//			{  
	//				spmult=(int)(sdata[j].Mltvar*spawnbvar)+spmultb;
	//				if (spmult!=0) DoSpawn(j,spmult,spawnvars,ptvel.Vl,TRUE);
	//			}
	//			//       if (pkind==RENDTYPE6)
	//			//         parts.radius[j]=ptvel.Size;       else
	//			if ((stype<2)||(maxgens==0))
	//			{
	//				parts.radius[j]=FigureOutSize(parts.ages[j],ptvel.Size,grow,fade,sdata[j].L,grate,frate)*(1+sdata[j].Vsz);
	//			}
	//			else 
	//			{ 
	//				if (sdata[j].gennum==0)
	//				parts.radius[j]=FigureOutSize(parts.ages[j],ptvel.Size,grow,0,sdata[j].L,grate,frate)*(1+sdata[j].Vsz);
	//				else if (sdata[j].gennum==maxgens)
	//				parts.radius[j]=FigureOutSize(parts.ages[j],sdata[j].Vsz,0,fade,sdata[j].L,grate,frate);
	//			}
	//		}
	//	}
	//	if (birth>0)
	//	{
	//		Matrix3 tm;   
	//		float stepCoef = 1.0f;
	//		BOOL newmesh=(ptvel.sym==3)&&(distnode);
	//		Point3 *fnorms=NULL;TriObject *triOb=NULL;Matrix3 disttm=NULL;Object *pobj=NULL;
	//		TimeValue checkone=tvalid-t0,acheck=abs(checkone);
	//		if (newmesh!=NULL)
	//		//    {  tm = distnode->GetObjTMAfterWSM((acheck<stepSize?t0:(tvalid-stepSize)));
	//		//    {  tm = distnode->GetObjectTM((acheck<stepSize?t0:(tvalid-stepSize))); //change
	//		//       stepCoef = acheck<stepSize ? float(tvalid-t0)/stepSize : 1.0;
	//		{  
	//			tm = distnode->GetObjectTM(tvalid-stepSize); //change
	//			stepCoef = 1.0;
	//		}
	//		else
	//		//    {  tm = node->GetObjTMBeforeWSM((atvalid<stepSize?t0:(tvalid-stepSize))); //tune 
	//		//    {  tm = node->GetObjTMBeforeWSM((acheck<stepSize?t0:(tvalid-stepSize)));
	//		//       stepCoef = acheck<stepSize ? float(tvalid-t0)/stepSize : 1.0f;
	//		{
	//			tm = node->GetObjTMBeforeWSM(tvalid-stepSize);
	//			stepCoef = 1.0f;
	//		}

	//		//    if (newmesh!=NULL)   tm = distnode->GetObjTMAfterWSM(tvalid+stepSize);
	//		if (newmesh!=NULL)   tm = distnode->GetObjectTM(tvalid+stepSize); //change
	//		else tm = node->GetObjTMBeforeWSM(tvalid+stepSize);

	//		if (newmesh!=NULL)
	//		//       tm = distnode->GetObjTMAfterWSM(tvalid);
	//			tm = distnode->GetObjectTM(tvalid); //change
	//		else 
	//			tm = node->GetObjTMBeforeWSM(tvalid);

	//		if (newmesh&&(born<birth)) 
	//		{ 
	//			pobj = distnode->EvalWorldState(tvalid).obj;
	//			triOb=TriIsUseable(pobj,tvalid);
	//			if (triOb&&(triOb->GetMesh().getNumFaces()>0)) 
	//			{ 
	//				int nf=triOb->GetMesh().getNumFaces();
	//				fnorms=new Point3[nf];
	//				Point3 v0, v1, v2;
	//				Face *face=triOb->GetMesh().faces;
	//				for (int fi=0; fi<nf; fi++) 
	//				{ 
	//					v1 = triOb->GetMesh().verts[face[fi].v[1]];
	//					fnorms[fi] = Normalize((v1-triOb->GetMesh().verts[face[fi].v[0]])^(triOb->GetMesh().verts[face[fi].v[2]]-v1));
	//				}
	//			} 
	//			else 
	//			{
	//				ptvel.sym=0;newmesh=FALSE;
	//				// tm=node->GetObjTMBeforeWSM(acheck<stepSize?t0:(tvalid-stepSize));
	//				tm=node->GetObjTMBeforeWSM(tvalid-stepSize);
	//				tm=node->GetObjTMBeforeWSM(tvalid);
	//			}
	//		}

	//		ptvel.bps.Init((newmesh==NULL) ? node : distnode, tvalid, stepSize);

	//		// Next, birth particles at the birth rate
	//		for (int j=counter; j<maincount; j++)
	//		{
	//			if (born>=birth) break;

	//			BirthParticle(node,tvalid,j,&ptvel,(newmesh?&triOb->GetMesh():NULL),fnorms,disttm);
	//			//        if ((pkind==RENDTYPE5)||(pkind==RENDTYPE6))
	//			//         parts.radius[j]=ptvel.Size; else 
	//			parts.radius[j]=(grow>0?basesize:ptvel.Size)*(1+sdata[j].Vsz);
	//			sdata[j].themtl=int((tvalid-t0)*FperT);
	//			sdata[j].showframe=(anioff==1?0:sdata[j].showframe);
	//			born++;counter++;
	//		}

	//		if (triOb&&(triOb!=pobj)) triOb->DeleteThis();
	//		if (fnorms) delete[] fnorms;
	//	}

	//	// Apply forces to modify velocity
	//	int fc=fields.Count();
	//	if ( fc > 0 )
	//	{
	//		for (int j=0; j < parts.Count(); j++) 
	//		{ 
	//			Point3 force,tvel=Zero;
	//			for (int i=0; i<fc; i++) 
	//			{ 
	//				if ((!isrend)&&(GetAsyncKeyState (VK_ESCAPE)) ) 
	//				{ 
	//					valid=FALSE;tvalid=t;cancelled=TRUE;parts.FreeAll();return;}
	//					if (!parts.Alive(j)) continue;
	//					parts[j]=sdata[j].V;
	//					force = fields[i]->Force(tvalid,parts[j],parts.vels[j],j);
	//					float curdt=(float)dt;
	//					if ((parts.ages[j]==0)&&(sdata[j].gennum==0))
	//					{
	//						curdt=tpf*(sdata[j].RV.x);
	//					}
	//					//        if ((parts.ages[j]==0)&&(sdata[j].gennum==0)) curdt=tpf*((0.5f*sdata[j].RV.x)+0.5f);
	//					tvel += 10.0f*force * curdt;
	//				}
	//			parts.vels[j]+=tvel;
	//		}
	//	}

	//	// Increment the positions
	//	count=0;
	//	// IPC IPC IPC IPC

	//	int IPC,ipcsteps;float B,Vb;
	//	pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_ON,tvalid,IPC,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_STEPS,tvalid,ipcsteps,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCE,tvalid,B,FOREVER);
	//	pblock->GetValue(FSParticlePCloud::PB_PCIPCOLLIDE_BOUNCEVAR,tvalid,Vb,FOREVER);
	//	if (IPC)
	//	{  
	//		CollideParticle cp;
	//		int ddt=dt/ipcsteps,remtime=0,snum=0;
	//		TimeValue curt=t;
	//		if (dt > 0)
	//		while (snum < ipcsteps)
	//		{ 
	//			if ((!isrend)&&(GetAsyncKeyState (VK_ESCAPE)) )
	//			{   
	//				valid=FALSE;
	//				tvalid=t;
	//				cancelled=TRUE;
	//				parts.FreeAll();
	//				return;
	//			}
	//			if (remtime==0) remtime=ddt;
	//			mindata md=cp.InterPartCollide(parts,cobjs,remtime,snum,B,Vb,curt,lc);
	//			for (int j=0; j<parts.Count(); j++)
	//			{  
	//				if (parts.ages[j]>0) 
	//				{  
	//					if ((j!=md.min)&&(j!=md.min2)) 
	//					{  
	//						if (fullframe) 
	//							MovePart(j,md.mintime,fullframe,tpf);
	//						else
	//							parts[j]+=parts.vels[j]*(float)md.mintime;
	//					}
	//					else if (fullframe) sdata[j].V=parts[j];
	//				}
	//			}
	//		}

	//		// If we didn't collide, then increment.
	//		for (int j=0; j<parts.Count(); j++)
	//		{  
	//			sdata[j].Ts = (float)ptvel.Spin*sdata[j].Ts0;
	//			sdata[j].LamTs += (FloatEQ0(sdata[j].Ts)?0.0f:dt/sdata[j].Ts);
	//		}
	//	}
	//	else
	//	for (int j=0; j<parts.Count(); j++)
	//	{  
	//		if ((!isrend)&&(GetAsyncKeyState (VK_ESCAPE)) )
	//		{ 
	//			valid=FALSE;tvalid=t;cancelled=TRUE;parts.FreeAll();return;
	//		}

	//		if ((!parts.Alive(j))||(parts.ages[j]==0)) continue;
	//		count++;
	//		// Check for collisions
	//		BOOL collide = FALSE;
	//		// float meaninglesstime;
	//		float collisionTime, remTime = (float)dt;
	//		BOOL maybeStuck = FALSE;
	//		// for (int i=0; i<cobjs.Count(); i++) 
	//		for (int i=0; i<cc.MAX_COLLISIONS_PER_STEP; i++)
	//		{
	//			//if (cobjs[i]->CheckCollision(
	//			//    tvalid,parts[j],parts.vels[j], (float)dt, j,&meaninglesstime,TRUE))
	//			if (cc.CheckCollision(tvalid, parts[j], parts.vels[j], remTime, j, &collisionTime, FALSE))
	//			{
	//				collide = TRUE;
	//				remTime -= collisionTime;
	//				if (remTime <= 0.0f) break; // time limit for the current inegration step
	//			}
	//			else break;
	//			if (i==cc.MAX_COLLISIONS_PER_STEP-1) maybeStuck = TRUE;
	//		}
	//		if (collide)
	//		{
	//			if (!maybeStuck) // if particle stuck we can't risk to propagate particle movement for the current frame
	//				parts[j] += parts.vels[j] * remTime;
	//			if (stype==1)
	//			{ 
	//				if (sdata[j].persist==0)
	//				{
	//					parts.ages[j] = -1;count--;
	//				}
	//				else 
	//				{
	//					sdata[j].DL=0;
	//				}
	//			}
	//			else if (fullframe &&((stype==COLLIDE)&&(sdata[j].gennum<maxgens)&&(sdata[j].SpVar<sper)))
	//			{ 
	//				int spmult=(int)(sdata[j].Mltvar*spawnbvar)+spmultb;
	//				if (spmult!=0) DoSpawn(j,spmult,spawnvars,ptvel.Vl,FALSE);
	//			}
	//		}

	//		sdata[j].Ts=(float)ptvel.Spin*sdata[j].Ts0;
	//		sdata[j].LamTs+=(FloatEQ0(sdata[j].Ts)?0.0f:dt/sdata[j].Ts);         
	//		// If we didn't collide, then increment.
	//		if (!collide) MovePart(j,dt,fullframe,tpf);
	//		else if (fullframe) sdata[j].V=parts[j];
	//	}
	//	if (first) first=FALSE;
	//}
	///* if ((frag==METABALLS)&&(onscreen==2))
	//{ float res,bstr,thres=0.6f;
	//  pblock->GetValue(FSParticlePCloud::PB_METATENSION,t,bstr,FOREVER);
	//  pblock->GetValue(FSParticlePCloud::PB_METACOURSE,0,res,FOREVER);
	// if (cmesh) delete[] cmesh;cmesh=new Mesh;
	//  metap.CreateMetas(parts,cmesh,thres,res,bstr);
	//}
	//else */
	//if ((frag==INSTGEOM)&&(onscreen>1)&&(custnode))
	//{ 
	//	TimeValue anist=GetAnimStart(),aniend=GetAnimEnd();
	//	thePCloudDraw.anifr=aniend+stepSize;
	//	thePCloudDraw.t=t;
	//	thePCloudDraw.anioff=anioff;
	//	if (count>0)
	//	{
	//		GetTimes(times,t,thePCloudDraw.anifr,anioff);
	//	}
	//	else
	//	{
	//		times.tl.ZeroCount();
	//	}
	//	if (onscreen==2)
	//	{
	//		GetMesh(t,subtree,custmtl);
	//	}
	//	else 
	//	{
	//		GetallBB(custnode,subtree,t);
	//	}
	//}  
	//if (isrend) rcounter=counter;
	//else
	//{ 
	//	vcounter=counter;
	//}
	//if ( tvalid < t)
	//{
	//	tvalid = t;
	//}
	//valid = TRUE;
}

//void DBG_DumpPCloudParams( ICBridgeableParticleSystemInParams &params )
//{
//	FUStringBuilder builder;
//
//	builder.append( "\n" );
//	builder.append( "PCloud Params" );
//	builder.append( "\n" );
//	builder.append( "-------------" );
//	builder.append( "\n" );
//
//	mprintf( "%s", builder.ToCharPtr() );
//}
