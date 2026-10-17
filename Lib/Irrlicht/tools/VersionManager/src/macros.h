#pragma once

#define SAFE_DELETE( p ){ if ( p ){ delete p; p = NULL; } }
#define SAFE_DELETE_ARRAY( p ){ if ( p ){ delete [] p; p = NULL; } }
#define GET_VALUE_SHIFTED_AND_MASKED( value, shift, mask )( ( value >> shift ) & mask )

#define ASSERT( cond, fallback ) \
if ( !( cond ))  \
{ \
	fallback; \
}