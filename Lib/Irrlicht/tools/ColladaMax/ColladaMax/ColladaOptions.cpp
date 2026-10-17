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
#include "ColladaOptions.h"
#include "FCDocument/FCDocument.h"
#include "FUtils/FUUri.h"
#include "AnimationClipManager.h"
#include "ColladaExporter.h"
#include "ICBatchExporter.h"
#include "ControllerExporter.h"
#include "UIHelpers.h"
#include "DocumentExporter.h"

#include <sstream>
#include <string>

#pragma warning(disable:4100)
#include <CS/BipedApi.h>
#pragma warning(default:4100)

extern FUStringBuilder g_warningLog;

const StrippableLibraryIdcToFlagMapping ColladaOptions::s_kStrippableLibraryIdcToFlagMap[] = 
{
	{ IDC_STRIP_LIB_ANIMS,				FArchiveXMLOptions::STRIP_LIB_ANIMS },
	{ IDC_STRIP_LIB_ANIMCLIPS,			FArchiveXMLOptions::STRIP_LIB_ANIMCLIPS },
	{ IDC_STRIP_LIB_CAMERAS,			FArchiveXMLOptions::STRIP_LIB_CAMERAS },
	{ IDC_STRIP_LIB_CONTROLLERS,		FArchiveXMLOptions::STRIP_LIB_CONTROLLERS },
	{ IDC_STRIP_LIB_GEOMETRIES,			FArchiveXMLOptions::STRIP_LIB_GEOMETRIES },
	{ IDC_STRIP_LIB_LIGHTS,				FArchiveXMLOptions::STRIP_LIB_LIGHTS },
	{ IDC_STRIP_LIB_NODES,				FArchiveXMLOptions::STRIP_LIB_NODES },
	{ IDC_STRIP_LIB_VISUALSCENES,		FArchiveXMLOptions::STRIP_LIB_VISUALSCENES },
	{ IDC_STRIP_LIB_EFFECTS,			FArchiveXMLOptions::STRIP_LIB_EFFECTS },
	{ IDC_STRIP_LIB_IMAGES,				FArchiveXMLOptions::STRIP_LIB_IMAGES },
	{ IDC_STRIP_LIB_MATERIALS,			FArchiveXMLOptions::STRIP_LIB_MATERIALS },
	{ IDC_STRIP_LIB_FORCEFIELDS,		FArchiveXMLOptions::STRIP_LIB_FORCEFIELDS },
	{ IDC_STRIP_LIB_PHYSICSMATERIALS,	FArchiveXMLOptions::STRIP_LIB_PHYSICSMATERIALS },
	{ IDC_STRIP_LIB_PHYSICSSCENES,		FArchiveXMLOptions::STRIP_LIB_PHYSICSSCENES },
	{ IDC_STRIP_LIB_PHYSICSMODELS,		FArchiveXMLOptions::STRIP_LIB_PHYSICSMODELS },
	{ IDC_STRIP_LIB_EXTRALIBS,			FArchiveXMLOptions::STRIP_LIB_EXTRALIBS },
	{ IDC_STRIP_LIB_SCENE,				FArchiveXMLOptions::STRIP_LIB_SCENE }
};

// GEOMETRY CHANNEL FORMAT
// indices correspond to ColladaOptions::GEOMETRY_CHANNEL_FORMAT enum values
const int ColladaOptions::s_kGeometryFormatOptionsStrings[ColladaOptions::GEOM_FORMAT_COUNT] = 
{
	IDS_GEOMETRY_CHANNEL_FORMAT_BYTE
	, IDS_GEOMETRY_CHANNEL_FORMAT_UNSIGNED_BYTE
	, IDS_GEOMETRY_CHANNEL_FORMAT_SHORT
	, IDS_GEOMETRY_CHANNEL_FORMAT_UNSIGNED_SHORT
	, IDS_GEOMETRY_CHANNEL_FORMAT_INT
	, IDS_GEOMETRY_CHANNEL_FORMAT_UNSIGNED_INT
	, IDS_GEOMETRY_CHANNEL_FORMAT_FLOAT
};

const ColladaOptions::GEOMETRY_CHANNEL_FORMAT ColladaOptions::s_kGeometryPositionFormatOptions[] = 
{
	ColladaOptions::GEOM_FORMAT_SHORT
	, ColladaOptions::GEOM_FORMAT_FLOAT
};

const ColladaOptions::GEOMETRY_CHANNEL_FORMAT ColladaOptions::s_kGeometryNormalFormatOptions[] = 
{
	ColladaOptions::GEOM_FORMAT_BYTE
	, ColladaOptions::GEOM_FORMAT_SHORT
	, ColladaOptions::GEOM_FORMAT_FLOAT
};

const ColladaOptions::GEOMETRY_CHANNEL_FORMAT ColladaOptions::s_kGeometryTextureFormatOptions[] = 
{
	ColladaOptions::GEOM_FORMAT_SHORT
	, ColladaOptions::GEOM_FORMAT_FLOAT
};

const ColladaOptions::GEOMETRY_CHANNEL_FORMAT ColladaOptions::s_kGeometryColorFormatOptions[] = 
{
	ColladaOptions::GEOM_FORMAT_UNSIGNED_BYTE
};

// TEXTURE COORDINATES ORIGIN
const int ColladaOptions::s_kTextureCoordinatesOriginStrings[ColladaOptions::TCO_COUNT] = 
{
	IDS_TCO_LOWER_LEFT
	, IDS_TCO_UPPER_LEFT
};

//
// Macros
//
#define MIN_KEY_COMPRESSION_COEFFICIENT 0
#define MAX_KEY_COMPRESSION_COEFFICIENT_POWER ( 9 )
#define MAX_KEY_COMPRESSION_COEFFICIENT pow( 10.f, MAX_KEY_COMPRESSION_COEFFICIENT_POWER )
#define MIN_KEY_COMPRESSION_EXPONENT (-4)
#define MAX_KEY_COMPRESSION_EXPONENT ( FLT_MAX_10_EXP - MAX_KEY_COMPRESSION_COEFFICIENT_POWER )

void GetKeyCompressionToleranceStandardForm( float keyCompressionTolerance, int &coefficient, int &exponent )
{
	exponent = -FMath::GetNumDecimals( keyCompressionTolerance, -MIN_KEY_COMPRESSION_EXPONENT );
	coefficient = ( int )( keyCompressionTolerance / pow( 10.f, (float)exponent ));
}

//
// ColladaOptions
//
ColladaOptions::ColladaOptions(Interface* _maxInterface, DocumentExporter *pDocumentExporter)
: m_pDocumentExporter( pDocumentExporter )
{
	maxInterface = _maxInterface;
	Reset();
}

ColladaOptions::~ColladaOptions()
{
	maxInterface = NULL;
}

// Displays the exporter options dialog to allow the user to change the options.
bool ColladaOptions::ShowDialog(bool exporter)
{
	// Prompt the user with our dialogbox, and get all the options.
	if (exporter)
	{
		BOOL doExport = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EXPORT_OPTIONS), maxInterface->GetMAXHWnd(), ExportOptionsDlgProcS, (LPARAM)this) != FALSE;
		if (!doExport) return false;
	}
	else
	{
		BOOL doImport = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_IMPORT_OPTIONS), maxInterface->GetMAXHWnd(), ImportOptionsDlgProcS, (LPARAM)this) != FALSE;
		if (!doImport) return false;
	}

	// Save the export options to the configuration file
	fm::string optionFileName = exporter ? m_pDocumentExporter->GetCOLLADAExporter()->GetOptionFileName() : OPTION_FILENAME_DEFAULT;
	SaveOptions( optionFileName );

	return true;
}

void ColladaOptions::DisplayPostExportMessageBox()
{
	if ( g_warningLog.empty() )
	{
		return;
	}

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_WARNING_DIALOG), maxInterface->GetMAXHWnd(), WarningDlgProcS, (LPARAM)this);
}

