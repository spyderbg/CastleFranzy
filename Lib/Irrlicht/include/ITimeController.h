#ifndef __ITIMECONTROLLER_H__
#define __ITIMECONTROLLER_H__

#include <irrTypes.h>
#include <IReferenceCounted.h>

namespace irr
{
namespace scene
{

//! Interface that control how the time is mapped
/**
	Default implementation doesn't update the time
*/
class ITimeController
	: virtual public IReferenceCounted
{
public:
	ITimeController() : m_CtrlTime(0) {}

	//! Get a transformated time based on the current time
	virtual void update(s32 timeMs) {};

	inline s32 getCtrlTime() const { return m_CtrlTime; };

protected:
	inline void setCtrlTime(s32 timeMs) { m_CtrlTime = timeMs; }

private:
	s32 m_CtrlTime;
};

};
};

#endif