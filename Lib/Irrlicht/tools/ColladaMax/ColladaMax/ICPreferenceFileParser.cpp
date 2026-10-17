#include "StdAfx.h"
#include "ICPreferenceFileParser.h"
#include "ICPreferenceFileSyntax.h"
#include "ICPreferenceFile.h"
#include "ICPreference.h"

#include "FUtils/FUFileManager.h"
#include "FUtils/FUXmlDocument.h"
#include "FUtils/FUXmlParser.h"

#include "VersionNumber.h"

int ICPreferenceFileParser::Read( const fm::string &filePath, ICPreferenceFile &preferenceFile )
{
	FUFileManager fileManager;

	// does preference file exist
	if ( !fileManager.FileExists( filePath ) )
	{
		return 0;
	}

	FUXmlDocument daeDocument( &fileManager, filePath.c_str(), true );

	xmlNode *pRootNode = daeDocument.GetRootNode();
	if ( !pRootNode || !IsEquivalent( pRootNode->name, ICPREF_CONFIG ) )
	{
		return -1;
	}

	// version
	{
		xmlNode *pVersionNode = FUXmlParser::FindChildByName( pRootNode, ICPREF_VERSION );
		if ( !pVersionNode )
		{
			return -1;
		}

		
		VersionNumber versionNumber = VersionNumber::Get( FUXmlParser::ReadNodeContentFull( pVersionNode ) );
		preferenceFile.SetVersionNumber( versionNumber );
	}

	// preferences
	{
		xmlNode *pPreferencesNode = FUXmlParser::FindChildByName( pRootNode, ICPREF_PREFERENCES );
		if ( !pPreferencesNode )
		{
			return -1;
		}		

		PreferenceList *pPreferences = preferenceFile.GetPreferences();
		if ( !pPreferences )
		{
			return 0;
		}

		PreferenceList &preferences = *pPreferences;
		for ( PreferenceList::iterator prefIter = preferences.begin(); prefIter != preferences.end(); ++prefIter )
		{
			ICPreference *pPreference = *prefIter;
			if ( !pPreference )
			{
				continue;
			}

			ICPreference &preference = *pPreference;
			const fm::string &preferenceName = preference.GetXMLElementName();
			xmlNode *pNode = FUXmlParser::FindChildByName( pPreferencesNode, preferenceName.c_str() );
			if ( !pNode )
			{
				continue;
			}

			fm::string nodeContent = FUXmlParser::ReadNodeContentFull( pNode );
			preference.SetFileValue( &nodeContent );
			preference.SetValueFromString( nodeContent );
		}
	}

	return 0;
}
