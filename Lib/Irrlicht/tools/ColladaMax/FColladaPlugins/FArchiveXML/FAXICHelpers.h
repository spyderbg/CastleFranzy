/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FAXICHELPERS_H_
#define _FAXICHELPERS_H_

#include "FArchiveXML.h"
#include "FCDocument/FCDExportBehavior.h"

class FCDSceneNode;

#define WRITE_IC_ENUM_PARAM_AS_STRING( __enumNamespace, __parentNode, __enumType, __fcdPtr, __fcdGetter, __paramName ) \
{ \
	__enumNamespace::__enumType __enumValue = (__enumNamespace::__enumType)(int)__fcdPtr->__fcdGetter(); \
	fstring __enumString = fstring( __enumNamespace::ToString##__enumType( __enumValue ) ); \
	AddChild( __parentNode, __paramName, __enumString ); \
	/*AddParameter( __parentNode, fstring( __paramName ).ToUpper(), DAEFC_STRING_ATTRIBUTE_TYPE, __enumString );*/ \
}

#define WRITE_IC_ENUM_PARAM_AS_INT( __enumNamespace, __parentNode, __enumType, __fcdPtr, __paramName ) \
{ \
	__enumNamespace::__enumType __enumValue = (__enumNamespace::__enumType)(int)__fcdPtr->Get##__enumType(); \
	AddChild( __parentNode, __paramName, __enumValue ); \
}

#define WRITE_IC_ENUM_PARAM_AS_CHILD_WITH_SID( __enumNamespace, __parentNode, __enumType, __fcdPtr, __childNodePtr ) \
{ \
	__enumNamespace::__enumType __enumValue = (__enumNamespace::__enumType)(int)__fcdPtr->Get##__enumType(); \
	fstring __enumString = fstring( __enumNamespace::ToString##__enumType( __enumValue ) ); \
	__childNodePtr = AddChildWithSid( __parentNode, __enumString, __enumString ); \
}

namespace FAXICHelpers
{
	template <class T>
	xmlNode *WriteAnimatedParameterContent( xmlNode* parent, fstring name, fstring type, T &value )
	{
		type;
		xmlNode *pParamNode = AddChild( parent, name, value );
		//xmlNode *pParamNode = AddParameter( parent, name.ToUpper(), type.c_str(), value );

		FArchiveXML::WriteAnimatedValue(&value, pParamNode, name.c_str());
		return pParamNode;
	}

	template <class T>
	void WriteAnimatedAttribute( xmlNode *pNode, fstring name, T &value, fstring attributeName )
	{
		FArchiveXML::WriteAnimatedValue(&value, pNode, name.c_str());
		AddAttribute( pNode, attributeName.c_str(), value );
	}

	template <class T>
	xmlNode *AddAnimatedChildParameter( xmlNode *parent, fstring name, T &value, fstring attributeName )
	{
		xmlNode *pParamNode = AddChild( parent, name );
		WriteAnimatedAttribute( pParamNode, name, value, attributeName );
		return pParamNode;
	}

	template <class T>
	void WriteAnimatedAttributeAsString( xmlNode *pNode, fstring name, T &value, fstring attributeName, FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior )
	{
		FArchiveXML::WriteAnimatedValue(&value, pNode, name.c_str());
		AddAttribute( pNode, attributeName.c_str(), pExportBehavior->ConvertEnumToString( ( int )value ) );
	}

	template <class T>
	xmlNode *AddAnimatedChildParameterAsString( xmlNode* parent, fstring name, T &value, fstring attributeName, FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior )
	{
		xmlNode *pParamNode = AddChild( parent, name );
		WriteAnimatedAttributeAsString( pParamNode, name, value, attributeName, pExportBehavior );
		return pParamNode;
	}

	template <class T>
	xmlNode *WriteUnanimatedParameter( xmlNode* parent, fstring name, fstring type, T value )
	{
		type;
		xmlNode *pParamNode = AddChild( parent, name, value );
		return pParamNode;
	}

	void AddUriContent( xmlNode* pNode, const fstring &uriString );

	bool IsPivotNode( FCDSceneNode *pSceneNode );

	// maxDepth ignored if < 0
	// horizontal search
	FCDSceneNode *FindFirstPivotDescendant( FCDSceneNode *pSceneNode, int maxDepth = -1 );
	FCDSceneNode *GetInstanceParentNode( FCDSceneNode *pSceneNode );

	FUUri GetEntityUri( FCDocument *pDocument, FCDEntity *pEntity );
}

#endif // _FAXICHELPERS_H_