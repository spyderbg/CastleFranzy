#ifndef __CCOLLADAANIMATIONTRACKSRT_H__
#define __CCOLLADAANIMATIONTRACKSRT_H__

#include "CColladaAnimationTrack.h"

namespace irr
{
namespace collada
{
namespace animation_track
{

class CSrt 
	: public CAnimationTrack
{
public:
	CSrt(collada::SAnimation &animation);

	virtual void GetKeyBasedValue(int iKey0, int iKey1, float ratio, void *pOutputPtr) const;

	virtual void GetKeyBasedValue(int iKey0, void *pOutputPtr) const;
protected:

};

}; // namespace irr
}; // namespace collada
}; // namespace animation_track

#endif