void ColladaOptions::GetOptionsFromExportDialog( HWND hWnd )
{
	// hit OK, pick up control values & end dialog
	colladaConverterCompliance = IsDlgButtonChecked(hWnd, IDC_CONVERTER_COMPLIANCE) == BST_CHECKED;
	bakeMatrices = IsDlgButtonChecked(hWnd, IDC_BAKE_MATRICES) == BST_CHECKED;
	decomposeMatrices = IsDlgButtonChecked(hWnd, IDC_DECOMPOSE_MATRICES) == BST_CHECKED;
	relativePaths = IsDlgButtonChecked(hWnd, IDC_RELATIVE_PATHS) == BST_CHECKED;
	removePaths = IsDlgButtonChecked(hWnd, IDC_REMOVE_PATHS) == BST_CHECKED;
	replacePaths = IsDlgButtonChecked(hWnd, IDC_REPLACE_PATHS) == BST_CHECKED;
	stripAssociatedNodes = IsDlgButtonChecked(hWnd, IDC_STRIP_ASSOCIATED_NODES_CHECKBOX) == BST_CHECKED;
	animations = IsDlgButtonChecked(hWnd, IDC_ANIM_ENABLE) == BST_CHECKED;
	sampleAnim = IsDlgButtonChecked(hWnd, IDC_ANIM_SAMPLE) == BST_CHECKED;
	forceAnimationKeySampling = IsDlgButtonChecked(hWnd, IDC_ANIM_FORCE_ANIMATION_KEY_SAMPLING) == BST_CHECKED;
	trimBipedAnims = IsDlgButtonChecked(hWnd, IDC_TRIM_BIPED_ANIMS) == BST_CHECKED;
	exportDegenerateAnimationCurves = IsDlgButtonChecked(hWnd, IDC_EXPORT_DEGENERATE_CURVES) == BST_CHECKED;
	exportInvalidTransformAnimations = IsDlgButtonChecked( hWnd, IDC_EXPORT_INVALID_TRANSFORM_ANIMATIONS ) == BST_CHECKED;
	exportInvalidMorphTargetWeightAnimations = IsDlgButtonChecked( hWnd, IDC_EXPORT_INVALID_MORPH_TARGET_WEIGHT_ANIMATIONS ) == BST_CHECKED;
	cleanAnimKeys = IsDlgButtonChecked(hWnd, IDC_CLEAN_ANIM_KEYS) == BST_CHECKED;
	compressAnimKeys = IsDlgButtonChecked(hWnd, IDC_COMPRESS_KEYS) == BST_CHECKED;
	oneChannelPerCurve = IsDlgButtonChecked(hWnd, IDC_ONECHANNEL_PER_CURVE) == BST_CHECKED;
	normals = IsDlgButtonChecked(hWnd, IDC_GEOM_NORMALS) == BST_CHECKED;
	triangulate = IsDlgButtonChecked(hWnd, IDC_GEOM_TRIANGLES) == BST_CHECKED;
	triangleStrips = IsDlgButtonChecked(hWnd, IDC_GEOM_TRIANGLE_STRIPS) == BST_CHECKED;
	xrefs = IsDlgButtonChecked(hWnd, IDC_GEOM_XREFS) == BST_CHECKED;
	tangents = IsDlgButtonChecked(hWnd, IDC_GEOM_TANGENTS) == BST_CHECKED;
	convertLinearSplinesToLineStrips = IsDlgButtonChecked(hWnd, IDC_CONVERT_LINEAR_SPLINES_TO_LINE_STRIPS) == BST_CHECKED;
	removeZeroWeights = IsDlgButtonChecked(hWnd, IDC_REMOVE_ZEROWEIGHTS) == BST_CHECKED;
	removeDuplicateWeights = IsDlgButtonChecked(hWnd, IDC_REMOVE_DUPLICATEWEIGHTS) == BST_CHECKED;
	deindexVertexData = IsDlgButtonChecked(hWnd, IDC_DEINDEX_VERTEX_DATA) == BST_CHECKED;
	dbgSwitch = IsDlgButtonChecked(hWnd, IDC_DEBUG_SWITCH_0) == BST_CHECKED;

	// warnings
	m_warningManager.SetWarningValuesFromWidgets( hWnd );

	if ( IsDlgButtonChecked( hWnd, IDC_Y_UP ) == BST_CHECKED )
	{
		exportUpAxis = Y;
	}
	else if ( IsDlgButtonChecked( hWnd, IDC_Z_UP ) == BST_CHECKED )
	{
		exportUpAxis = Z;
	}

	cubicInterpolation = m_cubicInterpolationRadioButtonGroup.GetSelectedValue( hWnd );

	RESETALL_FLAG( librariesToStrip );
	for ( int i = 0; i < FArchiveXMLOptions::GetNumStrippableLibraries(); i++ )
	{
		SET_FLAG( librariesToStrip, IsDlgButtonChecked(hWnd, ColladaOptions::GetStrippableLibraryIDC( i )) != BST_CHECKED ? ColladaOptions::GetStrippableLibraryFlag( i ) : 0 );
	}

	// Geometry Channel Format
	{
#define GET_COMBOBOX_CURSEL( comboBoxIdc, curString ) \
{ \
	HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc ); \
	int __curSelIdx = SendMessage( hComboBox, CB_GETCURSEL, NULL, NULL ); \
	int __curItemData = SendMessage( hComboBox, CB_GETITEMDATA, __curSelIdx, (LPARAM) NULL ); \
	curString = GetString( s_kGeometryFormatOptionsStrings[__curItemData] ); \
}

		GET_COMBOBOX_CURSEL( IDC_GEOMETRY_FORMAT_POSITION_COMBOBOX, geometryPositionFormat );
		GET_COMBOBOX_CURSEL( IDC_GEOMETRY_FORMAT_NORMAL_COMBOBOX, geometryNormalFormat );
		GET_COMBOBOX_CURSEL( IDC_GEOMETRY_FORMAT_TEXTURE_COMBOBOX, geometryTextureFormat );
		GET_COMBOBOX_CURSEL( IDC_GEOMETRY_FORMAT_COLOR_COMBOBOX, geometryColorFormat );

#undef GET_COMBOBOX_CURSEL	
	}

	// textureCoordinatesOrigin
	{
		int curSelIdx = UIHelpers::GetComboBoxSelectedIndex( hWnd, IDC_TCO_COMBOBOX );
		textureCoordinatesOrigin = GetString( s_kTextureCoordinatesOriginStrings[curSelIdx] );
	}

	// Extract replacement path
	HWND hwndEdit = GetDlgItem( hWnd, IDC_REPLACEMENT_PATH );
	char inputPath [MAX_PATH + 1];
	Edit_GetText( hwndEdit, inputPath, MAX_PATH + 1 );
	replacementPath.clear();
	replacementPath += inputPath;

	// get anim key compression tolerance
	keyCompressionTolerance = GetKeyCompressionToleranceFromEditControls( hWnd );

	// sample rate
	sampleRate = UIHelpers::GetSpinnerFValue( hWnd, IDC_SAMPLE_RATE_SPIN );

	// secondary sample rate
	secondarySampleRate = UIHelpers::GetSpinnerFValue( hWnd, IDC_SECONDARY_SAMPLE_RATE_SPIN );
}

// if hWnd = NULL, only modifies the options, not the associated widgets
void ColladaOptions::ApplyColladaConverterCompliance( HWND hWnd )
{
	if ( colladaConverterCompliance )
	{
#define FORCE_SET_OPTION( option, optionIDC, value ) \
option = value; \
if ( hWnd ) \
{ \
	CheckDlgButton(hWnd, optionIDC, value); \
	UIHelpers::SetControlEnabled(hWnd, optionIDC, false ); \
}

		FORCE_SET_OPTION( triangulate, IDC_GEOM_TRIANGLES, true );
		FORCE_SET_OPTION( sampleAnim, IDC_ANIM_SAMPLE, true );
		FORCE_SET_OPTION( bakeMatrices, IDC_BAKE_MATRICES, true );
		FORCE_SET_OPTION( decomposeMatrices, IDC_DECOMPOSE_MATRICES, true );
		FORCE_SET_OPTION( removeZeroWeights, IDC_REMOVE_ZEROWEIGHTS, true );
		FORCE_SET_OPTION( deindexVertexData, IDC_DEINDEX_VERTEX_DATA, true );
		FORCE_SET_OPTION( oneChannelPerCurve, IDC_ONECHANNEL_PER_CURVE, false );

		m_cubicInterpolationRadioButtonGroup.SetSelectedValue( hWnd, FUDaeInterpolation::DEFAULT_CUBIC );

		int numWarnings = m_warningManager.GetWarningCount();
		for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
		{
			ICWarning *pWarning = m_warningManager.GetWarning( warningIndex );
			if ( TEST_FLAG( pWarning->m_flags, ICWarningFlags::COMPLY_CONVERTER ))
			{
				SET_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED );
				if ( hWnd )
				{
					CheckDlgButton(hWnd, pWarning->m_idc, true );
					UIHelpers::SetControlEnabled(hWnd, pWarning->m_idc, false );
				}
			}
		}

#undef FORCE_SET_OPTION
	}
	else
	{
		if ( hWnd )
		{
			UIHelpers::SetControlEnabled(hWnd, IDC_GEOM_TRIANGLES, true );
			UIHelpers::SetControlEnabled(hWnd, IDC_BAKE_MATRICES, true );
			UIHelpers::SetControlEnabled(hWnd, IDC_DECOMPOSE_MATRICES, true );
			UIHelpers::SetControlEnabled(hWnd, IDC_REMOVE_ZEROWEIGHTS, true );
			UIHelpers::SetControlEnabled(hWnd, IDC_DEINDEX_VERTEX_DATA, true );

			int numWarnings = m_warningManager.GetWarningCount();
			for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
			{
				ICWarning *pWarning = m_warningManager.GetWarning( warningIndex );
				if ( TEST_FLAG( pWarning->m_flags, ICWarningFlags::COMPLY_CONVERTER ))
				{
					UIHelpers::SetControlEnabled(hWnd, pWarning->m_idc, true );
				}
			}
		}
	}
}

