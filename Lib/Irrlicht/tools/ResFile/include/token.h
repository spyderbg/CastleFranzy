#ifndef __TOKEN_H_INCLUDED__
#define __TOKEN_H_INCLUDED__


#include <iosfwd>


int RoundUp(int val, int po2);

struct Chunk;

struct Token 
{
	enum Endian {
		ENDIAN_BIG,
		ENDIAN_LITTLE,
		ENDIAN_RAW,
	};

	enum Type {
		TYPE_U8,
		TYPE_S8,
		TYPE_U4,
		TYPE_S4,
		TYPE_U2,
		TYPE_S2,
		TYPE_U1,
		TYPE_S1,

		TYPE_F4,
		TYPE_F8,

		TYPE_STR,
		TYPE_OFFSET_TO_CHUNK,

		TYPE_SIZEOF_TOKENS,
		TYPE_SIZEOF_CHUNK,

		TYPE_NUM_TOKENS,

		TYPE_RAW,

		TYPE_BUFFER_FILE,
		TYPE_BUFFER_RAW,

		TYPE_COUNT,

		TYPE_FLOAT	= TYPE_F4,
		TYPE_DOUBLE	= TYPE_F8,

	} m_type;

	union {
		unsigned int	u4;
		  signed int	s4;
		unsigned short	u2;
		  signed short	s2;
		unsigned char	u1;
		  signed char	s1;
		
		float	f4;
		double	f8;

		struct {
			size_t length;
			const char* str;
		} str;
		
		struct {
			Chunk* origin;
			int offset;
		} offsetToChunk;
		
		struct {
			size_t size;
			const char* path;
		} bufferFile;

		struct {
			size_t size;
			const void* data;
		} bufferRaw;

		struct {
			size_t size;
			const void* data;
		} raw;
		
		struct {
			Chunk* chunk;
		} sizeofTokens;

		struct {
			Chunk* chunk;
		} sizeofChunk;

		struct {
			Chunk* chunk;
		} numTokens;
	};

	Token(Type t);
	~Token();

	static void FromFile(Token& token, const char* path);

	void Write(std::ostream& out, Endian endian);
	size_t GetSize() const;
};


#endif
