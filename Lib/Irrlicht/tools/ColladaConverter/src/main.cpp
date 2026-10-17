#include <resFile.h>
#include <resFileWriter.h>
#include <token.h>

#ifdef WIN32
//for _getcwd
#include <direct.h>

#include <cctype> // for toupper
#include <string>
#include <algorithm>

#endif

#include "util.h"

#define INPUT_COMMAND "-i"
#define OUTPUT_COMMAND "-o"
#define DEBUG_COMMAND "-d"
#define HELP_COMMAND "?"
#define BIGENDIAN_COMMAND "-big_endian"
#define LITTLEENDIAN_COMMAND "-little_endian"
#define IPHONE_COMMAND "-iphone"
#define DEPENDENCIES_COMMAND "-dep"
#define HEADER_FILE_COMMAND "-h_file"

enum E_TARGET
{
	IPHONE,
	WII,
	PS3,
	XBOX360
};

bool g_bTarget = 0;
bool g_bDebug = false;

#include "SDataArray.h"
#include "CReadFile.h"
#include "CXMLReaderImpl.h"
#include "IReferenceCounted.h"
#include "IXMLReader.h"
#include "CXMLReader.h"


#include "CDatabaseCollada.h"

using namespace irr;
using namespace collada;
using namespace converter;


#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <time.h>
//#include <conio.h>
using namespace std;

bool g_bWriteDependancies = false;
ostream *g_pDependancies = 0;

void EchoDependencies(const char* filename)
{
	if(!g_pDependancies)
		return;
	*g_pDependancies << filename << endl;
}

void WriteToFile(const char* filename, const void* data, size_t length)
{
	fstream f(filename, ios_base::out | ios_base::binary);
	f.write(static_cast<const char*>(data), length);
	f.close();
}

#ifndef COLLADA_CONVERTER_STATIC_LIB
void
WriteUserProperties(Chunk &chk, std::string &userProperties)
{
	chk.WriteStr(userProperties.c_str());	
}
#endif

void
Help()
{
	cout << "==================================================================" << endl;
	cout << "- Collada Converter - Arguments list" << endl;
	cout << "- [-i %INPUT_FILE_PATH%] - Required" << endl;
	cout << "- [-o %OUTPUT_FILE_PATH%] - Optional" << endl;
	cout << "- [-d] - Optional - Debug info" << endl;
	cout << "- [-dep] - Optional - output a file %i.dep that list the dependancies" << endl;
	cout << "- [-big_endian] - Optional" << endl;
	cout << "- [-h_file %PREFIX%	- Optional] " <<endl;
	cout << "- ex. " << endl;
	cout << "-  ColladaConverter_2008.exe -d -i \"X:\\MyData\\City.dae\"" << endl;
	cout << "==================================================================" << endl;	
 	exit(0);
};