void ColladaOptions::RefreshAnimationOptions( HWND hWnd )
{
	UIHelpers::SetControlEnabled(hWnd, IDC_ANIM_SAMPLE, animations && !colladaConverterCompliance );

	UIHelpers::SetControlEnabled(hWnd, IDC_SAMPLE_RATE_LABEL, animations );
	UIHelpers::SetControlEnabled(hWnd, IDC_SAMPLE_RATE_EDIT, animations );
	UIHelpers::SetControlEnabled(hWnd, IDC_SAMPLE_RATE_SPIN, animations );

	UIHelpers::SetControlEnabled(hWnd, IDC_SECONDARY_SAMPLE_RATE_LABEL, animations );
	UIHelpers::SetControlEnabled(hWnd, IDC_SECONDARY_SAMPLE_RATE_EDIT, animations );
	UIHelpers::SetControlEnabled(hWnd, IDC_SECONDARY_SAMPLE_RATE_SPIN, animations );

	UIHelpers::SetControlEnabled(hWnd, IDC_ANIM_FORCE_ANIMATION_KEY_SAMPLING, animations );

	UIHelpers::SetControlEnabled(hWnd, IDC_ONECHANNEL_PER_CURVE, animations && !colladaConverterCompliance );
	UIHelpers::SetControlEnabled(hWnd, ID_OPEN_CLIP_DIALOG, animations);
	UIHelpers::SetControlEnabled(hWnd, IDC_TRIM_BIPED_ANIMS, animations);
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESS_KEYS, animations );

	UIHelpers::SetControlEnabled(hWnd, IDC_EXPORT_DEGENERATE_CURVES, animations);
	UIHelpers::SetControlEnabled(hWnd, IDC_EXPORT_INVALID_TRANSFORM_ANIMATIONS, animations);
	UIHelpers::SetControlEnabled(hWnd, IDC_EXPORT_INVALID_MORPH_TARGET_WEIGHT_ANIMATIONS, animations);

	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_LABEL_NAME, compressAnimKeys && animations && oneChannelPerCurve );
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_LABEL_EXPONENT, compressAnimKeys && animations && oneChannelPerCurve );
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_LABEL_EQUAL, compressAnimKeys && animations && oneChannelPerCurve );
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_EDIT_COEFFICIENT, compressAnimKeys && animations && oneChannelPerCurve );
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_SPIN_COEFFICIENT, compressAnimKeys && animations && oneChannelPerCurve );
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_EDIT_EXPONENT, compressAnimKeys && animations && oneChannelPerCurve );
	UIHelpers::SetControlEnabled(hWnd, IDC_COMPRESSION_TOLERANCE_SPIN_EXPONENT, compressAnimKeys && animations && oneChannelPerCurve );

	UIHelpers::SetControlEnabled(hWnd, IDC_CUBIC_INTERPOLATION_GROUPBOX, animations && !colladaConverterCompliance );
	UIHelpers::SetControlEnabled(hWnd, IDC_CUBIC_INTERPOLATION_HERMITE_RADIOBUTTON, animations && !colladaConverterCompliance );
	UIHelpers::SetControlEnabled(hWnd, IDC_CUBIC_INTERPOLATION_BEZIER_RADIOBUTTON, animations && !colladaConverterCompliance );

	UIHelpers::SetControlEnabled(hWnd, IDC_CLEAN_ANIM_KEYS, false );
	//UIHelpers::SetControlEnabled(hWnd, IDC_CLEAN_ANIM_KEYS, animations );
}

void ColladaOptions::RefreshClipEdit( HWND hWnd )
{
	std::stringstream stream;

	int numClips = AnimationClipManager::GetInst()->GetNumAnimClips();
	for (int i = 0; i < numClips; i++)
	{
		int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
		if ( TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ))
		{
			stream << AnimationClipManager::GetInst()->GetAnimClipListBoxName( i ) << " ; ";
		}
	}

	HWND hwndEdit = GetDlgItem( hWnd, IDC_ANIM_CLIP_EDIT );
	Edit_SetText( hwndEdit, stream.str().c_str() );
}

void ColladaOptions::RefreshKeyCompressionEdit( HWND hWnd )
{
	int exponent = 0;
	int coefficient = 0;
	GetKeyCompressionToleranceStandardForm( keyCompressionTolerance, coefficient, exponent );

	UIHelpers::SetEditControlValue( hWnd, IDC_COMPRESSION_TOLERANCE_EDIT_FLOAT, keyCompressionTolerance );

	// set spinner value
	UIHelpers::SetSpinnerValue( hWnd, IDC_COMPRESSION_TOLERANCE_SPIN_EXPONENT, exponent );
	UIHelpers::SetSpinnerValue( hWnd, IDC_COMPRESSION_TOLERANCE_SPIN_COEFFICIENT, coefficient );
}

void RefreshGeometryFormatGroupBox( HWND hWnd )
{
	// TODO
	//SetBKColor( hComboBox,
	//SetTextColor( GetDC( hComboBox ), RGB( 255, 0, 0 ) );
}

float ColladaOptions::GetKeyCompressionToleranceFromEditControls( HWND hWnd )
{
	int coefficient = UIHelpers::GetSpinnerIValue( hWnd, IDC_COMPRESSION_TOLERANCE_SPIN_COEFFICIENT );
	int exponent = UIHelpers::GetSpinnerIValue( hWnd, IDC_COMPRESSION_TOLERANCE_SPIN_EXPONENT );
	return FMath::ldexp( (float)coefficient, (float)exponent, 10.f );
}

void ColladaOptions::InitExportDialog( HWND hWnd )
{
	CenterWindow(hWnd, GetParent(hWnd));  

	{
#define INIT_COMBOBOX( comboBoxIdc, formatEnumValues ) \
{ \
	HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc ); \
	for ( int i = 0; i < ARRAY_SIZE( formatEnumValues ); ++i ) \
	{ \
		SendMessage( hComboBox, CB_ADDSTRING, NULL, (LPARAM) (LPCTSTR)GetString( s_kGeometryFormatOptionsStrings[formatEnumValues[i]] ) ); \
		SendMessage( hComboBox, CB_SETITEMDATA, i, (LPARAM) (DWORD)formatEnumValues[i] ); \
	} \
}

		INIT_COMBOBOX( IDC_GEOMETRY_FORMAT_POSITION_COMBOBOX, s_kGeometryPositionFormatOptions );
		INIT_COMBOBOX( IDC_GEOMETRY_FORMAT_NORMAL_COMBOBOX, s_kGeometryNormalFormatOptions );
		INIT_COMBOBOX( IDC_GEOMETRY_FORMAT_TEXTURE_COMBOBOX, s_kGeometryTextureFormatOptions );
		INIT_COMBOBOX( IDC_GEOMETRY_FORMAT_COLOR_COMBOBOX, s_kGeometryColorFormatOptions );

#undef INIT_COMBOBOX
	}

	// textureCoordinatesOrigin combo box
	{
		for ( int i = 0; i < ARRAY_SIZE( s_kTextureCoordinatesOriginStrings ); ++i )
		{
			UIHelpers::AddComboBoxString( hWnd, IDC_TCO_COMBOBOX, (LPCTSTR)GetString( s_kTextureCoordinatesOriginStrings[i] ), i );
		}
	}

	// sampling rate
	UIHelpers::InitSpinner( hWnd, 
		IDC_SAMPLE_RATE_SPIN, 
		IDC_SAMPLE_RATE_EDIT, 
		EDITTYPE_FLOAT, 
		1.f, 
		TIME_TICKSPERSEC,
		1.f,
		SAMPLE_RATE_DEFAULT_VALUE );

	// secondary sampling rate
	UIHelpers::InitSpinner( hWnd, 
		IDC_SECONDARY_SAMPLE_RATE_SPIN, 
		IDC_SECONDARY_SAMPLE_RATE_EDIT, 
		EDITTYPE_FLOAT,
		1.f,
		TIME_TICKSPERSEC,
		1.f,
		SAMPLE_RATE_DEFAULT_VALUE );

	// animation clips
	{
		AnimationClipManager::GetInst()->PrepareAnimClipListBoxNames();

		// Set initial animation clip selection states
		int numClips = AnimationClipManager::GetInst()->GetNumAnimClips();
		boolean bAtLeastOneUserCreatedClip = false;
		for (int i = 0; i < numClips; i++)
		{ 
			int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
			if ( !TEST_FLAG( currentFlags, AnimClip_Ticks::STANDARD_ANIM_CLIP ))
			{
				AnimationClipManager::GetInst()->SetAnimClipFlags( i, SET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ));
				bAtLeastOneUserCreatedClip = true;
			}
		}

		if ( !bAtLeastOneUserCreatedClip )
		{
			for (int i = 0; i < numClips; i++) 
			{
				int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
				if ( TEST_FLAG( currentFlags, AnimClip_Ticks::STANDARD_ANIM_CLIP ))
				{
					AnimationClipManager::GetInst()->SetAnimClipFlags( i, SET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ));
					break;
				}
			}
		}
	}

	int exponent = 0;
	int coefficient = 0;
	GetKeyCompressionToleranceStandardForm( keyCompressionTolerance, coefficient, exponent );

	// init key compression tolerance
	UIHelpers::InitSpinner( hWnd, 
		IDC_COMPRESSION_TOLERANCE_SPIN_EXPONENT, 
		IDC_COMPRESSION_TOLERANCE_EDIT_EXPONENT, 
		EDITTYPE_INT, 
		MIN_KEY_COMPRESSION_EXPONENT,
		MAX_KEY_COMPRESSION_EXPONENT,
		1.f,
		exponent );

	UIHelpers::InitSpinner( hWnd, 
		IDC_COMPRESSION_TOLERANCE_SPIN_COEFFICIENT, 
		IDC_COMPRESSION_TOLERANCE_EDIT_COEFFICIENT, 
		EDITTYPE_INT, 
		MIN_KEY_COMPRESSION_COEFFICIENT, 
		MAX_KEY_COMPRESSION_COEFFICIENT,
		10.f,
		coefficient );

	RefreshExportOptions( hWnd );
}

