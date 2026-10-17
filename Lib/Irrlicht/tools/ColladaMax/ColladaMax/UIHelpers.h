#ifndef _UI_HELPERS_H_
#define _UI_HELPERS_H_

#include "FUtils/FUDaeEnum.h"

class ICComboBox;
class DlgTemplateBuilder;

#if (MAX_VERSION_MAJOR >= 9)
	#pragma warning(disable:4245)
	#include <3dsmaxport.h>
	#pragma warning(default:4245)
#else
	template<typename Type>
	inline Type DLSetWindowLongPtr(HWND hWnd, Type ptr, int n = GWLP_USERDATA)
	{
	#	if !defined(_WIN64)
	   // SetWindowLongPtr() maps to SetWindowLong() in 32 bit land; react accordingly to keep
	   // the compiler happy, even with /Wp64.
	   return (Type)(static_cast<LONG_PTR>(::SetWindowLongPtr(hWnd, n, (LONG)((LONG_PTR)(ptr)))));
	#	else
	   return (Type)(static_cast<LONG_PTR>(::SetWindowLongPtr(hWnd, n, (LONG_PTR)(ptr))));
	#	endif // _WIN64
	}

	template<typename DataPtr>
	DataPtr DLGetWindowLongPtr(HWND hWnd, int n = GWLP_USERDATA, DataPtr = NULL)
	{
	   return (DataPtr)(static_cast<LONG_PTR>(::GetWindowLongPtr(hWnd, n)));
	}

	/*
	   GetTextExtentPoint32 works with strings up to 2G, even in Win64.  Not a problem,
	   but since the length is often obtained from _tcslen(), a lot of casting ensues.
	*/
	inline BOOL DLGetTextExtent(HDC hDC, const TCHAR* text, LPSIZE lpExtent)
	{
	   return ::GetTextExtentPoint32(hDC, text, text ? static_cast<int>(_tcslen(text)) : 0, 
									 lpExtent);
	}

	inline BOOL DLGetTextExtent(HDC hDC, const TCHAR* text, size_t len, LPSIZE lpExtent)
	{
	   return ::GetTextExtentPoint32(hDC, text, static_cast<int>(len), lpExtent); 
	}

#endif // MAX_VERSION

typedef fm::vector<int> IdcList;

namespace UIHelpers
{
	const float k_autoScale = -1.f;

	template<class T>
	T *GetExpPointer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
	{
		T* exp;
		if (message == WM_INITDIALOG)
		{
			// record exp instance pointer for subsequent callbacks
			exp = (T*) lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA,lParam); 
		}
		else
		{
			exp = (T*)(size_t) GetWindowLongPtr(hWnd, GWLP_USERDATA);
		}

