#include "stdafx.h"
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "encoding.h"

namespace irr
{
namespace core
{

// Variable determining the encoding format of characters
// default value is ENCODING_ANSI standard
static Encoding CharacterEncoding = ENCODING_ANSI;

// Sets the character encoding format
void setCharacterEncoding(Encoding encoding)
{
	_IRR_DEBUG_BREAK_IF (encoding != ENCODING_ANSI && encoding != ENCODING_UTF8)
	CharacterEncoding = encoding;
}

// Returns the character encoding format
Encoding getCharacterEncoding()
{
	return CharacterEncoding;
}

//! Returns the unsigned value of a single encoded character
u32 getCharValue(const c8* c)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	if(CharacterEncoding == ENCODING_ANSI)
		return (u32)(*c);

	if (CharacterEncoding == ENCODING_UTF8)
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

	return 0;
}

//! Returns the encoded representation of a character from it's value
core::stringc encodeChar(const u32 value)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	char c[5] = {0, 0, 0, 0, 0};

	if(CharacterEncoding == ENCODING_ANSI)
	{
		c[0] = value;
		return core::stringc(c);
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
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

	return core::stringc("");
}


//! Returns the next character's unsigned value of an encoded string
u32 iterateString(const c8*& text)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	char c[5] = {0, 0, 0, 0, 0};

	if(CharacterEncoding == ENCODING_ANSI)
	{
		memcpy(c, text, 1);
		++text;
		return (u32)c[0];
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
		//determine the number of bytes representing the utf8-encoded character based on its first bits
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
			//ignore invalid characters
			++text;
		}

		return getCharValue(c);
	}

	return 0;
}


//! Returns the number of encoded characters in the string
s32 encodedStrlen(const c8* text)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	if(CharacterEncoding == ENCODING_ANSI)
	{
		return (s32)strlen(text);
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
		s32 len = 0;
		
		//iterate each character in the string
		while(*text)
		{
			//determine the number of bytes representing the utf8-encoded character based on its first bits
			if((*text & 0x80) == 0x00)
			{
				++text;
				++len;
			}
			else if((*text & 0xE0) == 0xC0)
			{
				text += 2;
				++len;
			}
			else if((*text & 0xF0) == 0xE0)
			{
				text += 3;
				++len;
			}
			else if((*text & 0xF8) == 0xF0)
			{
				text += 4;
				++len;
			}
			else
			{
				//ignore invalid characters
				++text;
			}

		}

		return len;
	}
	
	return 0;
}

//! Returns the i-th character value in the encoded string.
// The passed i value represents the encoded character's index, not the byte index in the string.
u32 getValueFromCharIndex(const c8* text, s32 charIndex)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	char c[5] = {0, 0, 0, 0, 0};

	if (charIndex >= encodedStrlen(text))
		return 0;

	if(CharacterEncoding == ENCODING_ANSI)
	{
		memcpy(c, &text[charIndex], 1);
		return (u32)c[0];
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{

		//iterate through the string untill the right character's index
		for(int i=0; i<charIndex; ++i)
		{
			//determine the number of bytes representing the utf8-encoded character based on its first bits
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
				//ignore invalid characters
				++text;
				--i;
			}
		}
		
		//we are now at the first byte of the characer we want
		//determine the number of bytes representing the utf8-encoded character based on its first bits
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

		//return the character's value 
		return getCharValue(c);
	}

	return 0;
}

//! Returns the index of the character positionned at the i-th byte in the encoded string.
// The passed i value represents the byte index in the string, not the encoded character's index.
s32 getCharIndexFromByteIndex(const c8* text, s32 byteIndex)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	if (byteIndex >= (s32)strlen(text))
		return -1;

	if(CharacterEncoding == ENCODING_ANSI)
	{
		return byteIndex;
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
	
		s32 charIndex = 0;

		int i = 0;

		//iterate through the string untill the right byte's index
		while(i < byteIndex)
		{
			//determine the number of bytes representing the utf8-encoded character based on its first bits
			if((*text & 0x80) == 0x00)
			{
				++text;
				++i;
				++charIndex;
			}
			else if((*text & 0xE0) == 0xC0)
			{
				text += 2;
				i += 2;
				++charIndex;
			}
			else if((*text & 0xF0) == 0xE0)
			{
				text += 3;
				i += 3;
				++charIndex;
			}
			else if((*text & 0xF8) == 0xF0)
			{
				text += 4;
				i += 4;
				++charIndex;
			}
			else
			{
				//ignore invalid characters
				++text;
				++i;
			}
			
		}
		
		return charIndex;
	}

	return -1;
}

