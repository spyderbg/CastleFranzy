/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IRRCOLLADA_H_
#define _IRRCOLLADA_H_

#include "resource.h"
#include "ICUIManager.h"
#include <guplib.h>
#include <imenuman.h>

#define IRRCOLLADA_ID Class_ID(0x443f3bb8, 0x53d334e4)
#define IRRCOLLADA_FN_INTERFACE	Interface_ID(0x6cd46958, 0x71751290)
#define IRRCOLLADA_ACT_INTERFACE Interface_ID(0x6cd46958, 0x71751291)

class IrrCollada;

IrrCollada *GetIrrColladaPtr();
ClassDesc2* GetIrrColladaClassDesc();

// Class Desc
class IrrColladaClassDesc : public ClassDesc2
{
public:
	int				IsPublic() {return TRUE; }
	void*			Create(BOOL isLoading);

	const TCHAR*	ClassName() {return GetString(IDS_IC_CLASS_NAME);}
	SClass_ID		SuperClassID() {return GUP_CLASS_ID;}
	Class_ID 		ClassID() {return IRRCOLLADA_ID;}

	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return GetString(IDS_IC_CLASS_NAME); }
	HINSTANCE		HInstance() { return hInstance; }

	// Load/Save
	virtual BOOL NeedsToSave() { return FALSE; }
	virtual IOResult Save(ISave *is){ return IO_OK; }
	virtual IOResult Load(ILoad *il){ return IO_OK; }
};

class IIrrCollada : public FPStaticInterface {
	public:
		virtual void	SetVisible( BOOL show=TRUE, int icManagerId = 0 )=0;
		virtual	BOOL	IsVisible()=0;
		virtual void	LoadClass(ClassDesc* cd){}  // ensure class is fully loaded - jbw 7.4.00
};

inline IIrrCollada* GetIrrCollada() { return (IIrrCollada*)GetCOREInterface(IRRCOLLADA_FN_INTERFACE); }

class IrrCollada : public GUP, public IIrrCollada
{
public:
	enum
	{
		CID_DRAW_LAYER
		, CID_COUNT = CID_DRAW_LAYER + 1
		//, CID_VISIBILITY
		//, CID_COUNT
	};

private:
	fm::pvector<INode> m_selectedNodes;
	fm::pvector<ICUIManager> m_ICManagers;
	PROCESS_INFORMATION m_colladaEditorProcessInfo;
	PROCESS_INFORMATION m_textEditorProcessInfo;

	IMenu *CreateIrrColladaMenu( bool bRegisterMenu = true );
	void AddIrrColladaMenuItemToQuadMenu( IQuadMenuContext::RightClickContext context, int menuId, IMenu *pMenu );//, IMenuItem *pMenuItem )

public:
	IrrCollada(BOOL isLoading = FALSE);
	~IrrCollada();

	void Reset();
	void RefreshSelectedNodes();

	static void NotificationCallback( void *param, NotifyInfo *info );

	ActionItem *GetActionItem( int actionId );
	void RemoveMenuEntries();
	void RefreshMenu();
	inline fm::pvector<INode> &GetSelectedNodes(){ return m_selectedNodes; }
	inline fm::pvector<ICUIManager> &GetICManagers(){ return m_ICManagers; }

	// from GUP
	virtual DWORD Start ();
	virtual void Stop ();
	//virtual void	DeleteThis		( )	{ }
	virtual DWORD_PTR	Control			( DWORD parameter );
	//virtual void	Init(HWND hWnd);
	//virtual void	Destroy(HWND hWnd);

	// IIrrCollada
	void SetVisible(BOOL show=TRUE, int icManagerId = 0 );
	BOOL IsVisible();

	inline void SetColladaEditorProcessInfo( const PROCESS_INFORMATION &pi )
	{
		m_colladaEditorProcessInfo = pi;
	}

	inline PROCESS_INFORMATION GetColladaEditorProcessInfo()
	{
		return m_colladaEditorProcessInfo;
	}

	inline void SetTextEditorProcessInfo( const PROCESS_INFORMATION &pi )
	{
		m_textEditorProcessInfo = pi;
	}

	inline PROCESS_INFORMATION GetTextEditorProcessInfo()
	{
		return m_textEditorProcessInfo;
	}

	// function IDs 
	enum { pm_setVisible, pm_isVisible, pm_loadClass,  }; 

	DECLARE_DESCRIPTOR(IrrCollada)
	// dispatch map
	BEGIN_FUNCTION_MAP
		VFN_1(pm_loadClass,    LoadClass, TYPE_CLASS);
		PROP_FNS(pm_isVisible, IsVisible, pm_setVisible, SetVisible, TYPE_BOOL);
	END_FUNCTION_MAP
};

#endif // _IRRCOLLADA_H_


