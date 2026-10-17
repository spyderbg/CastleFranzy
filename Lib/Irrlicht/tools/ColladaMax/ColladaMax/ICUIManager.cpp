/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "stdafx.h"
#include "ICUIManager.h"
#include "UIHelpers.h"

void ICUIManager::InitDialogPos()
{
	if ( !m_hWnd )
	{
		return;
	}

	if ( TEST_FLAG( m_windowInfo.dwStyle, WS_MINIMIZE ) ||
		 TEST_FLAG( m_windowInfo.dwStyle, WS_MAXIMIZE ) )
	{
		return;
	}

	RECT windowRect = m_windowInfo.rcWindow;
	if ( windowRect.bottom == windowRect.top )
	{
		return;
		//HWND parentHWnd = GetParent( m_hWnd );
		//if ( parentHWnd )
		//{
		//	WINDOWINFO parentWindowInfo;
		//	GetWindowInfo( parentHWnd, &parentWindowInfo );
		//	
		//	int dx = 64;
		//	int dy = 64;
		//	
		//	windowRect.bottom = parentWindowInfo.rcWindow.bottom + dy;
		//	windowRect.top = parentWindowInfo.rcWindow.top + dy;
		//	windowRect.left = parentWindowInfo.rcWindow.left + dx;
		//	windowRect.right = parentWindowInfo.rcWindow.right + dx;
		//}
	}

	SetWindowPos( m_hWnd, HWND_TOP, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL );
}

void ICUIManager::Release()
{
	m_hWnd = NULL;
}

DLG_PROC_IMPL( ICUIManager, Main )
{
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_INITDIALOG: 
		{
			m_hWnd = hWnd;
			InitDialogPos();
			return TRUE;
		}

		case WM_COMMAND:
		{
			switch (loWord)
			{		
				case IDCANCEL:
				case IDOK:
				{
					//EnableAccelerators();
					GetWindowInfo( m_hWnd, &m_windowInfo );
					EndDialog( hWnd, 1 );
					m_hWnd = NULL;
					return TRUE;
				}
			}
			break;
		}
	}
	return FALSE;
}