#include "StdAfx.h"
#include "ICPreferenceFileWriter.h"
#include "ICPreferenceFileSyntax.h"
#include "ICPreferenceFile.h"
#include "ICPreference.h"

#include "FUtils/FUFileManager.h"
#include "FUtils/FUXmlDocument.h"
#include "FUtils/FUXmlWriter.h"

#include "VersionNumber.h"

int ICPreferenceFileWriter::Write( const fm::string &filePath, ICPreferenceFile &preferenceFile )
{
	FUFileManager fileManager;
	FUXmlDocument prefDocument( &fileManager, filePath.c_str(), false );

	xmlNode *pConfigNode = prefDocument.CreateRootNode( ICPREF_CONFIG );

	// version
	{
		const VersionNumber &versionNumber = preferenceFile.GetVersionNumber();
		FUXmlWriter::AddChild( pConfigNode, ICPREF_VERSION, TO_FSTRING( versionNumber ).c_str() );
	}

	// preferences
	{
		xmlNode *pPreferencesNode = FUXmlWriter::AddChild( pConfigNode, ICPREF_PREFERENCES );

		PreferenceList *pPreferences = preferenceFile.GetPreferences();
		if ( pPreferences )
		{
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
				fm::string preferenceString;
				preference.SetStringFromValue( preferenceString );

				FUXmlWriter::AddChild( pPreferencesNode, preferenceName.c_str(), preferenceString );
			}			
		}
	}

	prefDocument.Write();

	return 0;
}
