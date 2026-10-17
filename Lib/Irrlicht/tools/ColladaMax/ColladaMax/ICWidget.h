/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ICWIDGET_H_
#define _ICWIDGET_H_

// ----------------------------------------------------------------------
// Widget
// ----------------------------------------------------------------------
class ICWidget
{
public:
	enum Type
	{
		SpinnerInt
		, SpinnerFloat
		, SpinnerFloat3
		, ComboBox
		, ComboBox4
		, EditText
		, FileChooser
		, ColorChooser
	};

private:
	Type m_type;
	fm::string m_name;

protected:
	bool m_bIndeterminate;

public:
	ICWidget( Type type, const fm::string &name = fm::string() )
		: m_type( type )
		, m_bIndeterminate( false )
		, m_name( name )
	{}

	inline Type GetType() const
	{
		return m_type;
	}

	inline const fm::string &GetName() const
	{
		return m_name;
	}

	inline void SetName( const fm::string &name )
	{
		m_name = name;
	}

	inline virtual void SetIndeterminate( HWND UNUSED(hWnd), bool bIndeterminate )
	{
		m_bIndeterminate = bIndeterminate;
	}

	inline virtual bool IsIndeterminate( HWND UNUSED(hWnd) )
	{
		return m_bIndeterminate;
	}

	virtual void Init( HWND hWnd, void *pValue ) = 0;
	virtual void GetValue( HWND hWnd, void *pValue ) = 0;
	virtual void SetValue( HWND hWnd, void *pValue ) = 0;
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values ) = 0;
	virtual void SetEnabled( HWND hWnd, bool bEnabled ) = 0;

	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;

	virtual void SetKeyFrameAtTimeFeedbackEnabled( HWND hWnd, bool bEnabled ){}

	template <class T>
	void SetValueT( HWND hWnd, fm::vector<void *> &values )
	{
		bool bIsIndeterminate = true;
		T widgetValue = T();

		for ( size_t valueId = 0; valueId < values.size(); ++valueId )
		{
			const T &currentValue = *( T * )values[valueId];
			if ( valueId == 0 )
			{
				bIsIndeterminate = false;
				widgetValue = currentValue;
			}
			else
			{
				if ( !IsEquivalent( widgetValue , currentValue ) )
				{
					bIsIndeterminate = true;
					//widgetValue = m_defaultValue;
					break;
				}
			}
		}

		SetIndeterminate( hWnd, bIsIndeterminate );
		if ( !bIsIndeterminate )
		{
			SetValue( hWnd, &widgetValue );
		}
	}
};

// ----------------------------------------------------------------------
// Spinners
// ----------------------------------------------------------------------
class ICSpinner
	: public ICWidget
{
protected:
	EditSpinnerType m_editType;
	int m_labelIdc;
	int m_editIdc;
	int m_spinnerIdc;
	float m_minVal;
	float m_maxVal;
	float m_scale;
	float m_resetVal;
	bool m_bButtonDown;

public:
	ICSpinner( Type type, EditSpinnerType editType, const fm::string &name = fm::string() );

	virtual void Init( HWND hWnd, void *pValue );
	virtual void GetValue( HWND hWnd, void *pValue ) = 0;
	virtual void SetValue( HWND hWnd, void *pValue ) = 0;
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values ) = 0;
	virtual void SetEnabled( HWND hWnd, bool bEnabled );
	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual void SetIndeterminate( HWND hWnd, bool bIndeterminate );
	//virtual void BuildDialogTemplateItem();
	
	void SetKeyFrameAtTimeFeedbackEnabled( HWND hWnd, bool bEnabled );

	// @params :
	//  - scale : Use UIHelpers::k_autoScale to specify automatic scaling ( see ISpinnerControl::SetAutoScale for details )
	void Construct( int labelIdc, int editIdc, int spinnerIdc, float minVal, float maxVal, float scale, float resetVal );

	inline int GetLabelIdc() const
	{
		return m_labelIdc;
	}

	inline int GetEditIdc() const
	{
		return m_editIdc;
	}

	inline int GetSpinnerIdc() const
	{
		return m_spinnerIdc;
	}
};

class ICSpinnerInt
	: public ICSpinner
{
protected:

public:
	ICSpinnerInt( const fm::string &name = fm::string() );

	virtual void GetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values );
};

class ICSpinnerFloat
	: public ICSpinner
{
protected:

public:
	ICSpinnerFloat( const fm::string &name = fm::string() );

	virtual void GetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values );
};

class ICSpinnerFloat3
	: public ICWidget
{
protected:
	static const int k_numChildSpinners = 3;

protected:
	int m_labelIdc;
	fm::vector<ICSpinnerFloat> m_childSpinners;

public:
	ICSpinnerFloat3( const fm::string &name = fm::string() )
		: ICWidget( SpinnerFloat3, name )
		, m_labelIdc( -1 )
	{}

	virtual void Init( HWND hWnd, void *pValue );
	virtual void GetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values );
	virtual void SetEnabled( HWND hWnd, bool bEnabled );
	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual void SetIndeterminate( HWND hWnd, bool bIndeterminate );

	virtual void Construct( int labelIdc );

	inline ICSpinnerFloat &GetChildSpinnerRef( size_t index )
	{
		return m_childSpinners[index];
	}
};

