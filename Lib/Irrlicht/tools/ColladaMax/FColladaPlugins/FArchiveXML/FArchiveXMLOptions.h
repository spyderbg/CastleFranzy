/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCPARCHIVEOPTIONSCOLLADA_H_
#define _FCPARCHIVEOPTIONSCOLLADA_H_

//class FCDParameterAnimatable;
//
//#ifndef _FAXSTRUCTURES_H_
//#include "FAXStructures.h"
//#endif // _FAXSTRUCTURES_H_
//#ifndef _FCOLLADA_PLUGIN_H_
//#include "FColladaPlugin.h"
//#endif // _FCOLLADA_PLUGIN_H_
//#ifndef _FC_DOCUMENT_H_
//#include "FCDocument/FCDocument.h"
//#endif // _FC_DOCUMENT_H_
//#ifndef _FCD_MATERIAL_STANDARD_H_
//#include "FCDocument/FCDEffectStandard.h"
//#endif // _FCD_MATERIAL_STANDARD_H_
//#ifndef _FCD_ENTITY_H_
//#include "FCDocument/FCDEntity.h"
//#endif // _FCD_ENTITY_H_
//
//#define FCP_ARCHIVECOLLADA_NAME "XML Archive Plug-in"
//
//#define NUM_EXTENSIONS 2

class StrippableLibraryFlagToDaeElementNameMapping
{
public:
	int flag;
	const char *daeElementName;
};

class FArchiveXMLOptions
{
public:
	enum
	{
		STRIP_LIB_ANIMS            = 0x00000001,
		STRIP_LIB_ANIMCLIPS        = 0x00000002,
		STRIP_LIB_CAMERAS          = 0x00000004,
		STRIP_LIB_CONTROLLERS      = 0x00000008,
		STRIP_LIB_GEOMETRIES       = 0x00000010,
		STRIP_LIB_LIGHTS           = 0x00000020,
		STRIP_LIB_NODES            = 0x00000040,
		STRIP_LIB_VISUALSCENES     = 0x00000080,

		// COLLADA FX
		STRIP_LIB_EFFECTS          = 0x00000100,
		STRIP_LIB_IMAGES           = 0x00000200,
		STRIP_LIB_MATERIALS        = 0x00000400,

		// COLLADA PHYSICS
		STRIP_LIB_FORCEFIELDS      = 0x00001000,
		STRIP_LIB_PHYSICSMATERIALS = 0x00002000,
		STRIP_LIB_PHYSICSSCENES    = 0x00004000,
		STRIP_LIB_PHYSICSMODELS    = 0x00008000,

		STRIP_LIB_EXTRALIBS        = 0x00010000,
		STRIP_LIB_SCENE            = 0x00020000,

		STRIP_LIB_ALL              = 0xffffffff
	};

private:

	const static int s_kNumStrippableLibraries;
	const static StrippableLibraryFlagToDaeElementNameMapping s_kStrippableLibraryFlagToDaeElementNameMap[];

public:
	int m_librariesToStrip;
	bool m_bRemoveDuplicateWeights;
	bool m_bExportModularSkinCategoryAsNode;

public:
	FArchiveXMLOptions()
		: m_librariesToStrip( 0 )
		, m_bRemoveDuplicateWeights( false )
		, m_bExportModularSkinCategoryAsNode( false )
	{}

	//
	// Library stripping related functions
	//
	bool IsNodeStripped( xmlNode *pNode );
	void StripLibraries( xmlNode *pRootNode );

	static int GetStrippableLibraryFlag( int strippableLibraryId );
	static const char *GetStrippableLibraryDaeElementName( int strippableLibraryId );
	static int GetNumStrippableLibraries();

	inline bool ExportModularSkinCategoryAsNode()
	{
		return m_bExportModularSkinCategoryAsNode;
	}
};

inline int FArchiveXMLOptions::GetStrippableLibraryFlag( int strippableLibraryId )
{
	return s_kStrippableLibraryFlagToDaeElementNameMap[strippableLibraryId].flag;
}

inline const char *FArchiveXMLOptions::GetStrippableLibraryDaeElementName( int strippableLibraryId )
{
	return s_kStrippableLibraryFlagToDaeElementNameMap[strippableLibraryId].daeElementName;
}

inline int FArchiveXMLOptions::GetNumStrippableLibraries()
{
	return s_kNumStrippableLibraries;
}

#endif //_FCPARCHIVEOPTIONSCOLLADA_H_
