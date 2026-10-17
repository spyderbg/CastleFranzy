//-*-c++-*-
#ifndef __E_MATERIAL_FUNCS_H_INCLUDED__
#define __E_MATERIAL_FUNCS_H_INCLUDED__

namespace irr
{
namespace video
{

//! Enumerant for ROP comparison functions (e.g. for z-buffer).
enum E_COMPARE_FUNC
{
	// ECFN_ instead of ECF_ to avoid a clash with color format enums.

	ECFN_NEVER = 0,
	ECFN_LESS,
	ECFN_EQUAL,
	ECFN_LEQUAL,
	ECFN_GREATER,
	ECFN_NOTEQUAL,
	ECFN_GEQUAL,
	ECFN_ALWAYS
};

static const char* CompareFuncNames[] =
{
	"Never",
	"Less",
	"Equal",
	"Less or Equal",
	"Greater",
	"Not Equal",
	"Greater or Equal",
	"Always",
	0
};

} // end namespace video
} // end namespace irr

#endif
