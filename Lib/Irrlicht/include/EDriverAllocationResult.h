//-*-c++-*-
#ifndef __E_VIDEO_DRIVER_ALLOCATION_RESULT_H_INCLUDED__
#define __E_VIDEO_DRIVER_ALLOCATION_RESULT_H_INCLUDED__

namespace irr
{
namespace video
{

//!
enum E_DRIVER_ALLOCATION_RESULT
{
	//!
	EDAR_SUCCESS = 1 << 2,

	//!
	EDAR_SUCCESS_AND_SKIP_RENDERING = (unsigned int)(EDAR_SUCCESS | 1),

	//!
	EDAR_FAILED = 1 << 3,

	//!
	EDAR_MISMATCH = EDAR_FAILED | 1,

	//!
	EDAR_NOT_FOUND = 1 << 4,

};

} // end namespace video
} // end namespace irr

#endif
