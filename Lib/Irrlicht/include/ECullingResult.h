//-*-c++-*-
#ifndef __E_SCENE_CULLING_RESULT_H_INCLUDED__
#define __E_SCENE_CULLING_RESULT_H_INCLUDED__

namespace irr
{
namespace scene
{

//!
enum E_CULLING_RESULT
{
	//!
	ECR_OUTSIDE = 0,

	//!
	ECR_INSIDE,

	//!
	ECR_INTERSECT,
};

} // end namespace scene
} // end namespace irr

#endif
