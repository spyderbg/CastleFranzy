
#pragma once

#include "Core/irrlicht.h"

stringw FormatNumber( int number );

// replaces any %s in format with the wchar_t* argument
// replaces any %d with the int argument
stringw FormatString (const wchar_t* format, ...);
