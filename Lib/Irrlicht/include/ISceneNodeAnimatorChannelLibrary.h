#ifndef __ISCENENODEANIMATORCHANNELLIBRARY_H__
#define __ISCENENODEANIMATORCHANNELLIBRARY_H__

#include "ISceneNodeAnimator.h"

namespace irr
{
namespace scene
{

class ISceneNodeAnimatorChannelLibrary
	: public ISceneNodeAnimator
{
public:
	virtual void BindChannel(int iChannelID, void *pPtr) = 0;
	virtual s32 getChannelAnimatorsCount() const = 0;
};

}; // namespace scene
}; // namespace irr

#endif