void ColladaOptions::RefreshExportOptions( HWND hWnd )
{
	// setup checkboxes
	CheckDlgButton(hWnd, IDC_CONVERTER_COMPLIANCE, colladaConverterCompliance);
	CheckDlgButton(hWnd, IDC_GEOM_NORMALS, normals);
	CheckDlgButton(hWnd, IDC_GEOM_TRIANGLES, triangulate);
	CheckDlgButton(hWnd, IDC_GEOM_TRIANGLE_STRIPS, triangleStrips );
	CheckDlgButton(hWnd, IDC_GEOM_XREFS, xrefs);
	CheckDlgButton(hWnd, IDC_GEOM_TANGENTS, tangents);
	CheckDlgButton(hWnd, IDC_CONVERT_LINEAR_SPLINES_TO_LINE_STRIPS, convertLinearSplinesToLineStrips);
	CheckDlgButton(hWnd, IDC_ANIM_ENABLE, animations);
	CheckDlgButton(hWnd, IDC_ANIM_SAMPLE, sampleAnim );
	CheckDlgButton(hWnd, IDC_ANIM_FORCE_ANIMATION_KEY_SAMPLING, forceAnimationKeySampling );
	CheckDlgButton(hWnd, IDC_TRIM_BIPED_ANIMS, trimBipedAnims );
	CheckDlgButton(hWnd, IDC_EXPORT_DEGENERATE_CURVES, exportDegenerateAnimationCurves );
	CheckDlgButton(hWnd, IDC_EXPORT_INVALID_TRANSFORM_ANIMATIONS, exportInvalidTransformAnimations );
	CheckDlgButton(hWnd, IDC_EXPORT_INVALID_MORPH_TARGET_WEIGHT_ANIMATIONS, exportInvalidMorphTargetWeightAnimations );
	CheckDlgButton(hWnd, IDC_CLEAN_ANIM_KEYS, cleanAnimKeys);
	CheckDlgButton(hWnd, IDC_COMPRESS_KEYS, compressAnimKeys);
	CheckDlgButton(hWnd, IDC_ONECHANNEL_PER_CURVE, oneChannelPerCurve);
	CheckDlgButton(hWnd, IDC_BAKE_MATRICES, bakeMatrices);
	CheckDlgButton(hWnd, IDC_DECOMPOSE_MATRICES, decomposeMatrices);
	CheckDlgButton(hWnd, IDC_RELATIVE_PATHS, relativePaths);
	CheckDlgButton(hWnd, IDC_REMOVE_PATHS, removePaths);
	CheckDlgButton(hWnd, IDC_REPLACE_PATHS, replacePaths );
	CheckDlgButton(hWnd, IDC_STRIP_ASSOCIATED_NODES_CHECKBOX, stripAssociatedNodes );
	CheckDlgButton(hWnd, IDC_REMOVE_ZEROWEIGHTS, removeZeroWeights);
	CheckDlgButton(hWnd, IDC_REMOVE_DUPLICATEWEIGHTS, removeDuplicateWeights);
	CheckDlgButton(hWnd, IDC_DEINDEX_VERTEX_DATA, deindexVertexData);
	CheckDlgButton(hWnd, IDC_DEBUG_SWITCH_0, dbgSwitch);

	// warnings
	m_warningManager.SetWidgetsFromWarningValues( hWnd );

	for ( int i = 0; i < FArchiveXMLOptions::GetNumStrippableLibraries(); i++ )
	{
		CheckDlgButton(hWnd, ColladaOptions::GetStrippableLibraryIDC( i ), !TEST_FLAG( librariesToStrip, ColladaOptions::GetStrippableLibraryFlag( i )) );
	}

	// set replacement path
	UIHelpers::SetControlEnabled(hWnd, IDC_REPLACEMENT_PATH, replacePaths);
	HWND hwndEdit = GetDlgItem( hWnd, IDC_REPLACEMENT_PATH );
	Edit_SetText( hwndEdit, replacementPath.c_str() );

	// setup radio buttons
	if ( exportUpAxis == Y )
	{
		CheckRadioButton( hWnd, IDC_Z_UP, IDC_Y_UP, IDC_Y_UP );
	}
	else if ( exportUpAxis == Z )
	{
		CheckRadioButton( hWnd, IDC_Z_UP, IDC_Y_UP, IDC_Z_UP );
	}

	// Geometry Channel Format
	ResetGeometryChannelFormatOptions(); // overriding values in IrrCollada.ini to prevent issues with Irrlicht

	{
#define REFRESH_COMBOBOX( comboBoxIdc, curString ) \
{ \
	HWND hComboBox = GetDlgItem( hWnd, comboBoxIdc ); \
	/* getting index value corresponding to string */ \
	int __comboBoxItemIdx = SendMessage( hComboBox, CB_FINDSTRINGEXACT, -1, (LPARAM) (LPCTSTR)curString.c_str() ); \
	if ( __comboBoxItemIdx != CB_ERR ) \
	{ \
		SendMessage( hComboBox, CB_SETCURSEL, __comboBoxItemIdx, NULL ); \
	} \
}

		REFRESH_COMBOBOX( IDC_GEOMETRY_FORMAT_POSITION_COMBOBOX, geometryPositionFormat );
		REFRESH_COMBOBOX( IDC_GEOMETRY_FORMAT_NORMAL_COMBOBOX, geometryNormalFormat );
		REFRESH_COMBOBOX( IDC_GEOMETRY_FORMAT_TEXTURE_COMBOBOX, geometryTextureFormat );
		REFRESH_COMBOBOX( IDC_GEOMETRY_FORMAT_COLOR_COMBOBOX, geometryColorFormat );

#undef REFRESH_COMBOBOX
	}

	RefreshGeometryFormatGroupBox( hWnd );

	// textureCoordinatesOrigin combo box
	{
		int comboBoxItemIdx = UIHelpers::GetStringIndex( hWnd, IDC_TCO_COMBOBOX, textureCoordinatesOrigin.c_str() );
		if ( comboBoxItemIdx != -1 )
		{
			UIHelpers::SetComboBoxSelectedIndex( hWnd, IDC_TCO_COMBOBOX, comboBoxItemIdx );
		}
	}

	// radio button groups
	m_cubicInterpolationRadioButtonGroup.SetSelectedValue( hWnd, cubicInterpolation );

	UIHelpers::SetSpinnerValue( hWnd, IDC_SAMPLE_RATE_SPIN, sampleRate );
	UIHelpers::SetSpinnerValue( hWnd, IDC_SECONDARY_SAMPLE_RATE_SPIN, secondarySampleRate );

	ApplyColladaConverterCompliance( hWnd );
	RefreshAnimationOptions( hWnd );

	RefreshClipEdit( hWnd );

#ifdef _DEBUG
	HWND hwndDbgSwitch = GetDlgItem( hWnd, IDC_DEBUG_SWITCH_0 );

	UIHelpers::SetControlEnabled( hWnd, IDC_DEBUG_SWITCH_0, true );
	int style = GetWindowLong( hwndDbgSwitch, GWL_STYLE );
	SetWindowLong( hwndDbgSwitch, GWL_STYLE, style | WS_VISIBLE );
#endif // _DEBUG

	RefreshKeyCompressionEdit( hWnd );

	// Set Dialog Caption
	{
		SetWindowText( hWnd, m_pDocumentExporter->GetCOLLADAExporter()->GetExportDialogCaption().c_str() );
	}

	// Import Main Export Settings button
	{
		if ( m_pDocumentExporter->GetCOLLADAExporter()->GetExportMode() == COLLADAExporter::EM_PREVIEW )
		{
			UIHelpers::SetControlEnabled( hWnd, ID_IMPORT_MAIN_EXPORT_SETTINGS, TRUE );
			UIHelpers::SetControlVisible( hWnd, ID_IMPORT_MAIN_EXPORT_SETTINGS, TRUE );
		}
	}
}

