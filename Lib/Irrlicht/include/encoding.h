// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_ENCODING_H_INCLUDED__
#define __IRR_ENCODING_H_INCLUDED__

#include "irrString.h"

namespace irr
{
namespace core
{

	
enum Encoding
{
	ENCODING_ANSI,	//character values are represented by 1 byte and range from 0 to 255
	ENCODING_UTF8,	//character values are encoded using UTF-8 standard using up to 4 bytes and range from 0 to 0x10FFFF
	
	ENCODING_COUNT
};

// Sets the character encoding format
IRRLICHT_API void setCharacterEncoding(Encoding encoding);

// Returns the character encoding format
IRRLICHT_API Encoding getCharacterEncoding();

//! Returns the unsigned value of a single encoded character
IRRLICHT_API u32 getCharValue(const c8* c);

//! Encodes a character value into the encoding format that is set
IRRLICHT_API core::stringc encodeChar(const u32 value);

//! Returns the next character's unsigned value of an encoded string
IRRLICHT_API u32 iterateString(const c8*& text);

//! Returns the number of encoded characters in the string
IRRLICHT_API s32 encodedStrlen(const c8* text);

//! Returns the i-th character value in the encoded string.
// The passed i value represents the encoded character's index, not the byte index in the string.
IRRLICHT_API u32 getValueFromCharIndex(const c8* text, s32 charIndex);

//! Returns the index of the character positionned at the i-th byte in the encoded string.
// The passed i value represents the byte index in the string, not the encoded character's index.
IRRLICHT_API s32 getCharIndexFromByteIndex(const c8* text, s32 byteIndex);

//! Returns the index of first byte of the i-th character in the encoded string.
// The passed i value represents the encoded character's index in the string, not i-th byte's index.
IRRLICHT_API s32 getByteIndexFromCharIndex(const c8* text, s32 charIndex);

//! Returns a  encoded string with its contents initialized to a substring of the passed string.
// the pos parameter represents the first character's index from where to start the substring
// the len paramater represents the number of encoded characters in the substring's length
IRRLICHT_API core::stringc encodedSubstr(const c8* text, s32 pos, s32 len=0);

//! Decrements the value of the byte index representing the position of a character to 
// the value of the previous character in the encoded string 
IRRLICHT_API s32 getPrevCharIndex(const c8* text, s32 byteIndex);

//! Increments the value of the byte index representing the position of a character to 
// the value of the next character in the encoded string 
IRRLICHT_API s32 getNextCharIndex(const c8* text, s32 byteIndex);


} // end namespace core
} // end namespace irr

#endif

