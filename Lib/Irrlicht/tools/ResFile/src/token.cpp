#include <token.h>

#include <resFileWriter.h>

#include <ostream>
#include <fstream>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#else
#endif

void Write(std::ostream& out, Token::Endian endian, const void* p, size_t size)
{
	const std::ostream::char_type* ptr = static_cast<const std::ostream::char_type*>(p);
	
	if(endian == Token::ENDIAN_RAW)
		out.write(ptr, size);
	else if(endian == Token::ENDIAN_LITTLE)
		out.write(ptr, size);
	else if(endian == Token::ENDIAN_BIG)
	{
		for(size_t i = 0; i < size; ++i)
			out.write(&ptr[size - i - 1], 1);
	}
}

int RoundUp(int val, int po2)
{
	return (val + (po2-1)) & ~(po2-1);
}

Token::Token(Token::Type type) :
	m_type(type)
{
}

Token::~Token()
{
}

void Token::FromFile(Token& token, const char* path)
{
	token.m_type = TYPE_BUFFER_FILE;
	token.bufferFile.path = path;
#ifdef WIN32
	WIN32_FILE_ATTRIBUTE_DATA attributes;
	int succeed = GetFileAttributesEx(path, GetFileExInfoStandard, &attributes);
	assert(succeed);
	token.bufferFile.size = attributes.nFileSizeLow;

#else
	assert(false);
#endif
	
}

size_t Token::GetSize() const
{
	if(m_type == TYPE_F8 || m_type == TYPE_U8 || m_type == TYPE_S8)
		return 8;
	else if(m_type == TYPE_S2 || m_type == TYPE_U2)
		return 2;
	else if(m_type == TYPE_S1 || m_type == TYPE_U1)
		return 1;
	else if(m_type == TYPE_BUFFER_FILE)
		return 4 + RoundUp(bufferFile.size, 4);
	else if(m_type == TYPE_RAW)
		return RoundUp(raw.size, 4);
	else if(m_type == TYPE_STR)
		return 4 + RoundUp(str.length + 1, 4);
	else //if(m_type == TYPE_U4 || m_type == TYPE_S4 || m_type == TYPE_FLOAT || m_type == TYPE_OFFSET)
		return 4;
}

void Token::Write(std::ostream& out, Endian endian)
{
	switch(m_type) {
		case TYPE_S4: ::Write(out, endian, &s4, sizeof(s4)); break;
		case TYPE_S2: ::Write(out, endian, &s2, sizeof(s2)); break;
		case TYPE_S1: ::Write(out, endian, &s1, sizeof(s1)); break;

		case TYPE_U4: ::Write(out, endian, &u4, sizeof(u4)); break;
		case TYPE_U2: ::Write(out, endian, &u2, sizeof(u2)); break;
		case TYPE_U1: ::Write(out, endian, &u1, sizeof(u1)); break;

		case TYPE_F4: ::Write(out, endian, &f4, sizeof(f4)); break;
		case TYPE_F8: ::Write(out, endian, &f8, sizeof(f8)); break;

		case TYPE_RAW: {
			//::Write(out, endian, &raw.size, sizeof(raw.size)); 
			::Write(out, ENDIAN_RAW, raw.data, raw.size);
			int toalign = RoundUp(raw.size, 4) - raw.size;
			int pad = 0;
			::Write(out, endian, &pad, toalign);
		} break;
		case TYPE_BUFFER_FILE: {
			::Write(out, endian, &bufferFile.size, sizeof(bufferFile.size));

			std::fstream f(bufferFile.path, std::ios::in | std::ios::binary);

			char* p = (char*)malloc(bufferFile.size);
			f.read(p, bufferFile.size);
			f.close();
			::Write(out, ENDIAN_RAW, p, bufferFile.size);
			free(p);

			int toalign = RoundUp(bufferFile.size + 1, 4) - bufferFile.size;
			int pad = 0;
			::Write(out, endian, &pad, toalign);
		} break;
		case TYPE_BUFFER_RAW: {
			::Write(out, endian, &bufferRaw.size, sizeof(bufferRaw.size));
			::Write(out, ENDIAN_RAW, bufferRaw.data, bufferRaw.size);
			int toalign = RoundUp(bufferRaw.size + 1, 4) - bufferRaw.size;
			int pad = 0;
			::Write(out, endian, &pad, toalign);
		} break;
		case TYPE_STR: {
			::Write(out, endian, &str.length, sizeof(str.length)); 
			::Write(out, ENDIAN_RAW, str.str, str.length); 
			int toalign = RoundUp(str.length + 1, 4) - str.length;
			int pad = 0;
			::Write(out, endian, &pad, toalign);
		} break;

		case TYPE_OFFSET_TO_CHUNK: {
			int offset = offsetToChunk.origin->GetOffset();
			offset += offsetToChunk.offset;
			::Write(out, endian, &offset, sizeof(offset));
		} break;
		case TYPE_SIZEOF_CHUNK: {
			size_t size = sizeofChunk.chunk->GetTotalSize();
			::Write(out, endian, &size, sizeof(size));
		} break;
		case TYPE_SIZEOF_TOKENS: {
			size_t size = sizeofTokens.chunk->GetSizeOfTokens();
			::Write(out, endian, &size, sizeof(size));
		} break;
		case TYPE_NUM_TOKENS: {
			size_t num = numTokens.chunk->GetNumTokens();
			::Write(out, endian, &num, sizeof(num));
		} break;
		default:
			assert(false);
	}
}
