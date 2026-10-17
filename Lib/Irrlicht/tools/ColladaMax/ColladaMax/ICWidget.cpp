#include "StdAfx.h"
#include "ICWidget.h"
#include "DrawLayerManager.h"
#include "UIHelpers.h"
#include "FUtils/FUParameter.h"
#include "resource.h"
//#include "MAXHelpers.h"
//#include "imenuman.h"

ICWidget *ICWidgetFactory::Create( ICWidget::Type type, const fm::string &name )
{
	ICWidget *pWidget = NULL;

	if ( type == ICWidget::SpinnerInt ){ pWidget = new ICSpinnerInt( name ); }
	if ( type == ICWidget::SpinnerFloat ){ pWidget = new ICSpinnerFloat( name ); }
	else if ( type == ICWidget::SpinnerFloat3 ){ pWidget = new ICSpinnerFloat3( name ); }
	else if ( type == ICWidget::ComboBox ){ pWidget = new ICComboBox( name );	}
	else if ( type == ICWidget::ComboBox4 ){ pWidget = new ICComboBox4( name );	}
	else if ( type == ICWidget::EditText ){ pWidget = new ICEditText( name );	}
	else if ( type == ICWidget::FileChooser ){ pWidget = new ICFileChooser( name );	}
	else if ( type == ICWidget::ColorChooser ){ pWidget = new ICColorChooser( name );	}

	return pWidget;
}

// ----------------------------------------------------------------------
// Spinner
// ----------------------------------------------------------------------
ICSpinner::ICSpinner( Type type, EditSpinnerType editType, const fm::string &name )
	: ICWidget( type, name )
	, m_editType( editType )
	, m_labelIdc( -1 )
	, m_editIdc( -1 )
	, m_spinnerIdc( -1 )
	, m_minVal( 0.f )
	, m_maxVal( 0.f )
	, m_scale( 0.f )
	, m_resetVal( 0.f )
	, m_bButtonDown( false )
{
}

void ICSpinner::Construct( int labelIdc, int editIdc, int spinnerIdc, float minVal, float maxVal, float scale, float resetVal )
{
	m_labelIdc = labelIdc;
	m_editIdc = editIdc;
	m_spinnerIdc = spinnerIdc;
	m_minVal = minVal;
	m_maxVal = maxVal;
	m_scale = scale;
	m_resetVal = resetVal;
}

void ICSpinner::SetIndeterminate( HWND hWnd, bool bIndeterminate )
{
	ICWidget::SetIndeterminate( hWnd, bIndeterminate );
	UIHelpers::SetSpinnerIndeterminate( hWnd, m_spinnerIdc, bIndeterminate );
}

void ICSpinner::Init( HWND hWnd, void *pValue )
{
	UIHelpers::InitSpinner( hWnd, m_spinnerIdc, m_editIdc, m_editType, m_minVal, m_maxVal, m_scale, m_resetVal );
	SetValue( hWnd, pValue );
	m_bButtonDown = false;
}

void ICSpinner::SetEnabled( HWND hWnd, bool bEnabled )
{
	UIHelpers::SetControlEnabled( hWnd, m_spinnerIdc, bEnabled);
	UIHelpers::SetControlEnabled( hWnd, m_editIdc, bEnabled);
	UIHelpers::SetControlEnabled( hWnd, m_labelIdc, bEnabled);
}

BOOL ICSpinner::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		//case CC_SPINNER_BUTTONDOWN:
		//{
		//	int ctrlId = loWord;
		//	if ( ctrlId != m_spinnerIdc )
		//	{
		//		break;
		//	}

		//	m_bButtonDown = true;
		//}
		//case CC_SPINNER_BUTTONUP:
		//{
		//	int ctrlId = loWord;
		//	if ( ctrlId != m_spinnerIdc )
		//	{
		//		break;
		//	}

		//	m_bButtonDown = false;

		//	BOOL bCancelled = hiWord ? FALSE : TRUE;
		//	if ( bCancelled )
		//	{
		//		break;
		//	}

		//	return TRUE;
		//}
		case CC_SPINNER_CHANGE:
		{
			int ctrlId = loWord;
			if ( ctrlId != m_spinnerIdc )
			{
				break;
			}

			return TRUE;
		}
		//case WM_CUSTEDIT_ENTER:
		//{
		//	int ctrlId = wParam;
		//	if ( ctrlId != m_editIdc )
		//	{
		//		break;
		//	}
		//	int tmp = 0;
		//	break;
		//	//return !m_bButtonDown;
		//}
	}

	return FALSE;
}

