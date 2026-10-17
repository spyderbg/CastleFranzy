//-*-c++-*-
#ifndef __E_VIDEO_DRIVER_OPTION_H_INCLUDED__
#define __E_VIDEO_DRIVER_OPTION_H_INCLUDED__

namespace irr
{
namespace video
{

enum E_VIDEO_DRIVER_OPTION
{
	//! Forces the driver to create 16 bit textures always, independent of which
	//! format the file on disk has. When choosing this you may loose some color
	//! detail, but gain much speed and memory. 16 bit textures can be
	//! transferred twice as fast as 32 bit textures and only use half of the
	//! space in memory.  When using this flag, it does not make sense to use
	//! the flags EVDO_CREATE_TEXTURE_ALWAYS_32_BIT,
	//! EVDO_CREATE_TEXTURE_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY, or
	//! EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED at the same time.
	EVDO_CREATE_TEXTURE_ALWAYS_16_BIT = 1,

	//! Forces the driver to create 32 bit textures always, independent of which
	//! format the file on disk has. Please note that some drivers (like the
	//! software device) will ignore this, because they are only able to create
	//! and use 16 bit textures.  When using this flag, it does not make sense
	//! to use the flags EVDO_CREATE_TEXTURE_ALWAYS_16_BIT,
	//! EVDO_CREATE_TEXTURE_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY, or
	//! EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED at the same time.
	EVDO_CREATE_TEXTURE_ALWAYS_32_BIT = EVDO_CREATE_TEXTURE_ALWAYS_16_BIT << 1,

	//! Lets the driver decide in which format the textures are created and
	//! tries to make the textures look as good as possible.  Usually it simply
	//! chooses the format in which the texture was stored on disk.  When using
	//! this flag, it does not make sense to use the flags
	//! EVDO_CREATE_TEXTURE_ALWAYS_16_BIT, EVDO_CREATE_TEXTURE_ALWAYS_32_BIT, or
	//! EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED at the same time.
	EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_QUALITY = EVDO_CREATE_TEXTURE_ALWAYS_32_BIT << 1,

	//! Lets the driver decide in which format the textures are created and
	//! tries to create them maximizing render speed.  When using this flag, it
	//! does not make sense to use the flags EVDO_CREATE_TEXTURE_ALWAYS_16_BIT,
	//! EVDO_CREATE_TEXTURE_ALWAYS_32_BIT, or
	//! EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_QUALITY, at the same time.
	EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED = EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_QUALITY << 1,

	//! Only used by the engine internally. This is not a flag but a bit mask
	//! for all texture creation flags
	EVDO_CREATE_TEXTURE_FLAGS = (unsigned int)(((EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED << 1) - 1)
											   & ~((EVDO_CREATE_TEXTURE_ALWAYS_16_BIT << 1) - 1)),

	//! Automatically creates mip map levels for the textures.
	EVDO_CREATE_TEXTURE_MIPMAPS = EVDO_CREATE_TEXTURE_OPTIMIZED_FOR_SPEED << 1,

	//! Discard any alpha layer and use non-alpha color format.
	EVDO_CREATE_TEXTURE_NO_ALPHA = EVDO_CREATE_TEXTURE_MIPMAPS << 1,

	//! Only used by the engine internally.
	/** Sets the global material flag AllowZWriteOnTransparent.
		Use the SceneManager attribute to set this value from your app.
		Default behavior is to disable ZWrite, i.e. not set. */
	EVDO_ALLOW_ZWRITE_ON_TRANSPARENT = EVDO_CREATE_TEXTURE_NO_ALPHA << 1,

	// Until we have a proper driver hierarchy, the following two flags are
	// present here, eventhough it has no effect for some drivers

	//!
	EVDO_BATCHING = EVDO_ALLOW_ZWRITE_ON_TRANSPARENT << 1,

	//!
	EVDO_RETAIN_BATCH_ON_MISS = EVDO_BATCHING << 1,

	//! This values is the last bit mask used by flags common flags in video
	//! drivers. The range of flags can be extended for specific drivers by
	//! starting their bit mask at EVDO_LAST_OPTION << 1.
	EVDO_LAST_OPTION = EVDO_RETAIN_BATCH_ON_MISS
};

} // end namespace video
} // end namespace irr

#endif
