/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _WARNING_MANAGER_H_
#define _WARNING_MANAGER_H_

class ColladaOptions;
class FCDEntity;
class FCDGeometry;
class FCDGeometryMesh;
class FCDMorphTarget;
class FCDSceneNode;
class INode;
class morphChannel;
class Point3;

#define MISSING_TARGET_TEST 0
#define TARGET_DIFFERENCES_TEST 1

#define LOG( __warningManager, __warningId, __argCount, ... ) \
{ \
	if ( (__warningManager).IsWarningEnabled( __warningId ) ) \
	{ \
		ICWarning *__pWarning = (__warningManager).GetWarningById( __warningId ); \
		if ( __pWarning->m_pLogCallback ) \
		{ \
			__pWarning->m_pLogCallback( __argCount, __VA_ARGS__ ); \
		} \
	} \
}

namespace ICWarningFlags
{
	enum
	{
		ENABLED            = 0x00000001
		, COMPLY_CONVERTER = 0x00000002
	};	
};

class ICWarning
{
public:
	enum
	{
		NEGATIVE_SCALE
		, INCOMPATIBLE_MORPH_TARGET
		, INVALID_GEOMETRY
		, INVALID_SKIN
	};

public:
	int m_id;
	fstring m_name;
	fstring m_secondaryName; // drop this ASAP
	int m_idc;
	int m_flags;
	void (*m_pLogCallback)( void *param, ... );

public:
	ICWarning( int id = -1, fstring name = fstring(), fstring secondaryName = fstring(), int idc = -1, int flags = 0, void (*pLogCallback)( void *param, ... ) = NULL )
		: m_id( id )
		, m_name( name )
		, m_secondaryName( secondaryName )
		, m_idc( idc )
		, m_flags( flags )
		, m_pLogCallback( pLogCallback )
	{}
};

class ICWarningManager
{
private:
	fm::vector<ICWarning> m_warnings;

	static void Log_WarnOnNegativeScale( void *param, ... );
	static void Log_WarnOnIncompatibleMorphTarget( void *param, ... );
	static void Log_WarnOnInvalidGeometry( void *param, ... );
	static void Log_WarnOnInvalidSkin( void *param, ... );

public:
	ICWarningManager();
	~ICWarningManager();

	void Reset();

	inline int GetWarningCount() const { return m_warnings.size(); }
	inline ICWarning *GetWarning( size_t index ){ return &m_warnings[index]; }

	ICWarning *GetWarningById( int id );

	void SetWarningValuesFromWidgets( HWND hWnd );
	void SetWidgetsFromWarningValues( HWND hWnd );

	bool IsWarningEnabled( int id );
};

#endif // _WARNING_MANAGER_H_
