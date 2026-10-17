//-*-c++-*-
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_CORE_UTIL_H_INCLUDED__
#define __IRR_CORE_UTIL_H_INCLUDED__

#include "irrString.h"

namespace irr
{
namespace core
{

/*! \file irrxml.h
	\brief File containing useful basic utility functions
*/

// ----------- some basic quite often used string functions -----------------

//! cut the filename extension from a string
inline stringc& cutFilenameExtension ( stringc &dest, const stringc &source )
{
	s32 endPos = source.findLast ( '.' );
	dest = source.subString ( 0, endPos < 0 ? source.size () : endPos );
	return dest;
}

//! get the filename extension from a string
inline stringc& getFileNameExtension ( stringc &dest, const stringc &source )
{
	s32 endPos = source.findLast ( '.' );
	if ( endPos < 0 )
		dest = "";
	else
		dest = source.subString ( endPos, source.size () );
	return dest;
}

//! some standard function ( to remove dependencies )
#undef isdigit
#undef isspace
#undef isupper
inline s32 isdigit(s32 c) { return c >= '0' && c <= '9'; }
inline s32 isspace(s32 c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
inline s32 isupper(s32 c) { return c >= 'A' && c <= 'Z'; }

// ----------- some pointer arithmetic and block size utilities ----------------

//! Utility to add a stride defined in bytes to a pointer of arbitrary type.
template < typename T >
inline T* stepPointer(T* ptr, int stride)
{
	return reinterpret_cast<T*>(reinterpret_cast<u8*>(ptr) + stride);
}

//! Utility to add a stride defined in bytes to a pointer of arbitrary type.
/** \overload */
template < typename T >
inline const T* stepPointer(const T* ptr, int stride)
{
	return reinterpret_cast<const T*>(reinterpret_cast<const u8*>(ptr) + stride);
}

//! Utility to round up values to the next multiple of BlockSize.
/** Example: u32 s = roundup<32>(35); // returns 64 */
template < unsigned int BlockSize >
inline u32 roundup(u32 s)
{
	return ((s + BlockSize - 1) / BlockSize) * BlockSize;
}

//! Dynamic version of roundup<unsigned int>(u32).
inline u32 roundup(u32 s, u32 blockSize)
{
	return ((s + blockSize - 1) / blockSize) * blockSize;
}

//! Utility to round up block size s to the next multiple of sizeof(T).
/** Example: u32 s = roundup<u16>(35); // returns 36 */
template < typename T >
inline u32 roundup(u32 s)
{
	return roundup<sizeof(T)>(s);
}

// ----------- array copying with strides --------------------------------------

template < typename T >
inline T* copyArray(T* dest, u32 destStride,
					const T* src, u32 srcStride,
					u32 count)
{
	while (count-- > 0)
	{
		*dest = *src;
		dest = stepPointer(dest, destStride);
		src = stepPointer(src, srcStride);
	}
	return dest;
}

// ----------- hash functions --------------------------------------------------

inline u32 hashBytes(const void* ptr, u32 seed, u32 len)
{
	u32 hash = seed;
	const u8* bytes = reinterpret_cast<const u8*>(ptr);
	for (const u8* endBytes = bytes + len; bytes != endBytes; ++bytes)
	{
		hash = (hash * 13) + *bytes;
	}
	return hash;
}

// ----------- varia -----------------------------------------------------------

template < typename T >
inline void swap(T& lhs, T& rhs)
{
	T tmp(lhs);
	lhs = rhs;
	rhs = tmp;
}

} // end namespace core
} // end namespace irr

#endif

