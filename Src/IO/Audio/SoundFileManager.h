#ifndef IO_AUDIO_SOUNDFILEMANAGER_H
#define IO_AUDIO_SOUNDFILEMANAGER_H
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "Common/Singleton.h"

#include "AudioDefs.h"
#include "ISoundFile.h"

#include "irrArray.h"
#include "CZipReader.h"


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class CSoundFile : public ISoundFile
{
public:
	               CSoundFile(int soundID);
	virtual        ~CSoundFile();

	virtual int    read(void* buffer, int size);
	virtual void   seek(long offset, int origin); 


protected:
	irr::io::IReadFile*	m_file;
};


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
struct TSoundZipEntry
{
	irr::io::CZipReader*	pack;
	irr::core::stringc		name;
};

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
struct TSoundEntry
{
	int						type;
	int						zipIndex;
	int						fileIndex;
};


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class SoundFileManager : public CSingleton<SoundFileManager>
{
public:
	                        SoundFileManager();
	                        ~SoundFileManager();

	void                    Init();

	irr::io::CZipReader*    AddPack( const char* packName );
	void                    AddPack_Memory( const char* packName );
	void                    AddPack_Stream( const char* packName );

	irr::io::IReadFile*     OpenSoundFile( int soundID );

	int                     GetSoundIndexFromName( const char* soundName );
	const char*				GetSoundNameFromIndex(  int soundId );

	int GetNumberOfRegisteredSounds() const	            { return m_soundsDatabase.size(); }
	int	GetSoundType(int index) const                   { return m_soundsDatabase[index].type; }


protected:
	irr::core::array<TSoundZipEntry>   m_zipFileSystems;
	irr::core::array<TSoundEntry>      m_soundsDatabase;
};

#define GetSoundFileManager()	SoundFileManager::GetInstance()


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#endif //IO_AUDIO_SOUNDFILEMANAGER_H
