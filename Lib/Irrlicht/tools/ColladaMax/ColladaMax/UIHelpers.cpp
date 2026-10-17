#include "stdafx.h"
#include "UIHelpers.h"
#include "imenuman.h"
#include "ICWidget.h"
#include "DlgTemplateBuilder.h"

namespace UIHelpers
{
	//k_autoScale = -1.f;

	void SetControlEnabled(HWND window, uint32 controlId, BOOL enabled)
	{
		HWND control = GetDlgItem(window, controlId);
		if (control != NULL) EnableWindow(control, enabled);
	}

	void SetControlVisible( HWND window, uint32 controlId, BOOL enabled )
	{
		HWND control = GetDlgItem(window, controlId);
		if ( control == NULL )
		{
			return;
		}

		ShowWindow( control, enabled ? SW_SHOW : SW_HIDE );
	}

	void ListBox_SelectAll( HWND hWnd, uint32 controlId, bool bSelected )
	{
		HWND control = GetDlgItem( hWnd, controlId );
		if ( control )
		{
			int itemCount = ListBox_GetCount( control );
			ListBox_SelItemRange( control, bSelected, 0, itemCount - 1 );
		}
	}

	long GetEditControlValue( HWND hWnd, uint32 editIdc, int radix )
	{
	#define MAX_EDIT_CONTROL_TEXT_LENGTH 256

		HWND hwndEdit = GetDlgItem( hWnd, editIdc );
		if ( !hwndEdit )
		{
			return 0;
		}

		char str[MAX_EDIT_CONTROL_TEXT_LENGTH];
		Edit_GetText( hwndEdit, str, MAX_EDIT_CONTROL_TEXT_LENGTH );
		return strtol( str, NULL, radix );

	#undef MAX_EDIT_CONTROL_TEXT_LENGTH
	}

	int GetSpinnerIValue( HWND hWnd, int spinnerIdc )
	{
		HWND spinnerHwnd = GetDlgItem( hWnd, spinnerIdc );
		ISpinnerControl *pSpin = GetISpinner( spinnerHwnd );
		int val = pSpin->GetIVal();
		ReleaseISpinner( pSpin );
		return val;
	}

	float GetSpinnerFValue( HWND hWnd, int spinnerIdc )
	{
		HWND spinnerHwnd = GetDlgItem( hWnd, spinnerIdc );
		ISpinnerControl *pSpin = GetISpinner( spinnerHwnd );
		float val = pSpin->GetFVal();
		ReleaseISpinner( pSpin );
		return val;
	}

	void SetSpinnerIndeterminate( HWND hWnd, int spinnerIdc, bool bIndeterminate )
	{
		HWND spinnerHwnd = GetDlgItem( hWnd, spinnerIdc );
		ISpinnerControl *pSpin = GetISpinner( spinnerHwnd );
		pSpin->SetIndeterminate( bIndeterminate );
		ReleaseISpinner( pSpin );
	}

	void InitSpinner( HWND hWnd, int spinnerIdc, int editCtrlIdc, EditSpinnerType editType, float minVal, float maxVal, float scale, float resetVal )
	{
		HWND hwndSpin = GetDlgItem( hWnd, spinnerIdc );
		ISpinnerControl *pSpin = GetISpinner( hwndSpin );

		pSpin->LinkToEdit( GetDlgItem( hWnd, editCtrlIdc ), editType );

		if ( scale == UIHelpers::k_autoScale )
		{
			pSpin->SetAutoScale( TRUE );
		}
		else
		{
			pSpin->SetScale( scale );
		}

		if ( editType == EDITTYPE_INT )
		{
			pSpin->SetLimits( (int)minVal, (int)maxVal );
			pSpin->SetResetValue( (int)resetVal );
		}
		else if ( editType == EDITTYPE_FLOAT )
		{
			pSpin->SetLimits( minVal, maxVal );
			pSpin->SetResetValue( resetVal );
		}

		ReleaseISpinner( pSpin );
	}

