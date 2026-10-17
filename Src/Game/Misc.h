#pragma once

#include "Core/irrlicht.h"

void cleanNode (ISceneNode* node, bool unit = false, bool ref = false);
void setEmissiveColor (ISceneNode* node, SColor color);

position2di screenFrom3DPos (const vector3df& pos3d);
vector2df vector2DFrom3DPos (const vector3df& pos3d);

float GetFloatWithDefault (IAttributes* attr, const char* name, float default_val);

// the range is inclusive!
int RandInt (int min, int max);
float RandFloat (float min, float max);

// linearly remaps x from the interval [f0, f1] to the interval [t0, t1]
int IntRemap1 (int x, int f0, int f1, int t0, int t1);
int IntRemap2 (int x, int f0, int f1, int t0, int t1);
int IntRemap3 (int x, int f0, int f1, int t0, int t1);
float FloatRemap (float x, float f0, float f1, float t0, float t1);

// looks like this:
// 1            *-------------*
//             /               \
//            /                 \
// 0 --------*                   *--------
//          x1  x2           x3  x4
float LinearUpFlatDown (float x, float x1, float x2, float x3, float x4);

// looks like this:
//                           /
// +1                       *
//                         /x4
//                        /
//                       /
//  0       *-----------*
//         /x2          x3
//        /
//       /
// -1   *
//     /x1          
float LinearUpFlatUp (float x, float x1, float x2, float x3, float x4);

float LinearStep (float x, float x1, float x2);

vector3df randomVectorInSphere (float radius = 1.0f);

void RemoveUnusedTextures ();

v2f HermiteInterpolation (float t, v2f p1, v2f p2, v2f m1, v2f m2);
v2f HermiteInterpolationDerivative (float t, v2f p1, v2f p2, v2f m1, v2f m2);
v2f HermiteInterpolationSecondDerivative (float t, v2f p1, v2f p2, v2f m1, v2f m2);
v2f HermiteInterpolationOffset (float t, float offset, v2f p1, v2f p2, v2f m1, v2f m2);
float HermiteInterpolationCurvature (float t, v2f p1, v2f p2, v2f m1, v2f m2);

void CleanColladaCache (bool wholeCache);
void PreLoadColladaScene (const char* path);
ISceneNode* LoadColladaScene (const char* path, bool cache = false);

enum FrameEventTypes
{
	FET_Loading,
	FET_SpawnUnit,
	FET_PlaySound,
	FET_PlayMusic,
	FET_UnitDying,
	FET_UnitDead
};

static const char* FrameEventNames[] =
{
	"Loading",
	"SpawnUnit",
	"PlaySound",
	"PlayMusic",
	"Dying",
	"Dead"
};

struct FrameEvent
{
	int frame;
	int type;
	int data;
};

void FrameEnded (int ms);
void AddFrameEvent (int type, int data);
void SaveFrameStats ();
