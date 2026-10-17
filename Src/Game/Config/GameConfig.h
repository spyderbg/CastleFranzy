///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Common/Singleton.h"

#include "Game/Config/UserProfile.h"

///////////////////////////////////////////////////////////////////////

#define __GAME_CONFIG()				( CGameConfig::GetInstance() )

///////////////////////////////////////////////////////////////////////

typedef core::map<stringw, IAttributes*> TAttributesMap;
typedef core::map<stringw, TAttributesMap*> TObjectAttrMap;

class CGameConfig : public CSingleton<CGameConfig>
{
public:
	static const bool	DisableRTT;
	static const bool	EnableCheats;
	static SColor		EmissiveColorForInteractiveObjects;
	static bool			NoWeaponCD;
	static bool			AllWeaponsSold;
	static bool			AllowDowngrades;
	static bool			Invulnerable;
	static bool			DontSpawnUnits;
	static bool			RenderToTexture;
	static bool			DrawFPS;
	static bool			UnlockAllLevels;

private:
	friend class CSingleton<CGameConfig>;
	friend class CUserProfile;

	CGameConfig();
	virtual ~CGameConfig();

public:
	bool				importFile(const c8 * fileName);

	void				clear();
	void				clear( const stringw& type );
	void				clear( const stringw& type, const stringw & id );

	IAttributes*		getAttributes(const stringw & type, const stringw & id);
	void				setAttributes(const stringw & type, const stringw & id, IAttributes * attributes);

	void				printAttributes(const stringw & type, const stringw & id);
	void				printAttributes(IAttributes * attributes);

	TObjectAttrMap&		getObjectAttrMap ();
	TAttributesMap&		getAttributesMap (const stringw& type);

private:
	bool				exportObjects(const c8 * fileName, TAttributesMap & map, const wchar_t * objectName);
	bool				importObjects(const c8 * fileName, TAttributesMap & map, const stringw& type);

	void				copyAttributes(IAttributes * dest, IAttributes * src, bool overwrite);

	TObjectAttrMap		m_objectAttributes;
};
