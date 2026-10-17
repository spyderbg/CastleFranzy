// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Logs
//
//		Handles the different type of logging, and pick the proper output for each one of them
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#if 0

#define DBG_OUT( ... )			Logs::out( __VA_ARGS__ )
#define DBG_ERROR( ... )		Logs::error( __VA_ARGS__ )
#define DBG_WARNING( ... )		Logs::warning( __VA_ARGS__ )

#else

#define DBG_OUT( ... )
#define DBG_ERROR( ... )
#define DBG_WARNING( ... )

#endif

///////////////////////////////////////////////////////////////////////
/// Logs
///////////////////////////////////////////////////////////////////////

class Logs
{
public:
	static void		out( const char* fmt, ... );
	static void		error( const char* fmt, ... );
	static void		warning( const char* fmt, ... );
	static void		info(const char* msg);
	static void		debug(const char* msg);
	static void		trace(const char* header, const char* msg);

	static void		newPointer(void* ptr, const char* clsname, const char* filename, int lineNumber);

private:
	// TODO add an "InGame console", ".log file handle", etc.
	static bool		s_newSession;
};
