#include "stdafx.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace scene
{

IMeshBuffer::~IMeshBuffer()
{
	setDriverBinding(NULL);
}

void
IMeshBuffer::onDelete() 
{
	if (DriverBinding)
	{
		DriverBinding->onDelete();
	}
}

u32
IMeshBuffer::getVertexIndexStart() const
{
	// A default return value of 0 is always safe
	return 0;
}

u32
IMeshBuffer::getVertexIndexEnd() const
{
	// A default return value of the total number of vertices is always safe
	return getVertexCount();
}

} // end namespace scene
} // end namespace irr
