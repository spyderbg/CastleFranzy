#include "StdAfx.h"
#include "ICObjectPropertyManager.h"
#include "IrrCollada.h"
#include "resource.h"
#include "ICWidget.h"
#include "ICPanel.h"
#include "DrawLayerManager.h"
#include "UIHelpers.h"
#include "MAXHelpers.h"
#include "FUtils/FUDaeEnum.h"

ICObjectPropertyManager::~ICObjectPropertyManager()
{
	Release();
}

void ICObjectPropertyManager::Release()
{
	ICUIManager::Release();
	CLEAR_POINTER_VECTOR( m_objectProperties );
	CLEAR_POINTER_VECTOR( m_panels );
}

void ICObjectPropertyManager::Init()
{
	Release();

	ICObjectPropertyPanel *pPanel = NULL;

	// ----------------------------------------------------------------------
	// Standard Object Properties Panel
	// ----------------------------------------------------------------------
	pPanel = new ICObjectPropertyPanel( IDD_GENERAL_PROPERTIES_ROLLUP, IDS_GENERAL_PROPERTIES_ROLLUP_TITLE );
	m_panels.push_back( pPanel );

	// draw layer
	{
		ICObjectProperty *pObjectProperty = new ICObjectPropertyT<int>( ICWidget::SpinnerInt, IrrCollada::CID_DRAW_LAYER, 0 );
		ICSpinner &spinnerRef = ( ICSpinner & ) pObjectProperty->GetWidgetRef();
		spinnerRef.Construct( IDC_OP_DRAW_LAYER_LABEL, IDC_OP_DRAW_LAYER_EDIT, IDC_OP_DRAW_LAYER_SPIN, (float)SHRT_MIN, (float)SHRT_MAX, 1.f, 0.f );
		m_objectProperties.push_back( pObjectProperty );
		pPanel->AddObjectProperty( pObjectProperty );
	}

	// visibility
	//{
	//	ICObjectProperty *pObjectProperty = new ICObjectPropertyT<int>( ICWidget::SpinnerInt, IrrCollada::CID_VISIBILITY, 0 );
	//	ICSpinner &spinnerRef = ( ICSpinner & ) pObjectProperty->GetWidgetRef();
	//	spinnerRef.Construct( IDC_OP_VISIBILITY_LABEL, IDC_OP_VISIBILITY_EDIT, IDC_OP_VISIBILITY_SPIN, 0.f, 1.f, 1.f, 0.f );
	//	m_objectProperties.push_back( pObjectProperty );
	//	pPanel->AddObjectProperty( pObjectProperty );
	//}
	
	// old billboard implementation remnants
	// good examples of ObjectPropery + ICWidget use
	// ----------------------------------------------------------------------
	// Billboard Properties Panel
	// ----------------------------------------------------------------------
	//pPanel = new ICObjectPropertyPanel( IDD_BILLBOARD_ROLLUP, IDS_BILLBOARD_ROLLUP_TITLE );
	//m_panels.push_back( pPanel );

//#if ENABLE_IRRCOLLADA_BILLBOARD
//
//	// billboard type
//	{
//		ICObjectProperty *pObjectProperty = new ICObjectPropertyT<int>( ICWidget::ComboBox, IrrCollada::CID_BILLBOARD_TYPE, 0 );
//		ICComboBox &comboBoxRef = ( ICComboBox & ) pObjectProperty->GetWidgetRef();
//		comboBoxRef.Construct( IDC_BILLBOARD_TYPE_LABEL, IDC_BILLBOARD_TYPE_COMBOBOX );
//		for ( int bbType = 0; bbType < FUDaeBillboard::BT_COUNT; ++bbType )
//		{
//			comboBoxRef.AddItem( ICComboBoxItem( fm::string( FUDaeBillboard::ToStringBillboardType( ( FUDaeBillboard::BillboardType )bbType ) ).ToUpper(), bbType ) );
//		}		
//		m_objectProperties.push_back( pObjectProperty );
//		pPanel->AddObjectProperty( pObjectProperty );
//	}
//
//	// billboard orientation
//	{
//		ICObjectProperty *pObjectProperty = new ICObjectPropertyT<int>( ICWidget::ComboBox, IrrCollada::CID_BILLBOARD_ORIENTATION_TYPE, 0 );
//		ICComboBox &comboBoxRef = ( ICComboBox & ) pObjectProperty->GetWidgetRef();
//		comboBoxRef.Construct( IDC_BILLBOARD_ORIENTATION_LABEL, IDC_BILLBOARD_ORIENTATION_COMBOBOX );
//		for ( int enumId = 0; enumId < FUDaeBillboard::OT_COUNT; ++enumId )
//		{
//			comboBoxRef.AddItem( ICComboBoxItem( fm::string( FUDaeBillboard::ToStringOrientationType( ( FUDaeBillboard::OrientationType )enumId ) ).ToUpper(), enumId ) );
//		}		
//		m_objectProperties.push_back( pObjectProperty );
//		pPanel->AddObjectProperty( pObjectProperty );
//	}
//
//	// example float spinner
//	//{
//	//	ICObjectProperty *pObjectProperty = new ICObjectPropertyT<float>( ICWidget::SpinnerFloat, IrrCollada::CID_TEST_CHUNK, 0.f );
//	//	ICSpinner &spinnerRef = ( ICSpinner & ) pObjectProperty->GetWidgetRef();
//	//	spinnerRef.Construct( IDC_BILLBOARD_TEST_LABEL, IDC_BILLBOARD_TEST_EDIT, IDC_BILLBOARD_TEST_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 0.f );
//	//	m_objectProperties.push_back( pObjectProperty );
//	//	pPanel->AddObjectProperty( pObjectProperty );
//	//}
//
//	// front vector
//	{
//		ICObjectProperty *pObjectProperty = new ICObjectPropertyT<FMVector3>( ICWidget::SpinnerFloat3, IrrCollada::CID_BILLBOARD_FRONT, FMVector3( 0.f, 1.f, 0.f ) );
//		ICSpinnerFloat3 &spinnerRef = ( ICSpinnerFloat3 & ) pObjectProperty->GetWidgetRef();
//		
//		spinnerRef.Construct( IDC_BILLBOARD_FRONT_LABEL );
//
//		{
//			ICSpinnerFloat &childSpinnerRef = spinnerRef.GetChildSpinnerRef( 0 );
//			childSpinnerRef.Construct( IDC_BILLBOARD_FRONT_X_LABEL, IDC_BILLBOARD_FRONT_X_EDIT, IDC_BILLBOARD_FRONT_X_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 0.f );
//		}
//
//		{
//			ICSpinnerFloat &childSpinnerRef = spinnerRef.GetChildSpinnerRef( 1 );
//			childSpinnerRef.Construct( IDC_BILLBOARD_FRONT_Y_LABEL, IDC_BILLBOARD_FRONT_Y_EDIT, IDC_BILLBOARD_FRONT_Y_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 1.f );
//		}
//
//		{
//			ICSpinnerFloat &childSpinnerRef = spinnerRef.GetChildSpinnerRef( 2 );
//			childSpinnerRef.Construct( IDC_BILLBOARD_FRONT_Z_LABEL, IDC_BILLBOARD_FRONT_Z_EDIT, IDC_BILLBOARD_FRONT_Z_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 0.f );
//		}
//
//		m_objectProperties.push_back( pObjectProperty );
//		pPanel->AddObjectProperty( pObjectProperty );
//	}
//
//	// up vector
//	{
//		ICObjectProperty *pObjectProperty = new ICObjectPropertyT<FMVector3>( ICWidget::SpinnerFloat3, IrrCollada::CID_BILLBOARD_UP, FMVector3( 0.f, 0.f, 1.f ) );
//		ICSpinnerFloat3 &spinnerRef = ( ICSpinnerFloat3 & ) pObjectProperty->GetWidgetRef();
//		
//		spinnerRef.Construct( IDC_BILLBOARD_UP_LABEL );
//
//		{
//			ICSpinnerFloat &childSpinnerRef = spinnerRef.GetChildSpinnerRef( 0 );
//			childSpinnerRef.Construct( IDC_BILLBOARD_UP_X_LABEL, IDC_BILLBOARD_UP_X_EDIT, IDC_BILLBOARD_UP_X_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 0.f );
//		}
//
//		{
//			ICSpinnerFloat &childSpinnerRef = spinnerRef.GetChildSpinnerRef( 1 );
//			childSpinnerRef.Construct( IDC_BILLBOARD_UP_Y_LABEL, IDC_BILLBOARD_UP_Y_EDIT, IDC_BILLBOARD_UP_Y_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 0.f );
//		}
//
//		{
//			ICSpinnerFloat &childSpinnerRef = spinnerRef.GetChildSpinnerRef( 2 );
//			childSpinnerRef.Construct( IDC_BILLBOARD_UP_Z_LABEL, IDC_BILLBOARD_UP_Z_EDIT, IDC_BILLBOARD_UP_Z_SPIN, -FLT_MAX, FLT_MAX, UIHelpers::k_autoScale, 1.f );
//		}
//
//		m_objectProperties.push_back( pObjectProperty );
//		pPanel->AddObjectProperty( pObjectProperty );
//	}
//
//#endif // ENABLE_IRRCOLLADA_BILLBOARD
}

