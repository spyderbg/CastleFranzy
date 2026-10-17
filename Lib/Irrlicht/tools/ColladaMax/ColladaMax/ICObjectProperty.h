/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ICOBJECT_PROPERTY_H_
#define _ICOBJECT_PROPERTY_H_

#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDocumentTools.h"
#include "ICWidget.h"

class INode;

class ICObjectProperty
{
public:
	enum
	{
		INT
	};

protected:
	int m_type;
	ICWidget *m_pWidget;
	int m_chunkId;

public:
	ICObjectProperty( int type, int widgetType, int chunkId );
	~ICObjectProperty();

	inline int GetType() const
	{
		return m_type;
	}

	inline ICWidget &GetWidgetRef() const
	{
		return *m_pWidget;
	}

	virtual void SetWidgetValueFromDataChunk( HWND hWnd, fm::pvector<INode> &selectedNodes ) = 0;
	virtual void SetDataChunkFromWidgetValue( HWND hWnd, fm::pvector<INode> &selectedNodes ) = 0;
	virtual void WriteExtraNodeContentFromAppDataChunk( INode *pNode, FCDENode *pExtraNode, FCDocumentTools::FCDConversionSwapFunctor *pConversionSwapFunctor = NULL ) const = 0;
	virtual void CloneAppDataChunk( INode *pOrigNode, INode *pClonedNode ) const = 0;
	virtual void InitWidget( HWND hWnd ) = 0;
	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

protected:
	virtual int GetChunkSize() const = 0;

public:
	AppDataChunk *GetAppDataChunk( INode *pNode, void *pDefaultValue ) const;
	void SetAppDataChunkData( INode *pNode, void *pDefaultValue, void *pValue ) const;
	
	inline int GetChunkId() const
	{
		return m_chunkId;
	}
};

template <class T>
class ICObjectPropertyT : public ICObjectProperty
{
protected:
	T m_defaultValue;

public:
	ICObjectPropertyT( int widgetType, int chunkId, const T &defaultValue )
		: ICObjectProperty( ICObjectProperty::INT, widgetType, chunkId )
		, m_defaultValue( defaultValue )
	{}

	virtual void SetWidgetValueFromDataChunk( HWND hWnd, fm::pvector<INode> &selectedNodes )
	{
		int chunkSize = GetChunkSize();
		ClassDesc2 *pClassDesc = GetIrrColladaClassDesc();
		size_t numSelectedNodes = selectedNodes.size();
		fm::vector<void *> nodeValues;

		for ( size_t nodeId = 0; nodeId < numSelectedNodes; ++nodeId )
		{
			INode *pNode = selectedNodes[nodeId];
			AppDataChunk *pDataChunk = GetAppDataChunk( pNode, &m_defaultValue );
			nodeValues.push_back( pDataChunk->data );
		}

		m_pWidget->SetValue( hWnd, nodeValues );
	}
	
	virtual void SetDataChunkFromWidgetValue( HWND hWnd, fm::pvector<INode> &selectedNodes )
	{
		T value;
		m_pWidget->GetValue( hWnd, &value );

		int numSelectedNodes = selectedNodes.size();
		for ( size_t nodeId = 0; nodeId < selectedNodes.size(); ++nodeId )
		{
			INode *pNode = selectedNodes[nodeId];
			SetAppDataChunkData( pNode, &m_defaultValue, &value );
		}
	}
	
	virtual void WriteExtraNodeContentFromAppDataChunk( INode *pNode, FCDENode *pExtraNode, FCDocumentTools::FCDConversionSwapFunctor *pConversionSwapFunctor ) const
	{
		ClassDesc2 *pClassDesc = GetIrrColladaClassDesc();
		int chunkSize = GetChunkSize();

		AppDataChunk *pDataChunk = GetAppDataChunk( pNode, NULL );
		T &value = const_cast<T &>( pDataChunk ? *( T * )pDataChunk->data : m_defaultValue );

		// swizzle hack
		if ( pConversionSwapFunctor )
		{
			( *pConversionSwapFunctor )( value );
		}

		// enum hack
		if ( m_pWidget->GetType() == ICWidget::ComboBox )
		{
			ICComboBox &comboBoxRef = *( ICComboBox *)m_pWidget;
			const ICComboBoxItem *pItem = comboBoxRef.FindItem( value );
			if ( pItem )
			{
				pExtraNode->SetContent( pItem->GetName() );
				return;
			}
		}
		
		pExtraNode->SetContent( TO_FSTRING( value ) );
	}

	virtual void CloneAppDataChunk( INode *pOrigNode, INode *pClonedNode ) const
	{
		FUAssert( pOrigNode, return );
		FUAssert( pClonedNode, return );

		AppDataChunk *pDataChunk = GetAppDataChunk( pOrigNode, NULL );
		if ( pDataChunk && pDataChunk->length == GetChunkSize() )
		{
			SetAppDataChunkData( pClonedNode, ( void * )&m_defaultValue, pDataChunk->data );
		}
	}
	
	virtual void InitWidget( HWND hWnd )
	{
		m_pWidget->Init( hWnd, &m_defaultValue );
	}
	
protected:

	virtual int GetChunkSize() const { return sizeof( T ); }
};

#endif // _ICOBJECT_PROPERTY_H_


