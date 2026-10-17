#include <resFileWriter.h>

#include <cassert>
#include <iostream>


void* Chunk::operator new(size_t size) {
	return ::operator new(size);
}
void Chunk::operator delete(void* p) {
	::operator delete(p);
}

Chunk::Chunk(ResChunk& resChunk, const char* name) :
	m_parent(0),
	m_name(name),
	m_resChunk(resChunk),
	m_sizeofTokens(0)
{
	//dont use resChunk or m_ResChunk here
}

Chunk::Chunk(Chunk* parent, const char* name) : 
	m_parent(parent),
	m_name(name),
	m_resChunk(m_parent->m_resChunk),
	m_sizeofTokens(0)
{
	assert(m_parent);

	m_parent->AddSubChunk(this);
}

Chunk::~Chunk()
{
	for(size_t i = 0; i < m_childs.size(); ++i)
	{
		delete m_childs[i];
	}
}

ResChunk& Chunk::GetResChunk()
{
	return m_resChunk;
}

Chunk& Chunk::operator[](const std::string& key)
{
	Chunk* chunk = m_childs[key];
	
	if(chunk)
		return *chunk;

	return AddChunk(key.c_str());
}

Chunk& Chunk::operator[](size_t idx)
{
	assert(idx < m_childs.size());

	Chunk* chunk = m_childs[idx];

	return *chunk;
}

const std::string& Chunk::GetName() const
{
	return m_name;
}

bool Chunk::GetOffset(Chunk* chunk, int& offset)
{
	if(this == chunk)
		return true;


	size_t size = GetSizeOfTokens();
	size = RoundUp(size, 4);
	offset += (int)size;

	for(size_t i = 0; i < m_childs.size(); ++i)
	{
		if(m_childs[i]->GetOffset(chunk, offset))
			return true;
	}
	
	return false;
}

int Chunk::GetOffset()
{
	int offset = 0;
	
	GetResChunk().GetOffset(this, offset);
	
	return offset;
}

void Chunk::AddSubChunk(Chunk* chunk)
{
	m_childs.add(chunk->GetName(), chunk);
}

Chunk& Chunk::AddChunk(const char* name)
{
	char nameBuffer[64];

	if(name == 0)
	{
#ifdef WIN32		
		sprintf_s(nameBuffer, sizeof(nameBuffer), "chunk#%03d", m_childs.size());
#else
		snprintf(nameBuffer, sizeof(nameBuffer), "chunk#%03d", m_childs.size());
#endif
		
		name = nameBuffer;
	}

	return *new Chunk(this, name);
}

Chunk& Chunk::AddAndWriteChunkOffset(const char* str, int offset)
{
	Chunk& c = AddChunk(str);
	WriteOffsetToChunk(c, offset);
	return c;
}

size_t Chunk::GetSizeOfTokens() const
{
	return m_sizeofTokens;
	
//	size_t size = 0;
//	
//	for(size_t i = 0; i < m_tokens.size(); ++i)
//	{
//		size += m_tokens[i].GetSize();
//	}
//	
//	return size;
}


size_t Chunk::GetTotalSize() const
{
	size_t size = GetSizeOfTokens();
	size = RoundUp(size, 4);

	for(size_t i = 0; i < m_childs.size(); ++i) 
	{
		size += m_childs[i]->GetTotalSize();
	}

	return size;
}

size_t Chunk::GetNumTokens() const
{
	return m_tokens.size();
}

void Chunk::Write(std::ostream& out, Token::Endian endian)
{

	for(std::list<Token>::iterator it = m_tokens.begin(); it != m_tokens.end(); ++it)
	{
		it->Write(out, endian);
	}	
	
	int sizeOfTokens = GetSizeOfTokens();
	int alignedSize = RoundUp(sizeOfTokens, 4);

	for(int i = 0; i < alignedSize - sizeOfTokens; ++i) 
	{
		out.write("\0", 1);
	}

	size_t mc_size = m_childs.size();
	for(size_t i = 0; i < mc_size; ++i)
	{
		m_childs[i]->Write(out, endian);
	}
}

void Chunk::WriteToken(const Token& t)
{
	m_sizeofTokens += (int)t.GetSize();

	m_tokens.push_back(t);
}

