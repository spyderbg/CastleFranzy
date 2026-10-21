#ifndef __RESFILE_H_INCLUDED__
#define __RESFILE_H_INCLUDED__

#include "access.h"
#include "resPointer.h"
#include <string.h>

namespace irr
{
namespace res
{

template<typename T>
struct Data {
	res::Ptr<T> ptr;
	
	size_t GetSize() const {
		size_t s = *(reinterpret_cast<const std::uint32_t*>(static_cast<const T*>(ptr)) - 1);
		return s;
	}

	operator T*() { return ptr; }
	operator const T*() const { return ptr; }
};

struct String
	: public Data<char>
{
    static String Empty();
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
	std::uint32_t size;
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
	
	res::Ptr<std::uint32_t> offsets;
	res::Ptr<int> stringData;
	res::Ptr<char> data;
};

struct NodeData
{
	unsigned int size;
	unsigned int numNodes;
	
	struct ResNodeDicData {
		int ref;
		res::Ptr<const char> name;
		res::Ptr<char> ptr;
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


static_assert(sizeof(FileHeaderData) == 32, "BRES header layout changed");

struct File : public res::Access<res::FileHeaderData>
{
    File() : res::Access<res::FileHeaderData>(NULL), IsValid(false) {}
    explicit File(void* buffer) : res::Access<res::FileHeaderData>(NULL), IsValid(false) {
        if (buffer) Init(buffer);
    }
    ~File();
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    int Init(void* buffer);
    int Init(FileReader* file);
    bool isValid() const { return IsValid; }
private:
    bool IsValid;
};

}; // namespace res
}; // namespace irr

#endif
