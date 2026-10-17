#include "IrrCompileConfig.h"

#if defined(_MSC_VER) ||  defined(__BORLANDC__) || defined (__BCPLUSPLUS__) 
#	pragma pack( pop, packing )
#elif defined(__MWERKS__) 
#	pragma pack(0)
#endif

#undef PACK_STRUCT