void ICSpinner::SetKeyFrameAtTimeFeedbackEnabled( HWND hWnd, bool bEnabled )
{
	UIHelpers::SetSpinnerKeyBrackets( hWnd, m_spinnerIdc, bEnabled ? 1 : 0 );
}

// ----------------------------------------------------------------------
// SpinnerInt
// ----------------------------------------------------------------------
ICSpinnerInt::ICSpinnerInt( const fm::string &name )
	: ICSpinner( SpinnerInt, EDITTYPE_INT, name )
{}

void ICSpinnerInt::GetValue( HWND hWnd, void *pValue )
{
	int &intRef = *( int * )pValue;
	HWND spinnerHwnd = GetDlgItem( hWnd, m_spinnerIdc );
	ISpinnerControl *pSpin = GetISpinner( spinnerHwnd );
	intRef = UIHelpers::GetSpinnerIValue( hWnd, m_spinnerIdc );
	ReleaseISpinner( pSpin );
}

void ICSpinnerInt::SetValue( HWND hWnd, void *pValue )
{
	const int &value = *( int * )pValue;
	UIHelpers::SetSpinnerValue( hWnd, m_spinnerIdc, value );
}

void ICSpinnerInt::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	SetValueT<int>( hWnd, values );
}

// ----------------------------------------------------------------------
// SpinnerFloat
// ----------------------------------------------------------------------
ICSpinnerFloat::ICSpinnerFloat( const fm::string &name )
	: ICSpinner( SpinnerFloat, EDITTYPE_FLOAT, name )
{}

void ICSpinnerFloat::GetValue( HWND hWnd, void *pValue )
{
	float &ref = *( float * )pValue;
	HWND spinnerHwnd = GetDlgItem( hWnd, m_spinnerIdc );
	ISpinnerControl *pSpin = GetISpinner( spinnerHwnd );
	ref = UIHelpers::GetSpinnerFValue( hWnd, m_spinnerIdc );
	ReleaseISpinner( pSpin );
}

void ICSpinnerFloat::SetValue( HWND hWnd, void *pValue )
{
	const float &value = *( float * )pValue;
	UIHelpers::SetSpinnerValue( hWnd, m_spinnerIdc, value );
}

void ICSpinnerFloat::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	SetValueT<float>( hWnd, values );
}

// ----------------------------------------------------------------------
// SpinnerFloat3
// ----------------------------------------------------------------------
void ICSpinnerFloat3::Construct( int labelIdc )
{
	m_labelIdc = labelIdc;
	m_childSpinners.resize( k_numChildSpinners );
}

void ICSpinnerFloat3::Init( HWND hWnd, void *pValue )
{
	for ( size_t childSpinnerId = 0; childSpinnerId < m_childSpinners.size(); ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		childSpinner.Init( hWnd, pValue );
	}
}

void ICSpinnerFloat3::GetValue( HWND hWnd, void *pValue )
{
	FMVector3 &floatVector = *( FMVector3 * )pValue;

	float val[k_numChildSpinners];
	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		childSpinner.GetValue( hWnd, &val[childSpinnerId] );
	}

	floatVector.Set( val[0], val[1], val[2] );
}

void ICSpinnerFloat3::SetValue( HWND hWnd, void *pValue )
{
	FMVector3 &floatVector = *( FMVector3 * )pValue;

	float val[k_numChildSpinners];
	val[0] = floatVector.x; val[1] = floatVector.y; val[2] = floatVector.z;
	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		childSpinner.SetValue( hWnd, &val[childSpinnerId] );
	}
}

