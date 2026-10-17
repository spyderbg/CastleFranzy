// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_UTF8_H_INCLUDED__
#define __IRR_UTF8_H_INCLUDED__

#include "irrString.h"

namespace irr
{
namespace core
{

//! Returns the unsigned value of a single UTF-8 encoded character
inline u32 getUTF8charValue(const char* c)
{
	u32 value = 0;

	u32 firstByteMask[4] = { 0x7F, 0x1F, 0x0F, 0x07};
	u32 secondaryByteMask = 0x3F;

	//decode the utf8-encoded character's value based on its length (1 to 4 bytes)
	switch (strlen(c))
	{
		case 1: 
			value |= (c[0] & firstByteMask[0]);
			break;
		case 2: 
			value |= (c[0] & firstByteMask[1])  << 6;
			value |= (c[1] & secondaryByteMask);
			break;
		case 3: 
			value |= (c[0] & firstByteMask[2])  << 12;
			value |= (c[1] & secondaryByteMask) << 6;
			value |= (c[2] & secondaryByteMask);
			break;
		case 4: 
			value |= (c[0] & firstByteMask[3])  << 18;
			value |= (c[1] & secondaryByteMask) << 12;
			value |= (c[2] & secondaryByteMask) << 6;
			value |= (c[3] & secondaryByteMask);
			break;
	}

	return value;
}

//! Returns the UTF-8 representation of a character from it's value
inline core::stringc toUTF8(const u32 value)
{
	char c[5] = {0, 0, 0, 0, 0};

	//decode the utf8-encoded character's value based on its length (1 to 4 bytes)
	if(value < 0x80)
	{
		c[0] = (value &  0x7F | 0x00);
	}
	else if (value < 0x800)
	{
		c[0] |= (value >> 6) & 0x1F | 0xC0;
		c[1] |= value & 0x3F | 0x80;
	}
	else if (value < 0x10000)
	{
		c[0] |= (value >> 12) & 0x0F | 0xE0;
		c[1] |= (value >> 6) & 0x3F | 0x80;
		c[2] |= value & 0x3F | 0x80;
	}
	else if (value < 110000)
	{
		c[0] |= (value >> 18) & 0x07 | 0xF0;
		c[1] |= (value >> 12) & 0x3F | 0x80;
		c[2] |= (value >> 6) & 0x3F | 0x80;
		c[3] |= value & 0x3F | 0x80;
	}

	return core::stringc(c);
}


//! Returns the next character's unsigned value of a UTF-8 encoded string
inline u32 iterateUTF8String(const char*& text)
{
	char c[5] = {0, 0, 0, 0, 0};

	if((*text & 0x80) == 0x00)
	{
		memcpy(c, text, 1); 
		++text;
	}
	else if((*text & 0xE0) == 0xC0)
	{
		memcpy(c, text, 2); 
		text += 2;
	}
	else if((*text & 0xF0) == 0xE0)
	{
		memcpy(c, text, 3); 
		text += 3;
	}
	else if((*text & 0xF8) == 0xF0)
	{
		memcpy(c, text, 4); 
		text += 4;
	}
	else
	{
		++text;
	}

	return getUTF8charValue(c);
}


//! Returns the number of UTF-8 encoded characters in the string
inline s32 UTF8strlen(const char* text)
{
	s32 len = 0;

	while(*text)
	{
		if((*text & 0x80) == 0x00)
		{
			++text;
		}
		else if((*text & 0xE0) == 0xC0)
		{
			text += 2;
		}
		else if((*text & 0xF0) == 0xE0)
		{
			text += 3;
		}
		else if((*text & 0xF8) == 0xF0)
		{
			text += 4;
		}
		else
		{
			++text;
			--len;
		}

		++len;
	}

	return len;
}

//! Returns the i-th character value in the UTF-8 encoded string.
// The passed i value represents the encoded character's index, not the byte index in the string.
inline u32 getUTF8ValueFromCharIndex(const char* text, s32 charIndex)
{
	char c[5] = {0, 0, 0, 0, 0};

	if (charIndex >= UTF8strlen(text))
		return 0;

	//iterate through the string untill the right character's index
	for(int i=0; i<charIndex-1; ++i)
	{
		if((*text & 0x80) == 0x00)
		{
			++text;
		}
		else if((*text & 0xE0) == 0xC0)
		{
			text += 2;
		}
		else if((*text & 0xF0) == 0xE0)
		{
			text += 3;
		}
		else if((*text & 0xF8) == 0xF0)
		{
			text += 4;
		}
		else
		{
			++text;
		}
	}
	
	//get the character's value
	if((*text & 0x80) == 0x00)
	{
		memcpy(c, text, 1); 
	}
	else if((*text & 0xE0) == 0xC0)
	{
		memcpy(c, text, 2); 
	}
	else if((*text & 0xF0) == 0xE0)
	{
		memcpy(c, text, 3); 
	}
	else if((*text & 0xF8) == 0xF0)
	{
		memcpy(c, text, 4); 
	}
	else
	{
		memcpy(c, text, 1); 
	}

	return getUTF8charValue(c);
}

//! Returns the index of the character positionned at the i-th byte in the UTF-8 encoded string.
// The passed i value represents the byte index in the string, not the encoded character's index.
inline s32 getUTF8CharIndexFromByteIndex(const char* text, s32 byteIndex)
{
	if (byteIndex >= (s32)strlen(text))
		return 0;
	
	s32 charIndex = 0;

	int i = 0;

	//iterate through the string untill the right byte's index
	while(i <= byteIndex)
	{
		if((*text & 0x80) == 0x00)
		{
			++text;
			++i;
		}
		else if((*text & 0xE0) == 0xC0)
		{
			text += 2;
			i += 2;
		}
		else if((*text & 0xF0) == 0xE0)
		{
			text += 3;
			i += 3;
		}
		else if((*text & 0xF8) == 0xF0)
		{
			text += 4;
			i += 4;
		}
		else
		{
			++text;
			++i;
		}
		++charIndex;
	}
	
	return charIndex;
}

//! Returns the index of first byte of the i-th character in the UTF-8 encoded string.
// The passed i value represents the encoded character's index in the string, not i-th byte's index.
inline s32 getByteIndexFromUTF8CharIndex(const char* text, s32 charIndex)
{
	if (charIndex >= UTF8strlen(text))
		return 0;
	
	s32 byteIndex =0;

	//iterate through the string untill the right byte's index
	for(int i=0; i<charIndex-1; ++i)
	{
		if((*text & 0x80) == 0x00)
		{
			++text;
			++byteIndex;
		}
		else if((*text & 0xE0) == 0xC0)
		{
			text += 2;
			byteIndex += 2;
		}
		else if((*text & 0xF0) == 0xE0)
		{
			text += 3;
			byteIndex += 3;
		}
		else if((*text & 0xF8) == 0xF0)
		{
			text += 4;
			byteIndex += 4;
		}
		else
		{
			++text;
			++byteIndex;
		}
	}
	
	return byteIndex+1;
}

//! Returns a UTF8 encoded string with its contents initialized to a substring of the passed UTF-8 string.
// the pos parameter represents the first character's index from where to start the substring
// the len paramater represents the number of UTF-8 encoded characters in the substring's length
inline core::stringc UTF8substr(const char* text, s32 pos, s32 len=0)
{
	s32 textLen = UTF8strlen(text);
	if (pos >= textLen)
		return core::stringc("");

	if (pos + len > textLen || len == 0)
		len = textLen - pos;

	s32 firstByteIndex = 0;
	s32 lastByteIndex = 0;

	//iterate through the string untill the right character's index
	for(int i=0; i<pos-1; ++i)
	{
		if((*text & 0x80) == 0x00)
		{
			++text;
			++firstByteIndex;
		}
		else if((*text & 0xE0) == 0xC0)
		{
			text += 2;
			firstByteIndex += 2;
		}
		else if((*text & 0xF0) == 0xE0)
		{
			text += 3;
			firstByteIndex += 3;
		}
		else if((*text & 0xF8) == 0xF0)
		{
			text += 4;
			firstByteIndex += 4;
		}
		else
		{
			++text;
			++firstByteIndex;
		}
	}

	lastByteIndex = firstByteIndex;

	//iterate through the string untill the end of the substring
	for(int i=0; i<len; ++i)
	{
		if((*text & 0x80) == 0x00)
		{
			++text;
			++lastByteIndex;
		}
		else if((*text & 0xE0) == 0xC0)
		{
			text += 2;
			lastByteIndex += 2;
		}
		else if((*text & 0xF0) == 0xE0)
		{
			text += 3;
			lastByteIndex += 3;
		}
		else if((*text & 0xF8) == 0xF0)
		{
			text += 4;
			lastByteIndex += 4;
		}
		else
		{
			++text;
			++lastByteIndex;
		}
	}

	s32 length = lastByteIndex - firstByteIndex + 1;

	core::stringc substring = text;
	return substring.subString(firstByteIndex, length);
}

//! Decrements the value of the byte index representing the position of a character to 
// the value of the previous character in the UTF-8 encoded string 
inline s32 getPrevUTF8CharIndex(const char* text, s32 byteIndex)
{
	s32 charIndex = core::getUTF8CharIndexFromByteIndex(text, byteIndex);
	return core::getByteIndexFromUTF8CharIndex(text, --charIndex);
}

//! Increments the value of the byte index representing the position of a character to 
// the value of the next character in the UTF-8 encoded string 
inline s32 getNextUTF8CharIndex(const char* text, s32 byteIndex)
{
	s32 charIndex = core::getUTF8CharIndexFromByteIndex(text, byteIndex);
	return core::getByteIndexFromUTF8CharIndex(text, ++charIndex);
}


} // end namespace core
} // end namespace irr

#endif