void ICObjectPropertyManager::DisplayDialog( BOOL show )
{
	if ( show )
	{
		HWND hWnd = GetHWnd();
		if ( !hWnd )
		{
			hWnd = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_OBJECT_PROPERTIES_UTILITY_DIALOG), GetCOREInterface()->GetMAXHWnd(), DLG_PROCS_NAME( ICObjectPropertyDialog ), (LPARAM)this );
			GetCOREInterface()->RegisterDlgWnd( hWnd ); // Max doc recommends to call this for floating dialogs
		}
		else
		{
			WINDOWINFO windowInfo;
			int retVal;
			if ( !(retVal = GetWindowInfo( hWnd, &windowInfo )))
			{
				return;
			}

			SetActiveWindow( hWnd );
			if ( TEST_FLAG( windowInfo.dwStyle, WS_MINIMIZE ) )
			{
				ShowWindow( hWnd, SW_RESTORE );
			}
		}
	}
}

void ICObjectPropertyManager::RefreshDialog()
{
	HWND hWnd = GetHWnd();
	if ( hWnd )
	{
		// dialog caption
		fm::string caption = ConstructDialogCaption();
		SetWindowText( hWnd, caption.c_str() );

		fm::pvector<INode> &selectedNodes = m_pIrrCollada->GetSelectedNodes();
		int numSelectedNodes = selectedNodes.size();

		// hack in the meantime dialog management is cleaned up
		HWND generalPropertiesHWnd = GetDlgItem( hWnd, IDC_OBJECT_PROPERTIES_ROLLUP );
		IRollupWindow *pRollup = GetIRollup( generalPropertiesHWnd );

		for ( size_t panelId = 0; panelId < m_panels.size(); ++panelId )
		{
			ICObjectPropertyPanel &panel = *m_panels[panelId];
			int panelRollupId = panel.GetPanelRollupId();
			HWND panelHWnd = pRollup->GetPanelDlg( panelRollupId );

			size_t numObjectProperties = panel.GetNumObjectProperties();
			for ( size_t propId = 0; propId < numObjectProperties; ++propId )
			{
				ICObjectProperty &prop = panel.GetObjectPropertyRef( propId );
				prop.SetWidgetValueFromDataChunk( panelHWnd, selectedNodes );
				prop.GetWidgetRef().SetEnabled( panelHWnd, numSelectedNodes > 0 );
			}
		}
		
		ReleaseIRollup( pRollup );
	}
}

