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
#include "DocumentExporter.h"
#include "AnimationExporter.h"
#include "CameraExporter.h"
#include "ControllerExporter.h"
#include "EmitterExporter.h"
#include "EntityExporter.h"
#include "ColladaOptions.h"
#include "ForceExporter.h"
#include "GeometryExporter.h"
#include "LightExporter.h"
#include "MaterialExporter.h"
#include "NodeExporter.h"
#include "ModularSkinExporter.h"
#include "BillboardExporter.h"
#include "XRefManager.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDAsset.h"
#include "FUtils/FUFileManager.h"
#include "FCDocument/FCDocumentTools.h"
#include "FCDocument/FCDSceneNode.h"
#include "AnimationClipManager.h"

#include "common/ConversionFunctions.h"
#include "ICBatchExporter.h"

DocumentExporter::DocumentExporter( DWORD opts, COLLADAExporter *pCOLLADAExporter )
: m_pCOLLADAExporter( pCOLLADAExporter )
{
	// Set-up FCollada.
	FCollada::SetDereferenceFlag(false);
	colladaDocument = FCollada::NewTopDocument(); // must be first

	// Members Initialization (in order)
	pMaxInterface		= GetCOREInterface();
	animationExporter	= new AnimationExporter(this);
	cameraExporter		= new CameraExporter(this);
	controllerExporter	= new ControllerExporter(this);
	entityExporter		= new EntityExporter(this);
	geometryExporter	= new GeometryExporter(this);
	lightExporter		= new LightExporter(this);
	materialExporter	= new MaterialExporter(this);
	nodeExporter		= new NodeExporter(this);
	emitterExporter		= new EmitterExporter(this);
	forceExporter		= new ForceExporter(this);
	modularSkinExporter	= new ModularSkinExporter(this);
	billboardExporter	= new BillboardExporter(this);
	options				= new ColladaOptions(pMaxInterface,this);
}

DocumentExporter::~DocumentExporter() 
{
	SAFE_DELETE(animationExporter);
	SAFE_DELETE(cameraExporter);
	SAFE_DELETE(controllerExporter);
	SAFE_DELETE(entityExporter);
	SAFE_DELETE(geometryExporter);
	SAFE_DELETE(lightExporter);
	SAFE_DELETE(materialExporter);
	SAFE_DELETE(nodeExporter);
	SAFE_DELETE(emitterExporter);
	SAFE_DELETE(forceExporter);
	SAFE_DELETE(modularSkinExporter);
	SAFE_DELETE(billboardExporter);

	SAFE_DELETE(colladaDocument); // must be last
	SAFE_DELETE(options);

	pMaxInterface = NULL;
}

void DocumentExporter::PreExport()
{
	options->CleanReplacementPath();

	// Set relative/absolute export
	colladaDocument->GetFileManager()->SetForceAbsoluteFlag(!options->ExportRelativePaths());
	colladaDocument->GetFileManager()->SetRemovePathsFlag(options->RemovePaths());
	colladaDocument->GetFileManager()->SetReplacePathsFlag(options->ReplacePaths());
	colladaDocument->GetFileManager()->SetReplacementPath(options->ReplacementPath());

	// process batch exporter anim clip options
	bool bIsBatchExporting = BatchExporter::GetInst()->IsBatchExporting();
	if ( bIsBatchExporting )
	{
		options->ProcessBatchExporterAnimClipOptions();
	}

	if ( options->ExportAnimations() )
	{
		options->PreExportAnimClips();
	}

	options->ResolveExportedAnimsRange();
}

bool DocumentExporter::ShowExportOptions(BOOL suppressPrompts)
{
	if (!suppressPrompts) 
	{
		// Prompt the user with our dialogbox, and get all the options.
		// The user may cancel the export at this point.
		if (!options->ShowDialog(TRUE)) return false;
	}

	return true;
}

void DocumentExporter::SortVisualScene( FCDSceneNode *pNode )
{
	if ( !pNode )
	{
		return;
	}

	size_t childrenCount = pNode->GetChildrenCount();
	for ( size_t childId = 0; childId < childrenCount; ++childId )
	{
		SortVisualScene( pNode->GetChild( childId ) );
	}

	pNode->SortChildren( FCDSceneNode::k_daeIdComparator );
}