void ICSpinnerFloat3::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	fm::vector<fm::vector<void *>> decomposedValues( k_numChildSpinners );
	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		decomposedValues[childSpinnerId].resize( values.size() );
	}

	for ( size_t valueId = 0; valueId < values.size(); ++valueId )
	{
		FMVector3 &currentValue = *( FMVector3 * )values[valueId];
		decomposedValues[0][valueId] = &currentValue.x;
		decomposedValues[1][valueId] = &currentValue.y;
		decomposedValues[2][valueId] = &currentValue.z;
	}

	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		childSpinner.SetValue( hWnd, decomposedValues[childSpinnerId] );
	}
}

void ICSpinnerFloat3::SetEnabled( HWND hWnd, bool bEnabled )
{
	UIHelpers::SetControlEnabled( hWnd, m_labelIdc, bEnabled);
	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		childSpinner.SetEnabled( hWnd, bEnabled );
	}
}

BOOL ICSpinnerFloat3::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		if ( childSpinner.DialogProc( hWnd, message, wParam, lParam ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void ICSpinnerFloat3::SetIndeterminate( HWND hWnd, bool bIndeterminate )
{
	for ( size_t childSpinnerId = 0; childSpinnerId < k_numChildSpinners; ++childSpinnerId )
	{
		ICSpinnerFloat &childSpinner = m_childSpinners[childSpinnerId];
		childSpinner.SetIndeterminate( hWnd, bIndeterminate );
	}
}

// ----------------------------------------------------------------------
// ComboBoxes
// ----------------------------------------------------------------------
ICComboBox::ICComboBox( const fm::string &name )
: ICWidget( ComboBox, name )
, m_labelIdc( -1 )
, m_comboBoxIdc( -1 )
, m_bKeyFrameAtTimeFeedbackEnabled( false )
{}

void ICComboBox::Construct( int labelIdc, int comboBoxIdc )
{
	m_labelIdc = labelIdc;
	m_comboBoxIdc = comboBoxIdc;
}

void ICComboBox::Init( HWND hWnd, void *pValue )
{
	HWND hComboBox = GetDlgItem( hWnd, m_comboBoxIdc );
	for ( int itemId = 0; itemId < ( int )m_items.size(); ++itemId )
	{
		SendMessage( hComboBox, CB_ADDSTRING, NULL, (LPARAM) (LPCTSTR)m_items[itemId].GetName().c_str() );
		SendMessage( hComboBox, CB_SETITEMDATA, itemId, (LPARAM) (DWORD)m_items[itemId].GetData() );
	}
	SetValue( hWnd, pValue );
}

void ICComboBox::SetValue( HWND hWnd, void *pValue )
{
	const int &itemDataRef = *( int * )pValue;
	HWND hComboBox = GetDlgItem( hWnd, m_comboBoxIdc );

	int comboBoxItemIdx = -1;
	int numItems = SendMessage( hComboBox, CB_GETCOUNT, NULL, NULL );
	for ( int itemId = 0; itemId < numItems; ++itemId )
	{
		int itemData = SendMessage( hComboBox, CB_GETITEMDATA, itemId, NULL );
		if ( itemData == itemDataRef )
		{
			comboBoxItemIdx = itemId;
			break;
		}
	}
	
	//FUAssert( comboBoxItemIdx > -1, return false );
	SendMessage( hComboBox, CB_SETCURSEL, comboBoxItemIdx, NULL );
}

void ICComboBox::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	SetValueT<int>( hWnd, values );
}

void ICComboBox::GetValue( HWND hWnd, void *pValue )
{
	int &itemDataRef = *( int * )pValue;
	HWND hComboBox = GetDlgItem( hWnd, m_comboBoxIdc );
	int comboBoxItemIdx = SendMessage( hComboBox, CB_GETCURSEL, NULL, NULL );
	itemDataRef = SendMessage( hComboBox, CB_GETITEMDATA, comboBoxItemIdx, (LPARAM) NULL );
}

void ICComboBox::SetEnabled( HWND hWnd, bool bEnabled )
{
	UIHelpers::SetControlEnabled( hWnd, m_labelIdc, bEnabled);
	UIHelpers::SetControlEnabled( hWnd, m_comboBoxIdc, bEnabled);
}

size_t ICComboBox::AddItem( const ICComboBoxItem &comboBoxItem )
{
	m_items.push_back( comboBoxItem );
	return m_items.size() - 1;
}

BOOL ICComboBox::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_INITDIALOG:
		{
			// TODO : Determine combo box item height from font height
			UIHelpers::SetComboBoxItemHeight( hWnd, m_comboBoxIdc, -1, 15 );
			UIHelpers::SetComboBoxItemHeight( hWnd, m_comboBoxIdc, 0, 13 );
			break;
		}

		// this can be used for Owner Drawn Variable heigt combo boxes
		//case WM_MEASUREITEM:
		//{
		//	LPMEASUREITEMSTRUCT pMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
		//	if ( pMeasureItem->CtlID != m_comboBoxIdc )
		//	{
		//		break;
		//	}

		//	pMeasureItem->itemHeight = 13;
		//	//HFONT hSysFont = GetStockObject( SYSTEM_FONT );
		//	//hOldFont = SelectObject( pMeasureItem->CtlID, hSysFont );
		//	break;
		//}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT) lParam;
			if ( pDrawItem->CtlID != m_comboBoxIdc )
			{
				break;
			}

			if ( pDrawItem->itemID < 0 || pDrawItem->itemID >= m_items.size() )
			{
				break;
			}

			ICComboBoxItem &comboBoxItem = m_items[pDrawItem->itemID];

			// get text of current item
			char szItem[MAX_PATH];
			strcpy( szItem, comboBoxItem.GetName().c_str() );
			
			if( pDrawItem->itemAction & ( ODA_DRAWENTIRE | ODA_SELECT ) )
			{
				COLORREF colorText;
				COLORREF colorBack;

				if( pDrawItem->itemState & ODS_SELECTED )
				{
					colorText = SetTextColor( pDrawItem->hDC, GetSysColor( COLOR_HIGHLIGHTTEXT ));
					colorBack = SetBkColor( pDrawItem->hDC, GetSysColor( COLOR_HIGHLIGHT ));
				}

				// set icon
				bool bDrawIcon = TEST_FLAG( pDrawItem->itemState, ODS_COMBOBOXEDIT ) && m_bKeyFrameAtTimeFeedbackEnabled;

				HICON hLeftBracket = NULL;
				HICON hRightBracket = NULL;
				if ( bDrawIcon )
				{
					hLeftBracket = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_BRACKET_LEFT_RED ) );
					hRightBracket = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_BRACKET_RIGHT_RED ) );
				}

				// icon && text positions
				int indentX = TEST_FLAG( pDrawItem->itemState, ODS_COMBOBOXEDIT ) ? 1 : 2;
				int indentY = TEST_FLAG( pDrawItem->itemState, ODS_COMBOBOXEDIT ) ? 1 : 0;

				int leftBracketPosX = pDrawItem->rcItem.left + indentX;
				int leftBracketPosY = pDrawItem->rcItem.top + indentY;

				int textPosX = leftBracketPosX + 2;
				int textPosY = leftBracketPosY;

				SIZE stringDimensions;
				if ( bDrawIcon )
				{
					BOOL bSuccess = GetTextExtentPoint32( pDrawItem->hDC, szItem, lstrlen( szItem ), &stringDimensions );
					if ( !bSuccess )
					{
						stringDimensions.cx = pDrawItem->rcItem.right - pDrawItem->rcItem.left - 7;
					}
				}
				
				int rightBracketPosX = textPosX + stringDimensions.cx;
				int rightBracketPosY = leftBracketPosY;

				// draw
				ExtTextOut( pDrawItem->hDC
					, textPosX
					, textPosY
					, ETO_OPAQUE | ETO_CLIPPED
					, &pDrawItem->rcItem
					, szItem
					, lstrlen( szItem )
					, NULL
					);

				// draw item icon
				if ( bDrawIcon )
				{
					DrawIcon( pDrawItem->hDC, leftBracketPosX, leftBracketPosY, hLeftBracket );
					DrawIcon( pDrawItem->hDC, rightBracketPosX, rightBracketPosY, hRightBracket );
				}

				if( pDrawItem->itemState & ODS_SELECTED )
				{
					SetTextColor( pDrawItem->hDC, colorText );
					SetBkColor( pDrawItem->hDC, colorBack );
				}
			}

			// draw focus rect
			if( ( pDrawItem->itemAction & ODA_FOCUS ) || ( pDrawItem->itemState & ODS_FOCUS ) )
			{
				DrawFocusRect( pDrawItem->hDC, &pDrawItem->rcItem );
			}
			break;
		}

		case WM_COMMAND:
		{
			switch (hiWord)
			{		
				case LBN_SELCHANGE:
				{
					int ctrlId = loWord;
					return ctrlId == m_comboBoxIdc;
				}
			}
			break;
		}
	}
	return FALSE;
}

