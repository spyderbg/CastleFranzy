#include "StdAfx.h"
#include "ICObjectProperty.h"
#include "ICWidget.h"
#include "MAXHelpers.h"
#include "IrrCollada.h"

ICObjectProperty::ICObjectProperty( int type, int widgetType, int chunkId )
: m_type( type )
, m_chunkId( chunkId )
{
	m_pWidget = ICWidgetFactory::Create( ( ICWidget::Type )widgetType );
}


ICObjectProperty::~ICObjectProperty()
{
	SAFE_DELETE( m_pWidget );
}

BOOL ICObjectProperty::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		case WM_INITDIALOG: 
		{
			break;
		}
		default:
		{
			if ( m_pWidget->DialogProc( hWnd, message, wParam, lParam ) )
			{
				fm::pvector<INode> &selectedNodes = GetIrrColladaPtr()->GetSelectedNodes();
				SetDataChunkFromWidgetValue( hWnd, selectedNodes );
				break;
			}
			return FALSE;
		}
	}

	return TRUE;
}

AppDataChunk *ICObjectProperty::GetAppDataChunk( INode *pNode, void *pDefaultValue ) const
{
	ClassDesc2 *pClassDesc = GetIrrColladaClassDesc();
	int chunkSize = GetChunkSize();

	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
		, pClassDesc->ClassID()
		, pClassDesc->SuperClassID()
		, m_chunkId
		, chunkSize
		, pDefaultValue
		);	

	return pDataChunk;
}

void ICObjectProperty::SetAppDataChunkData( INode *pNode, void *pDefaultValue, void *pValue ) const
{
	ClassDesc2 *pClassDesc = GetIrrColladaClassDesc();
	int chunkSize = GetChunkSize();

	MAXHelpers::SetAppDataChunkData( pNode
		, pClassDesc->ClassID()
		, pClassDesc->SuperClassID()
		, m_chunkId
		, chunkSize
		, pDefaultValue
		, pValue 
		);
}