void DocumentExporter::Sort()
{
	// visual scene nodes
	FCDSceneNode *pNode = colladaDocument->GetVisualSceneInstance();
	SortVisualScene( pNode );

	// libraries
#define SORT_LIBRARY_ENTITIES( LibraryEntityName ) \
{ \
	FCD##LibraryEntityName##Library *pLib = colladaDocument->Get##LibraryEntityName##Library(); \
	FCD##LibraryEntityName##Library::DaeIdComparator<> comp; \
	pLib->SortEntities( comp ); \
}

	SORT_LIBRARY_ENTITIES( Animation )
	SORT_LIBRARY_ENTITIES( AnimationClip )
	SORT_LIBRARY_ENTITIES( Camera )
	SORT_LIBRARY_ENTITIES( Controller )
	SORT_LIBRARY_ENTITIES( Effect )
	SORT_LIBRARY_ENTITIES( ForceField )
	SORT_LIBRARY_ENTITIES( Emitter )
	SORT_LIBRARY_ENTITIES( Force )
	SORT_LIBRARY_ENTITIES( ModularSkinMaster )
	SORT_LIBRARY_ENTITIES( ModularSkinCategory )
	SORT_LIBRARY_ENTITIES( BillboardManager )
	SORT_LIBRARY_ENTITIES( Geometry )
	SORT_LIBRARY_ENTITIES( Image )
	SORT_LIBRARY_ENTITIES( Light )
	SORT_LIBRARY_ENTITIES( Material )
	SORT_LIBRARY_ENTITIES( PhysicsModel )
	SORT_LIBRARY_ENTITIES( PhysicsMaterial )
	SORT_LIBRARY_ENTITIES( PhysicsScene )

#undef SORT_LIBRARY_ENTITIES
}

void DocumentExporter::ExportCurrentMaxScene()
{
	// pre
	{
		// Set the document's start/end time as the animation timeline.
		colladaDocument->SetStartTime(FSConvert::MaxTickToSecs(options->AnimStart()));
		colladaDocument->SetEndTime(FSConvert::MaxTickToSecs(options->AnimEnd()));

		ExportAsset();
		nodeExporter->PreExportPreviewCamera();
	}
	
	nodeExporter->ExportScene();
	
	// post
	{
		//animationExporter->FinalizeExport();

		if( options->ExportUpAxis() == ColladaOptions::Y )
		{
			FCDocumentTools::StandardizeUpAxisAndLength( colladaDocument, FMVector3::Origin, 0.0f, true, false );
		}

		// needs to be called after StandardizeUpAxisAndLength to function as it should
		nodeExporter->PostExportPreviewCamera();

		Sort();
		PostExportAsset();
	}
}

void DocumentExporter::PostExportAsset()
{
	FCDAsset* asset = colladaDocument->GetAsset();
	FCDAssetContributor* contributor = asset->GetContributor( 0 ); // currently only one contributor

	const FCDAnimationClipLibrary *animationClipLibrary = colladaDocument->GetAnimationClipLibrary();
	contributor->SetExportedAnimationClips( animationClipLibrary );
}

