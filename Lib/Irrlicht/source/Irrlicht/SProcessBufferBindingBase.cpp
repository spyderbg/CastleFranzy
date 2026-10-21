#include <cstdint>
#include "stdafx.h"
#include "SProcessBufferBindingBase.h"

namespace irr
{
namespace video
{
namespace detail
{

u32
getStrides(u32 attributes,
		   S3DVertexComponentArrays& components)
{
	_IRR_DEBUG_BREAK_IF(attributes == 0);
	u32 stride = 0;
	switch (attributes)
	{
		case EVA_POSITION :
		{
			stride = components.PositionStride = sizeof(core::vector3df);
		}
		break;

		case EVA_POSITION | EVA_NORMAL :
		{
			stride
				= components.PositionStride
				= components.NormalStride = 2 * sizeof(core::vector3df);
		}
		break;

		default:
		{
			if (attributes & EVA_POSITION)
			{
				stride += sizeof(core::vector3df);
			}
			if (attributes & EVA_NORMAL)
			{
				components.Normal = reinterpret_cast<core::vector3df*>(stride);
				stride += sizeof(core::vector3df);
			}
			if (attributes & EVA_COLOR0)
			{
				components.Color0 = reinterpret_cast<SColor*>(stride);
				stride += sizeof(SColor);
			}
			u32 texMask = attributes & EVA_TEXCOORDS_MASK;
			for (u32 i = 0; texMask; ++i)
			{
				u32 unitMask = video::EVA_TEXCOORD0 << i;
				texMask &= ~unitMask;
				if (attributes & unitMask)
				{
					components.TexCoord[i].Coord
						= reinterpret_cast<core::vector2df*>(stride);
					stride += sizeof(core::vector2df);
				}
			}
		}
	}
	return stride;
}

void
assignPointers(u8* buffer,
			   u32 stride,
			   u32 attributes,
			   S3DVertexComponentArrays& components)
{
	_IRR_DEBUG_BREAK_IF(attributes == 0);
	switch (attributes)
	{
		case EVA_POSITION :
		{
			components.Position = reinterpret_cast<core::vector3df*>(buffer);
			_IRR_DEBUG_BREAK_IF(components.PositionStride
								!= sizeof(core::vector3df));
		}
		break;

		case EVA_POSITION | EVA_NORMAL :
		{
			components.Position = reinterpret_cast<core::vector3df*>(buffer);
			components.Normal = core::stepPointer(
				reinterpret_cast<core::vector3df*>(buffer),
				sizeof(core::vector3df)
			);
			_IRR_DEBUG_BREAK_IF((components.PositionStride
								 != 2 * sizeof(core::vector3df))
								|| (components.NormalStride
									!= 2 * sizeof(core::vector3df)));
		}
		break;

		default :
		{
			if (attributes & EVA_POSITION)
			{
				components.Position = reinterpret_cast<core::vector3df*>(buffer);
				components.PositionStride = stride;
			}
			if (attributes & EVA_NORMAL)
			{
				components.Normal
					= core::stepPointer(reinterpret_cast<core::vector3df*>(buffer),
										reinterpret_cast<std::uintptr_t>(components.Normal));
				components.NormalStride = stride;
			}
			if (attributes & EVA_COLOR0)
			{
				components.Color0
					= core::stepPointer(reinterpret_cast<SColor*>(buffer),
										reinterpret_cast<std::uintptr_t>(components.Color0));
				components.Color0Stride = stride;
			}
			u32 texMask = attributes & EVA_TEXCOORDS_MASK;
			for (u32 i = 0; texMask; ++i)
			{
				u32 unitMask = video::EVA_TEXCOORD0 << i;
				texMask &= ~unitMask;
				if (attributes & unitMask)
				{
					components.TexCoord[i].Coord
						= core::stepPointer(
							reinterpret_cast<core::vector2df*>(buffer),
							reinterpret_cast<std::uintptr_t>(components.TexCoord[i].Coord)
						);
					components.TexCoord[i].Stride = stride;
				}
			}
		}
	}
}

} // end namespace detail
} // end namespace video
} // end namespace irr
