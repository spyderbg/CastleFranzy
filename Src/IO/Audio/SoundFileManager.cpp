//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "IO/Audio/SoundManager.h"
#include "IO/Audio/SoundFileManager.h"

#include "IrrlichtDevice.h"
#include "IFileSystem.h"
#include "IReadFile.h"
#include "irrString.h"

using namespace gameloft;
using namespace irr;
using namespace core;
using namespace io;

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

SoundFileManager::SoundFileManager()
{
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
SoundFileManager::~SoundFileManager()
{
	m_soundsDatabase.clear();

	for (int i = 0; i < m_zipFileSystems.size(); ++i)
	{
		m_zipFileSystems[i].pack->drop();
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CZipReader* SoundFileManager::AddPack( const char* packName )
{
	CZipReader* zr = 0;

	IReadFile* file = __IRR_FILE_SYSTEM()->createAndOpenFile(packName);
	if (file)
	{
		zr = new CZipReader(file, false, true);
		if (zr)
		{
			TSoundZipEntry sze;
			sze.pack = zr;
			sze.name = stringc(packName);

			m_zipFileSystems.push_back(sze);
		}

		file->drop();
	}

	return zr;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void SoundFileManager::AddPack_Memory( const char* packName )
{
	CZipReader* zr = AddPack(packName);

	int zipIndex = m_zipFileSystems.size() - 1;

	int count = zr->getFileCount();
	for(int i = 0; i < zr->getFileCount(); i++)
	{
		TSoundEntry se;
		se.type      =  (audio::k_typeMemory | audio::k_typePreloadData | audio::k_typeKeepDataLoaded);
		se.type      |=	audio::k_typeWAV;

		se.zipIndex  =  zipIndex;
		se.fileIndex =  i;

		m_soundsDatabase.push_back(se);
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void SoundFileManager::AddPack_Stream( const char* packName )
{
	CZipReader* zr = AddPack(packName);

	int zipIndex = m_zipFileSystems.size() - 1;

	int count = zr->getFileCount();
	for(int i = 0; i < zr->getFileCount(); i++)
	{
		TSoundEntry se;
		se.type      =  audio::k_typeStreamedFile;
		se.type      |= audio::k_typeWAV;

		se.zipIndex  =  zipIndex;
		se.fileIndex =  i;

		if(strncmp("m_", zr->getFileInfo(i)->simpleFileName.c_str(), 2) == 0)
		{
			se.type |= audio::k_typeMusic;
		}

		m_soundsDatabase.push_back(se);
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IReadFile* SoundFileManager::OpenSoundFile( int soundID )
{
	IReadFile* file = 0;

	// assert streams are not compressed
	const TSoundEntry& se = m_soundsDatabase[soundID];

	IReadFile* file1  = createReadFile(m_zipFileSystems[se.zipIndex].name.c_str());

	file1->seek(m_zipFileSystems[se.zipIndex].pack->getFileInfo(se.fileIndex)->fileDataPosition);

	IReadFile* file2 = createLimitReadFile("", file1, m_zipFileSystems[se.zipIndex].pack->getFileInfo(se.fileIndex)->header.DataDescriptor.UncompressedSize);
	file1->drop();

	return file2;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void SoundFileManager::Init()
{
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int SoundFileManager::GetSoundIndexFromName( const char* soundName )
{
	const int numSounds = m_soundsDatabase.size();

	for(int i = 0; i < numSounds; i++)
	{
		const TSoundEntry&	se		=	m_soundsDatabase[i];
		const stringc&		name	=	m_zipFileSystems[se.zipIndex].pack->getFileInfo(se.fileIndex)->simpleFileName;

		if(strcmp(name.c_str(), soundName) == 0)
		{
			return i;
		}
	}

	return -1;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

const char* SoundFileManager::GetSoundNameFromIndex(  int soundId )
{
	if(soundId < 0 || soundId >= m_soundsDatabase.size())
		return 0;

	const TSoundEntry&	se		=	m_soundsDatabase[soundId];
	const stringc&		name	=	m_zipFileSystems[se.zipIndex].pack->getFileInfo(se.fileIndex)->simpleFileName;

	return name.c_str();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundFile::CSoundFile(int soundID) : ISoundFile()
{
	m_file = GetSoundFileManager()->OpenSoundFile(soundID);
	GX_ASSERT(m_file);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
CSoundFile::~CSoundFile()
{
	if(m_file)
	{
		m_file->drop();
		m_file = 0;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int CSoundFile::read(void* buffer, int size)
{
	return m_file->read(buffer, size);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void CSoundFile::seek(long offset, int origin)
{
	m_file->seek(offset, origin == SEEK_CUR);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
ISoundFile* CreateSoundFile(int soundID)
{
	return new CSoundFile(soundID);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------



