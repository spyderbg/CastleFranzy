///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Common/Singleton.h"

///////////////////////////////////////////////////////////////////////

#define MAX_PROFILE_NUM				10

////////////////////////////////////////////////////////
/// CUserProfile
////////////////////////////////////////////////////////

typedef core::map<stringw, irr::s32> TBuffOffset;

class CUserProfile
{
	friend class CUserProfileManager;

public:
	CUserProfile( const CUserProfile& profile );
	virtual	~CUserProfile();

	CUserProfile&	operator=( CUserProfile& profile ); // not works for now

	/// Reset to default
			void			ResetToDefault();

	/// Getters
	const	stringc&		GetFileName() const;
	const	stringw&		GetName() const;

	/// Setters
			void			SetFileName( const stringc& file_name );

	/// Active method
	const	bool			IsActive() const;
			void			SetAsActive() const;

	/// Fields methods
			bool			Get( stringc var, irr::s32 & f ) { return profileField( var, f, false ); }
			bool			Set( stringc var, irr::s32 f )	{ return profileField( var, f, true ); }

			bool			Get( stringc var, irr::f32 & f ) { return profileField( var, f, false ); }
			bool			Set( stringc var, irr::f32 f )	{ return profileField( var, f, true ); }

			bool			Get( stringc var, stringc & f )	{ return profileField( var, f, false ); }
			bool			Set( stringc var, stringc f )	{ return profileField( var, f, true ); }

protected:
	void	Release();

	bool	Load( const stringc& fileName );
	bool	Save( bool defaultProfile = false );

private:
	CUserProfile( const stringw& name );
public:
	CUserProfile( const stringw& name, const stringc& fileName );
private:

	void	CopyAllFields( const CUserProfile& profile );

	/// Load help methods
	bool	LoadFile( const stringc& name );

	void	DefProfile_Sync( bool overwrite );
	bool	DefProfile_LoadOrCreate();
	bool	DefProfile_Save();
	bool	DefProfile_AddNewAttribute( const stringc& var ); 

	bool	IsAcceptableFileName( const stringc& fileName );

	/// Fields methods
	bool	profileField( stringc var, irr::s32 & f, bool set );
	bool	profileField( stringc var, irr::f32 & f, bool set );
	bool	profileField( stringc var, stringc & f,  bool set );

private:
	stringc		m_FileName;
	stringw		m_Name;

	/// Buffers
private:
	typedef struct
	{
		irr::s32 offset;
		irr::s32 length;
	} TBuff;

	TBuffOffset			m_BufferList;
	core::array<TBuff>	m_BufferOffsets;
	
	irr::u8*					m_Buffer;
};


////////////////////////////////////////////////////////
/// CUserProfileManager
////////////////////////////////////////////////////////

class CUserProfileManager : public CSingleton<CUserProfileManager>
{
	friend class CSingleton<CUserProfileManager>;

public:
	const	CUserProfile	GetCreateProfile( const stringw& name );
	const	CUserProfile	GetCreateProfile( const stringw& name, const stringc& fileName );

			void			Init();
private:
	CUserProfileManager();
	virtual ~CUserProfileManager();

	bool		IsActiveProfileNameValid();

private:
	stringw		m_ActiveProfileName;
};
