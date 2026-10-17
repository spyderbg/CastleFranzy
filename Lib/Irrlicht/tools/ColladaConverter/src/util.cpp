#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "irrString.h"

const char *
GetCommandLineArg(int argc, const char **argv, const char *arg)
{
	for(int i = 0; i < argc; i++)
	{
		if(0 == strcmp(argv[i], arg))
		{
			if(i + 1 < argc)
			{
				return argv[i + 1];
			}
		}
	}
	return 0;
}

bool
HasCommandLineArg(int argc, const char **argv, const char *arg)
{
	for(int i = 0; i < argc; i++)
	{
		if(0 == strcmp(argv[i], arg))
		{
			if(i + 1 < argc)
			{
				return true;
			}
		}
	}
	return false;
}

//! places pointer to next begin of a token
void findNextWhiteSpace(const char** start)
{
	const char* p = *start;

	while(*p && (*p!=' ' && *p!='\n' && *p!='\r' && *p!='\t'))
		++p;

	// TODO: skip comments <!-- -->

	*start = p;
}

//! places pointer to next begin of a token
void findNextNoneWhiteSpace(const char** start)
{
	const char* p = *start;

	while(*p && (*p==' ' || *p=='\n' || *p=='\r' || *p=='\t'))
		++p;

	// TODO: skip comments <!-- -->

	*start = p;
}

void WriteURL(Chunk &chk, std::string &url)
{
	std::string filename;
	std::string suburl;

	if(url.find('#') != 0)
	{
		chk.WriteStr(url.c_str(), url.find('#'));
		chk.WriteStr(url.c_str() + url.find('#'));
	}
	else
	{
		chk.Write4u(0);
		chk.WriteStr(url.c_str());
	}
};

void ChangeExtension(std::string &url)
{
	std::string newString;
	if(url.find(".dae#") != std::string::npos)
	{
		newString = url.substr(0, url.find(".dae#"));
		newString.append(".bdae");
		newString.append(url.substr(url.find("#")));
		url = newString;
	}
}

//! reads floats from inside of xml element until end of xml element
void readFloatsInsideElement(irr::io::IXMLReaderUTF8* reader, float* floats, unsigned int count)
{
	if (count == 0 || reader->isEmptyElement())
		return;

	while(reader->read())
	{
		// TODO: check for comments inside the element
		// and ignore them.

		if (reader->getNodeType() == irr::io::EXN_TEXT)
		{
			// parse float data
			irr::core::stringc data = reader->getNodeData();
			data.trim();
			const char* p = &data[0];

			for (unsigned int i=0; i<count; ++i)
			{
				findNextNoneWhiteSpace(&p);
				if (*p)
					floats[i] = readFloat(&p);
				else
					floats[i] = 0.0f;
			}
		}
		else
			if (reader->getNodeType() == irr::io::EXN_ELEMENT_END)
				break; // end parsing text
	}
}

//! reads a list of Name or ID element and store it in outArray
void readStringArray(const char* data, std::vector<std::string*> &outArray, int iCount)
{
	outArray.clear();
	

	const std::string str(data);
	int beginPos = 0;
	int nextPos = str.find_first_of(' ');
	int i = 0;

	while(nextPos != -1 && iCount-- > 0)
	{
		std::string *pNew = new std::string(str.substr(beginPos, nextPos - beginPos));
		outArray.push_back(pNew);
		beginPos = nextPos + 1;
		nextPos = str.find_first_of(' ', beginPos);
	}

	std::string *pNew = new std::string(str.substr(beginPos, str.size() - beginPos));
	outArray.push_back(pNew);

}

void readStringArray(const char* data, std::vector<std::string*> &outArray)
{
	outArray.clear();
	

	const std::string str(data);
	int beginPos = 0;
	int nextPos = str.find_first_of(' ');
	int i = 0;

	while(nextPos != -1)
	{
		std::string *pNew = new std::string(str.substr(beginPos, nextPos - beginPos));
		outArray.push_back(pNew);
		beginPos = nextPos + 1;
		nextPos = str.find_first_of(' ', beginPos);
	}

	std::string *pNew = new std::string(str.substr(beginPos, str.size() - beginPos));
	outArray.push_back(pNew);

}