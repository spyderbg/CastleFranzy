/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDEffectPassState.h
	This file contains the FCDEffectPassState class.
*/

#ifndef _FCD_EXPORT_BEHAVIOR_H_
#define _FCD_EXPORT_BEHAVIOR_H_

//#ifndef _FCD_PARAMETER_ANIMATABLE_H_
//#include "FCDocument/FCDParameterAnimatable.h"
//#endif // _FCD_PARAMETER_ANIMATABLE_H_

//#include "FCDEffectPassState.h"

class FCDExportBehavior 
{
public:
	enum Type
	{
		T_CONVERT_ENUM_TO_STRING
	};

private:
	Type m_type;

public:
	FCDExportBehavior( Type type )
		: m_type( type )
	{}
	
	inline virtual fm::string ConvertEnumToString( int UNUSED(_enum) )
	{
		FUFail( return fm::string() );
	}

	inline Type GetType()
	{
		return m_type;
	}

	inline void SetType( Type type )
	{
		m_type = type;
	}

	//virtual FCDExportBehavior *Clone(FCDEffectPassStateParameter* clone = NULL) const;
};

class FCDExportBehavior__T_CONVERT_ENUM_TO_STRING
	: public FCDExportBehavior
{
private:
	const char *( *m_pEnumConversionFunction )( int );

public:
	FCDExportBehavior__T_CONVERT_ENUM_TO_STRING( const char *( *pEnumConversionFunction )( int ) = NULL )
		: FCDExportBehavior( FCDExportBehavior::T_CONVERT_ENUM_TO_STRING )
		, m_pEnumConversionFunction( pEnumConversionFunction )
	{}

	inline void SetEnumConversionFunction( const char *( *pEnumConversionFunction )( int ) )
	{
		m_pEnumConversionFunction = pEnumConversionFunction;
	}
	
	inline fm::string ConvertEnumToString( int _enum )
	{
		return (*m_pEnumConversionFunction)( _enum );
	}
};

#endif _FCD_EXPORT_BEHAVIOR_H_