// optins dialog message handler
DLG_PROC_IMPL( ColladaOptions, ExportOptions )
{
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_INITDIALOG: 
		{
			// disabling option
			cleanAnimKeys = false;
			InitExportDialog( hWnd );
			break;
		}

		// Message sent when gaining focus after child dialog is destroyed
		case WM_ENABLE:
		{
			if ( wParam == TRUE )
			{
				RefreshClipEdit( hWnd );
				break;
			}
			return FALSE;
		}

		case CC_SPINNER_CHANGE:
			{
				int ctrlId = loWord;

				if ( ctrlId == IDC_COMPRESSION_TOLERANCE_SPIN_COEFFICIENT ||
					ctrlId == IDC_COMPRESSION_TOLERANCE_SPIN_EXPONENT )
				{
					keyCompressionTolerance = GetKeyCompressionToleranceFromEditControls( hWnd );
					UIHelpers::SetEditControlValue( hWnd, IDC_COMPRESSION_TOLERANCE_EDIT_FLOAT, keyCompressionTolerance );
					break;
				}
				return FALSE;
			}

		case WM_COMMAND:
		{
			switch (loWord) {
				case IDC_ANIM_SAMPLE:
				case IDC_ONECHANNEL_PER_CURVE:
				case IDC_COMPRESS_KEYS:
				case IDC_ANIM_ENABLE:
				{
					sampleAnim = IsDlgButtonChecked(hWnd, IDC_ANIM_SAMPLE) == BST_CHECKED;
					animations = IsDlgButtonChecked(hWnd, IDC_ANIM_ENABLE) == BST_CHECKED;
					oneChannelPerCurve = IsDlgButtonChecked(hWnd, IDC_ONECHANNEL_PER_CURVE) == BST_CHECKED;
					compressAnimKeys = IsDlgButtonChecked(hWnd, IDC_COMPRESS_KEYS) == BST_CHECKED;
					RefreshAnimationOptions( hWnd );
					break; 
				}

				case IDC_COMPRESSION_TOLERANCE_EDIT_COEFFICIENT:
				{
					if ( hiWord == EN_KILLFOCUS ) //EN_CHANGE ) //EN_UPDATE )
					{
						keyCompressionTolerance = GetKeyCompressionToleranceFromEditControls( hWnd );
						UIHelpers::SetEditControlValue( hWnd, IDC_COMPRESSION_TOLERANCE_EDIT_FLOAT, keyCompressionTolerance );
					}
					break;
				}

				case ID_SELECT_ALL_LIBRARIES:
					{
						for ( int i = 0; i < FArchiveXMLOptions::GetNumStrippableLibraries(); i++ )
						{
							CheckDlgButton(hWnd, ColladaOptions::GetStrippableLibraryIDC( i ), true );
						}
						break;
					}

				case ID_DESELECT_ALL_LIBRARIES:
					{
						for ( int i = 0; i < FArchiveXMLOptions::GetNumStrippableLibraries(); i++ )
						{
							CheckDlgButton(hWnd, ColladaOptions::GetStrippableLibraryIDC( i ), false );
						}
						break;
					}

				case IDC_REMOVE_PATHS:
					{
						removePaths = IsDlgButtonChecked(hWnd, IDC_REMOVE_PATHS) == BST_CHECKED;
						replacePaths = removePaths ? false : replacePaths ;
						CheckDlgButton(hWnd, IDC_REPLACE_PATHS, replacePaths );

						HWND hwndEdit = GetDlgItem( hWnd, IDC_REPLACEMENT_PATH );
						Edit_Enable( hwndEdit, replacePaths );
						break;
					}
				case IDC_REPLACE_PATHS:
					{
						replacePaths = IsDlgButtonChecked(hWnd, IDC_REPLACE_PATHS) == BST_CHECKED;
						removePaths = replacePaths ? false : removePaths ;
						CheckDlgButton(hWnd, IDC_REMOVE_PATHS, removePaths);

						HWND hwndEdit = GetDlgItem( hWnd, IDC_REPLACEMENT_PATH );
						Edit_Enable( hwndEdit, replacePaths );
						break;
					}

				case IDC_CONVERTER_COMPLIANCE:
					{
						colladaConverterCompliance = IsDlgButtonChecked(hWnd, IDC_CONVERTER_COMPLIANCE) == BST_CHECKED;
						ApplyColladaConverterCompliance( hWnd );
						RefreshAnimationOptions( hWnd );
						break;
					}

				case ID_BATCH_STRING:
					{
						DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CMD_DIALOG), hWnd, BatchOptionStringDlgProcS, (LPARAM)this);
						break;
					}

				case ID_OPEN_CLIP_DIALOG:
					{
						DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ANIM_CLIP_DIALOG), hWnd, AnimClipDlgProcS, (LPARAM)this);
						break;
					}

				case ID_IMPORT_MAIN_EXPORT_SETTINGS:
					{
						size_t returnCode = MessageBox( hWnd, "Are you sure you want to overwrite your preview export settings with the main export settings?", "Confirmation", MB_YESNO );
						if ( returnCode == IDYES )
						{
							LoadOptions( OPTION_FILENAME_DEFAULT );
							RefreshExportOptions( hWnd );
						}
						break;
					}
				
				case IDOK:
					{
						GetOptionsFromExportDialog( hWnd );
						EndDialog(hWnd, 1);
						break;
					}

				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
			switch (hiWord)
			{
				case CBN_SELCHANGE:
				{
					switch ( loWord )
					{
						case IDC_GEOMETRY_FORMAT_POSITION_COMBOBOX :
						case IDC_GEOMETRY_FORMAT_NORMAL_COMBOBOX :
						case IDC_GEOMETRY_FORMAT_TEXTURE_COMBOBOX :
						case IDC_GEOMETRY_FORMAT_COLOR_COMBOBOX :
						{
							RefreshGeometryFormatGroupBox( ( HWND )lParam );
							break;
						}
					}
					break;
				}
			}
		}
		default:
			return FALSE;
	}
	return TRUE;
}

DLG_PROC_IMPL( ColladaOptions, ImportOptions )
{
	switch (message)
	{
		case WM_INITDIALOG: {
			CenterWindow(hWnd, GetParent(hWnd));  

			// setup checkboxes
			CheckDlgButton(hWnd, IDC_IMPORT_UNITS, importUnits);
			CheckDlgButton(hWnd, IDC_IMPORT_UPAXIS, importUpAxis);
			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					// hit OK, pick up control values & end dialog
					importUnits = IsDlgButtonChecked(hWnd, IDC_IMPORT_UNITS) == BST_CHECKED;
					importUpAxis = IsDlgButtonChecked(hWnd, IDC_IMPORT_UPAXIS) == BST_CHECKED;
					EndDialog(hWnd, 1);
					break;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
		default:
			return FALSE;
	}
	return TRUE;
}

DLG_PROC_IMPL( ColladaOptions, BatchOptionString )
{
	switch (message)
	{
		case WM_INITDIALOG: 
		{
			CStr str;
			BatchExporter::GetInst()->m_pDefaultColladaOptions->GetOptionsFromExportDialog( GetParent(hWnd) );
			BatchExporter::GetInst()->WriteOptions( str, BatchExporter::GetInst()->m_pDefaultColladaOptions );

			HWND hwndEdit = GetDlgItem( hWnd, IDC_CMD_EDIT );
			Edit_SetText( hwndEdit, str.data() );
			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					EndDialog(hWnd, 1);
					break;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
		default:
			return FALSE;
	}
	return TRUE;
}

DLG_PROC_IMPL( ColladaOptions, AnimClip )
{
	switch (message)
	{
		case WM_INITDIALOG: 
		{
			// Populate anim clip lists
			HWND userAnimClipList = GetDlgItem( hWnd, IDC_ANIMCLIP_USERLIST );
			HWND stdAnimClipList = GetDlgItem( hWnd, IDC_ANIMCLIP_STDLIST );  

			ListBox_ResetContent( userAnimClipList );
			ListBox_ResetContent( stdAnimClipList );

			int numClips = AnimationClipManager::GetInst()->GetNumAnimClips();
			int firstAutoClipIndex = -1;

			for (int i = 0; i < numClips; i++) 
			{ 
				const char *name = AnimationClipManager::GetInst()->GetAnimClipListBoxName( i );
				HWND currentAnimClipList = userAnimClipList;
				if ( TEST_FLAG( AnimationClipManager::GetInst()->GetAnimClipFlags( i ), AnimClip_Ticks::STANDARD_ANIM_CLIP ) )
				{
					currentAnimClipList = stdAnimClipList;
				}	

				int pos = (int)SendMessage(currentAnimClipList, LB_ADDSTRING, 0, (LPARAM) name ); 
				SendMessage(currentAnimClipList, LB_SETITEMDATA, pos, (LPARAM) i ); 
			}

			// Set initial item selection
			int userAnimClipListItemCount = ListBox_GetCount( userAnimClipList );
			bool bAtLeastOneUserClipSelected = false;
			for ( int i = 0; i < userAnimClipListItemCount; ++i )
			{
				int clipId = ListBox_GetItemData( userAnimClipList, i );
				int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( clipId );
				ListBox_SelItemRange( userAnimClipList, TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ), i, i );

				if ( !bAtLeastOneUserClipSelected )
				{
					bAtLeastOneUserClipSelected = TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED );
				}
			}

			// Set initial item selection
			if ( !bAtLeastOneUserClipSelected )
			{
				int stdAnimClipListItemCount = ListBox_GetCount( stdAnimClipList );
				for ( int i = 0; i < stdAnimClipListItemCount; ++i )
				{
					int clipId = ListBox_GetItemData( stdAnimClipList, i );
					int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( clipId );
					if ( TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ) )
					{
						ListBox_SetCurSel( stdAnimClipList, i );
						break;
					}
				}
			}

			// Check radio button
			if ( bAtLeastOneUserClipSelected )
			{
				CheckRadioButton( hWnd, IDC_ANIMCLIP_USER_RADIOBUTTON, IDC_ANIMCLIP_STD_RADIOBUTTON, IDC_ANIMCLIP_USER_RADIOBUTTON );
			}
			else
			{
				CheckRadioButton( hWnd, IDC_ANIMCLIP_USER_RADIOBUTTON, IDC_ANIMCLIP_STD_RADIOBUTTON, IDC_ANIMCLIP_STD_RADIOBUTTON );
			}

			RefreshAnimClipDialog( hWnd );

			// disabling user clip radio button if no user clips
			if ( userAnimClipListItemCount == 0 )
			{
				UIHelpers::SetControlEnabled( hWnd, IDC_ANIMCLIP_USER_RADIOBUTTON, false );
			}

			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_SELECT_ALL_USERCLIPS:
				{
					UIHelpers::ListBox_SelectAll( hWnd, IDC_ANIMCLIP_USERLIST, true );
					break;
				}

				//case ID_DESELECT_ALL_USERCLIPS:
				//{
				//	UIHelpers::ListBox_SelectAll( hWnd, IDC_ANIMCLIP_USERLIST, false );
				//	break;
				//}

				case IDC_ANIMCLIP_USER_RADIOBUTTON:
				case IDC_ANIMCLIP_STD_RADIOBUTTON:
				{
					RefreshAnimClipDialog( hWnd );

					// automatically selecting items
					if ( IsDlgButtonChecked( hWnd, IDC_ANIMCLIP_USER_RADIOBUTTON ) == BST_CHECKED )
					{
						UIHelpers::ListBox_SelectAll( hWnd, IDC_ANIMCLIP_USERLIST, true );
						HWND stdAnimClipList = GetDlgItem( hWnd, IDC_ANIMCLIP_STDLIST );
						ListBox_SetCurSel( stdAnimClipList, -1 );
					}
					else
					{
						UIHelpers::ListBox_SelectAll( hWnd, IDC_ANIMCLIP_USERLIST, false );
						HWND stdAnimClipList = GetDlgItem( hWnd, IDC_ANIMCLIP_STDLIST );
						ListBox_SetCurSel( stdAnimClipList, 0 );
					}
					break;
				}

				case IDOK:
				case IDCANCEL:
				{
					SaveAnimClipDialogData( hWnd );
					EndDialog(hWnd, 1);
					break;
				}
			}
		default:
			return FALSE;
	}
	return TRUE;
}