	void RemoveSubMenu( IMenu *pParentMenu, IMenu *pMenu )
	{
		IMenuManager *pMenuManager = GetCOREInterface()->GetMenuManager();
		if ( pMenuManager && pParentMenu && pMenu )
		{
			// find menu item corresponding to sub menu and remove it
			for ( size_t menuItemId = 0; menuItemId < pParentMenu->NumItems(); ++menuItemId )
			{
				IMenuItem *pMenuItem = pParentMenu->GetItem( menuItemId );
				IMenu *pSubMenu = pMenuItem->GetSubMenu();
				if ( pSubMenu == pMenu )
				{
					pMenuItem->SetSubMenu( NULL );
					pMenuItem->SetActionItem( NULL );
					pParentMenu->RemoveItem( pMenuItem );
					break;
				}
			}
		
			pMenuManager->UnRegisterMenu( pMenu );
			pMenuManager->UpdateMenuBar();
		}
	}

	void RemoveSubMenu( IMenu *pParentMenu, TCHAR *pName )
	{
		IMenuManager *pMenuManager = GetCOREInterface()->GetMenuManager();
		if ( pMenuManager && pParentMenu && pName )
		{
			IMenu *pIrrColladaMenu = pMenuManager->FindMenu( pName );
			RemoveSubMenu( pParentMenu, pIrrColladaMenu );
		}
	}

	void SetButtonEnabled(HWND hWnd,int buttonIdc,BOOL ison)
	{  
		ICustButton *iBut;
		iBut = GetICustButton(GetDlgItem(hWnd,buttonIdc));
		if ( iBut ) 
		{ 
			if (ison) iBut->Enable(); 
			else iBut->Disable();
		}
		ReleaseICustButton(iBut);
	}