Chunk& Chunk::Write4u(unsigned int u4)
{
	Token t(Token::TYPE_U4);
	t.u4 = u4;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::Write4s(signed int s4)
{
	Token t(Token::TYPE_S4);
	t.s4 = s4;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::Write2u(unsigned short u2)
{
	Token t(Token::TYPE_U2);
	t.u2 = u2;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::Write2s(signed short s2)
{
	Token t(Token::TYPE_S2);
	t.s2 = s2;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::Write1u(unsigned char u1)
{
	Token t(Token::TYPE_U1);
	t.u1 = u1;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::Write1s(signed char s1)
{
	Token t(Token::TYPE_S1);
	t.s1 = s1;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteFloat(volatile float f4)
{
	if(f4 == -0.0f)
	{
		f4 = 0.0f;
	}
	Token t(Token::TYPE_FLOAT);
	t.f4 = f4;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteDouble(double f8)
{
	Token t(Token::TYPE_DOUBLE);
	t.f8 = f8;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteStr(const char* str, int length)
{
	if(length < 0)
	{
		length = (int)strlen(str);
	}

	Token t = GetResChunk().AddString(this, str, length);

	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteOffsetToChunk(Chunk* chunk, int offset)
{
	GetResChunk().AddOffset(this);

	Token t(Token::TYPE_OFFSET_TO_CHUNK);
	t.offsetToChunk.origin = chunk;
	t.offsetToChunk.offset = offset;

	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteBufferFromFile(const char* path)
{
	m_filenames.push_back(path);
	path = m_filenames[m_filenames.size() - 1].c_str();

	Token t(Token::TYPE_BUFFER_FILE);
	Token::FromFile(t, path);

	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteBuffer(const void* data, int length)
{
	Token t(Token::TYPE_BUFFER_RAW);
	t.bufferRaw.size = length;
	t.bufferRaw.data = data;

	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteRaw(const void* data, int length)
{
	Token t(Token::TYPE_RAW);
	t.raw.size = length;
	t.raw.data = data;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteRawExternal(const void* data, int length)
{
	Chunk& atEnd = GetResChunk().AddChunk();
	atEnd.Write4u(length);
	atEnd.WriteRaw(data, length);
	WriteOffsetToChunk(atEnd, 4);

	return *this;
}

Chunk& Chunk::WriteRawAtEnd(const void* data, int length)
{
	Chunk& atEnd = AddChunk();
	atEnd.WriteRaw(data, length);
	WriteOffsetToChunk(atEnd);

	return *this;
}


Chunk& Chunk::WriteSizeOfTokens(Chunk* chunk)
{
	Token t(Token::TYPE_SIZEOF_TOKENS);
	if(chunk == 0)
		chunk = this;
	t.sizeofTokens.chunk = chunk;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteSizeOfChunk(Chunk* chunk)
{
	Token t(Token::TYPE_SIZEOF_CHUNK);
	if(chunk == 0)
		chunk = this;
	t.sizeofChunk.chunk = chunk;
	WriteToken(t);

	return *this;
}

Chunk& Chunk::WriteNumTokens(Chunk* chunk)
{
	Token t(Token::TYPE_NUM_TOKENS);
	if(chunk == 0)
		chunk = this;
	t.numTokens.chunk = chunk;
	WriteToken(t);

	return *this;
}

template<>
Chunk& Chunk::Write(std::list<std::string> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);

		std::list<std::string>::iterator it = arr.begin();
		for(unsigned int i = 0; i < arr.size(); i++, it++)
		{
			arrayChk.WriteStr((*it).c_str());
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
}

template<>
Chunk& Chunk::Write(std::vector<std::string> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);

		std::vector<std::string>::iterator it = arr.begin();
		for(unsigned int i = 0; i < arr.size(); i++, it++)
		{
			arrayChk.WriteStr((*it).c_str());
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
}

template<>
Chunk& Chunk::Write(std::vector<std::string *> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);

		std::vector<std::string *>::iterator it = arr.begin();
		for(unsigned int i = 0; i < arr.size(); i++, it++)
		{
			arrayChk.WriteStr((*it)->c_str());
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<float> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.WriteFloat(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<int> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.Write4s(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<unsigned int> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.Write4u(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<short> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.Write2s(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<unsigned short> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.Write2u(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<char> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.Write1u(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

template <>
Chunk& Chunk::Write(std::vector<unsigned char> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());

	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arrayChk.Write1u(arr[i]);
		}
	}
	else
	{
		Write4u(0);
	}

	return *this;
}

#pragma warning(disable:4355)

bool ResChunk::s_bOutputDebugInfo = false;

ResChunk::ResChunk() :
	Chunk(*this, "__resfile__")
{
	m_header	= &AddChunk("__header__");
	m_offsets	= &AddChunk("__offsets__");
	m_strings	= &AddChunk("__strings__");
	m_data		= &AddChunk("__data__");
	
	m_header->WriteRaw("BRES", 4);
	
	m_header->Write2u(0xfffe);
	m_header->Write2u(0x0000);

	m_header->WriteSizeOfTokens();
	m_header->WriteSizeOfChunk(this);
	
	m_header->WriteNumTokens(m_offsets);
	m_header->WriteOffsetToChunk(m_offsets);
	m_header->WriteOffsetToChunk(m_strings);
	m_header->WriteOffsetToChunk(m_data);
}

#include <iostream>
using namespace std;
void ResChunk::Write(std::ostream& out, Token::Endian endian)
{


	for(std::list<Token>::iterator it = m_tokens.begin(); it != m_tokens.end(); ++it)
	{
		it->Write(out, endian);
	}
	
	int sizeOfTokens = GetSizeOfTokens();
	int alignedSize = RoundUp(sizeOfTokens, 4);

	for(int i = 0; i < alignedSize - sizeOfTokens; ++i) 
	{
		out.write('\0', 1);
	}

	size_t mc_size = m_childs.size();
	for(size_t i = 0; i < mc_size; ++i)
	{
		std::ostream::pos_type pos = out.tellp();
		m_childs[i]->Write(out, endian);
		if(s_bOutputDebugInfo)
		{
			std::ostream::pos_type size = out.tellp() - pos;
			cout << m_childs[i]->GetName().c_str() << " - " << size << endl;
		}
	}
}

void ResChunk::WriteToken(const Token& t)
{
	m_data->WriteToken(t);
}

Chunk& ResChunk::WriteStr(const char* str, int length)
{
	if(length < 0)
	{
		length = (int)strlen(str);
	}

	Token t = GetResChunk().AddString(m_data, str, length);

	WriteToken(t);

	return *this;
}

Token ResChunk::AddString(Chunk* chunk, const char* str, int length)
{
	Token offset(Token::TYPE_OFFSET_TO_CHUNK);

	// Check in the token string pool to see if a token already exist for this string
	MapStringToOffset::iterator iter = GetResChunk().m_str.find(str);
	if( iter == GetResChunk().m_str.end() )
	{
		offset.offsetToChunk.origin = m_strings;
		offset.offsetToChunk.offset = (int)m_strings->GetSizeOfTokens() + 4;
		GetResChunk().m_str[str] = offset.offsetToChunk.offset;
		
		Token tokenString(Token::TYPE_STR);
		tokenString.str.str = str;
		tokenString.str.length = length;
		m_strings->WriteToken(tokenString);
	}
	else
	{
		offset.offsetToChunk.origin = m_strings;
		offset.offsetToChunk.offset = (*iter).second;
	}

	AddOffset(chunk);

	return offset;
}

void ResChunk::AddOffset(Chunk* chunk)
{
	Token t(Token::TYPE_OFFSET_TO_CHUNK);
	
	//if writing offset to ResChunk, use m_data as ref chunk
	if(chunk == this)
		chunk = m_data;

	t.offsetToChunk.origin = chunk;
	t.offsetToChunk.offset = RoundUp((int)chunk->GetSizeOfTokens(), 4);

	m_offsets->WriteToken(t);
}




void Write(Chunk &chk, const int &data)
{
	chk.Write4s(data);
};

void Write(Chunk &chk, const short &data)
{
	chk.Write2s(data);
};
void Write(Chunk &chk, const char &data)
{
	chk.Write1s(data);
};
void Write(Chunk &chk, const unsigned int &data)
{
	chk.Write4u(data);
};
void Write(Chunk &chk, const unsigned short &data)
{
	chk.Write2u(data);
};
void Write(Chunk &chk, const unsigned char &data)
{
	chk.Write1u(data);
};
void Write(Chunk &chk, const char *&data)
{
	chk.WriteStr(data);
};
void Write(Chunk &chk, const void *&data, int iLength)
{
	chk.WriteRawExternal(data, iLength);
};