int
main(int argc, const char **argv)
{
	const char *inputFile = GetCommandLineArg(argc,argv,INPUT_COMMAND);
	const char *outputFile = GetCommandLineArg(argc,argv,OUTPUT_COMMAND);
	bool bDebug = HasCommandLineArg(argc,argv,DEBUG_COMMAND);
	bool bHelp = HasCommandLineArg(argc,argv,HELP_COMMAND);
	bool bBigEndian = HasCommandLineArg(argc,argv,BIGENDIAN_COMMAND);
	bool bDependancies = HasCommandLineArg(argc,argv,DEPENDENCIES_COMMAND);
	const char *prefix = GetCommandLineArg(argc,argv,HEADER_FILE_COMMAND);

	long inputFileSize;
	long ouputFileSize;

	if(bHelp || argc == 1)
	{
		Help();
	}

	std::string outfilename;
	
	if(inputFile == 0)
	{
		cout << "Invalid arguments" << endl;
		exit(-1);
	}

	if(outputFile == 0)
	{
		outfilename = inputFile;
		outfilename = outfilename.substr(0, outfilename.find(".dae"));
		outfilename.append(".bdae");
		outputFile = outfilename.c_str();
	}

	if(inputFile == 0 || outputFile == 0)
	{
		cout << "Invalid arguments" << endl;
		exit(-1);
	}
	

	if(bDebug)
	{
		cout << setiosflags( ios::fixed );
		cout << "==================================================================" << endl;
		cout << "- Collada Converter" << endl;
		cout << "- [(i)nput file]\t" << inputFile << endl;
		cout << "- [(o)utput file]\t" << outputFile << endl;
		cout << "==================================================================" << endl;

		
	}
	// output library size
	ResChunk::s_bOutputDebugInfo = bDebug;

	std::string path;
	std::string filename;

	static char startWorkingDirectory[1024];
#ifdef WIN32
	_getcwd(startWorkingDirectory, 1024);
#else
	getcwd(startWorkingDirectory, 1024);
#endif
	
	path = inputFile;
	int i = path.rfind("\\");
	if(i == std::string::npos)
	{
		i = path.rfind("/");
	}
	if(i != std::string::npos)
	{
		path = path.substr(0, i + 1);
	}

	filename = inputFile;
	i = filename.rfind("\\");
	if(i == std::string::npos)
	{
		i = path.rfind("/");
	}
	if(i != std::string::npos)
	{
		filename = filename.substr(i + 1);
	}

#ifdef WIN32	
	_chdir(path.c_str());
#else
	chdir(path.c_str());
#endif

	io::CReadFile* file = new irr::io::CReadFile(filename.c_str());
	if (!file->isOpen())
	{
		cout << "**** Error: Input file not found ****" << endl;
		return EXIT_FAILURE;
	}

	if(bDependancies)
	{
		g_pDependancies = new std::fstream((filename + ".dep").c_str(), std::ios_base::out);
	}


	inputFileSize = file->getSize();

	clock_t beginTime = clock();
	io::IXMLReaderUTF8* reader = createIXMLReaderUTF8(file);

	CCollada *pColladaDB = new CCollada();
	

	pColladaDB->Read(reader);
	clock_t beginPackage = clock();
	if(bDebug)
	{
		cout << "Reading time  : " << setw(10) << setprecision(3) << (double)(beginPackage - beginTime)*1000.0/CLOCKS_PER_SEC << " ms" << endl;
	}

	
	
	ResChunk resfile;
	pColladaDB->Write(resfile);

	clock_t beginWrite = clock();
	if(bDebug)
	{
		cout << "Packaging time: " << setw(10) << setprecision(3) << (double)(beginWrite - beginPackage)*1000.0/CLOCKS_PER_SEC << " ms" << endl;
	}

#ifdef WIN32	
	_chdir(startWorkingDirectory);
#else
	chdir(startWorkingDirectory);
#endif
	
	std::list<CAnimationClip> list = pColladaDB->libraryAnimationClips.animationClips;	
	std::list<CAnimationClip>::iterator it = list.begin();
	//Must have at least 2 animations 
	if (prefix && list.size() >= 2)
	{
		std::ostringstream ostr ;          	
		int counter = 0;

		ostr <<  std::endl;
		
		std::string h = std::string(prefix);
		h.append("_");
		h.append(filename.substr(0, filename.find(".dae")));		
		std::transform(h.begin(), h.end(), h.begin(), (int(*)(int)) std::toupper );

		for(; it != list.end(); ++it)
		{
			std::string n = std::string(h.c_str());
			n.append("_");
			n.append((*it).id);		
			std::transform(n.begin(), n.end(), n.begin(), (int(*)(int)) std::toupper );

			ostr << std::left << "#define "  << std::setw(20) << n << "		" <<std::setw(20)<<  counter <<  endl ;
			counter ++ ;
		}
	
		std::string s = ostr.str();		

		h.append(".h");

		WriteToFile(h.c_str(), s.c_str(), s.size());
	}


	std::ostringstream out_le(std::ios_base::out | std::ios_base::binary);

	if(bBigEndian)
	{
		resfile.Write(out_le, Token::ENDIAN_BIG);
	}
	else
	{
		resfile.Write(out_le, Token::ENDIAN_LITTLE);
	}
	std::string sLe = out_le.str();
	WriteToFile(outputFile, sLe.c_str(), sLe.size());
	
	ouputFileSize = sLe.size();

	clock_t endWrite = clock();
	if(bDebug)
	{
		cout << "Writing time  : " << setw(10) << setprecision(3) << (double)(endWrite - beginWrite)*1000.0/CLOCKS_PER_SEC << " ms" << endl;
		cout << "Size In(" << inputFileSize << ")->Out(" << ouputFileSize << ")" << endl;
		cout << "Rest : " << ((double)ouputFileSize / (double)inputFileSize * 100.0) << "%" << endl;
	}

	if(g_pDependancies)
	{
		delete g_pDependancies;
		g_pDependancies = 0;
	}

	return EXIT_SUCCESS;
}



