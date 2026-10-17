/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ICUIMANAGER_H_
#define _ICUIMANAGER_H_

class IrrCollada;

class ICUIManager
{
public:
	enum ManagerType
	{
		MT_OBJECT_PROPERTY_MANAGER
		, MT_PREFERENCES_MANAGER
		, MT_COUNT
	};

private:
	int m_type;
	WINDOWINFO m_windowInfo;

protected:
	IrrCollada *m_pIrrCollada;
	HWND m_hWnd;

public:
	ICUIManager( int type, IrrCollada *pIrrCollada )
		: m_type( type )
		, m_pIrrCollada( pIrrCollada )
	{
		FUDbgAssert( pIrrCollada, );
	}

	inline int GetType() const { return m_type; }

	virtual void RefreshDialog() = 0;
	virtual void DisplayDialog( BOOL show ) = 0;
	virtual fm::string ConstructDialogCaption() = 0;
	virtual void InitDialogPos();

	virtual void PostCloneNotification( INodeTab *pOrigNodes, INodeTab *pClonedNodes, CloneType cloneType ){}

	DLG_PROC_DECL( Main )

	inline HWND GetHWnd() const
	{
		return m_hWnd;
	}

	virtual void Release();
};

#endif // _ICUIMANAGER_H_