const ICComboBoxItem *ICComboBox::FindItem( int itemData ) const
{
	const ICComboBoxItem *pItem = NULL;

	for ( size_t itemId = 0; itemId < m_items.size(); ++itemId )
	{
		if ( m_items[itemId].GetData() == itemData )
		{
			pItem = &m_items[itemId];
			break;
		}
	}

	return pItem;
}

const ICComboBoxItem *ICComboBox::FindItem( const fm::string &name ) const
{
	const ICComboBoxItem *pItem = NULL;

	for ( size_t itemId = 0; itemId < m_items.size(); ++itemId )
	{
		if ( m_items[itemId].GetName() == name )
		{
			pItem = &m_items[itemId];
			break;
		}
	}

	return pItem;
}

// ----------------------------------------------------------------------
// ComboBox4
// ----------------------------------------------------------------------
void ICComboBox4::Construct( int labelIdc )
{
	m_labelIdc = labelIdc;
	m_childWidgets.resize( k_numChildWidgets );
}

void ICComboBox4::Init( HWND hWnd, void *pValue )
{
	for ( size_t childWidgetId = 0; childWidgetId < m_childWidgets.size(); ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		childWidget.Init( hWnd, pValue );
	}
}

void ICComboBox4::GetValue( HWND hWnd, void *pValue )
{
	Point4 &floatVector = *( Point4 * )pValue;
	
	int val[k_numChildWidgets];
	for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		childWidget.GetValue( hWnd, &val[childWidgetId] );
	}

	floatVector.Set( val[0], val[1], val[2], val[3] );
}

