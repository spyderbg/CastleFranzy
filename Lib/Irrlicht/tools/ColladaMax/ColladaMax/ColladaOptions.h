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

#ifndef _COLLADA_OPTIONS_H_
#define _COLLADA_OPTIONS_H_

#include "FUtils/FUDaeEnum.h"
#include "FArchiveXMLOptions.h"
#include "resource.h"
#include "ICWarningManager.h"

class Interface;
class DocumentExporter;
class INode;
typedef int TimeValue;

#define ANIM_KEY_COMPRESSION_DEFAULT_TOLERANCE 0.01f
#define SAMPLE_RATE_DEFAULT_VALUE 30

#define OPTION_FILENAME_DEFAULT       "IrrColladaMax.ini"
#define	OPTION_FILENAME_PREVIEW       "IrrColladaMax_Preview.ini"

#define EXPORT_DIALOG_CAPTION_DEFAULT       "IrrColladaMax Export"
#define EXPORT_DIALOG_CAPTION_PREVIEW       "IrrColladaMax Export [Preview]"

class StrippableLibraryIdcToFlagMapping
{
public:
	int idc;
	int flag;
};

template<class T>
class RadioButton
{
public:
	int m_idc;
	T m_value;
	RadioButton( int idc = -1, T value = T() )
		: m_idc( idc )
		, m_value( value )
	{}
};

template<class T>
class RadioButtonGroup
{
private:
	fm::vector<RadioButton<T>> m_buttons;

public:
	// Order in which radio buttons are added is important
	void AddRadioButton( int idc, T value );
	void SetSelectedValue( HWND hWnd, T value );
	T GetSelectedValue( HWND hWnd );
	inline void Clear(){ m_buttons.clear(); }
};

class ColladaOptions
{
public:

	enum AXIS
	{
		X = 0
		, Y = 1
		, Z = 2
	};

	enum ROT_FORMAT
	{
		EULER_XYZ = 0
		, ANGLE_AXIS = 1
		, QUATERNION = 2
	};

	enum GEOMETRY_CHANNEL_FORMAT
	{
		GEOM_FORMAT_BYTE
		, GEOM_FORMAT_UNSIGNED_BYTE
		, GEOM_FORMAT_SHORT
		, GEOM_FORMAT_UNSIGNED_SHORT
		, GEOM_FORMAT_INT
		, GEOM_FORMAT_UNSIGNED_INT
		, GEOM_FORMAT_FLOAT
		, GEOM_FORMAT_COUNT
	};

	enum TEXTURE_COORDINATES_ORIGIN
	{
		TCO_LOWER_LEFT
		, TCO_UPPER_LEFT
		, TCO_COUNT
	};

	Interface* maxInterface;

	// options dialog state
	bool normals;			// export normals
	bool triangulate;		// convert Editable Polygons to triangles
	bool triangleStrips;	// converts Editable Polygons to triangle strips
	bool xrefs;				// export external references
	bool tangents;			// export tangents and binormals
	bool convertLinearSplinesToLineStrips; // export linear splines as line strips inside a <mesh> element
	bool animations;		// export animations;
	bool sampleAnim;		// export sampled animation
	float sampleRate;		// sampling rate
	float secondarySampleRate; // sampling rate currently used for controllers with constraints on them
	bool forceAnimationKeySampling; // forces sampling at animation key times
	bool trimBipedAnims;	// remove translation and scale animation tracks on every biped nodes except Bip01
	bool exportDegenerateAnimationCurves; // export degenerate animations if they have a valid animation controller
	bool exportInvalidTransformAnimations; // export degenerate animations on transforms even if associated controller is invalid
	bool exportInvalidMorphTargetWeightAnimations; // export degenerate animations on morph target weight even if associated controller is invalid
	FUDaeInterpolation::Interpolation cubicInterpolation; // Cubic interpolation method
	bool cleanAnimKeys;		// removes all animation keys referenced by 0 animation clips
	bool compressAnimKeys;	// removes redundant animation keys ( LINEAR sampling )
	float keyCompressionTolerance;
	bool oneChannelPerCurve;// create one channel per animation curve instead of multiple curves per channel
	bool bakeMatrices;		// export transforms as matrices
	bool decomposeMatrices;	// export transforms as decomposed matrices ( translation, rotation, scale )
	bool removeUnusedSkinJoints; // Removes unused skin joints from skin controllers
	int rotationFormat;		// 
	bool relativePaths;		// export relative paths
	bool removePaths;		// Strips path and keeps only the filename
	bool replacePaths;		// Replaces file path with a user provided one
	bool stripAssociatedNodes;	// strips nodes associated with stripped libraries
	int  exportUpAxis;		// export assets with the specified up axis
	int  librariesToStrip;	// defines what libraries are stripped
	bool removeZeroWeights;	// remove zero skin weights
	bool removeDuplicateWeights; // remove duplicate skin weights
	bool deindexVertexData; // Generates unique indices for all vertex data
	fstring textureCoordinatesOrigin; // Transforms UV coordinates according to the option chosen - Max's texture coordinate origin is LOWER_LEFT, so if UPPER_LEFT is chosen, v = ( 1.f - v )
	bool importUnits;		// import assets in their units
	bool importUpAxis;		// import assets in their up-axis
	fstring geometryPositionFormat; // Geometry Channel Position Format
	fstring geometryNormalFormat; // Geometry Channel Normal Format
	fstring geometryTextureFormat; // Geometry Channel Texture Format
	fstring geometryColorFormat; // Geometry Channel Color Format

