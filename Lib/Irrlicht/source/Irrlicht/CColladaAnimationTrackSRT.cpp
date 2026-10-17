#include "CColladaAnimationTrackSRT.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

CSrt::CSrt(collada::SAnimation &animation)
: CAnimationTrack(animation)
{
	
}

void 
CSrt::GetKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const
{
	SVectorSRT &vSrt = *(SVectorSRT*)&getOutput();
	STransformSRT &output = *(STransformSRT*)pOutputPtr;

	output = vSrt[iKey0]; // don't interpolate for now
}

void 
CSrt::GetKeyBasedValue(int iKey0, void *pOutputPtr) const
{
	SVectorSRT &vSrt = *(SVectorSRT*)&getOutput();
	STransformSRT &output = *(STransformSRT*)pOutputPtr;

	output = vSrt[iKey0];	
}

}; // animation_track
}; // collada
}; // irr