void DocumentExporter::ExportAsset()
{
	FCDAsset* asset = colladaDocument->GetAsset();
	FCDAssetContributor* contributor = asset->AddContributor();

	// Set the author
	TCHAR* userName = getenv("USERNAME");
	if (userName == NULL || *userName == 0) userName = getenv("USER");
	if (userName != NULL && *userName != 0) contributor->SetAuthor(fstring(userName));

    // Set the authoring tool
	fchar authoringToolString[1024];
	fsnprintf(authoringToolString, 1024, FC("3dsMax %d - Feeling ColladaMax v%d.%02d%s."), MAX_VERSION_MAJOR, FCOLLADA_VERSION >> 16, FCOLLADA_VERSION & 0xFFFF, FCOLLADA_BUILDSTR);
	authoringToolString[1023] = 0;
	contributor->SetAuthoringTool(authoringToolString);

	// set the authoring tool version ( IrrColladaMax version )
	fchar authoringToolVersionString[1024];
	VersionNumber verNum = BatchExporter::GetInst()->GetIrrColladaVersionNumber();
	fsnprintf(authoringToolVersionString, 1024, FC("%d.%d.%d.%d"), verNum.m_major, verNum.m_minor, verNum.m_build, verNum.m_revision );
	authoringToolVersionString[1023] = 0;
	contributor->SetAuthoringToolVersion( authoringToolVersionString );

	const TSTR& filename = GetMaxInterface()->GetCurFilePath();
	contributor->SetSourceData(filename.data());

	// set *system* unit information
	int systemUnitType = UNITS_CENTIMETERS;
	float systemUnitScale = 1.0f;
	GetMasterUnitInfo(&systemUnitType, &systemUnitScale);

	switch (systemUnitType)
	{
	case UNITS_INCHES: asset->SetUnitName(FC("inch")); asset->SetUnitConversionFactor(systemUnitScale * 0.0254f); break;
	case UNITS_FEET: asset->SetUnitName(FC("feet")); asset->SetUnitConversionFactor(systemUnitScale * 0.3048f); break;
	case UNITS_MILES: asset->SetUnitName(FC("mile")); asset->SetUnitConversionFactor(systemUnitScale * 1609.344f); break;
	case UNITS_MILLIMETERS: asset->SetUnitName(FC("millimeter")); asset->SetUnitConversionFactor(systemUnitScale * 0.001f); break;
	case UNITS_CENTIMETERS: asset->SetUnitName(FC("centimeter")); asset->SetUnitConversionFactor(systemUnitScale * 0.01f); break;
	case UNITS_METERS: asset->SetUnitName(FC("meter")); asset->SetUnitConversionFactor(systemUnitScale); break;
	case UNITS_KILOMETERS: asset->SetUnitName(FC("kilometer")); asset->SetUnitConversionFactor(systemUnitScale * 1000.0f); break;
	default: break;
	}

	// Dump the export options into the comments section of the contributor
	FUStringBuilder optionsSz(FC(EXPORT_OPTIONS_PREFIX));

#define APPEND_OPTION(optionName,optionVar) \
	optionsSz.append(optionName); \
	optionsSz.append( "=" ); \
	optionsSz.append(optionVar); \
	optionsSz.append(';');

#define APPEND_OPTION_TIME(optionName,optionVar) \
	optionsSz.append(optionName); \
	optionsSz.append( "=" ); \
	optionsSz.append(FSConvert::MaxTickToSecs(optionVar)); \
	optionsSz.append(';');

	APPEND_OPTION("ExportNormals",options->ExportNormals());
	APPEND_OPTION("ExportEPolyAsTriangles",options->ExportEPolyAsTriangles());
	APPEND_OPTION("ExportEPolyAsTriangleStrips",options->ExportEPolyAsTriangleStrips());
	APPEND_OPTION("ExportXRefs",options->ExportXRefs());
	APPEND_OPTION("ExportSelected",options->ExportSelected());
	APPEND_OPTION("ExportTangents",options->ExportTangents());
	APPEND_OPTION("ConvertLinearSplinesToLineStrips",options->ConvertLinearSplinesToLineStrips());
	APPEND_OPTION("DeindexVertexData",options->DeindexVertexData());
	APPEND_OPTION("ExportAnimations",options->ExportAnimations());
	APPEND_OPTION("SampleAnim",options->SampleAnim());
	APPEND_OPTION("SampleRate",options->SampleRate());
	APPEND_OPTION("SecondarySampleRate",options->SecondarySampleRate());
	APPEND_OPTION("ForceAnimationKeySampling",options->ForceAnimationKeySampling());
	APPEND_OPTION("TrimBipedAnims",options->TrimBipedAnims());
	APPEND_OPTION("ExportDegenerateAnimationCurves",options->ExportDegenerateAnimationCurves());
	APPEND_OPTION("ExportInvalidTransformAnimations",options->ExportInvalidTransformAnimations());
	APPEND_OPTION("ExportInvalidMorphTargetWeightAnimations",options->ExportInvalidMorphTargetWeightAnimations());
	APPEND_OPTION("CubicInterpolation",options->CubicInterpolation());
	APPEND_OPTION("CleanAnimKeys",options->CleanAnimKeys());
	APPEND_OPTION("CompressAnimKeys",options->CompressAnimKeys());
	APPEND_OPTION("KeyCompressionTolerance",options->KeyCompressionTolerance());
	APPEND_OPTION("OneChannelPerCurve",options->OneChannelPerCurve());
	APPEND_OPTION("BakeMatrices",options->BakeMatrices());
	APPEND_OPTION("DecomposeMatrices",options->DecomposeMatrices());
	//APPEND_OPTION("RemoveUnusedSkinJoints",options->RemoveUnusedSkinJoints());
	APPEND_OPTION("ExportRelativePaths",options->ExportRelativePaths());
	APPEND_OPTION("RemovePaths",options->RemovePaths());
	APPEND_OPTION("ReplacePaths",options->ReplacePaths());
	APPEND_OPTION("ReplacementPath",options->ReplacementPath());
	APPEND_OPTION("StripAssociatedNodes",options->StripAssociatedNodes());
	APPEND_OPTION("ExportUpAxis",options->ExportUpAxis());
	APPEND_OPTION("TextureCoordinatesOrigin",options->TextureCoordinatesOrigin());
	APPEND_OPTION("LibrariesToStrip",options->LibrariesToStrip());
	APPEND_OPTION("GeometryPositionFormat",options->GeometryPositionFormat());
	APPEND_OPTION("GeometryNormalFormat",options->GeometryNormalFormat());
	APPEND_OPTION("GeometryTextureFormat",options->GeometryTextureFormat());
	APPEND_OPTION("GeometryColorFormat",options->GeometryColorFormat());
	APPEND_OPTION("RemoveZeroWeights",options->RemoveZeroWeights());
	APPEND_OPTION("RemoveDuplicateWeights",options->RemoveDuplicateWeights());

	// warnings
	int numWarnings = options->m_warningManager.GetWarningCount();
	for ( size_t warningIndex = 0; warningIndex < numWarnings; ++warningIndex )
	{
		ICWarning *pWarning = options->m_warningManager.GetWarning( warningIndex );
		APPEND_OPTION( pWarning->m_secondaryName.c_str(), TEST_FLAG( pWarning->m_flags, ICWarningFlags::ENABLED ));
	}

	contributor->SetComments(optionsSz.ToString());

#undef APPEND_OPTION
#undef APPEND_OPTION_TIME

	// Set the up-axis
	asset->SetUpAxis( options->ExportUpAxisAsVector() );
}