DLG_PROC_IMPL( ColladaOptions, Warning )
{
	switch (message)
	{
		case WM_INITDIALOG: 
		{
			fm::string warningLogStr = g_warningLog.ToString();

			// also printing to Maxscript listener
			mprintf( warningLogStr.c_str() );

			// replace all '\n' with '\r\n' so end lines are properly interpreted by edit control
			for( size_t i = 0; i < warningLogStr.size(); ++i )
			{
				if ( warningLogStr[i] == '\n' )
				{
					warningLogStr.insert( i, "\r", 1 );
					++i;
				}
			}

			HWND hwndEdit = GetDlgItem( hWnd, IDC_WARNING_EDIT );
			Edit_SetText( hwndEdit, warningLogStr.c_str() );
			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					EndDialog(hWnd, 1);
					break;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					break;
			}
		default:
			return FALSE;
	}
	return TRUE;
}

void ColladaOptions::SaveAnimClipListData( HWND hList )
{
	int itemCount = ListBox_GetCount( hList );
	for ( int i = 0; i < itemCount; i++ )
	{
		bool bSelected = ListBox_GetSel( hList, i ) > 0;
		int clipId = ( int )ListBox_GetItemData( hList, i );
		int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( clipId );

		if ( bSelected )
		{
			SET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED );
		}
		else
		{
			RESET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED );
		}
		
		AnimationClipManager::GetInst()->SetAnimClipFlags( clipId, currentFlags );
	}
}

void ColladaOptions::SaveAnimClipDialogData( HWND hWnd )
{
	HWND userAnimClipList = GetDlgItem( hWnd, IDC_ANIMCLIP_USERLIST );
	HWND stdAnimClipList = GetDlgItem( hWnd, IDC_ANIMCLIP_STDLIST );

	SaveAnimClipListData( userAnimClipList );
	SaveAnimClipListData( stdAnimClipList );
}

void ColladaOptions::RefreshAnimClipDialog( HWND hWnd )
{
	if ( IsDlgButtonChecked( hWnd, IDC_ANIMCLIP_USER_RADIOBUTTON ) == BST_CHECKED )
	{
		UIHelpers::SetControlEnabled( hWnd, IDC_ANIMCLIP_USERLIST, true );
		UIHelpers::SetControlEnabled( hWnd, ID_SELECT_ALL_USERCLIPS, true );
		//UIHelpers::SetControlEnabled( hWnd, ID_DESELECT_ALL_USERCLIPS, true );

		UIHelpers::SetControlEnabled( hWnd, IDC_ANIMCLIP_STDLIST, false );
	}
	else
	{
		UIHelpers::SetControlEnabled( hWnd, IDC_ANIMCLIP_USERLIST, false );
		UIHelpers::SetControlEnabled( hWnd, ID_SELECT_ALL_USERCLIPS, false );
		//UIHelpers::SetControlEnabled( hWnd, ID_DESELECT_ALL_USERCLIPS, false );

		UIHelpers::SetControlEnabled( hWnd, IDC_ANIMCLIP_STDLIST, true );
	}
}

// Save the export options to a XML file
void ColladaOptions::SaveOptions( const fm::string &fileName )
{
	TSTR configurationPath = maxInterface->GetDir(APP_PLUGCFG_DIR);
	TSTR filename = configurationPath + "\\" + fileName.c_str();
	FILE* file = fopen(filename.data(), _T("wb"));
	if (file == NULL) return;

	// Write down a standard INI header to allow MaxScript edition.
	CStr headerString("[ColladaMax]\r\n");
	fwrite(headerString.data(), headerString.length(), 1, file);

	// Write down the options, one by one, in the form: 'option=X\n'
	CStr optionString;
#define WRITE_OPTION_TOFILE fwrite(optionString.data(), optionString.length(), 1, file)
#define WRITE_OPTION(optionName,optionValue,formatStr) \
	optionString.printf( "%s=" #formatStr "\r\n", optionName, optionValue); \
	WRITE_OPTION_TOFILE

	WRITE_OPTION("normals",normals,%d);
	WRITE_OPTION("triangulate",triangulate,%d);
	WRITE_OPTION("triangleStrips",triangleStrips,%d);
	WRITE_OPTION("xrefs",xrefs,%d);
	WRITE_OPTION("tangents",tangents,%d);
	WRITE_OPTION("convertLinearSplinesToLineStrips",convertLinearSplinesToLineStrips,%d);
	WRITE_OPTION("animations",animations,%d);
	WRITE_OPTION("sampleAnim",sampleAnim,%d);
	WRITE_OPTION("sampleRate",sampleRate,%f);
	WRITE_OPTION("secondarySampleRate",secondarySampleRate,%f);
	WRITE_OPTION("forceAnimationKeySampling",forceAnimationKeySampling,%d);
	WRITE_OPTION("trimBipedAnims",trimBipedAnims,%d);
	WRITE_OPTION("exportDegenerateAnimationCurves",exportDegenerateAnimationCurves,%d);
	WRITE_OPTION("exportInvalidTransformAnimations",exportInvalidTransformAnimations,%d);
	WRITE_OPTION("exportInvalidMorphTargetWeightAnimations",exportInvalidMorphTargetWeightAnimations,%d);
	WRITE_OPTION("cubicInterpolation",cubicInterpolation,%d);
	WRITE_OPTION("colladaConverterCompliance",colladaConverterCompliance,%d);
	WRITE_OPTION("cleanAnimKeys",cleanAnimKeys,%d);
	WRITE_OPTION("compressAnimKeys",compressAnimKeys,%d);
	WRITE_OPTION("keyCompressionTolerance",keyCompressionTolerance,%f);
	WRITE_OPTION("oneChannelPerCurve",oneChannelPerCurve,%d);
	WRITE_OPTION("bakeMatrices",bakeMatrices,%d);
	WRITE_OPTION("decomposeMatrices",decomposeMatrices,%d);
	//WRITE_OPTION("removeUnusedSkinJoints",removeUnusedSkinJoints,%d);
	WRITE_OPTION("relativePaths",relativePaths,%d);
	WRITE_OPTION("removePaths",removePaths,%d);
	WRITE_OPTION("replacePaths",replacePaths,%d);
	WRITE_OPTION("replacementPath",replacementPath.c_str(),%s);
	WRITE_OPTION("stripAssociatedNodes",stripAssociatedNodes,%d);
	WRITE_OPTION("exportUpAxis",exportUpAxis,%d);
	WRITE_OPTION("animStart",animStart,%d);
	WRITE_OPTION("animEnd",animEnd,%d);
	WRITE_OPTION("importUnits",importUnits,%d);
	WRITE_OPTION("importUpAxis",importUpAxis,%d);
	WRITE_OPTION("librariesToStrip",librariesToStrip,%d);
	WRITE_OPTION("geometryPositionFormat",geometryPositionFormat.c_str(),%s);
	WRITE_OPTION("geometryNormalFormat",geometryNormalFormat.c_str(),%s);
	WRITE_OPTION("geometryTextureFormat",geometryTextureFormat.c_str(),%s);
	WRITE_OPTION("geometryColorFormat",geometryColorFormat.c_str(),%s);
	WRITE_OPTION("removeZeroWeights",removeZeroWeights,%d);
	WRITE_OPTION("removeDuplicateWeights",removeDuplicateWeights,%d);
	WRITE_OPTION("deindexVertexData",deindexVertexData,%d);
	WRITE_OPTION("textureCoordinatesOrigin",textureCoordinatesOrigin.c_str(),%s);

	// warnings
	int numWarnings = m_warningManager.GetWarningCount();
	for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
	{
		ICWarning *pWarning = m_warningManager.GetWarning( warningIndex );
		WRITE_OPTION( pWarning->m_name.c_str(), TEST_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED ),%d);
	}

#undef WRITE_OPTION
#undef WRITE_OPTION_TOFILE

	fclose(file);
}