void ICComboBox4::SetValue( HWND hWnd, void *pValue )
{
	Point4 &floatVector = *( Point4 * )pValue;

	int val[k_numChildWidgets];
	val[0] = ( int )floatVector.x; val[1] = ( int )floatVector.y; val[2] = ( int )floatVector.z; val[3] = ( int )floatVector.w;
	for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		childWidget.SetValue( hWnd, &val[childWidgetId] );
	}
}

void ICComboBox4::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	// TODO
	//fm::vector<fm::vector<void *>> decomposedValues( k_numChildWidgets );
	//for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	//{
	//	decomposedValues[childWidgetId].resize( values.size() );
	//}

	//for ( size_t valueId = 0; valueId < values.size(); ++valueId )
	//{
	//	Point4 &currentValue = *( Point4 * )values[valueId];
	//	decomposedValues[0][valueId] = &currentValue.x;
	//	decomposedValues[1][valueId] = &currentValue.y;
	//	decomposedValues[2][valueId] = &currentValue.z;
	//	decomposedValues[3][valueId] = &currentValue.w;
	//}

	//for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	//{
	//	ICComboBox &childWidget = m_childWidgets[childWidgetId];
	//	childWidget.SetValue( hWnd, decomposedValues[childWidgetId] );
	//}
}

void ICComboBox4::SetEnabled( HWND hWnd, bool bEnabled )
{
	UIHelpers::SetControlEnabled( hWnd, m_labelIdc, bEnabled);
	for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		childWidget.SetEnabled( hWnd, bEnabled );
	}
}

