/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __COLLADAEXPORTER__H
#define __COLLADAEXPORTER__H

#include <string>

// Exporter class ID
#define COLLADAEXPORTER_CLASS_ID	Class_ID(0x442057b7, 0x764b0df3)

// main exporter class
class COLLADAExporter : public SceneExport 
{
public:
	enum ExportMode
	{
		EM_DEFAULT
		, EM_PREVIEW
	};

private:
	bool m_bExportSuccess;
	ExportMode m_exportMode;
	fm::string m_previewCameraIdentifier;
	void ForceLowercaseExtension( std::string &name );

public:
	COLLADAExporter();
	~COLLADAExporter();

	// from SceneExport
	int				ExtCount();					// Number of extensions supported
	const TCHAR*	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR*	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR*	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR*	AuthorName();				// ASCII Author name
	const TCHAR*	CopyrightMessage();			// ASCII Copyright message
	const TCHAR*	OtherMessage1();			// Other message #1
	const TCHAR*	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

	BOOL			SupportsOptions(int ext, DWORD options);
	int				DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts=FALSE, DWORD options=0);

	inline bool GetExportSuccess()
	{
		return m_bExportSuccess;
	}

	inline void SetExportSuccess( bool bSuccess )
	{
		m_bExportSuccess = bSuccess;
	}

	fm::string GetOptionFileName();
	fm::string GetExportDialogCaption();

	inline void SetExportMode( ExportMode exportMode )
	{
		m_exportMode = exportMode;
	}

	inline ExportMode GetExportMode()
	{
		return m_exportMode;
	}

	inline void SetPreviewCameraIdentifier( const fm::string &identifier )
	{
		m_previewCameraIdentifier = identifier;
	}

	inline const fm::string &GetPreviewCameraIdentifier()
	{
		return m_previewCameraIdentifier;
	}
};

ClassDesc2* GetCOLLADAExporterDesc();

#endif // __COLLADAEXPORTER__H
