/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FArchiveXMLOptions.h"

const int FArchiveXMLOptions::s_kNumStrippableLibraries = 17;
const StrippableLibraryFlagToDaeElementNameMapping FArchiveXMLOptions::s_kStrippableLibraryFlagToDaeElementNameMap[] = 
{
	{ FArchiveXMLOptions::STRIP_LIB_ANIMS,				DAE_LIBRARY_ANIMATION_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_ANIMCLIPS,			DAE_LIBRARY_ANIMATION_CLIP_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_CAMERAS,			DAE_LIBRARY_CAMERA_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_CONTROLLERS,		DAE_LIBRARY_CONTROLLER_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_GEOMETRIES,			DAE_LIBRARY_GEOMETRY_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_LIGHTS,				DAE_LIBRARY_LIGHT_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_NODES,				DAE_LIBRARY_NODE_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_VISUALSCENES,		DAE_LIBRARY_VSCENE_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_EFFECTS,			DAE_LIBRARY_EFFECT_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_IMAGES,				DAE_LIBRARY_IMAGE_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_MATERIALS,			DAE_LIBRARY_MATERIAL_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_FORCEFIELDS,		DAE_LIBRARY_FFIELDS_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_PHYSICSMATERIALS,	DAE_LIBRARY_PMATERIAL_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_PHYSICSSCENES,		DAE_LIBRARY_PSCENE_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_PHYSICSMODELS,		DAE_LIBRARY_PMODEL_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_EXTRALIBS,			DAE_EXTRA_ELEMENT },
	{ FArchiveXMLOptions::STRIP_LIB_SCENE,				DAE_SCENE_ELEMENT }
};

#ifndef TEST_FLAG
#define TEST_FLAG(dest, flag) (((dest) & (flag)) != 0)
#endif

bool FArchiveXMLOptions::IsNodeStripped( xmlNode *pNode )
{
	bool bIsNodeStripped = false;

	for ( int i = 0; i < GetNumStrippableLibraries(); i++ )
	{
		const xmlChar *libName = ( const xmlChar * )GetStrippableLibraryDaeElementName( i );
		if ( !xmlStrcmp( libName, pNode->name ) )
		{
			bIsNodeStripped = TEST_FLAG( m_librariesToStrip, GetStrippableLibraryFlag( i ) );
			break;
		}
	}

	return bIsNodeStripped;
}

void FArchiveXMLOptions::StripLibraries( xmlNode *pRootNode )
{
	xmlNode *pCurChild = pRootNode->children;
	while ( pCurChild )
	{
		xmlNode *pNextChild = pCurChild->next;

		bool bStrip = IsNodeStripped( pCurChild );
		if ( !bStrip )
		{
			pCurChild = pNextChild;
			continue;
		}

		xmlUnlinkNode( pCurChild );
		xmlFreeNode( pCurChild );

		pCurChild = pNextChild;
	}
}

#undef TEST_FLAG