int ColladaOptions::LoadOptions( const fm::string &fileName )
{
	TSTR configurationPath = maxInterface->GetDir(APP_PLUGCFG_DIR);
	TSTR filename = configurationPath + "\\" + fileName.c_str();
	FILE* file = fopen(filename.data(), _T("rb"));
	if (file == NULL) return -1;

	// Read the whole configuration file
	fseek(file, 0, SEEK_END);
	int fileLength = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* wholeFile = new char[fileLength + 1];
	fread(wholeFile, 1, fileLength, file);
	fclose(file);
	wholeFile[fileLength] = 0;

	// Read in the options, one by one, in the form: 'option=X\n'
	char* token = strtok(wholeFile, "\n");
	while (token != NULL)
	{
		// Skip whitespaces
		while (*token != 0 && (*token == ' ' || *token == '\r' || *token == '\t')) ++token;
		if (*token != 0)
		{
			if (*token == '[') {} // Standard INI header. Just skip the line for now.

			// Read in the Property=Value lines.
			char* value = strchr(token, '=');
			if (value != NULL)
			{
				*(value++) = 0;
				while (*value != 0 && (*value == ' ' || *value == '\r' || *value == '\t')) ++value;
				if (*value != 0) // skip empty lines.
				{
					// Look for/read in the ColladaMax options.
#define READ_OPTION(optionCast, optionName, optionValue) \
					if (strcmp(token, optionName) == 0) { \
						optionValue = (optionCast) atoi(value); } else
#define READ_OPTION_BOOL(optionName,optionValue) \
					if (strcmp(token, optionName) == 0) { \
						optionValue = *value != 'f' && *value != 'F' && *value != '0'; } else
#define READ_OPTION_FLOAT(optionName,optionValue) \
					if (strcmp(token, optionName) == 0) { \
						optionValue = (float) strtod( value, NULL ); } else
#define READ_OPTION_FSTRING(optionName,optionValue) \
					if (strcmp(token, optionName) == 0) { \
						optionValue.clear(); \
						while ( *value != '\r' ) \
						{ \
							optionValue.append( *value ); \
							value++; \
						} } else

					READ_OPTION_BOOL("normals",normals);
					READ_OPTION_BOOL("triangulate",triangulate);
					READ_OPTION_BOOL("triangleStrips",triangleStrips);
					READ_OPTION_BOOL("xrefs",xrefs);
					READ_OPTION_BOOL("tangents",tangents);
					READ_OPTION_BOOL("convertLinearSplinesToLineStrips",convertLinearSplinesToLineStrips);
					READ_OPTION_BOOL("sampleAnim",sampleAnim);
					READ_OPTION_FLOAT("sampleRate",sampleRate);
					READ_OPTION_FLOAT("secondarySampleRate",secondarySampleRate);
					READ_OPTION_BOOL("forceAnimationKeySampling",forceAnimationKeySampling);
					READ_OPTION_BOOL("trimBipedAnims",trimBipedAnims);
					READ_OPTION_BOOL("exportDegenerateAnimationCurves",exportDegenerateAnimationCurves);
					READ_OPTION_BOOL("exportInvalidTransformAnimations",exportInvalidTransformAnimations);
					READ_OPTION_BOOL("exportInvalidMorphTargetWeightAnimations",exportInvalidMorphTargetWeightAnimations);
					READ_OPTION(FUDaeInterpolation::Interpolation, "cubicInterpolation", cubicInterpolation);
					READ_OPTION_BOOL("colladaConverterCompliance",colladaConverterCompliance);
					READ_OPTION_BOOL("animations",animations);
					READ_OPTION_BOOL("cleanAnimKeys",cleanAnimKeys);
					READ_OPTION_BOOL("compressAnimKeys",compressAnimKeys);
					READ_OPTION_FLOAT("keyCompressionTolerance",keyCompressionTolerance);
					READ_OPTION_BOOL("oneChannelPerCurve",oneChannelPerCurve);
					READ_OPTION_BOOL("bakeMatrices",bakeMatrices);
					READ_OPTION_BOOL("decomposeMatrices",decomposeMatrices);
					//READ_OPTION_BOOL("removeUnusedSkinJoints",removeUnusedSkinJoints);
					READ_OPTION_BOOL("relativePaths",relativePaths);
					READ_OPTION_BOOL("removePaths",removePaths);
					READ_OPTION_BOOL("replacePaths",replacePaths);
					READ_OPTION_FSTRING("replacementPath",replacementPath);
					READ_OPTION_BOOL("stripAssociatedNodes",stripAssociatedNodes);
					READ_OPTION_BOOL("importUnits",importUnits);
					READ_OPTION_BOOL("importUpAxis",importUpAxis);
					READ_OPTION_BOOL("removeZeroWeights",removeZeroWeights);
					READ_OPTION_BOOL("removeDuplicateWeights",removeDuplicateWeights);
					READ_OPTION_BOOL("deindexVertexData",deindexVertexData);
					READ_OPTION_FSTRING("textureCoordinatesOrigin",textureCoordinatesOrigin);
					READ_OPTION(int, "exportUpAxis", exportUpAxis);
					READ_OPTION(int, "librariesToStrip", librariesToStrip);
					READ_OPTION_FSTRING( "geometryPositionFormat", geometryPositionFormat );
					READ_OPTION_FSTRING( "geometryNormalFormat", geometryNormalFormat );
					READ_OPTION_FSTRING( "geometryTextureFormat", geometryTextureFormat );
					READ_OPTION_FSTRING( "geometryColorFormat", geometryColorFormat );
					READ_OPTION(int, "animStart", animStart);
					READ_OPTION(int, "animEnd", animEnd);

					// warnings
					int numWarnings = m_warningManager.GetWarningCount();
					for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
					{
						ICWarning *pWarning = m_warningManager.GetWarning( warningIndex );
						
						if (strcmp(token, pWarning->m_name.c_str()) == 0)
						{
							bool bWarningEnabled = false;
							bWarningEnabled = *value != 'f' && *value != 'F' && *value != '0';
							
							RESET_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED );
							SET_FLAG( pWarning->m_flags, bWarningEnabled ? ICWarningFlags::ENABLED : 0);
						}
					}
#undef READ_OPTION
#undef READ_OPTION_BOOL
#undef READ_OPTION_FLOAT
#undef READ_OPTION_FSTRING
					{
						// Handle unknown option here.
					}
				}
			}
		}

		token = strtok(NULL, "\n");
	}

	SAFE_DELETE_ARRAY(wholeFile);

	return 0;
}

// Bug 233. Sampling fails if IK is involved.
bool ColladaOptions::DoSampleMatrices(INode* maxNode)
{
	bool sampleMatrices = SampleAnim();

	// even of option is not checked, non-standard transforms need to be sampled too
	if (!sampleMatrices && maxNode != NULL)
	{
		Control* maxCtrl = maxNode->GetTMController();

		// The -only- class we can hope to export without sampling is the PRS controller
		// Using curve fitting for biped controllers
		sampleMatrices = ( maxCtrl->ClassID() != Class_ID(PRS_CONTROL_CLASS_ID, 0) && !ControllerExporter::IsBipedController( maxCtrl ) )
			|| ControllerExporter::FindConstraint(maxCtrl) != 0;
	}
	return sampleMatrices;
}

// This function is used to determine if the node should be sampled with GetObjTMAfterWSM or GetNodeTM
// The difference is that GetNodeTM does NOT include the Obj Offset matrix.  When using GetObjTMAfterWSM
// then objTM is included, however this can cause problems with skinning etc, where the referenced matrix
// is actually the GetNodeTM matrix.
// If this function returns TRUE: 
//		Export Node with GetNodeTM
//		Export Pivot using GetObjTMAfterWSM * Inv(GetNodeTM)
// Else
//		Export Node with GetNodeTM
//		Export Pivot using GetPivotTM
// This should happen if the node in question is:
// being sampled AND
// NOT a BONE
// NOT a Biped object.
bool ColladaOptions::IncludeObjOffset(INode* node)
{
	return DoSampleMatrices(node) && !node->GetBoneNodeOnOff() && (GetBipMasterInterface(node->GetTMController()) == NULL);
}

FMVector3 ColladaOptions::ExportUpAxisAsVector() const
{
	FMVector3 v;

	if ( exportUpAxis == X )
	{
		v = FMVector3::XAxis;
	}
	else if ( exportUpAxis == Y )
	{
		v = FMVector3::YAxis;
	}
	else if ( exportUpAxis == Z )
	{
		v = FMVector3::ZAxis;
	}

	return v;
}

void ColladaOptions::ResolveExportedAnimsRange()
{
	Interval userDefinedAnimClipsRange = AnimationClipManager::GetInst()->GetUserDefinedAnimClipsRange();
	Interval exportedAnimClipRange = AnimationClipManager::GetInst()->GetExportedAnimClipsRange();
	Interval sceneAnimRange = AnimationClipManager::GetInst()->GetSceneAnimRange();
	Interval animRange;

	// If not exporting animations, the animation range is irrelevant.
	// The animation range will however be present in the exported file anyway
	// So we still provide a meaningfull value for it
	if ( animations && AnimationClipManager::GetInst()->ValidateAnimRange( exportedAnimClipRange ) )
	{
		animRange = exportedAnimClipRange;
	}
	else if ( AnimationClipManager::GetInst()->ValidateAnimRange( userDefinedAnimClipsRange ) )
	{
		animRange = userDefinedAnimClipsRange;
	}
	else 
	if ( AnimationClipManager::GetInst()->ValidateAnimRange( sceneAnimRange ) )
	{
		animRange = sceneAnimRange;
	}
	else
	{
		animRange = GetCOREInterface()->GetAnimRange();
	}

	animStart = animRange.Start();
	animEnd = animRange.End();
}