	bool stripThirdTextureCoord; // force on

	// debug switch
	bool dbgSwitch;

	TimeValue	animStart;	// start sample TimeValue(frame * GetTicksPerFrame)
	TimeValue	animEnd;	// end sample TimeValue  (frame * GetTicksPerFrame)

	// not-persistent export options
	bool selectionOnly;		// export only selected nodes/objects

	// dialog control to flag map
	static const StrippableLibraryIdcToFlagMapping s_kStrippableLibraryIdcToFlagMap[];
	
	fstring replacementPath;

	// batch exporter specific
	fm::string animClipOptions;

	// ui specific option
	bool colladaConverterCompliance;

	// radio button groups
	RadioButtonGroup<FUDaeInterpolation::Interpolation> m_cubicInterpolationRadioButtonGroup;
	ICWarningManager m_warningManager;

	DocumentExporter *m_pDocumentExporter;

	static const int s_kTextureCoordinatesOriginStrings[ColladaOptions::TCO_COUNT];

public:
	ColladaOptions( Interface* maxInterface, DocumentExporter *pDocumentExporter = NULL );
	~ColladaOptions();

	// Displays the exporter options dialog to allow the user to change the options.
	bool ShowDialog(bool exporter);

	// Access the export options
	bool ExportNormals() const { return normals; }		
	bool ExportEPolyAsTriangles() const { return triangulate; }
	bool ExportEPolyAsTriangleStrips() const { return triangleStrips; }
	bool ExportXRefs() const { return xrefs; }
	bool ExportTangents() const { return tangents; }
	bool ConvertLinearSplinesToLineStrips() const { return convertLinearSplinesToLineStrips; }
	bool ExportAnimations() const { return animations; }
	bool SampleAnim() const { return sampleAnim; }	
	float SampleRate() const { return sampleRate; }
	float SecondarySampleRate() const { return secondarySampleRate; }
	bool ForceAnimationKeySampling() const { return forceAnimationKeySampling; }
	bool TrimBipedAnims() const { return trimBipedAnims; }	
	bool ExportDegenerateAnimationCurves() const { return exportDegenerateAnimationCurves; }	
	bool ExportInvalidTransformAnimations() const { return exportInvalidTransformAnimations; }	
	bool ExportInvalidMorphTargetWeightAnimations() const { return exportInvalidMorphTargetWeightAnimations; }	
	FUDaeInterpolation::Interpolation CubicInterpolation() const { return cubicInterpolation; }	
	bool CleanAnimKeys() const { return cleanAnimKeys; }	
	bool CompressAnimKeys() const { return compressAnimKeys; }	
	float KeyCompressionTolerance() const { return keyCompressionTolerance; }
	bool OneChannelPerCurve() const { return oneChannelPerCurve; }	
	bool BakeMatrices() const { return bakeMatrices; }
	bool DecomposeMatrices() const { return decomposeMatrices; }
	bool RemoveUnusedSkinJoints() const { return removeUnusedSkinJoints; }
	int RotationFormat() const { return rotationFormat; }
	bool ExportRelativePaths() const { return relativePaths; }	
	bool RemovePaths() const { return removePaths; }	
	bool ReplacePaths() const { return replacePaths; }	
	bool StripAssociatedNodes() const { return stripAssociatedNodes; }	
	bool ExportSelected() const { return selectionOnly; }
	int  ExportUpAxis() const { return exportUpAxis; }
	int  LibrariesToStrip() const { return librariesToStrip; }
	bool RemoveZeroWeights() const { return removeZeroWeights; }	
	bool RemoveDuplicateWeights() const { return removeDuplicateWeights; }	
	bool StripThirdTextureCoord() const { return stripThirdTextureCoord; }	
	bool DeindexVertexData() const { return deindexVertexData; }	
	fstring TextureCoordinatesOrigin() const { return textureCoordinatesOrigin; }
	fstring GeometryPositionFormat() const { return geometryPositionFormat; }
	fstring GeometryNormalFormat() const { return geometryNormalFormat; }
	fstring GeometryTextureFormat() const { return geometryTextureFormat; }
	fstring GeometryColorFormat() const { return geometryColorFormat; }
	FMVector3 ExportUpAxisAsVector() const;

