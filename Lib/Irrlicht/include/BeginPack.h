#include "IrrCompileConfig.h"

#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#	pragma warning(disable:4103)
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#elif defined(__MWERKS__)
#	pragma pack(1)
#	define PACK_STRUCT
#else
#	error compiler not supported
#endif