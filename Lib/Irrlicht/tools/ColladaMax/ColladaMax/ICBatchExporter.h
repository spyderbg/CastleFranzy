/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __BATCHEXPORTER__H
#define __BATCHEXPORTER__H

#include "VersionManager.h"

#include <vector>

class ColladaOptions;

#define GET_BATCH_OPTION( name, type )( ( type##_BatchOption *) BatchExporter::GetInst()->GetBatchOption( name ) )
#define GET_BATCH_OPTION_VALUE( name, type )( GET_BATCH_OPTION( name, type )->m_value )

class BatchOption
{
public:
	fm::string m_name;

public:
	BatchOption( fm::string name )
		: m_name( name )
	{}

	virtual void Reset(){}
	virtual void ParseAndSetValue( fm::string valueAsString ){}
};

#define DECLARE_BATCH_OPTION( type, default ) \
class type##_BatchOption : public BatchOption \
{ \
public: \
	type m_value; \
\
public: \
	type##_BatchOption( fm::string name, type value = default ) \
		: BatchOption( name ) \
		, m_value( value ) \
	{} \
\
	virtual void Reset(){ m_value = default; } \
	virtual void ParseAndSetValue( fm::string valueAsString ); \
	static type GetDefaultValue(){ return default; } \
}

DECLARE_BATCH_OPTION( bool, false );
DECLARE_BATCH_OPTION( uint32, 0 );
DECLARE_BATCH_OPTION( fstring, fstring() );

#undef DECLARE_BATCH_OPTION

class FileOptionMapping
{
public:
	std::string m_srcFilePath;
	std::string m_dstFilePath;
	ColladaOptions *m_pOptions;
public:
	FileOptionMapping();
	FileOptionMapping(FileOptionMapping const &copy);
	~FileOptionMapping();
};

class BatchExporter
{
public:
	enum
	{
		BE_INVALID = -1,
		BE_FILE,
		BE_DIRECTORY,
		BE_FILE_LIST
	};

private:
	static BatchExporter *s_pBatchExporter;

	BatchExporter();
	~BatchExporter();

	char *ParseFiles( char *token );
	char *ParseCustomScripts( char *token );
	char *ParseBatchOptions( char *token );
	void static ParseColladaOptions( const char *token, ColladaOptions *options );
	void ResetBatchOptions();
public:
	static BatchExporter *GetInst();
	static void Release();
	void Reset();
	void ParseCfg();
	void WriteOptions( CStr &str, ColladaOptions *options );
	bool ShouldExportFile( int fileId );
	bool IsBatchExporting();

	// These funcs should be added to VersionManager.  I however encountered link errors that 
	// I could not resolve when doing so.
	VersionNumber FetchIrrColladaVersionNumberFromResources();  // This func also crashes when batch exporting...
	VersionNumber FetchIrrColladaVersionNumberFromFile();

	VersionNumber GetIrrColladaVersionNumber();

	BatchOption *GetBatchOption( fm::string optionName );

	int m_exportMode;
	std::string m_exportSrc;
	std::string m_exportOpts;
	ColladaOptions *m_pDefaultColladaOptions;
	std::vector<FileOptionMapping> m_fileOptionsMap;
	int m_currentFileId;
	bool m_bSkippedFileExport;
	StringList m_customScripts;
	VersionNumber m_irrColladaVersionNumber;
	std::vector<BatchOption *> m_batchOptions;

	static const int k_numAnimClipOptions = 5;
	static const fm::string k_animClipOptions[k_numAnimClipOptions];
};

inline
BatchExporter *BatchExporter::GetInst()
{
	return s_pBatchExporter ? s_pBatchExporter : ( s_pBatchExporter = new BatchExporter() );
}

inline
void BatchExporter::Release()
{
	SAFE_DELETE( s_pBatchExporter );
}

void ClearWarningLog();
void ClearExportLog();
void PrintExportLog();


#endif // __BATCHEXPORTER__H