	bool DbgSwitch() const { return dbgSwitch; }	

	TimeValue AnimStart() const { return animStart; }		
	TimeValue AnimEnd() const { return animEnd; }

	void ResolveExportedAnimsRange();

	// Access the import options
	bool ImportUnits() const { return importUnits; }
	bool ImportUpAxis() const { return importUpAxis; }

	// Change the animation start/end for sampling
	void SetAnimBounds(int start, int end) { animStart = start; animEnd = end; }
	void SetExportSelected(bool _selectionOnly) { selectionOnly = _selectionOnly; }

	bool DoSampleMatrices(INode* maxNode);
	bool IncludeObjOffset(INode* node);

	fstring ReplacementPath() const { return replacementPath; }

	static int GetStrippableLibraryIDC( int strippableLibraryId );
	static int GetStrippableLibraryFlag( int strippableLibraryId );

	// Save/Load the export options
	void SaveOptions( const fm::string &fileName );

	// returns -1 if file does not exist
	int LoadOptions( const fm::string &fileName );

	void RefreshExportOptions( HWND hWnd );

	ColladaOptions *Clone();
	void Reset();

	void CleanReplacementPath();

	void GetOptionsFromExportDialog( HWND hWnd );
	void InitExportDialog( HWND hWnd );

	void ProcessBatchExporterAnimClipOptions();
	void PreExportAnimClips();

	void DisplayPostExportMessageBox();

	void ApplyColladaConverterCompliance( HWND hWnd );

	FArchiveXMLOptions ConstructArchiveXMLOptions();

	inline void SetDocumentExporter( DocumentExporter *pDocumentExporter )
	{
		m_pDocumentExporter = pDocumentExporter;
	}

	inline DocumentExporter *GetDocumentExporter() const
	{
		return m_pDocumentExporter;
	}

	inline void SetMaxInterface( Interface *pInterface )
	{
		maxInterface = pInterface;
	}

	inline Interface *GetMaxInterface() const
	{
		return maxInterface;
	}

private:

	static const int s_kGeometryFormatOptionsStrings[ColladaOptions::GEOM_FORMAT_COUNT];
	static const GEOMETRY_CHANNEL_FORMAT s_kGeometryPositionFormatOptions[];
	static const GEOMETRY_CHANNEL_FORMAT s_kGeometryNormalFormatOptions[];
	static const GEOMETRY_CHANNEL_FORMAT s_kGeometryTextureFormatOptions[];
	static const GEOMETRY_CHANNEL_FORMAT s_kGeometryColorFormatOptions[];

	void ResetGeometryChannelFormatOptions();

	void RefreshAnimationOptions( HWND hWnd );
	void RefreshAnimClipDialog( HWND hWnd );
	void SaveAnimClipDialogData( HWND hWnd );
	void SaveAnimClipListData( HWND hList );
	void RefreshClipEdit( HWND hWnd );
	void RefreshKeyCompressionEdit( HWND hWnd );
	float GetKeyCompressionToleranceFromEditControls( HWND hWnd );

	DLG_PROC_DECL( ExportOptions )
	DLG_PROC_DECL( ImportOptions )
	DLG_PROC_DECL( BatchOptionString )
	DLG_PROC_DECL( AnimClip )
	DLG_PROC_DECL( Warning )
};

inline int ColladaOptions::GetStrippableLibraryIDC( int strippableLibraryId )
{
	return s_kStrippableLibraryIdcToFlagMap[strippableLibraryId].idc;
}

inline int ColladaOptions::GetStrippableLibraryFlag( int strippableLibraryId )
{
	return s_kStrippableLibraryIdcToFlagMap[strippableLibraryId].flag;
}

#endif // _COLLADA_OPTIONS_H_
