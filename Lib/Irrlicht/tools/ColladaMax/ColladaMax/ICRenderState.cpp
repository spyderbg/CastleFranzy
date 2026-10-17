/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICRenderState.h"
#include "ICUIParameter.h"
#include "ICRenderStateManager.h"
#include "resource.h"

ICRenderState::~ICRenderState()
{
	SAFE_DELETE( m_pParameter );
	CLEAR_POINTER_VECTOR( m_children );
}
	
ICUIParameter *ICRenderState::SetParameter( FSStandardMaterial::PID pid, ICWidget::Type widgetType )
{
	SAFE_DELETE( m_pParameter );
	m_pParameter = new ICUIParameter( m_pManager->GetParentParamBlock(), pid, widgetType );
	return m_pParameter;
}

void ICRenderState::InitUI( TimeValue t, HWND hWnd )
{
	if ( m_pParameter )
	{
		m_pParameter->SetParamBlock( m_pManager->GetParentParamBlock() );
		m_pParameter->InitUI( t, hWnd );
	}

	for ( RenderStateList::iterator renderStateIter = m_children.begin(); renderStateIter != m_children.end(); ++renderStateIter )
	{
		ICRenderState *pRenderState = *renderStateIter;
		pRenderState->InitUI( t, hWnd );
	}
}

void ICRenderState::UpdateUI( TimeValue t, HWND hWnd )
{
	if ( m_pParameter )
	{
		m_pParameter->SetParamBlock( m_pManager->GetParentParamBlock() );
		m_pParameter->UpdateUI( t, hWnd );
	}

	for ( RenderStateList::iterator renderStateIter = m_children.begin(); renderStateIter != m_children.end(); ++renderStateIter )
	{
		ICRenderState *pRenderState = *renderStateIter;
		pRenderState->UpdateUI( t, hWnd );
	}
}

void ICRenderState::GetValidity( TimeValue t, Interval &valid )
{
	if ( m_pParameter )
	{
		m_pParameter->SetParamBlock( m_pManager->GetParentParamBlock() );
		m_pParameter->GetValidity( t, valid );
	}

	for ( RenderStateList::iterator renderStateIter = m_children.begin(); renderStateIter != m_children.end(); ++renderStateIter )
	{
		ICRenderState *pRenderState = *renderStateIter;
		pRenderState->GetValidity( t, valid );
	}
}

BOOL ICRenderState::DlgProc( TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( m_pParameter )
	{
		m_pParameter->SetParamBlock( m_pManager->GetParentParamBlock() );
		if ( m_pParameter->DlgProc( t, map, hWnd, msg, wParam, lParam ) )
		{
			return TRUE;
		}
	}

	for ( RenderStateList::iterator renderStateIter = m_children.begin(); renderStateIter != m_children.end(); ++renderStateIter )
	{
		ICRenderState *pRenderState = *renderStateIter;
		if ( pRenderState->DlgProc( t, map, hWnd, msg, wParam, lParam ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

ICRenderState *ICRenderState::AddChild( const fm::string &name, ICRenderStateManager *pManager )
{
	ICRenderState *pRenderState = new ICRenderState( name, pManager );
	m_children.push_back( pRenderState );
	return pRenderState;
}