BOOL ICComboBox4::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		if ( childWidget.DialogProc( hWnd, message, wParam, lParam ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void ICComboBox4::SetIndeterminate( HWND hWnd, bool bIndeterminate )
{
	for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		childWidget.SetIndeterminate( hWnd, bIndeterminate );
	}
}

void ICComboBox4::SetKeyFrameAtTimeFeedbackEnabled( HWND hWnd, bool bEnabled )
{
	for ( size_t childWidgetId = 0; childWidgetId < k_numChildWidgets; ++childWidgetId )
	{
		ICComboBox &childWidget = m_childWidgets[childWidgetId];
		childWidget.SetKeyFrameAtTimeFeedbackEnabled( hWnd, bEnabled );
	}	
}

// ----------------------------------------------------------------------
// EditText
// ----------------------------------------------------------------------
ICEditText::ICEditText( const fm::string &name )
	: ICWidget( EditText, name )
	, m_labelIdc( -1 )
	, m_editIdc( -1 )
	, m_resetVal( "" )
{
}

void ICEditText::Construct( int labelIdc, int editIdc, const fm::string &resetVal  )
{
	m_labelIdc = labelIdc;
	m_editIdc = editIdc;
	m_resetVal = resetVal;
}

void ICEditText::Init( HWND hWnd, void *pValue )
{
	SetValue( hWnd, pValue );
}

void ICEditText::SetEnabled( HWND hWnd, bool bEnabled )
{
	UIHelpers::SetControlEnabled( hWnd, m_editIdc, bEnabled);
	UIHelpers::SetControlEnabled( hWnd, m_labelIdc, bEnabled);
}

void ICEditText::GetValue( HWND hWnd, void *pValue )
{
	fm::string &ref = *( fm::string * )pValue;
	UIHelpers::GetCustEditText( hWnd, m_editIdc, ref );
}

void ICEditText::SetValue( HWND hWnd, void *pValue )
{
	fm::string &value = *( fm::string * )pValue;
	UIHelpers::SetCustEditText( hWnd, m_editIdc, value );
}

void ICEditText::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	SetValueT<fm::string>( hWnd, values );
}

BOOL ICEditText::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_CUSTEDIT_ENTER:
		{
			int ctrlId = wParam;
			return ctrlId == m_editIdc;
		}
	}

	return FALSE;
}

// ----------------------------------------------------------------------
// FileChooser
// ----------------------------------------------------------------------
ICFileChooser::ICFileChooser( const fm::string &name )
	: ICWidget( FileChooser, name )
	, m_buttonIdc( -1 )
{
}

void ICFileChooser::Construct( int labelIdc, int editIdc, int buttonIdc, const fm::string &resetVal  )
{
	m_editText.Construct( labelIdc, editIdc, resetVal );
	m_buttonIdc = buttonIdc;
}

void ICFileChooser::Init( HWND hWnd, void *pValue )
{
	m_editText.SetValue( hWnd, pValue );
}

void ICFileChooser::SetEnabled( HWND hWnd, bool bEnabled )
{
	m_editText.SetEnabled( hWnd, bEnabled );
	UIHelpers::SetControlEnabled( hWnd, m_buttonIdc, bEnabled);
}

void ICFileChooser::GetValue( HWND hWnd, void *pValue )
{
	m_editText.GetValue( hWnd, pValue );
}

void ICFileChooser::SetValue( HWND hWnd, void *pValue )
{
	m_editText.SetValue( hWnd, pValue );
}

void ICFileChooser::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	m_editText.SetValue( hWnd, values );
}

int ICFileChooser::OpenDialog( HWND hWnd, fm::string &filePath/*, const fm::string &initPath*/ )
{
	//FUAssert( initPath.size() >= MAX_PATH, return );

	OPENFILENAME ofn;
	char szFile[MAX_PATH];
	//HWND hwnd;
	HANDLE hf;

	//strcpy( szFile, initPath.c_str() );

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;

	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';

	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Executable Files\0*.EXE\0";
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	BOOL bSuccess = GetOpenFileName( &ofn ) == TRUE;
	if ( bSuccess )
	{
		filePath = szFile;
	}

	return bSuccess;
}