void ColladaOptions::CleanReplacementPath()
{
	if ( replacementPath.size() < 1 )
	{
		return;
	}

	// removing leading white spaces
	replacementPath.trim();
	replacementPath.replace( FC('\\'), FC('/') );
	
	if ( replacementPath.at( replacementPath.length() - 1 ) != FC('/'))
	{
		replacementPath.append( FC('/'));
	}
}

ColladaOptions *ColladaOptions::Clone()
{
	ColladaOptions *pClone = new ColladaOptions( maxInterface, m_pDocumentExporter );
	*pClone = *this;
	return pClone;
}

void ColladaOptions::ResetGeometryChannelFormatOptions()
{
	geometryPositionFormat = GetString( s_kGeometryFormatOptionsStrings[ColladaOptions::GEOM_FORMAT_FLOAT] );
	geometryNormalFormat = GetString( s_kGeometryFormatOptionsStrings[ColladaOptions::GEOM_FORMAT_FLOAT] );
	geometryTextureFormat = GetString( s_kGeometryFormatOptionsStrings[ColladaOptions::GEOM_FORMAT_FLOAT] );
	geometryColorFormat = GetString( s_kGeometryFormatOptionsStrings[ColladaOptions::GEOM_FORMAT_UNSIGNED_BYTE] );
}

void ColladaOptions::Reset()
{
	// Set the option values to their default
	normals = true;
	triangulate = true;
	triangleStrips = false;
	xrefs = true;
	tangents = false;
	convertLinearSplinesToLineStrips = false;
	animations = true;
	sampleAnim = false;
	sampleRate = SAMPLE_RATE_DEFAULT_VALUE;
	secondarySampleRate = SAMPLE_RATE_DEFAULT_VALUE;
	forceAnimationKeySampling = false;
	trimBipedAnims = false;
	exportDegenerateAnimationCurves = false;
	exportInvalidTransformAnimations = false;
	exportInvalidMorphTargetWeightAnimations = false;
	cubicInterpolation = FUDaeInterpolation::DEFAULT_CUBIC;
	cleanAnimKeys = false;
	compressAnimKeys = false;
	keyCompressionTolerance = ANIM_KEY_COMPRESSION_DEFAULT_TOLERANCE;
	oneChannelPerCurve = false;
	bakeMatrices = false;
	decomposeMatrices = false;
	removeUnusedSkinJoints = true;
	rotationFormat = ANGLE_AXIS;
	relativePaths = true;
	removePaths = false;
	replacePaths = false;
	replacementPath.append("");
	stripAssociatedNodes = false;
	exportUpAxis = Z;
	librariesToStrip = 0;
	removeZeroWeights = true;
	removeDuplicateWeights = true;
	deindexVertexData = false;
	textureCoordinatesOrigin = GetString( s_kTextureCoordinatesOriginStrings[ColladaOptions::TCO_UPPER_LEFT] );
	importUnits = true;
	importUpAxis = true;
	selectionOnly = false;

	stripThirdTextureCoord = true;

	ResetGeometryChannelFormatOptions();
	
	// debug switches
	dbgSwitch = false;

	animStart = TIME_INITIAL_POSE;
	animEnd = TIME_PosInfinity;	

	animClipOptions = "UserClips";

	colladaConverterCompliance = true;

	m_cubicInterpolationRadioButtonGroup.Clear();
	m_cubicInterpolationRadioButtonGroup.AddRadioButton( IDC_CUBIC_INTERPOLATION_HERMITE_RADIOBUTTON, FUDaeInterpolation::HERMITE );
	m_cubicInterpolationRadioButtonGroup.AddRadioButton( IDC_CUBIC_INTERPOLATION_BEZIER_RADIOBUTTON, FUDaeInterpolation::BEZIER );

	// reset warnings ( see Reset() for default warning state ( enabled/disabled ))
	m_warningManager.Reset();
}

void ColladaOptions::ProcessBatchExporterAnimClipOptions()
{
	bool bFallbackRequired = false;
	if ( !_stricmp( "UserClips", animClipOptions.c_str() ) )
	{
		// Exporting all user defined animation clips
		bFallbackRequired = true;
		int numAnimClips = AnimationClipManager::GetInst()->GetNumAnimClips();
		for ( int i = 0; i < numAnimClips; ++i )
		{
			int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
			if ( !TEST_FLAG( currentFlags, AnimClip_Ticks::STANDARD_ANIM_CLIP ))
			{
				AnimationClipManager::GetInst()->SetAnimClipFlags( i, SET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ) );
				bFallbackRequired = false;
			}
		}
	}

#define PROCESS_STD_ANIM_CLIP_OPTION( name ) \
if ( !_stricmp( AnimationClipManager::GenerateStandardAnimClipName( name ).c_str(), animClipOptions.c_str() ) || bFallbackRequired ) \
{ \
	bFallbackRequired = true; \
	int stdAnimClipsClipId = AnimationClipManager::GetInst()->GetAnimClipId( AnimationClipManager::GenerateStandardAnimClipName( name ).c_str() ); \
	if ( stdAnimClipsClipId >= 0 ) \
	{ \
		int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( stdAnimClipsClipId ); \
		AnimationClipManager::GetInst()->SetAnimClipFlags( stdAnimClipsClipId, SET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ) ); \
		bFallbackRequired = false; \
	} \
}

	PROCESS_STD_ANIM_CLIP_OPTION( "AnimClips" );
	PROCESS_STD_ANIM_CLIP_OPTION( "SceneAnims" );
	PROCESS_STD_ANIM_CLIP_OPTION( "Current" );
	PROCESS_STD_ANIM_CLIP_OPTION( "Total" );

#undef PROCESS_STD_ANIM_CLIP_OPTION

}

void ColladaOptions::PreExportAnimClips()
{
	// Hack : if no clips selected for export at this point, set Current animation range as clip to export
	// .dae Quick preview mode benefits from this
	{
		bool bAtLeastOneExportedClip = false;
		{
			int numAnimClips = AnimationClipManager::GetInst()->GetNumAnimClips();
			for ( int i = 0; i < numAnimClips; ++i )
			{
				int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
				if ( TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ))
				{
					bAtLeastOneExportedClip = true;
					break;
				}
			}
		}

		if ( !bAtLeastOneExportedClip )
		{
			int clipId = AnimationClipManager::GetInst()->GetAnimClipId( AnimationClipManager::GenerateStandardAnimClipName( "Current" ).c_str() );
			FUDbgAssert( clipId >= 0, );
			if ( clipId >= 0 )
			{
				int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( clipId );
				AnimationClipManager::GetInst()->SetAnimClipFlags( clipId, SET_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ) );
			}
		}
	}

	{
		AnimationClipManager::GetInst()->InitAnimClipsToExport();
		std::vector<int> &animClipsToExport = AnimationClipManager::GetInst()->GetAnimClipsToExport();

		int numAnimClips = AnimationClipManager::GetInst()->GetNumAnimClips();
		for ( int i = 0; i < numAnimClips; ++i )
		{
			int currentFlags = AnimationClipManager::GetInst()->GetAnimClipFlags( i );
			if ( TEST_FLAG( currentFlags, AnimClip_Ticks::IS_SELECTED ))
			{
				animClipsToExport.push_back( i );
			}
		}

		int numAnimClipsToExport = animClipsToExport.size();
		AnimationClipManager::GetInst()->CalculateExportedAnimClipsRange();
	}
}

template <class T>
void RadioButtonGroup<T>::AddRadioButton( int idc, T value )
{
	m_buttons.push_back( RadioButton<T>( idc, value ) );
}

template <class T>
void RadioButtonGroup<T>::SetSelectedValue( HWND hWnd, T value )
{
	for ( size_t i = 0; i < m_buttons.size(); ++i )
	{
		if ( m_buttons[i].m_value == value )
		{
			CheckRadioButton( hWnd, m_buttons.front().m_idc, m_buttons.back().m_idc, m_buttons[i].m_idc );
			break;
		}
	}
}

template <class T>
T RadioButtonGroup<T>::GetSelectedValue( HWND hWnd )
{
	T retVal;
	for ( size_t i = 0; i < m_buttons.size(); ++i )
	{
		if ( IsDlgButtonChecked( hWnd, m_buttons[i].m_idc ) == BST_CHECKED )
		{
			retVal = m_buttons[i].m_value;
			break;
		}
	}
	return retVal;
}

FArchiveXMLOptions ColladaOptions::ConstructArchiveXMLOptions()
{
	FArchiveXMLOptions options;

	options.m_librariesToStrip = librariesToStrip;
	options.m_bRemoveDuplicateWeights = removeDuplicateWeights;
	options.m_bExportModularSkinCategoryAsNode = EXPORT_MODULAR_SKIN_CATEGORY_AS_NODE;

	return options;
}

#undef MIN_KEY_COMPRESSION_EXPONENT
#undef MAX_KEY_COMPRESSION_EXPONENT
#undef MIN_KEY_COMPRESSION_COEFFICIENT
#undef MAX_KEY_COMPRESSION_COEFFICIENT_POWER
#undef MAX_KEY_COMPRESSION_COEFFICIENT
