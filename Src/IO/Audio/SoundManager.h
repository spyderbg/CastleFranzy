#ifndef IO_AUDIO_SOUNDMANAGER_H
#define IO_AUDIO_SOUNDMANAGER_H
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "BaseSoundManager.h"

#include "Entities/IUpdatable.h"

#include "Common/Singleton.h"

#include "vector3d.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
class CSoundManager : public IUpdatable, public BaseSoundManager, public CSingleton<CSoundManager>
{
public:
	      CSoundManager();
	      ~CSoundManager();

	void  init( const char* profile_name );

	void  update(TimeStamp timestamp);

	int   playInPosition(int soundId, const irr::core::vector3df& position, bool loop = false, float volume = 1.f);
	void  updatePosition(int soundId, int soundIndex, const irr::core::vector3df& position);

	int    GetSoundIndexFromName( const char* soundName );
	const char*   GetSoundNameFromIndex( int soundId );

private:
	virtual int  getSoundType(int soundId) const;
};

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#define GetSoundManager() CSoundManager::GetInstance()


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#endif // IO_AUDIO_SOUNDMANAGER_H