BOOL ICFileChooser::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	// edit text
	{
		BOOL bMessageProcessed = m_editText.DialogProc( hWnd, message, wParam, lParam );
		if ( bMessageProcessed )
		{
			return bMessageProcessed;
		}
	}

	// file open button
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_COMMAND:
		{
			if ( loWord == m_buttonIdc )
			{
				fm::string filePath;
				if ( OpenDialog( hWnd, filePath ) )
				{
					m_editText.SetValue( hWnd, &filePath );
				}
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

// ----------------------------------------------------------------------
// ColorChooser
// ----------------------------------------------------------------------
ICColorChooser::ICColorChooser( const fm::string &name )
	: ICWidget( ColorChooser, name )
	, m_labelIdc( -1 )
	, m_colorSwatchIdc( -1 )
	//, m_buttonIdc( -1 )
{
}

void ICColorChooser::Construct( int labelIdc, int editIdc/*, int buttonIdc, const fm::string &resetVal*/  )
{
	m_labelIdc = labelIdc;
	m_colorSwatchIdc = labelIdc;
	//m_editText.Construct( labelIdc, editIdc, resetVal );
	//m_buttonIdc = buttonIdc;
}

void ICColorChooser::Init( HWND hWnd, void *pValue )
{
	//UIHelpers::Init
	//m_editText.SetValue( hWnd, pValue );
}

void ICColorChooser::SetEnabled( HWND hWnd, bool bEnabled )
{
	//m_editText.SetEnabled( hWnd, bEnabled );
	//UIHelpers::SetControlEnabled( hWnd, m_buttonIdc, bEnabled);
}

void ICColorChooser::GetValue( HWND hWnd, void *pValue )
{
	//m_editText.GetValue( hWnd, pValue );
}

void ICColorChooser::SetValue( HWND hWnd, void *pValue )
{
	//m_editText.SetValue( hWnd, pValue );
}

void ICColorChooser::SetValue( HWND hWnd, fm::vector<void *> &values )
{
	//m_editText.SetValue( hWnd, values );
}

int ICColorChooser::OpenDialog( HWND hWnd, fm::string &filePath/*, const fm::string &initPath*/ )
{
	////FUAssert( initPath.size() >= MAX_PATH, return );

	//OPENFILENAME ofn;
	//char szColor[MAX_PATH];
	////HWND hwnd;
	//HANDLE hf;

	////strcpy( szColor, initPath.c_str() );

	//// Initialize OPENFILENAME
	//ZeroMemory(&ofn, sizeof(ofn));
	//ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = hWnd;

	//ofn.lpstrColor = szColor;
	//// Set lpstrColor[0] to '\0' so that GetOpenColorName does not 
	//// use the contents of szColor to initialize itself.
	//ofn.lpstrColor[0] = '\0';

	//ofn.nMaxColor = sizeof(szColor);
	//ofn.lpstrFilter = "Executable Colors\0*.EXE\0";
	//ofn.nFilterIndex = 0;
	//ofn.lpstrColorTitle = NULL;
	//ofn.nMaxColorTitle = 0;
	//ofn.lpstrInitialDir = NULL;
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	//// Display the Open dialog box. 
	//BOOL bSuccess = GetOpenColorName( &ofn ) == TRUE;
	//if ( bSuccess )
	//{
	//	filePath = szColor;
	//}

	//return bSuccess;
	return FALSE;
}

BOOL ICColorChooser::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	//// file open button
	//int loWord = LOWORD( wParam );
	//int hiWord = HIWORD( wParam );

	//switch (message)
	//{
	//	case WM_COMMAND:
	//	{
	//		if ( loWord == m_buttonIdc )
	//		{
	//			fm::string filePath;
	//			if ( OpenDialog( hWnd, filePath ) )
	//			{
	//				m_editText.SetValue( hWnd, &filePath );
	//			}
	//			return TRUE;
	//		}
	//		break;
	//	}
	//}

	return FALSE;
}