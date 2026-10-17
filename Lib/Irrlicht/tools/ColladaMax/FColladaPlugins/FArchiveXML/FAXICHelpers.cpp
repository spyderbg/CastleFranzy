/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "stdafx.h"
#include "FAXICHelpers.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDEntityReference.h"

namespace FAXICHelpers
{
	void AddUriContent( xmlNode* pNode, const fstring &uriString )
	{
		if ( !IsEquivalent( uriString, "#" ) )
		{
			AddContentUnprocessed( pNode, uriString );
		}
	}

	bool IsPivotNode( FCDSceneNode *pSceneNode )
	{
		if ( !pSceneNode )
		{
			return false;
		}

		const fstring &pivotSuffix = PIVOT_SUFFIX;
		const fstring &nodeId = pSceneNode->GetDaeId();

		size_t pivotSuffixLength = pivotSuffix.length();
		size_t nodeIdLength = nodeId.length();
		
		if ( nodeIdLength < pivotSuffixLength )
		{
			return false;
		}

		const fstring &nodeIdEnd = nodeId.substr( nodeIdLength - pivotSuffixLength, fstring::npos );
		return IsEquivalent( nodeIdEnd, pivotSuffix );
	}

	FCDSceneNode *FindFirstPivotDescendant( FCDSceneNode *pSceneNode, int maxDepth )
	{
		if ( maxDepth == 0 )
		{
			return NULL;
		}

		size_t numChildren = pSceneNode->GetChildrenCount();
		for ( size_t childId = 0; childId < numChildren; ++childId )
		{
			FCDSceneNode *pChild = pSceneNode->GetChild( childId );
			if ( IsPivotNode( pChild ) )
			{
				return pChild;
			}
		}

		for ( size_t childId = 0; childId < numChildren; ++childId )
		{
			FCDSceneNode *pPivotNode = FindFirstPivotDescendant( pSceneNode->GetChild( childId ), maxDepth < 0 ? maxDepth : maxDepth - 1 );
			if ( pPivotNode )
			{
				return pPivotNode;
			}
		}

		return NULL;
	}

	FCDSceneNode *GetInstanceParentNode( FCDSceneNode *pSceneNode )
	{
		if ( !pSceneNode )
		{
			return NULL;
		}

		size_t numInstances = pSceneNode->GetInstanceCount();
		if ( numInstances >= 1 )
		{
			return pSceneNode;
		}

		pSceneNode = FAXICHelpers::FindFirstPivotDescendant( pSceneNode, 1 );
		if ( !pSceneNode )
		{
			return NULL;
		}

		numInstances = pSceneNode->GetInstanceCount();
		if ( numInstances <= 0 )
		{
			return NULL;
		}

		return pSceneNode;
	}

	FUUri GetEntityUri( FCDocument *pDocument, FCDEntity *pEntity )
	{
		if ( !pDocument || !pEntity )
		{
			return FUUri();
		}

		FCDEntityReference ref( pDocument, NULL );
		ref.SetEntity( pEntity );
		return ref.GetUri();
	}
}
