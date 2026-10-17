/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICBatchExporter.h"
#include "AnimationClipManager.h"
#include "ColladaOptions.h"
#include "FCDocument/FCDocument.h"

#include "Common/ConversionFunctions.h"

#include "FUtils/FUXmlDocument.h"
#include "FUtils/FUXmlParser.h"
#include "FUtils/FUStringBuilder.h"

// for debug output
FUStringBuilder g_exportLog;
FUStringBuilder g_warningLog;

BatchExporter *BatchExporter::s_pBatchExporter = NULL;

const fm::string BatchExporter::k_animClipOptions[k_numAnimClipOptions] = 
{
	fm::string( "UserClips" )
	, fm::string( AnimationClipManager::GenerateStandardAnimClipName( "AnimClips" ) )
	, fm::string( AnimationClipManager::GenerateStandardAnimClipName( "SceneAnims" ) )
	, fm::string( AnimationClipManager::GenerateStandardAnimClipName( "Current_Range" ) )
	, fm::string( AnimationClipManager::GenerateStandardAnimClipName( "Total_Range" ) )
};

BatchExporter::BatchExporter()
{
	m_pDefaultColladaOptions = new ColladaOptions( NULL );
	
	m_batchOptions.push_back( new bool_BatchOption( "CloseMaxWhenDone" ) );
	m_batchOptions.push_back( new bool_BatchOption( "ConditionalExport" ) );
	m_batchOptions.push_back( new fstring_BatchOption( "LogFile" ) );

	Reset();
}

BatchExporter::~BatchExporter()
{
	SAFE_DELETE( m_pDefaultColladaOptions );
	CLEAR_POINTER_VECTOR( m_batchOptions );
}

void BatchExporter::Reset()
{
	m_exportMode = BE_INVALID;
	m_exportSrc.clear();
	m_exportOpts.clear();
	m_pDefaultColladaOptions->Reset();
	m_fileOptionsMap.clear();
	m_currentFileId = -1;
	m_bSkippedFileExport = false;
	m_customScripts.clear();
	m_irrColladaVersionNumber = VersionNumber::Invalid();

	ResetBatchOptions();
}


BatchOption *BatchExporter::GetBatchOption( fm::string optionName )
{
	BatchOption *pOption;

	for ( size_t i = 0; i < m_batchOptions.size(); ++i )
	{
		if ( m_batchOptions[i]->m_name == optionName )
		{
			pOption = m_batchOptions[i];
		}
	}

	return pOption;
}

void BatchExporter::ResetBatchOptions()
{
	for ( size_t i = 0; i < m_batchOptions.size(); ++i )
	{
		m_batchOptions[i]->Reset();
	}
}