// ----------------------------------------------------------------------
// ComboBoxes
// ----------------------------------------------------------------------
class ICComboBoxItem
{
private:
	fm::string m_name;
	int m_data;

public:
	ICComboBoxItem( const fm::string &name = "", int itemData = 0 )
		: m_name( name )
		, m_data( itemData )
	{}

public:

	inline const fm::string &GetName() const
	{
		return m_name;
	}

	inline int GetData() const
	{
		return m_data;
	}
};

typedef fm::vector<ICComboBoxItem> ComboBoxItemList;

class ICComboBox
	: public ICWidget
{
protected:
	int m_labelIdc;
	int m_comboBoxIdc;
	bool m_bKeyFrameAtTimeFeedbackEnabled;

	ComboBoxItemList m_items;

public:
	ICComboBox( const fm::string &name = fm::string() );

	virtual void Init( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, void *pValue );
	virtual void GetValue( HWND hWnd, void *pValue );
	virtual void SetEnabled( HWND hWnd, bool bEnabled );
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values );

	// returns true if user modified widget value
	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	void Construct( int labelIdc, int comboBoxIdc );
	
	size_t AddItem( const ICComboBoxItem &comboBoxItem );
	const ICComboBoxItem *FindItem( int itemData ) const;
	const ICComboBoxItem *FindItem( const fm::string &name ) const;

	// enum hack
	inline const ICComboBoxItem *FindItem( const FMVector3 &vector ){ return NULL; }

	inline void SetKeyFrameAtTimeFeedbackEnabled( HWND hWnd, bool bEnabled )
	{
		m_bKeyFrameAtTimeFeedbackEnabled = bEnabled;
	}

	inline int GetLabelIdc() const
	{
		return m_labelIdc;
	}

	inline int GetComboBoxIdc() const
	{
		return m_comboBoxIdc;
	}
};

class ICComboBox4
	: public ICWidget
{
public:
	static const int k_numChildWidgets = 4;

protected:
	int m_labelIdc;
	fm::vector<ICComboBox> m_childWidgets;

public:
	ICComboBox4( const fm::string &name = fm::string() )
		: ICWidget( ComboBox4, name )
		, m_labelIdc( -1 )
	{}

	virtual void Init( HWND hWnd, void *pValue );
	virtual void GetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, void *pValue );
	virtual void SetValue( HWND hWnd, fm::vector<void *> &values );
	virtual void SetEnabled( HWND hWnd, bool bEnabled );
	virtual BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual void SetIndeterminate( HWND hWnd, bool bIndeterminate );

	virtual void Construct( int labelIdc );

	void SetKeyFrameAtTimeFeedbackEnabled( HWND hWnd, bool bEnabled );

	inline ICComboBox &GetChildWidgetRef( size_t index )
	{
		return m_childWidgets[index];
	}

	inline int GetLabelIdc() const
	{
		return m_labelIdc;
	}
};

class ICEditText
	: public ICWidget
{
protected:
	int m_labelIdc;
	int m_editIdc;
	fm::string m_resetVal;

public:
	ICEditText( const fm::string &name = fm::string() );

	void Init( HWND hWnd, void *pValue );
	void GetValue( HWND hWnd, void *pValue );
	void SetValue( HWND hWnd, void *pValue );
	void SetValue( HWND hWnd, fm::vector<void *> &values );
	void SetEnabled( HWND hWnd, bool bEnabled );
	BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	void Construct( int labelIdc, int editIdc, const fm::string &resetVal );
};

class ICFileChooser
	: public ICWidget
{
protected:
	ICEditText m_editText;
	int m_buttonIdc;

public:
	ICFileChooser( const fm::string &name = fm::string() );

	void Init( HWND hWnd, void *pValue );
	void GetValue( HWND hWnd, void *pValue );
	void SetValue( HWND hWnd, void *pValue );
	void SetValue( HWND hWnd, fm::vector<void *> &values );
	void SetEnabled( HWND hWnd, bool bEnabled );
	BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	void Construct( int labelIdc, int editIdc, int buttonIdc, const fm::string &resetVal );

	// returns 0 and file chosen in filePath if success, returns -1 if error.
	int OpenDialog( HWND hWnd, fm::string &filePath/*, const fm::string &initPath*/ );
};

class ICColorChooser
	: public ICWidget
{
protected:
	ICEditText m_editText;
	int m_labelIdc;
	int m_colorSwatchIdc;
	//int m_buttonIdc;

public:
	ICColorChooser( const fm::string &name = fm::string() );

	void Init( HWND hWnd, void *pValue );
	void GetValue( HWND hWnd, void *pValue );
	void SetValue( HWND hWnd, void *pValue );
	void SetValue( HWND hWnd, fm::vector<void *> &values );
	void SetEnabled( HWND hWnd, bool bEnabled );
	BOOL DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	void Construct( int labelIdc, int editIdc/*, int buttonIdc, const fm::string &resetVal*/ );

	// returns 0 and file chosen in filePath if success, returns -1 if error.
	int OpenDialog( HWND hWnd, fm::string &filePath/*, const fm::string &initPath*/ );

	inline int GetLabelIdc() const
	{
		return m_labelIdc;
	}
};

class ICWidgetFactory
{
public:
	static ICWidget *Create( ICWidget::Type type, const fm::string &name = fm::string() );
};

#endif // _ICWIDGET_H_


