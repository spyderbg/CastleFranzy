#ifndef __IRR_PORTABILITY_ALIASES_H_INCLUDED__
#define __IRR_PORTABILITY_ALIASES_H_INCLUDED__

#include "IrrCompileConfig.h"
#if defined(_IRR_POSIX_API_)
#include <strings.h>
#endif

namespace irr
{

//
// //<string.h> 's stricmp is not std C, and guess what, OSX doesnt define it...
//

inline int stricmp( const char* str1, const char* str2 )
{
	#if defined(_IRR_OSX_PLATFORM_) || defined(_IRR_POSIX_API_)
		return ::strcasecmp( str1, str2 );
	#elif defined(_IRR_PS3_PLATFORM_)
		return ::strcasecmp( str1, str2 );
	#elif defined(_IRR_WINDOWS_API_)
		return ::_stricmp(str1, str2);
	#else
		//defaults to stricmp
		return ::stricmp(str1, str2);
	#endif

}


};

#endif
