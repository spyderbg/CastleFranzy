///////////////////////////////////////////////////////////////////////
/// Logs.cpp
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Core/Logs.h"

#include <stdio.h>

///////////////////////////////////////////////////////////////////////

#define  CRT_SECURE_NO_WARNINGS 

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/// Logs
///////////////////////////////////////////////////////////////////////

bool Logs::s_newSession = true;

void Logs::error( const char* fmt, ... )
{
#ifdef OS_WIN32
	
	OutputDebugStringA("\nERROR: \n");

	char str[1024];
	va_list arg_list;

	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	va_end(arg_list);

	printf("%s", str);
	OutputDebugStringA(str);

	OutputDebugStringA("\n");
	
#endif	
	
#if defined(OS_IPHONE) || defined(OS_LINUX)

	printf("\nERROR: \n");
	
	char str[1024];
	va_list arg_list;
	
	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	va_end(arg_list);
	
	printf("%s", str);
	
#endif
}

void Logs::warning( const char* fmt, ... )
{
#ifdef OS_WIN32
	
	OutputDebugStringA("WARNING: ");

	char str[1024];
	va_list arg_list;

	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	va_end(arg_list);

	printf("%s", str);
	OutputDebugStringA(str);

	OutputDebugStringA("\n");
	
#endif
	
#if defined(OS_IPHONE) || defined(OS_LINUX)
	
	printf("WARNING: ");
	
	char str[1024];
	va_list arg_list;
	
	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	va_end(arg_list);
	
	printf("%s", str);
	
#endif	
}

void Logs::info(const char* msg)
{
	printf("%s\n", msg);
}

void Logs::out( const char* fmt, ... )
{
#ifdef OS_WIN32
	
	char str[1024];
	va_list arg_list;

	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	va_end(arg_list);

	printf("%s", str);
	OutputDebugStringA(str);
	
#endif	

#if defined(OS_IPHONE) || defined(OS_LINUX)
	
	char str[1024];
	va_list arg_list;
	
	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	va_end(arg_list);
	
	printf("%s", str);
	
#endif	
}

void Logs::debug(const char* msg)
{
#if _DEBUG
	printf("DEBUG: %s\n", msg);
#endif
}

void Logs::trace(const char* header, const char* msg)
{
#if _DEBUG
	printf("TRACE: <%s> %s\n", header, msg);
#endif
}


void Logs::newPointer(void* ptr, const char* clsname, const char* filename, int lineNumber)
{
#if defined(WIN32)
	FILE* f;
	fopen_s(&f, "__new.log", s_newSession ? "w" : "a");
	fprintf(f, " @ %08x : %s (%s:%d)\n", ptr, clsname, filename, lineNumber);
	fclose(f);

	s_newSession = false;
#endif
}

