#ifndef __RESFILE_H_INCLUDED__
#define __RESFILE_H_INCLUDED__

#include "access.h"
#include <string.h>

namespace irr
{
namespace res
{

template<typename T>
struct Data {
	T* ptr;
	
	size_t GetSize() const {
		size_t s = *(reinterpret_cast<const size_t*>(ptr) - 1);
		return s;
	}

	operator T*() { return ptr; }
	operator const T*() const { return ptr; }
};

struct String
	: public Data<char>
{
	bool
	operator ==(const String &str2) const
	{
		return (strcmp(ptr, str2.ptr) == 0);
	}

	bool
	operator ==(const char *str2) const
	{
		return (strcmp(ptr, str2) == 0);
	}
};

template<typename T>
struct Buffer {
	size_t size;
	T data[1];

	T& operator[](int idx) {
		return data[idx];
	}
	const T& operator[](int idx) const {
		return data[idx];
	}
};

struct FileHeaderData {
	unsigned int signature;
	unsigned short endianCheck;
	unsigned short version;
	unsigned int sizeOfHeader;
	unsigned int sizeOfFile;
	unsigned int numOffsets;
	
	res::Access<res::Access<res::Access<int> > > offsets;
	res::Access<int> stringData;
	res::Access<char> data;
};

struct NodeData
{
	unsigned int size;
	unsigned int numNodes;
	
	struct ResNodeDicData {
		int ref;
		res::Access<const char> name;
		res::Access<char> ptr;
	} data[1];
};

struct Node : public res::Access<res::NodeData>
{
	unsigned int GetNumNodes() const;
	res::Node GetResNode(const char* name);
};

class FileReader {
public:
	FileReader(){}
	virtual ~FileReader(){}

	virtual int read( void* destination, unsigned int size ) = 0;
	virtual long seek( long pos, bool relativeMovement = false ) = 0;
	virtual long getSize() = 0;
};


struct File : public res::Access<res::FileHeaderData>
{
	File():
		IsValid(false),
		OffsetTable(NULL)
	{

	}

	File(void* ptr, void* offsetTable = NULL) : 
		res::Access<res::FileHeaderData>(ptr),
		IsValid(false),
		OffsetTable(offsetTable)
	{
		if(ptr)
		{
			IsValid = (Init() == 0);
		}
	}

	int Init(void *ptr)
	{
		*this = File(ptr,NULL);
		return IsValid != 1;
	}

	int Init(FileReader *pFile);

	int Init();

	bool isValid() const { return IsValid; }
protected:
	bool IsValid;
	void *OffsetTable;
};

}; // namespace res
}; // namespace irr

#endif