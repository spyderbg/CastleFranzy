/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __IC_RENDER_STATE__H
#define __IC_RENDER_STATE__H

//#include "FUtils/FUDaeEnum.h"
class ICRenderStateManager;
class FCDEffectPass;
class FCDEffectPassState;

#include "ICWidget.h"
#include "Common/StandardMtl.h"

class ICUIParameter;
typedef fm::pvector<ICUIParameter> UIParameterList;

class ICRenderState;
typedef fm::pvector<ICRenderState> RenderStateList;

class ICRenderState
{
private:
	RenderStateList m_children;
	ICUIParameter *m_pParameter;
	fm::string m_name;

	ICRenderStateManager *m_pManager;

public:
	ICRenderState( const fm::string name, ICRenderStateManager *pManager = NULL )
		: m_pManager( pManager )
		, m_name( name )
		, m_pParameter( NULL )
	{}
	~ICRenderState();

	inline const fm::string &GetName()
	{
		return m_name;
	}

	inline const fm::string &SetName( const fm::string &name )
	{
		m_name = name;
	}

	inline ICRenderStateManager *GetRenderStateManager()
	{
		return m_pManager;
	}

	inline void SetRenderStateManager( ICRenderStateManager *pManager )
	{
		m_pManager = pManager;
	}

	inline ICUIParameter *GetParameter()
	{
		return m_pParameter;
	}

	inline size_t GetChildCount()
	{
		return m_children.size();
	}

	inline ICRenderState *GetChild( size_t index )
	{
		FUAssert( index < GetChildCount(), return NULL );
		return m_children[index];
	}

	ICUIParameter *SetParameter( FSStandardMaterial::PID pid, ICWidget::Type widgetType );
	ICRenderState *AddChild( const fm::string &name, ICRenderStateManager *pManager = NULL );

	virtual void InitUI( TimeValue t, HWND hWnd );
	virtual void UpdateUI( TimeValue t, HWND hWnd );
	virtual void GetValidity( TimeValue t, Interval &valid );
	virtual BOOL DlgProc( TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

typedef fm::pvector<ICRenderState> RenderStateList;

#endif