	void SpinnerOn(HWND hWnd,int SpinNum,int Winnum)
	{  
		if ( !hWnd )
		{
			return;
		}

		ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));

		if( !spin2 )
		{
			return;
		}

		spin2->Enable();
		EnableWindow(GetDlgItem(hWnd,Winnum),TRUE);
		ReleaseISpinner(spin2);
	}

	void SpinnerOff(HWND hWnd,int SpinNum,int Winnum)
	{  
		if ( !hWnd )
		{
			return;
		}

		ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));

		if( !spin2 )
		{
			return;
		}

		spin2->Disable();
		EnableWindow(GetDlgItem(hWnd,Winnum),FALSE);
		ReleaseISpinner(spin2);
	}

	void SetSpinnerEditTextLabelEnabled( HWND hWnd, int spinnerIdc, int editCtrlIdc, int labelIdc, BOOL bEnabled )
	{
		if ( bEnabled )
		{
			SpinnerOn( hWnd, spinnerIdc, editCtrlIdc );
		}
		else
		{
			SpinnerOff( hWnd, spinnerIdc, editCtrlIdc );
		}

		EnableWindow( GetDlgItem( hWnd, labelIdc ), bEnabled );
	}

	void SetSpinnerKeyBrackets( HWND hWnd, int spinnerIdc, BOOL bEnabled )
	{
		if ( !hWnd )
		{
			return;
		}
		
		HWND hSpinner = GetDlgItem( hWnd, spinnerIdc );
		if ( !hSpinner )
		{
			return;
		}

		ISpinnerControl *pSpinner = GetISpinner( hSpinner );
		if ( !pSpinner )
		{
			return;
		}

		pSpinner->SetKeyBrackets( bEnabled );
		ReleaseISpinner( pSpinner );
	}

	void ClearComboBox( HWND hWnd, int comboBoxIdc )
	{
		if ( !hWnd )
		{
			return;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return;
		}

		SendMessage( hComboBox, CB_RESETCONTENT, NULL, NULL );
	}

	int AddComboBoxString( HWND hWnd, int comboBoxIdc, LPCSTR pString, int itemData )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return -1;
		}

		LRESULT stringIndex = SendMessage( hComboBox, CB_ADDSTRING, NULL, ( LPARAM ) pString );
		SendMessage( hComboBox, CB_SETITEMDATA, stringIndex, ( LPARAM )( DWORD )itemData );

		return stringIndex;
	}

	void SetComboBoxSelectedIndex( HWND hWnd, int comboBoxIdc, int comboBoxItemIdx )
	{
		if ( !hWnd )
		{
			return;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return;
		}

		SendMessage( hComboBox, CB_SETCURSEL, comboBoxItemIdx, NULL );
	}

	int GetComboBoxSelectedIndex( HWND hWnd, int comboBoxIdc )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return -1;
		}

		return SendMessage( hComboBox, CB_GETCURSEL, NULL, NULL );	
	}

	int GetComboBoxItemCount( HWND hWnd, int comboBoxIdc )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return -1;
		}

		return SendMessage( hComboBox, CB_GETCOUNT, NULL, NULL );	
	}

	int GetStringIndex( HWND hWnd, int comboBoxIdc, LPCTSTR str )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return -1;
		}

		int result = SendMessage( hComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM) str );
		return result != CB_ERR ? result : -1;
	}

	int SetComboBoxItemHeight( HWND hWnd, int comboBoxIdc, int itemIndex, int itemHeight )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc );
		if ( !hComboBox )
		{
			return -1;
		}

		return ComboBox_SetItemHeight( hComboBox, itemIndex, itemHeight );
	}

	int SetScrollInfo( HWND hWnd, int scrollIdc, LPCSCROLLINFO pScrollInfo, bool bRedraw )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hScroll = GetDlgItem( hWnd, scrollIdc );
		if ( !hScroll )
		{
			return -1;
		}

		return SetScrollInfo( hScroll, SB_CTL, pScrollInfo, bRedraw ? 1 : 0 );
	}

	bool GetScrollInfo( HWND hWnd, int scrollIdc, LPCSCROLLINFO pScrollInfo )
	{
		if ( !hWnd )
		{
			return false;
		}
		
		HWND hScroll = GetDlgItem( hWnd, scrollIdc );
		if ( !hScroll )
		{
			return false;
		}

		return GetScrollInfo( hScroll, SB_CTL, pScrollInfo );
	}

	int SetScrollPos( HWND hWnd, int scrollIdc, int pos, bool bRedraw )
	{
		if ( !hWnd )
		{
			return -1;
		}
		
		HWND hScroll = GetDlgItem( hWnd, scrollIdc );
		if ( !hScroll )
		{
			return -1;
		}

		return SetScrollPos( hScroll, SB_CTL, pos, bRedraw ? 1 : 0 );
	}

	int GetCustEditText( HWND hWnd, int editIdc, fm::string &text )
	{
		if ( !hWnd )
		{
			return -1;
		}

		HWND editHwnd = GetDlgItem( hWnd, editIdc );
		if ( !editHwnd )
		{
			return -1;
		}

		ICustEdit *pCustEdit = GetICustEdit( editHwnd );
		MCHAR currentText[1025];
		pCustEdit->GetText( currentText, 1024 );
		text = currentText;
		ReleaseICustEdit( pCustEdit );

		return 0;
	}

	int SetCustEditText( HWND hWnd, int editIdc, const fm::string &text )
	{
		if ( !hWnd )
		{
			return -1;
		}

		HWND editHwnd = GetDlgItem( hWnd, editIdc );
		if ( !editHwnd )
		{
			return -1;
		}

		ICustEdit *pCustEdit = GetICustEdit( editHwnd );
		MCHAR currentText[1025];
		strcpy( currentText, text.c_str() );
		pCustEdit->SetText( currentText );
		ReleaseICustEdit( pCustEdit );

		return 0;
	}

	void AddActionMenuItem( IMenu *pMenu, ActionItem *pActionItem )
	{
		if ( !pMenu || !pActionItem )
		{
			return;
		}

		IMenuItem *pMenuItem = GetIMenuItem();
		pMenuItem->SetActionItem( pActionItem );
		pMenu->AddItem( pMenuItem );	
	}

	IMenuItem *AddSeparator( IMenu *pMenu )
	{
		IMenuItem *pMenuItem = GetIMenuItem();
		pMenuItem->ActAsSeparator();
		pMenu->AddItem( pMenuItem );
		return pMenuItem;
	}

	IMenu *AddMenu( const TCHAR *title, bool bRegisterMenu )
	{
		IMenuManager *pMenuManager = GetCOREInterface()->GetMenuManager();
		IMenu *pMenu = GetIMenu();
		pMenu->SetTitle( title );
		if ( bRegisterMenu )
		{
			pMenuManager->RegisterMenu( pMenu );
		}
		return pMenu;
	}

	IMenuItem *CreateSubMenuItem( IMenu *pMenu )
	{
		IMenuItem *pMenuItem = GetIMenuItem();
		pMenuItem->SetSubMenu( pMenu );
		return pMenuItem;
	}

	IMenuItem *AddSubMenu( IMenu *pParentMenu, IMenu *pChildMenu, int position )
	{
		IMenuItem *pSubMenuItem = CreateSubMenuItem( pChildMenu );
		pParentMenu->AddItem( pSubMenuItem, position );
		return pSubMenuItem;
	}

	void FillComboBox( ICComboBox &widget, const FUDaeCommon::EnumMapping enumMap[], size_t mappingCount )
	{
		for ( int enumId = 0; enumId < mappingCount; ++enumId )
		{
			const FUDaeCommon::EnumMapping &enumMapping = enumMap[enumId];
			widget.AddItem( ICComboBoxItem( fm::string( enumMapping.GetName() ).ToUpper(), enumMapping.GetValue() ) );
		}
	}

	DLGITEMTEMPLATE &BuildDialogTemplateItem( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle
		, uint32 windowClass )
	{
		// Create a label for the parameter control
		tb.Align();
		DLGITEMTEMPLATE& widget = tb.Append<DLGITEMTEMPLATE>();
		//dialogCount++;
		widget.x = posX;
		widget.y = posY;
		widget.id = idc;
		widget.cx = width;
		widget.cy = height;
		widget.style = style;
		widget.dwExtendedStyle = extendedStyle;

		tb.Append( windowClass ); // STATIC window class
		tb.AppendWString( name.c_str() ); // Window text
		tb.Append((uint16) 0); // extra bytes count

		return widget;
	}

	DLGITEMTEMPLATE &BuildDialogTemplateItem( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle
		, const MCHAR *windowClass )
	{
		// Create a label for the parameter control
		tb.Align();
		DLGITEMTEMPLATE& widget = tb.Append<DLGITEMTEMPLATE>();
		//dialogCount++;
		widget.x = posX;
		widget.y = posY;
		widget.id = idc;
		widget.cx = width;
		widget.cy = height;
		widget.style = style;
		widget.dwExtendedStyle = extendedStyle;

		tb.AppendWString( windowClass ); // STATIC window class
		tb.AppendWString( name.c_str() ); // Window text
		tb.Append((uint16) 0); // extra bytes count

		return widget;
	}

	DLGITEMTEMPLATE& BuildDialogTemplateItem_Label( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle )
	{
		return BuildDialogTemplateItem( tb
			//, dialogCount, lineCount
			, idc, name
			, posX, posY, width, height
			, style, extendedStyle
			, (uint32) 0x0082FFFF );
	}

	DLGITEMTEMPLATE& BuildDialogTemplateItem_ComboBox( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle )
	{
		return BuildDialogTemplateItem( tb
			//, dialogCount, lineCount
			, idc, name
			, posX, posY, width, height
			, style, extendedStyle
			, (uint32) 0x0085FFFF );
	}

	DLGITEMTEMPLATE& BuildDialogTemplateItem_EditText( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle )
	{
		return BuildDialogTemplateItem( tb
			//, dialogCount, lineCount
			, idc, name
			, posX, posY, width, height
			, style, extendedStyle
			, CUSTEDITWINDOWCLASS );
	}

	DLGITEMTEMPLATE& BuildDialogTemplateItem_Spinner( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle )
	{
		return BuildDialogTemplateItem( tb
			//, dialogCount, lineCount
			, idc, name
			, posX, posY, width, height
			, style, extendedStyle
			, SPINNERWINDOWCLASS );
	}
}