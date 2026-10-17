#ifndef _WIITEXTURECONV_H_
#define _WIITEXTURECONV_H_

#include "IrrCompileConfig.h"
#ifdef _IRR_USE_WII_DEVICE_

#include "irrTypes.h"

namespace irr
{

void ConvertRGBAtoRGBA8( u8* imgData,  u32 width, u32 height );

// Internal use
void TCPackTile_RGBA8( u8* rgba_data, u32 width, u32 height, u32 x, u32 y, u8* dstPtr );
void TCWriteTplImage_RGBA8 ( u8* rgba_data, u32 width, u32 height, u8* tplBuffer );

}

#endif //_IRR_COMPILE_WITH_WII_

#endif //_WIITEXTURECONV_H_