		return exp;
	}

	void SetControlEnabled(HWND window, uint32 controlId, BOOL enabled);
	void SetControlVisible( HWND window, uint32 controlId, BOOL enabled );
	void ListBox_SelectAll( HWND hWnd, uint32 controlId, bool bSelected );

	template <class T>
	void SetEditControlValue( HWND hWnd, uint32 editIdc, T value )
	{
		std::stringstream stream;
		stream << value;

		HWND hwndEdit = GetDlgItem( hWnd, editIdc );
		if ( hwndEdit )
		{
			Edit_SetText( hwndEdit, stream.str().c_str() );
		}
	}

	long GetEditControlValue( HWND hWnd, uint32 editIdc, int radix = 10 );

	int GetSpinnerIValue( HWND hWnd, int spinnerIdc );
	float GetSpinnerFValue( HWND hWnd, int spinnerIdc );
	void InitSpinner( HWND hWnd, int spinnerIdc, int editCtrlIdc, EditSpinnerType editType, float minVal, float maxVal, float scale, float resetVal );

	void RemoveSubMenu( IMenu *pParentMenu, TCHAR *pName );
	void RemoveSubMenu( IMenu *pParentMenu, IMenu *pMenu );

	template <class T>
	void SetSpinnerValue( HWND hWnd, int spinnerIdc, T value  )
	{
		HWND spinnerHwnd = GetDlgItem( hWnd, spinnerIdc );
		ISpinnerControl *pSpin = GetISpinner( spinnerHwnd );
		pSpin->SetValue( value, FALSE );
		ReleaseISpinner( pSpin );
	}

	void SetSpinnerIndeterminate( HWND hWnd, int spinnerIdc, bool bIndeterminate  );

	void SetButtonEnabled(HWND hWnd,int buttonIdc,BOOL ison);
	void SpinnerOn(HWND hWnd,int SpinNum,int Winnum);
	void SpinnerOff(HWND hWnd,int SpinNum,int Winnum);
	void SetSpinnerKeyBrackets( HWND hWnd, int spinnerIdc, BOOL bEnabled );

	void SetSpinnerEditTextLabelEnabled( HWND hWnd, int spinnerIdc, int editCtrlIdc, int labelIdc, BOOL bEnabled );

	void ClearComboBox( HWND hWnd, int comboBoxIdc );

	// returns index of newly added string in combo box
	int AddComboBoxString( HWND hWnd, int comboBoxIdc, LPCSTR pString, int itemData );

	void SetComboBoxSelectedIndex( HWND hWnd, int comboBoxIdc, int comboBoxItemIdx );
	int GetComboBoxSelectedIndex( HWND hWnd, int comboBoxIdc );
	int GetComboBoxItemCount( HWND hWnd, int comboBoxIdc );
	int GetStringIndex( HWND hWnd, int comboBoxIdc, LPCTSTR str );
	int SetComboBoxItemHeight( HWND hWnd, int comboBoxIdc, int itemIndex, int itemHeight );

	// returns the current position of the scroll box
	int SetScrollInfo( HWND hWnd, int scrollIdc, LPCSCROLLINFO pScrollInfo, bool bRedraw = true );
	
	// returns true if at least one value was gotten
	bool GetScrollInfo( HWND hWnd, int scrollIdc, LPCSCROLLINFO pScrollInfo );

	// returns the previous position of the scroll box
	int SetScrollPos( HWND hWnd, int scrollIdc, int pos, bool bRedraw = true );

	// returns -1 if error, 0 if ok
	int GetCustEditText( HWND hWnd, int editIdc, fm::string &text );
	int SetCustEditText( HWND hWnd, int editIdc, const fm::string &text );

	IMenuItem *CreateSubMenuItem( IMenu *pMenu );
	void AddActionMenuItem( IMenu *pMenu, ActionItem *pActionItem );
	IMenuItem *AddSeparator( IMenu *pMenu );
	IMenu *AddMenu( const TCHAR *title, bool bRegisterMenu );
	IMenuItem *AddSubMenu( IMenu *pParentMenu, IMenu *pChildMenu, int position = -1 );

	// Operations on ICWidgets
	void FillComboBox( ICComboBox &widget, const FUDaeCommon::EnumMapping enumMap[], size_t mappingCount );

	// DlgTemplate helpers
	DLGITEMTEMPLATE &BuildDialogTemplateItem( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle
		, uint32 windowClass );

	DLGITEMTEMPLATE &BuildDialogTemplateItem( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle
		, const MCHAR *windowClass );

	DLGITEMTEMPLATE& BuildDialogTemplateItem_Label( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle );

	DLGITEMTEMPLATE& BuildDialogTemplateItem_ComboBox( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle );

	DLGITEMTEMPLATE& BuildDialogTemplateItem_EditText( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle );

	DLGITEMTEMPLATE& BuildDialogTemplateItem_Spinner( DlgTemplateBuilder& tb
		//, WORD &dialogCount, WORD lineCount
		, WORD idc, const fm::string &name
		, short posX, short posY, short width, short height
		, DWORD style, DWORD extendedStyle );
}

#endif // _UI_HELPERS_H_