DLG_PROC_IMPL( ICObjectPropertyManager, ICObjectPropertiesRollout )
{
	bool bMessageProcessed = false;
	for ( size_t propId = 0; propId < m_objectProperties.size(); ++propId )
	{
		ICObjectProperty &prop = *m_objectProperties[propId];
		if ( prop.DialogProc( hWnd, message, wParam, lParam ) )
		{
			bMessageProcessed = true;
			break;
		}
	}

	return bMessageProcessed;
}

DLG_PROC_IMPL( ICObjectPropertyManager, ICObjectPropertyDialog )
{
	ICUIManager::MainDlgProc( hWnd, message, wParam, lParam );

	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_INITDIALOG: 
		{
			HWND generalPropertiesHWnd = GetDlgItem( hWnd, IDC_OBJECT_PROPERTIES_ROLLUP );
			IRollupWindow *pRollup = GetIRollup( generalPropertiesHWnd );

			for ( size_t panelId = 0; panelId < m_panels.size(); ++panelId )
			{
				m_panels[panelId]->Init( pRollup, DLG_PROCS_NAME( ICObjectPropertiesRollout ), this );	
			}

			pRollup->Show();
			ReleaseIRollup( pRollup );

			RefreshDialog();
			return TRUE;
		}
	}
	return FALSE;
}

