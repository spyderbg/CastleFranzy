#ifndef __RESFILEWRITER_H_INCLUDED__
#define __RESFILEWRITER_H_INCLUDED__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cassert>

#include "token.h"
#include "access.h"
#include <vector>
#include <list>


template<typename _key, typename _value, typename _less = std::less<_key> >
struct odict {
	odict() {
	}
	void add(const _key& key, _value& value) {
		m_vec.push_back(value);
		m_map.insert(std::pair<_key,_value>(key, value));
	}
	size_t size() const {
		return m_vec.size();
	}
	_value& operator[](const _key& key) {
		return m_map[key];
	}
	const _value& operator[](const _key& key) const {
		return m_map[key];
	}
	_value& operator[](size_t idx) {
		return m_vec[idx];
	}
	const _value& operator[](size_t idx) const {
		return m_vec[idx];
	}
private:
	std::vector<_value> m_vec;
	std::map<_key, _value, _less> m_map;
};


struct ResChunk;

struct Chunk
{
	friend struct ResChunk;
	Chunk& AddChunk(const char* name=0);
	const std::string& GetName() const;

	size_t GetSizeOfTokens() const;
	size_t GetTotalSize() const;
	size_t GetNumTokens() const;
	int GetOffset();
	bool GetOffset(Chunk* chunk, int& offset);

	ResChunk& GetResChunk();

 
	Chunk& operator[](const std::string& key);
	Chunk& operator[](size_t idx);

	Chunk& Write4u(unsigned int u4);
	Chunk& Write4s(  signed int s4);
	Chunk& Write2u(unsigned short u2);
	Chunk& Write2s(  signed short s2);
	Chunk& Write1u(unsigned char u1);
	Chunk& Write1s(  signed char s1);

	Chunk& WriteFloat(float f4);
	Chunk& WriteDouble(double f8);

	Chunk& AddAndWriteChunkOffset(const char* str=0, int offset=0);

	virtual Chunk& WriteStr(const char* str, int length = -1);
	Chunk& WriteOffsetToChunk(Chunk& chunk, int offset = 0) {
		return WriteOffsetToChunk(&chunk, offset);
	}
	Chunk& WriteOffsetToChunk(Chunk* chunk, int offset = 0);
	
	Chunk& WriteBuffer(const void* data, int length);
	Chunk& WriteRaw(const void* data, int length);
	Chunk& WriteRawExternal(const void* data, int length);
	Chunk& WriteRawAtEnd(const void* data, int length);

	Chunk& WriteBufferFromFile(const char* path);

	Chunk& WriteSizeOfTokens(Chunk* chunk = 0);
	Chunk& WriteSizeOfChunk(Chunk* chunk = 0);

	Chunk& WriteNumTokens(Chunk* chunk);

	template<typename T>
	Chunk& Write(std::vector<T> &arr, const char *pName = 0);

	template<typename T>
	Chunk& Write(std::vector<T*> &arr, const char *pName = 0);

	template<typename T>
	Chunk& Write(std::list<T> &arr, const char *pName = 0);
	
	template<typename T>
	Chunk& Write(std::list<T *> &arr, const char *pName = 0);

	template<typename T>
	Chunk& WriteExternal(T &ext, const char *pName = 0);

	template<typename T>
	Chunk& WriteExternal(std::vector<T> &ext, const char *pName = 0);

	virtual void WriteToken(const Token& t);

protected:
	Chunk(ResChunk& resfile, const char* name);
	Chunk(Chunk* parent, const char* name);

	virtual ~Chunk();

	virtual void Write(std::ostream& out, Token::Endian endian);
	
protected:
	Chunk(const Chunk&);
	Chunk& operator=(const Chunk&);

	void* operator new(size_t size);
	void operator delete(void* p);

	void AddSubChunk(Chunk* chunk);

	Chunk* m_parent;
	ResChunk& m_resChunk;

	int m_sizeofTokens;
	std::string m_name;
	std::list<Token> m_tokens;
	std::vector<std::string> m_filenames;

	odict<std::string, Chunk*> m_childs;
};

struct ResChunk : public Chunk
{
	static bool s_bOutputDebugInfo;
	ResChunk();

	void Write(std::ostream& out, Token::Endian endian);

	Token AddString(Chunk* chunk, const char* str, int length);
	void AddOffset(Chunk* chunk);

	Chunk& WriteStr(const char* str, int length=-1);

private:
	void WriteToken(const Token& t);

	Chunk* m_header;
	Chunk* m_offsets;
	Chunk* m_strings;
	Chunk* m_data;

	typedef std::map<std::string, int> MapStringToOffset;
	MapStringToOffset m_str;
};

template<>
Chunk& Chunk::Write(std::list<std::string> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<std::string> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<std::string *> &arr, const char *pName);

template<typename T>
Chunk& Chunk::Write(std::vector<T> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arr[i].Write(arrayChk);
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
}

template<typename T>
Chunk& Chunk::Write(std::vector<T*> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);
		for(unsigned int i = 0; i < arr.size(); i++)
		{
			arr[i]->Write(arrayChk);
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
}

template<typename T>
Chunk& Chunk::Write(std::list<T *> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);

		typename std::list<T *>::iterator it = arr.begin();
		for(unsigned int i = 0; i < arr.size(); i++, it++)
		{
			(*it)->Write(arrayChk);
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
};

template<typename T>
Chunk& Chunk::Write(std::list<T> &arr, const char *pName)
{
	Write4u((unsigned int)arr.size());
	if(arr.size())
	{
		Chunk &arrayChk = AddAndWriteChunkOffset(pName);

		typename std::list<T>::iterator it = arr.begin();
		for(unsigned int i = 0; i < arr.size(); i++, it++)
		{
			it->Write(arrayChk);
		}
	}
	else
	{
		Write4u(0);
	}
	return *this;
}



template<typename T>
Chunk& Chunk::WriteExternal(T &ext, const char *pName)
{
	Chunk &arrayChk = AddAndWriteChunkOffset(pName);
	ext.Write(arrayChk);
	return *this;
}

template<typename T>
Chunk& Chunk::WriteExternal(std::vector<T> &ext, const char *pName)
{
	Chunk &arrayChk = AddAndWriteChunkOffset(pName);
	arrayChk.Write(ext);
	return *this;
}

template<>
Chunk& Chunk::Write(std::vector<float> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<int> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<unsigned int> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<short> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<unsigned short> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<char> &arr, const char *pName);

template<>
Chunk& Chunk::Write(std::vector<unsigned char> &arr, const char *pName);

#endif
