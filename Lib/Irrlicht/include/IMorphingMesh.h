// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MORPHING_MESH_H_INCLUDED__
#define __I_MORPHING_MESH_H_INCLUDED__

#include "IAnimatedMesh.h"

namespace irr
{
namespace scene
{

	enum E_MORPHING_METHOD
	{
		EMM_NORMALIZED = 0,
		EMM_RELATIVE,
		EMM_COUNT
	};


	//! Interface for using some special functions of Skinned meshes
	class IMorphingMesh : public IAnimatedMesh
	{
	public:
		IMorphingMesh(E_MORPHING_METHOD method = EMM_NORMALIZED) : m_Method(method) {}



	protected:
		E_MORPHING_METHOD m_Method;
	};

} // end namespace scene
} // end namespace irr

#endif