fm::string ICObjectPropertyManager::ConstructDialogCaption()
{
	FUStringBuilder strBuilder;

	fm::pvector<INode> &selectedNodes = m_pIrrCollada->GetSelectedNodes();

	strBuilder.append( GetString( IDS_IC_SUBMENU_WINDOW_TITLE ) );
	strBuilder.append( " - " );
	strBuilder.append( GetString( IDS_IC_OBJECT_PROPERTIES_WINDOW_TITLE ) );

	size_t numSelectedNodes = selectedNodes.size();

	if ( numSelectedNodes > 0 )
	{
		strBuilder.append( " [ " );

		strBuilder.append( selectedNodes[0]->GetName() );
		for ( size_t nodeId = 1; nodeId < numSelectedNodes; ++nodeId )
		{
			strBuilder.append( " , " );
			strBuilder.append( selectedNodes[nodeId]->GetName() );
		}

		strBuilder.append( " ] " );
	}

	return strBuilder.ToString();
}

void ICObjectPropertyManager::PostCloneNotification( INodeTab *pOrigNodes, INodeTab *pClonedNodes, CloneType cloneType )
{
	FUAssert( pOrigNodes, return );
	FUAssert( pClonedNodes, return );
	FUAssert( pOrigNodes->Count() == pClonedNodes->Count(), return );

	for ( int nodePairId = 0; nodePairId < pOrigNodes->Count(); ++nodePairId )
	{
		INode *pOrigNode = (*pOrigNodes)[nodePairId];
		INode *pClonedNode = (*pClonedNodes)[nodePairId];

		for ( size_t propId = 0; propId < m_objectProperties.size(); ++propId )
		{
			m_objectProperties[propId]->CloneAppDataChunk( pOrigNode, pClonedNode );
		}
	}
}

const ICObjectProperty *ICObjectPropertyManager::GetObjectPropertyPtrByCID( const int cid ) const
{
	if ( cid < 0 || cid >= IrrCollada::CID_COUNT )
	{
		return NULL;
	}

	const ICObjectProperty *pReturnedObjectProperty = NULL;
	for ( size_t propId = 0; m_objectProperties.size(); ++propId )
	{
		const ICObjectProperty *pObjectProperty = m_objectProperties[propId];
		if ( cid == pObjectProperty->GetChunkId() )
		{
			pReturnedObjectProperty = pObjectProperty;
			break;
		}
	}

	return pReturnedObjectProperty;
}