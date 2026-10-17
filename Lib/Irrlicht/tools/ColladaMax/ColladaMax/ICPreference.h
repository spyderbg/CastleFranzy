/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PREFERENCE_H_
#define _IC_PREFERENCE_H_

#include "ICWidget.h"

//
//class INode;
//
class ICPreference
{
public:
	enum PreferenceFlags
	{
		PF_DIRTY = 0x00000001
	};

private:
	fm::string m_xmlElementName;

protected:
	ICWidget *m_pWidget;
	int m_flags;

public:
	ICPreference( int widgetType, const fm::string &xmlElementName );
	~ICPreference();

	inline ICWidget &GetWidgetRef() const
	{
		return *m_pWidget;
	}

	inline const fm::string &GetXMLElementName()
	{
		return m_xmlElementName;
	}

	inline int GetFlags()
	{
		return m_flags;
	}

	inline void SetFlags( int flags )
	{
		m_flags = flags;
	}

	//virtual void SetWidgetValueFromDataChunk( HWND hWnd, fm::pvector<INode> &selectedNodes ) = 0;
	virtual void SetValueFromWidget( HWND hWnd ) = 0;
	//virtual void WriteExtraNodeContentFromAppDataChunk( INode *pNode, FCDENode *pExtraNode, FCDocumentTools::FCDConversionSwapFunctor *pConversionSwapFunctor = NULL ) const = 0;
	//virtual void CloneAppDataChunk( INode *pOrigNode, INode *pClonedNode ) const = 0;
	virtual void InitWidget( HWND hWnd ) = 0;
	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual void SetValueFromString( const fm::string &str ) = 0;
	virtual void SetStringFromValue( fm::string &str ) = 0;
	virtual void SetFileValue( void *value ) = 0;

//protected:
//	virtual int GetChunkSize() const = 0;
//
//public:
//	AppDataChunk *GetAppDataChunk( INode *pNode, void *pDefaultValue ) const;
//	void SetAppDataChunkData( INode *pNode, void *pDefaultValue, void *pValue ) const;
//	
//	inline int GetChunkId() const
//	{
//		return m_chunkId;
//	}
};

typedef fm::pvector<ICPreference> PreferenceList;

template <class T>
class ICPreferenceT : public ICPreference
{
protected:
	T m_defaultValue;
	T m_value;
	T m_fileValue;

public:
	ICPreferenceT( int widgetType, const T &defaultValue, const fm::string &xmlElementName )
		: ICPreference( widgetType, xmlElementName )
		, m_value( defaultValue )
		, m_defaultValue( defaultValue )
		, m_fileValue( defaultValue )
	{}

	virtual void InitWidget( HWND hWnd )
	{
		m_pWidget->Init( hWnd, &m_value );
	}

	virtual void SetFileValue( void *value )
	{
		m_fileValue = *( T * )value;
	}

	virtual void SetValueFromString( const fm::string &str )
	{
		// TODO for other T than fm::string
		SetValue( str );
	}

	virtual void SetStringFromValue( fm::string &str )
	{
		str = TO_FSTRING( m_value );
	}

	virtual void SetValueFromWidget( HWND hWnd )
	{
		T value;
		m_pWidget->GetValue( hWnd, &value );
		SetValue( value );
	}

private:

	void SetValue( const T &value )
	{
		m_value = value;
		if ( IsEquivalent( m_value, m_fileValue ) )
		{
			RESET_FLAG( m_flags, PF_DIRTY );
		}
		else
		{
			SET_FLAG( m_flags, PF_DIRTY );
		}
	}
};

#endif // _IC_PREFERENCE_H_