#define PRINT_OPTION( optionName ) \
{ \
	stream.str(""); \
	stream << m_fileOptionsMap[fileId].m_pOptions->##optionName; \
	mprintf( "%s : %s\n", #optionName, stream.str().c_str() ); \
}

FileOptionMapping::FileOptionMapping()
{
	m_pOptions = new ColladaOptions( NULL );
}

FileOptionMapping::~FileOptionMapping()
{
	SAFE_DELETE( m_pOptions );
}

FileOptionMapping::FileOptionMapping(FileOptionMapping const &copy)
{
	m_srcFilePath = copy.m_srcFilePath;
	m_dstFilePath = copy.m_dstFilePath;
	m_pOptions = copy.m_pOptions == NULL ? NULL : copy.m_pOptions->Clone();
}

#define TOKEN_SEPARATOR "\r\n"
#define SKIP_SPACES( token ){ while (*token != 0 && (*token == ' ' || *token == '\r' || *token == '\t')) ++token; }
#define SKIP_COMMENT( token ){ if ( *token == '#' ){ token = strtok( NULL, TOKEN_SEPARATOR ); continue; }}
#define TRIM( token ) \
{ \
	fm::string tmp( token ); \
	tmp.trim(); \
	strcpy( optionValue, token ); \
}

#define MAX_OPTION_NAME 256
#define MAX_OPTION_VALUE 1024
// Look for/read in the ColladaMax options.
#define READ_OPTION(optionCast, optionVar) \
if (_stricmp(optionName, #optionVar) == 0) { \
	options->##optionVar = (optionCast) strtol(value, NULL, 0 ); } else
#define READ_OPTION_BOOL(optionVar) \
if (_stricmp(optionName, #optionVar) == 0) { \
	options->##optionVar = *value != 'f' && *value != 'F' && *value != '0'; } else
#define READ_OPTION_FLOAT(optionVar) \
if (_stricmp(optionName, #optionVar) == 0) { \
	options->##optionVar = (float) strtod(value, NULL ); } else
#define READ_OPTION_FSTRING(optionVar) \
if (_stricmp(optionName, #optionVar) == 0) { \
	options->##optionVar.clear(); \
	while ( *value != '\0' ) \
	{ \
		options->##optionVar.append( *value ); \
		value++; \
	} } else

int find( const char *str, char c )
{
	int index = -1;
	for ( int count = 0; str[count] != '\0'; count++ )
	{
		if ( str[count] == c )
		{
			index = count;
			break;
		}
	}

	return index;
}

void BatchExporter::WriteOptions( CStr &str, ColladaOptions *options )
{
	CStr optionString;

#define WRITE_OPTION( optionName, optionValue, formatString ) \
	optionString.printf( "%s=" #formatString ",", optionName, optionValue ); \
	str += optionString;

	WRITE_OPTION("bakeMatrices",options->bakeMatrices,%d);
	WRITE_OPTION("decomposeMatrices",options->decomposeMatrices,%d);
	//WRITE_OPTION("removeUnusedSkinJoints",options->removeUnusedSkinJoints,%d);
	WRITE_OPTION("relativePaths",options->relativePaths,%d);
	WRITE_OPTION("removePaths",options->removePaths,%d);
	WRITE_OPTION("replacePaths",options->replacePaths,%d);
	WRITE_OPTION("replacementPath",options->replacementPath.c_str(),%s);
	WRITE_OPTION("stripAssociatedNodes",options->stripAssociatedNodes,%d);
	WRITE_OPTION("normals",options->normals,%d);
	WRITE_OPTION("triangulate",options->triangulate,%d);
	WRITE_OPTION("xrefs",options->xrefs,%d);
	WRITE_OPTION("tangents",options->tangents,%d);
	WRITE_OPTION("convertLinearSplinesToLineStrips",options->convertLinearSplinesToLineStrips,%d);
	WRITE_OPTION("deindexVertexData",options->deindexVertexData,%d);
	WRITE_OPTION("animations",options->animations,%d);
	WRITE_OPTION("sampleAnim",options->sampleAnim,%d);
	WRITE_OPTION("sampleRate",options->sampleRate,%f);
	WRITE_OPTION("secondarySampleRate",options->secondarySampleRate,%f);
	WRITE_OPTION("forceAnimationKeySampling",options->forceAnimationKeySampling,%d);
	WRITE_OPTION("trimBipedAnims",options->trimBipedAnims,%d);
	WRITE_OPTION("exportDegenerateAnimationCurves",options->exportDegenerateAnimationCurves,%d);
	WRITE_OPTION("exportInvalidTransformAnimations",options->exportInvalidTransformAnimations,%d);
	WRITE_OPTION("exportInvalidMorphTargetWeightAnimations",options->exportInvalidMorphTargetWeightAnimations,%d);
	WRITE_OPTION("cubicInterpolation",options->cubicInterpolation,%d);
	//WRITE_OPTION("cleanAnimKeys",options->cleanAnimKeys,%d);
	WRITE_OPTION("compressAnimKeys",options->compressAnimKeys,%d);
	WRITE_OPTION("keyCompressionTolerance",options->keyCompressionTolerance,%f);
	WRITE_OPTION("oneChannelPerCurve",options->oneChannelPerCurve,%d);

	// Animation clip batch option
	int numClips = AnimationClipManager::GetInst()->GetNumAnimClips();
	for (int i = 0; i < numClips; i++)
	{
		int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
		if ( TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ))
		{
			if ( !TEST_FLAG( currentFlags, AnimClip_Ticks::STANDARD_ANIM_CLIP ))
			{
				optionString.printf( "animClipOptions=UserClips," );
			}
			else
			{
				const char *name = AnimationClipManager::GetInst()->GetAnimClipName( i );
				optionString.printf( "animClipOptions=%s,", name );
			}
			str += optionString;
			break;
		}
	}

	WRITE_OPTION("removeZeroWeights",options->removeZeroWeights,%d);
	WRITE_OPTION("removeDuplicateWeights",options->removeDuplicateWeights,%d);
	WRITE_OPTION("exportUpAxis",options->exportUpAxis,%d);
	WRITE_OPTION("librariesToStrip",options->librariesToStrip,0x%X);
	WRITE_OPTION("geometryPositionFormat",options->geometryPositionFormat.c_str(),%s);
	WRITE_OPTION("geometryNormalFormat",options->geometryNormalFormat.c_str(),%s);
	WRITE_OPTION("geometryTextureFormat",options->geometryTextureFormat.c_str(),%s);
	WRITE_OPTION("geometryColorFormat",options->geometryColorFormat.c_str(),%s);
	WRITE_OPTION("textureCoordinatesOrigin",options->textureCoordinatesOrigin.c_str(),%s);

	// warnings
	int numWarnings = options->m_warningManager.GetWarningCount();
	for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
	{
		ICWarning *pWarning = options->m_warningManager.GetWarning( warningIndex );
		WRITE_OPTION( pWarning->m_name.c_str(), TEST_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED ),%d);
	}

#undef WRITE_OPTION

	str.Resize( str.Length() - 1 );
}

void BatchExporter::ParseColladaOptions( const char *token, ColladaOptions *options )
{
	int equalIndex = -1;
	int commaIndex = -1;
	int endLineIndex = -1;
	char optionName[MAX_OPTION_NAME];
	char optionValue[MAX_OPTION_VALUE];

	while ( *token != '\0' )
	{
		SKIP_SPACES( token );
		equalIndex = find( token, '=' );
		FUAssert( equalIndex != -1, );
		strncpy( optionName, token, equalIndex );
		optionName[ equalIndex ] = '\0';
		token += equalIndex + 1;

		SKIP_SPACES( token );
		commaIndex = find( token, ',' );
		if ( commaIndex != -1 )
		{
			strncpy( optionValue, token, commaIndex );	
			optionValue[ commaIndex ] = '\0';
			token += commaIndex + 1;		
		}
		else
		{
			strcpy( optionValue, token );
		}

		char *value = optionValue;
		//mprintf( "OPTION: %s ==> %s\n", optionName, optionValue );

		if ( *optionValue != '\0' )
		{
			READ_OPTION_BOOL(normals)
			READ_OPTION_BOOL(triangulate)
			READ_OPTION_BOOL(triangleStrips)
			READ_OPTION_BOOL(xrefs)
			READ_OPTION_BOOL(tangents)
			READ_OPTION_BOOL(convertLinearSplinesToLineStrips)
			READ_OPTION_BOOL(sampleAnim)
			READ_OPTION_FLOAT(sampleRate)
			READ_OPTION_FLOAT(secondarySampleRate)
			READ_OPTION_BOOL(forceAnimationKeySampling)
			READ_OPTION_BOOL(trimBipedAnims)
			READ_OPTION_BOOL(exportDegenerateAnimationCurves)
			READ_OPTION_BOOL(exportInvalidTransformAnimations)
			READ_OPTION_BOOL(exportInvalidMorphTargetWeightAnimations)
			READ_OPTION(FUDaeInterpolation::Interpolation, cubicInterpolation)
			READ_OPTION_BOOL(animations)
			READ_OPTION_BOOL(cleanAnimKeys)
			READ_OPTION_BOOL(compressAnimKeys)
			READ_OPTION_FLOAT(keyCompressionTolerance)
			READ_OPTION_BOOL(oneChannelPerCurve)
			READ_OPTION_BOOL(bakeMatrices)
			READ_OPTION_BOOL(decomposeMatrices)
			READ_OPTION_BOOL(removeUnusedSkinJoints)
			READ_OPTION_BOOL(relativePaths)
			READ_OPTION_BOOL(removePaths)
			READ_OPTION_BOOL(replacePaths)
			READ_OPTION_FSTRING(replacementPath)
			READ_OPTION_BOOL(stripAssociatedNodes)
			//READ_OPTION_BOOL(importUnits)
			//READ_OPTION_BOOL(importUpAxis)
			READ_OPTION_BOOL(removeZeroWeights)
			READ_OPTION_BOOL(removeDuplicateWeights)
			READ_OPTION_BOOL(deindexVertexData)
			READ_OPTION_FSTRING(textureCoordinatesOrigin)
			READ_OPTION(int, exportUpAxis)
			READ_OPTION(int, librariesToStrip)
			READ_OPTION_FSTRING(geometryPositionFormat)
			READ_OPTION_FSTRING(geometryNormalFormat)
			READ_OPTION_FSTRING(geometryTextureFormat)
			READ_OPTION_FSTRING(geometryColorFormat)
			READ_OPTION(int, animStart)
			READ_OPTION(int, animEnd)
			READ_OPTION_FSTRING(animClipOptions)
			;

			if ( !_stricmp( optionName, "animStart" ))
			{
				options->animStart = FramesToTicks( options->animStart );
			}
			else if ( !_stricmp( optionName, "animEnd" ))
			{
				options->animEnd = options->animEnd != TIME_PosInfinity ? FramesToTicks( options->animEnd ) : options->animEnd;
			}

			// warnings
			int numWarnings = options->m_warningManager.GetWarningCount();
			for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
			{
				ICWarning *pWarning = options->m_warningManager.GetWarning( warningIndex );
				if ( _stricmp(optionName, pWarning->m_name.c_str() ) == 0 )
				{
					bool bWarningEnabled = *value != 'f' && *value != 'F' && *value != '0';
					RESET_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED );
					SET_FLAG( pWarning->m_flags, bWarningEnabled ? ICWarningFlags::ENABLED : 0);
				}		
			}
		}
	
		if ( commaIndex == -1 )
		{
			break;
		}
	}
}

#define PARSE_BATCH_OPTION( optionIndex, optionName, optionValue ) \
if ( !_stricmp( m_batchOptions[optionIndex]->m_name, optionName ) ) \
{ \
	m_batchOptions[optionIndex]->ParseAndSetValue( optionValue ); \
	break; \
}


char *BatchExporter::ParseBatchOptions( char *token )
{
	int equalIndex = -1;
	char optionName[MAX_OPTION_NAME];
	char optionValue[MAX_OPTION_VALUE];

	while ( token != NULL && *token != '[' )
	{
		SKIP_COMMENT( token );
		SKIP_SPACES( token );

		equalIndex = find( token, '=' );
		FUAssert( equalIndex != -1, );
		strncpy( optionName, token, equalIndex );
		optionName[ equalIndex ] = '\0';
		token += equalIndex + 1;

		strcpy( optionValue, token );
		TRIM( optionValue );

		for ( size_t i = 0; i < m_batchOptions.size(); ++i )
		{
			PARSE_BATCH_OPTION( i, optionName, optionValue );
		}

		token = strtok( NULL, TOKEN_SEPARATOR );
	}

	return token;
}

#undef PARSE_BATCH_OPTION

char *BatchExporter::ParseCustomScripts( char *token )
{
	while ( token != NULL && *token != '[' )
	{
		SKIP_COMMENT( token );
		m_customScripts.push_front( fm::string( token ) );
		m_customScripts.front().trim();
		token = strtok( NULL, TOKEN_SEPARATOR );
	}

	return token;
}

char *BatchExporter::ParseFiles( char *token )
{
	int equalIndex = -1;
	int commaIndex = -1;
	int optionsSeparatorIndex = -1;
	int currentSeparatorIndex = -1;
	char optionName[MAX_OPTION_NAME];
	char optionValue[MAX_OPTION_VALUE];
	
	while ( token != NULL && *token != '[' )
	{
		SKIP_SPACES( token );
		SKIP_COMMENT( token );
		equalIndex = find( token, '=' );
		FUAssert( equalIndex != -1, );
		strncpy( optionName, token, equalIndex );
		optionName[ equalIndex ] = '\0';
		token += equalIndex + 1;

		SKIP_SPACES( token );
		optionsSeparatorIndex = find( token, ';' );
		commaIndex = find( token, ',' );
		currentSeparatorIndex = ( commaIndex != -1 && ( optionsSeparatorIndex == -1 || commaIndex < optionsSeparatorIndex )) ? commaIndex : optionsSeparatorIndex;
		if ( currentSeparatorIndex != -1 )
		{
			strncpy( optionValue, token, currentSeparatorIndex );	
			optionValue[ currentSeparatorIndex ] = '\0';
			token += currentSeparatorIndex + 1;
		}
		else
		{
			strcpy( optionValue, token );
		}

		if ( !_stricmp( optionName, "src" ) )
		{
			m_fileOptionsMap.push_back( FileOptionMapping() );
			FileOptionMapping &fileOptionMapping = m_fileOptionsMap.back();
			if ( m_pDefaultColladaOptions )
			{
				*fileOptionMapping.m_pOptions = *m_pDefaultColladaOptions;
			}

			fileOptionMapping.m_srcFilePath = std::string( optionValue );
		}
		else if ( !_stricmp( optionName, "dst" ) )
		{
			FileOptionMapping &fileOptionMapping = m_fileOptionsMap.back();
			fileOptionMapping.m_dstFilePath = std::string( optionValue );
		}
		
		if ( currentSeparatorIndex == -1 || *token == NULL )
		{
			token = strtok( NULL, TOKEN_SEPARATOR );
		}
		else if ( currentSeparatorIndex == optionsSeparatorIndex || *token == ';' )
		{
			if ( *token == ';' )
			{
				token++;
			}

			FileOptionMapping &fileOptionMapping = m_fileOptionsMap.back();
			ParseColladaOptions( token, fileOptionMapping.m_pOptions );
			token = strtok( NULL, TOKEN_SEPARATOR );
		}	
	}

	for ( std::vector<FileOptionMapping>::iterator it = m_fileOptionsMap.begin(); it != m_fileOptionsMap.end(); it++ )
	{
		if ( it->m_dstFilePath.empty() )
		{
			it->m_dstFilePath = it->m_srcFilePath;
			
			// replace .max with .dae
			int ext = it->m_dstFilePath.rfind( '.' );
			if ( ext != std::string::npos )
			{
				it->m_dstFilePath.replace( ext + 1, 3, "dae" );
			}
			else
			{
				it->m_dstFilePath += ".dae";
			}
		}
	
		//mprintf( "FILE_SRC: %s\n", it->m_srcFilePath.c_str() );
		//mprintf( "FILE_DST: %s\n", it->m_dstFilePath.c_str() );
	}

	return token;
}

// TODO : Clean parser spaghetti and tokenize properly
void BatchExporter::ParseCfg()
{
	FILE* file = fopen( m_exportSrc.c_str(), _T("rb"));

	// Read the whole configuration file
	fseek(file, 0, SEEK_END);
	int fileLength = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* wholeFile = new char[fileLength + 1];
	fread(wholeFile, 1, fileLength, file);
	fclose(file);
	wholeFile[fileLength] = 0;

	char* token = strtok( wholeFile, TOKEN_SEPARATOR );
	char sectionName[MAX_OPTION_NAME];
	int closingBraceIndex = -1;
	while ( token != NULL )
	{
		SKIP_SPACES( token );
		SKIP_COMMENT( token );
		if ( *token == '[' )
		{
			token++;

			closingBraceIndex = find( token, ']' );
			FUAssert( closingBraceIndex != -1, );
			strncpy( sectionName, token, closingBraceIndex );
			sectionName[ closingBraceIndex ] = '\0';
			token = strtok( NULL, TOKEN_SEPARATOR ); //strchr( token, '\n' ) + 1;

			if ( !_stricmp( sectionName, "DEFAULT_OPTIONS" ) )
			{
				ParseColladaOptions( token, m_pDefaultColladaOptions );
				token = strtok( NULL, TOKEN_SEPARATOR );
			}
			else if ( !_stricmp( sectionName, "FILES" ) )
			{
				token = ParseFiles( token );
			}
			else if ( !_stricmp( sectionName, "CUSTOM_SCRIPTS" ) )
			{
				token = ParseCustomScripts( token );
			}
			else if ( !_stricmp( sectionName, "BATCH_OPTIONS" ) )
			{
				token = ParseBatchOptions( token );
			}
			else
			{
				token = strtok( NULL, TOKEN_SEPARATOR );
			}

			sectionName[0] = '\0';
		}
		else
		{
			token = strtok( NULL, TOKEN_SEPARATOR );
		}

		//++token;
	}
}

// Tests if file should be exported.
// Tests :
//	1) Timestamp comparison between .max and .dae
//	2) IrrCollada version comparison between plugin and .dae
//	3) Options comparison between .dae and current options
//  4) Animation clips comparison
bool BatchExporter::ShouldExportFile( int fileId )
{
	FUAssert( fileId >= 0, return false );
	m_bSkippedFileExport = false;

	// get timestamps
	ULARGE_INTEGER srcModifTime;
	ULARGE_INTEGER dstModifTime;
	WIN32_FILE_ATTRIBUTE_DATA fileAttrData;
	int retVal;

#define GET_FILETIME( filePath, largeInt ) \
	{ \
		retVal = GetFileAttributesEx( filePath, GetFileExInfoStandard, &fileAttrData ); \
		largeInt.LowPart = fileAttrData.ftLastWriteTime.dwLowDateTime; \
		largeInt.HighPart = fileAttrData.ftLastWriteTime.dwHighDateTime; \
	}

	GET_FILETIME( m_fileOptionsMap[fileId].m_srcFilePath.c_str(), srcModifTime );
	GET_FILETIME( m_fileOptionsMap[fileId].m_dstFilePath.c_str(), dstModifTime );

#undef GET_FILETIME

	// check if dst exists
	if ( retVal == 0 )
	{
		return true;
	}

	// compare timestamps
	if ( srcModifTime.QuadPart > dstModifTime.QuadPart )
	{
		return true;
	}

	// get .dae plugin version
	FCDocument *fcdocument = new FCDocument();
	FUErrorSimpleHandler handler;

	fcdocument->SetFileUrl(fstring(m_fileOptionsMap[fileId].m_dstFilePath.c_str()));
	FUXmlDocument daeDocument(fcdocument->GetFileManager(), fcdocument->GetFileUrl(), true);

#define DELETE_DOC_AND_RETURN( bValue ) \
SAFE_DELETE( fcdocument ); \
return bValue;

	if ( !handler.IsSuccessful() )
	{
		DELETE_DOC_AND_RETURN( true );
	}

	xmlNode *pRootNode = daeDocument.GetRootNode();
	xmlNode *pNode = NULL;

	pNode = FUXmlParser::FindChildByName( pRootNode, DAE_ASSET_ELEMENT );
	pNode = FUXmlParser::FindChildByName( pNode, DAE_CONTRIBUTOR_ASSET_ELEMENT );
	pNode = FUXmlParser::FindChildByName( pNode, DAE_AUTHORINGTOOL_ASSET_PARAMETER );
	
	fm::string verString = FUXmlParser::ReadNodeProperty( pNode, IRRCOLLADA_VERSION_ATTRIBUTE );
	VersionNumber verNum = VersionNumber::Get( verString.c_str() );

	// compare .dae plugin version and current plugin version
	if ( verNum == VersionNumber::Invalid() ||
		 verNum != GetIrrColladaVersionNumber() )
	{
		DELETE_DOC_AND_RETURN( true );
	}

	// get file last export options
	pNode = FUXmlParser::FindChildByName( pRootNode, DAE_ASSET_ELEMENT );
	pNode = FUXmlParser::FindChildByName( pNode, DAE_CONTRIBUTOR_ASSET_ELEMENT );
	pNode = FUXmlParser::FindChildByName( pNode, DAE_COMMENTS_ASSET_PARAMETER );

	fm::string content = FUXmlParser::ReadNodeContentFull( pNode );
	int prefixLength = strlen( EXPORT_OPTIONS_PREFIX );
	fm::string optString = content.substr( prefixLength );
	const char *token = optString.c_str();

	// get current export options
	ColladaOptions *currentOptions = m_fileOptionsMap[m_currentFileId].m_pOptions;
	currentOptions->CleanReplacementPath();

	int equalIndex = -1;
	int semiColonIndex = -1;
	char optionName[MAX_OPTION_NAME];
	char optionValue[MAX_OPTION_VALUE];

	while ( *token != '\0' )
	{
		SKIP_SPACES( token );
		equalIndex = find( token, '=' );
		FUAssert( equalIndex != -1, );
		strncpy( optionName, token, equalIndex );
		optionName[ equalIndex ] = '\0';
		token += equalIndex + 1;

		SKIP_SPACES( token );
		semiColonIndex = find( token, ';' );
		if ( semiColonIndex != -1 )
		{
			strncpy( optionValue, token, semiColonIndex );	
			optionValue[ semiColonIndex ] = '\0';
			token += semiColonIndex + 1;		
		}
		else
		{
			strcpy( optionValue, token );
		}

		char *value = optionValue;
		//mprintf( "OPTION: %s ==> %s\n", optionName, optionValue );

		if ( *optionValue != '\0' )
		{

#define VALIDATE_OPTION( __optionName, test ) \
if ( !_stricmp(optionName, __optionName) && ( test )) \
{ \
	DELETE_DOC_AND_RETURN( true ); \
}

#define VALIDATE_OPTION_INT( optionVar ) VALIDATE_OPTION( #optionVar, currentOptions->##optionVar##() != ( int ) strtol( value, NULL, 0 ) ) else
#define VALIDATE_OPTION_FLOAT( optionVar ) VALIDATE_OPTION( #optionVar, currentOptions->##optionVar##() != ( float ) strtod( value, NULL ) ) else
#define VALIDATE_OPTION_BOOL( optionVar ) VALIDATE_OPTION( #optionVar, currentOptions->##optionVar##() != ( *value != '0' ) ) else
#define VALIDATE_OPTION_FSTRING( optionVar ) VALIDATE_OPTION( #optionVar, currentOptions->##optionVar##() != fstring( value ) ) else
#define VALIDATE_OPTION_TIME( optionVar ) VALIDATE_OPTION( #optionVar, !IsEquivalent( FSConvert::MaxTickToSecs(currentOptions->##optionVar##()),( float ) atof( value )) ) else

			VALIDATE_OPTION_BOOL(ExportNormals)
			VALIDATE_OPTION_BOOL(ExportEPolyAsTriangles)
			VALIDATE_OPTION_BOOL(ExportEPolyAsTriangleStrips)
			VALIDATE_OPTION_BOOL(ExportXRefs)
			VALIDATE_OPTION_BOOL(ExportSelected)
			VALIDATE_OPTION_BOOL(ExportTangents)
			VALIDATE_OPTION_BOOL(ConvertLinearSplinesToLineStrips)
			VALIDATE_OPTION_BOOL(DeindexVertexData)
			VALIDATE_OPTION_BOOL(ExportAnimations)
			VALIDATE_OPTION_BOOL(SampleAnim)
			VALIDATE_OPTION_FLOAT(SampleRate)
			VALIDATE_OPTION_FLOAT(SecondarySampleRate)
			VALIDATE_OPTION_BOOL(ForceAnimationKeySampling)
			VALIDATE_OPTION_BOOL(TrimBipedAnims)
			VALIDATE_OPTION_BOOL(ExportDegenerateAnimationCurves)
			VALIDATE_OPTION_BOOL(ExportInvalidTransformAnimations)
			VALIDATE_OPTION_BOOL(ExportInvalidMorphTargetWeightAnimations)
			VALIDATE_OPTION_INT(CubicInterpolation)
			VALIDATE_OPTION_BOOL(CleanAnimKeys)
			VALIDATE_OPTION_BOOL(CompressAnimKeys)
			VALIDATE_OPTION_FLOAT(KeyCompressionTolerance)
			VALIDATE_OPTION_BOOL(OneChannelPerCurve)
			VALIDATE_OPTION_BOOL(BakeMatrices)
			VALIDATE_OPTION_BOOL(DecomposeMatrices)
			//VALIDATE_OPTION_BOOL(RemoveUnusedSkinJoints)
			VALIDATE_OPTION_BOOL(ExportRelativePaths)
			VALIDATE_OPTION_BOOL(RemovePaths)
			VALIDATE_OPTION_BOOL(ReplacePaths)
			VALIDATE_OPTION_FSTRING(TextureCoordinatesOrigin)
			VALIDATE_OPTION_FSTRING(ReplacementPath)
			VALIDATE_OPTION_BOOL(StripAssociatedNodes)
			VALIDATE_OPTION_INT(ExportUpAxis)
			VALIDATE_OPTION_INT(LibrariesToStrip)
			VALIDATE_OPTION_FSTRING(GeometryPositionFormat)
			VALIDATE_OPTION_FSTRING(GeometryNormalFormat)
			VALIDATE_OPTION_FSTRING(GeometryTextureFormat)
			VALIDATE_OPTION_FSTRING(GeometryColorFormat)
			VALIDATE_OPTION_BOOL(RemoveZeroWeights)
			VALIDATE_OPTION_BOOL(RemoveDuplicateWeights)
			;

			// warnings
			int numWarnings = currentOptions->m_warningManager.GetWarningCount();
			for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
			{
				ICWarning *pWarning = currentOptions->m_warningManager.GetWarning( warningIndex );
				VALIDATE_OPTION( pWarning->m_secondaryName, TEST_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED ) != ( *value != '0' ) );
			}

#undef VALIDATE_OPTION
#undef VALIDATE_OPTION_INT
#undef VALIDATE_OPTION_FLOAT
#undef VALIDATE_OPTION_BOOL
#undef VALIDATE_OPTION_FSTRING
#undef VALIDATE_OPTION_TIME
		}
	
		if ( semiColonIndex == -1 )
		{
			break;
		}
	}

	// Animation clips comparison
	fm::vector<AnimClip_Secs> oldAnimClips;
	fm::vector<AnimClip_Secs> currentAnimClips;

	// get current anim clips
	std::vector<int> &clips = AnimationClipManager::GetInst()->GetAnimClipsToExport();
	int numCurrentAnimClips = clips.size();
	for ( int i = 0; i < numCurrentAnimClips; ++i )
	{
		int clipId = clips[i];
		const char *name = AnimationClipManager::GetInst()->GetAnimClipName( clipId );
		float animClipStart = ( float )AnimationClipManager::GetInst()->GetAnimClipStart( clipId );
		float animClipEnd = ( float )AnimationClipManager::GetInst()->GetAnimClipEnd( clipId );
		currentAnimClips.push_back( AnimClip_Secs( TicksToSecs( animClipStart ), TicksToSecs( animClipEnd ), fm::string( name ) ) );
	}

	// get old anim clips
	pNode = FUXmlParser::FindChildByName( pRootNode, DAE_ASSET_ELEMENT );
	pNode = FUXmlParser::FindChildByName( pNode, DAE_CONTRIBUTOR_ASSET_ELEMENT );
	pNode = FUXmlParser::FindChildByName( pNode, EXPORTED_ANIMATION_CLIPS );

	if ( pNode != NULL )
	{
		for (xmlNode* pChild = pNode->children; pChild != NULL; pChild = pChild->next)
		{
			if (pChild->type != XML_ELEMENT_NODE) continue;

			if (IsEquivalent(pChild->name, DAE_ANIMCLIP_ELEMENT))
			{
				fm::string animClipNameStr = FUXmlParser::ReadNodeProperty(pChild, DAE_ID_ATTRIBUTE);
				fm::string animClipStartStr = FUXmlParser::ReadNodeProperty(pChild, DAE_START_ATTRIBUTE);
				fm::string animClipEndStr = FUXmlParser::ReadNodeProperty(pChild, DAE_END_ATTRIBUTE);

				float animClipStartSecs = FUStringConversion::ToFloat( animClipStartStr );
				float animClipEndSecs = FUStringConversion::ToFloat( animClipEndStr );

				oldAnimClips.push_back( AnimClip_Secs( 
					animClipStartSecs,
					animClipEndSecs,
					animClipNameStr	));
			}
		}
	}

	if ( oldAnimClips.size() != currentAnimClips.size() )
	{
		DELETE_DOC_AND_RETURN( true );
	}

	// sort
	oldAnimClips.sort( AnimClip_Secs::NameComparator() );
	currentAnimClips.sort( AnimClip_Secs::NameComparator() );

	// compare anim clips, one by one
	for ( size_t i = 0; i < oldAnimClips.size(); ++i )
	{
		if ( !IsEquivalent( oldAnimClips[i], currentAnimClips[i] ) )
		{
			DELETE_DOC_AND_RETURN( true );
		}
	}

	m_bSkippedFileExport = true;
	DELETE_DOC_AND_RETURN( false );

#undef DELETE_DOC_AND_RETURN
}

#undef SKIP_SPACES
#undef SKIP_COMMENT
#undef TRIM
#undef TOKEN_SEPARATOR
#undef MAX_OPTION_NAME
#undef MAX_OPTION_VALUE
#undef READ_OPTION
#undef READ_OPTION_BOOL
#undef READ_OPTION_FLOAT
#undef READ_OPTION_FSTRING

VersionNumber BatchExporter::FetchIrrColladaVersionNumberFromFile()
{
	HINSTANCE hInst = GetModuleHandle( "IrrColladaMax.dle" );
	char modulePath[MAX_PATH];
	FUAssert( GetModuleFileName( hInst, modulePath, MAX_PATH ), return VersionNumber::Invalid() );

	DWORD dummyHandle;
	int fileVersionInfoSize = 0;
	void *pBuf = NULL;
	unsigned int len = 0;

	FUAssert( ( fileVersionInfoSize = GetFileVersionInfoSize( modulePath, &dummyHandle ) ) > 0, return VersionNumber::Invalid() );

	void *pFileVersionInfoData = malloc( fileVersionInfoSize );
	FUAssert( GetFileVersionInfo( modulePath, dummyHandle, fileVersionInfoSize, pFileVersionInfoData ) > 0, return VersionNumber::Invalid() );

	FUAssert( VerQueryValue( pFileVersionInfoData, "\\", &pBuf, &len ) > 0, return VersionNumber::Invalid() );
	FUAssert( pBuf, return VersionNumber::Invalid() );

	VS_FIXEDFILEINFO *pFileVersionInfo = ( VS_FIXEDFILEINFO * )pBuf;

	m_irrColladaVersionNumber = VersionNumber( 
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionMS, 16, 0xffff ),
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionMS, 0, 0xffff ),
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionLS, 16, 0xffff ),
		GET_VALUE_SHIFTED_AND_MASKED( pFileVersionInfo->dwFileVersionLS, 0, 0xffff ) 
		);
	
	free( pFileVersionInfoData );
	return m_irrColladaVersionNumber;
}

VersionNumber BatchExporter::FetchIrrColladaVersionNumberFromResources()
{
	HINSTANCE hInst = GetModuleHandle( "IrrColladaMax.dle" );
	HRSRC resInfo = FindResource( hInst, (LPCTSTR)VS_VERSION_INFO, RT_VERSION );
	HGLOBAL resData = LoadResource( hInst, resInfo );
	LPVOID pData = LockResource( resData );
	DWORD vLen,langD;
	BOOL retVal;    
	LPVOID retbuf=NULL;
	char *csRet = NULL;
	static char fileEntry[256];

	if ( pData  == NULL )
	{
		return VersionNumber::Invalid();
	}

	sprintf( fileEntry,"\\VarFileInfo\\Translation" );
	retVal = VerQueryValue(pData,fileEntry,&retbuf,(UINT *)&vLen);
	if (retVal && vLen==4) 
	{
		memcpy(&langD,retbuf,4);            
		sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
			(langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
			(langD & 0xff0000)>>16, "FileVersion");  
	}
	else 
	{
		sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID(), "FileVersion");
	}

	if (VerQueryValue(pData,fileEntry,&retbuf,(PUINT)&vLen))
	{
		csRet = (TCHAR*)retbuf;
	}

	m_irrColladaVersionNumber = VersionNumber::Get( csRet );

	UnlockResource( resData );
	FreeResource( resData );

	return m_irrColladaVersionNumber;
}

VersionNumber BatchExporter::GetIrrColladaVersionNumber()
{
	return m_irrColladaVersionNumber;
}

bool BatchExporter::IsBatchExporting()
{
	return m_currentFileId >= 0;
}

void bool_BatchOption::ParseAndSetValue( fm::string valueAsString )
{
	m_value = valueAsString[0] != '0';
}

void fstring_BatchOption::ParseAndSetValue( fm::string valueAsString )
{
	m_value = valueAsString;
}

void ClearWarningLog()
{
	g_warningLog.clear();
}

void ClearExportLog()
{
	g_exportLog.clear();
}

void PrintExportLog()
{
	//show_listener();
	const char *exportLog = g_exportLog.ToCharPtr();

	// output to max script listener
	mprintf( exportLog );

	// output to log file
	fstring logFile = GET_BATCH_OPTION_VALUE( "LogFile", fstring );
	if ( !logFile.empty() )
	{
		const char *logFilePath = logFile.c_str();
		FILE *pFile = fopen( logFilePath, "w" );

		if ( !pFile )
		{
			mprintf( "Error accessing log file \"%s\"\n", logFilePath );
		}

		fputs( exportLog, pFile );

		fclose( pFile );
	}
}
