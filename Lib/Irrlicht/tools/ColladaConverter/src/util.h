#ifndef __COLLADA_CONVERTER_UTIL_H__
#define __COLLADA_CONVERTER_UTIL_H__

#include "IXMLReader.h"
#include <vector>
#include <resFileWriter.h>
#include "irrString.h"

using namespace irr;

const char *
GetCommandLineArg(int argc, const char **argv, const char *arg);

bool
HasCommandLineArg(int argc, const char **argv, const char *arg);



template < typename T >
int findEntryNumber(T &vect, const char *id)
{
	for(int i = 0; i < vect.size(); i++)
	{
		if(vect[i]->GetID() == id)
		{
			return i;
		}
	}

	return -1;
}

//! places pointer to next begin of a token
void findNextWhiteSpace(const char** start);

//! places pointer to next begin of a token
void findNextNoneWhiteSpace(const char** start);

//! parses a float from a char pointer and moves the pointer
//! to the end of the parsed float
inline float readFloat(const char** p)
{
	float ftmp;
	ftmp = (float)atof(*p);
	findNextWhiteSpace(p);
	return ftmp;
}

void readStringArray(const char* data, std::vector<std::string*> &outArray);

void readStringArray(const char* data, std::vector<std::string*> &outArray, int iCount);

void readFloatsInsideElement(irr::io::IXMLReaderUTF8* reader, float* floats, unsigned int count);

//! parses an int from a char pointer and moves the pointer to
//! the end of the parsed float
inline int readInt(const char** p)
{
	return (int)readFloat(p);
}

//! reads ints from inside of xml element until end of xml element
template <typename T>
void readIntsInsideElement(irr::io::IXMLReaderUTF8* reader, T* ints, unsigned int count)
{
	if (count == 0 || reader->isEmptyElement())
		return;

	while(reader->read())
	{
		// TODO: check for comments inside the element
		// and ignore them.

		if (reader->getNodeType() == io::EXN_TEXT)
		{
			// parse float data
			core::stringc data = reader->getNodeData();
			data.trim();
			const c8* p = &data[0];

			for (u32 i=0; i<count; ++i)
			{
				findNextNoneWhiteSpace(&p);
				if (*p)
					ints[i] = readInt(&p);
				else
					ints[i] = 0;
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
			break; // end parsing text
	}
}
//! reads ints from inside of xml element until end of xml element
template <typename T>
void readIntsInsideElement(irr::io::IXMLReaderUTF8* reader, std::vector<T> &ints)
{
	if (reader->isEmptyElement())
		return;

	while(reader->read())
	{
		// TODO: check for comments inside the element
		// and ignore them.

		if (reader->getNodeType() == io::EXN_TEXT)
		{
			// parse float data
			core::stringc data = reader->getNodeData();
			data.trim();
			const c8* p = &data[0];

			for (;*p;)
			{
				findNextNoneWhiteSpace(&p);
				if (*p)
					ints.push_back(readInt(&p));
			}
		}
		else
		if (reader->getNodeType() == io::EXN_ELEMENT_END)
			break; // end parsing text
	}
}

void WriteURL(Chunk &chk, std::string &url);

void ChangeExtension(std::string &url);

#endif //__COLLADA_CONVERTER_UTIL_H__