//! Returns the index of first byte of the i-th character in the encoded string.
// The passed i value represents the encoded character's index in the string, not i-th byte's index.
s32 getByteIndexFromCharIndex(const c8* text, s32 charIndex)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	if (charIndex >= encodedStrlen(text))
		return -1;
	
	if(CharacterEncoding == ENCODING_ANSI)
	{
		return charIndex;
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
		s32 byteIndex =0;

		//iterate through the string untill the right byte's index
		for(int i=0; i<charIndex; ++i)
		{
			//determine the number of bytes representing the utf8-encoded character based on its first bits
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
				--i;
			}
		}
		
		return byteIndex;
	}

	return -1;
}

//! Returns a encoded string with its contents initialized to a substring of the passed string.
// the pos parameter represents the first character's index from where to start the substring
// the len paramater represents the number of UTF-8 encoded characters in the substring's length
core::stringc encodedSubstr(const c8* text, s32 pos, s32 len)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	s32 textLen = encodedStrlen(text);
	if (pos >= textLen)
		return core::stringc("");

	if (pos + len > textLen || len == 0)
		len = textLen - pos;

	if(CharacterEncoding == ENCODING_ANSI)
	{
		return irr::core::stringc(text).subString(pos, len);
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
		irr::core::stringc substring(text);

		s32 firstByteIndex = 0;
		s32 lastByteIndex = 0;

		//iterate through the string untill the beginning character's index
		for(int i=0; i<pos; ++i)
		{
			//determine the number of bytes representing the utf8-encoded character based on its first bits
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
				--i;
			}
		}

		lastByteIndex = firstByteIndex;

		//iterate through the string untill the desired length of the substring
		for(int i=0; i<len; ++i)
		{
			//determine the number of bytes representing the utf8-encoded character based on its first bits
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
				--i;
			}
		}

		s32 length = lastByteIndex - firstByteIndex;

		return substring.subString(firstByteIndex, length);
	}

	return core::stringc("");
}

//! Decrements the value of the byte index representing the position of a character to 
// the value of the previous character in the encoded string 
s32 getPrevCharIndex(const c8* text, s32 byteIndex)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	if (byteIndex >= (s32)strlen(text))
		return -1;

	if(CharacterEncoding == ENCODING_ANSI)
	{
		//if it's the first character of the string
		if(byteIndex == 0)
			return 0;

		return --byteIndex;
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
		s32 charIndex = core::getCharIndexFromByteIndex(text, byteIndex);

		//if it's the first character of the string
		if(charIndex == 0)
			return 0;

		return core::getByteIndexFromCharIndex(text, --charIndex);
	}

	return -1;
}

//! Increments the value of the byte index representing the position of a character to 
// the value of the next character in the encoded string 
s32 getNextCharIndex(const c8* text, s32 byteIndex)
{
	_IRR_DEBUG_BREAK_IF (CharacterEncoding != ENCODING_ANSI && CharacterEncoding != ENCODING_UTF8);

	if (byteIndex >= (s32)strlen(text))
		return -1;

	if(CharacterEncoding == ENCODING_ANSI)
	{
		if(byteIndex == strlen(text)-1)
			return byteIndex;

		return ++byteIndex;
	}

	if (CharacterEncoding == ENCODING_UTF8)
	{
		s32 charIndex = core::getCharIndexFromByteIndex(text, byteIndex);

		//if it's the last character of the string
		if(charIndex == encodedStrlen(text)-1)
			return byteIndex;

		return core::getByteIndexFromCharIndex(text, ++charIndex);
	}

	return -1;
}


} // end namespace core
} // end namespace irr