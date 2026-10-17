//-*-c++-*-
#ifndef __I_VIDEO_DRIVER_BINDING_H_INCLUDED__
#define __I_VIDEO_DRIVER_BINDING_H_INCLUDED__

#include "EDriverTypes.h"

namespace irr
{
namespace video
{

//!
class IDriverBinding
{
public:

	//!
	virtual ~IDriverBinding();

	//!
	virtual E_DRIVER_TYPE getDriverType() const = 0;

	//!
	virtual void onDelete();

protected:

	//!
	IDriverBinding() {}
};

} // end namespace video
} // end namespace irr

#endif
