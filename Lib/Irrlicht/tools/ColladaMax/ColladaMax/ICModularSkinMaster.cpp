/**********************************************************************
 *<
	FILE: ModularSkinMaster.cpp

	DESCRIPTION: A simple planar deflector

	CREATED BY: Rolf Berteig
			

	HISTORY: Peter Watje 3-15-00

 *>	Copyright (c) 200, All Rights Reserved.
 **********************************************************************/

#include "StdAfx.h"
#include "ICModularSkinMaster.h"
#include "ControllerExporter.h"
#include "ColladaModifiers.h"
#include "FUtils/FUParameter.h"
#include "MAXHelpers.h"

//--- ClassDescriptor and class vars ---------------------------------
HWND ICModularSkinMasterObject::s_selectedSkinnedMeshesRollupHWnd = NULL;
HWND ICModularSkinMasterObject::s_presetsRollupHWnd = NULL;

static ICModularSkinMasterClassDesc modularSkinMasterDesc;
ClassDesc* GetICModularSkinMasterObjDesc() {return &modularSkinMasterDesc;}

ICModularSkinMasterObject::HideUnselectedSkinPBAccessor ICModularSkinMasterObject::s_hideUnselectedSkinAccessor;

ICModularSkinMasterObject::SkinnedMeshListPBAccessor ICModularSkinMasterObject::s_skinnedMeshListAccessor;
ICModularSkinMasterObject::SkinnedMeshListPBValidator ICModularSkinMasterObject::s_skinnedMeshListValidator;

ICModularSkinMasterObject::SkinnedMeshCategoriesListPBValidator ICModularSkinMasterObject::s_skinnedMeshCategoriesListValidator;
ICModularSkinMasterObject::SkinnedMeshCategoriesListPBAccessor ICModularSkinMasterObject::s_skinnedMeshListCategoriesAccessor;

ICModularSkinMasterObject::SelectedSkinnedMeshListPBAccessor ICModularSkinMasterObject::s_selectedSkinnedMeshListAccessor;

ICModularSkinMasterObject::CategoriesDlgProc ICModularSkinMasterObject::s_categoriesDlgProc;

const Point3 ICModularSkinMasterObject::k_gizmoColor( 1.f, 0.f, 0.f );
const Point3 ICModularSkinMasterObject::k_categoryLinksColor( 0.8f, 0.f, 0.f );
const Point3 ICModularSkinMasterObject::k_skinnedMeshLinksColor( 0.9f, 0.3f, 0.f );

// per instance geosphere block
static ParamBlockDesc2 s_modularSkinMasterParamBlockDesc
( 
	ICModularSkinMasterObject::PB_MAIN_ID
	, _T("ModularSkinMasterParameters")
	, 0
	, &modularSkinMasterDesc
	, P_AUTO_CONSTRUCT + P_AUTO_UI + P_MULTIMAP + P_VERSION
	, 0
	, ICModularSkinMasterObject::PB_MAIN_REF_ID

	//rollouts
	, ICModularSkinMasterObject::R_COUNT
	, ICModularSkinMasterObject::R_DISPLAY_OPTIONS, IDD_MODULAR_SKIN_MASTER_DISPLAY_OPTIONS, IDS_MODULAR_SKIN_DISPLAY_OPTIONS_ROLLUP_LABEL, 0, 0, NULL
	, ICModularSkinMasterObject::R_UNCATEGORIZED_SKINNED_MESHES, IDD_MODULAR_SKIN_MASTER_OBJECT, IDS_MODULAR_SKIN_MASTER_OBJECT_UNCATEGORIZED_SKINNED_MESH_LIST, 0, APPENDROLL_CLOSED, NULL
	, ICModularSkinMasterObject::R_SKINNED_MESH_CATEGORIES, IDD_MODULAR_SKIN_MASTER_OBJECT_CATEGORIES_ROLLUP, IDS_MODULAR_SKIN_MASTER_OBJECT_CATEGORIES_ROLLUP_LABEL, 0, 0, &ICModularSkinMasterObject::s_categoriesDlgProc/*NULL*/
	//, ICModularSkinMasterObject::R_PRESETS, IDD_SUPRPRTS_SAVE, IDS_MODULAR_SKIN_MASTER_OBJECT_PRESETS_ROLLUP_LABEL, 0, 0, NULL
	//, ICModularSkinMasterObject::R_PRESETS, IDD_MODULAR_SKIN_MASTER_OBJECT_CATEGORIES_ROLLUP, IDS_MODULAR_SKIN_MASTER_OBJECT_PRESETS_ROLLUP_LABEL, 0, 0, NULL

	// display rollup
	, ICModularSkinMasterObject::PI_DISPLAY_LINKS, _T("display_links"), TYPE_INT, 0, IDS_MODULAR_SKIN_DISPLAY_LINKS
		, p_ui, ICModularSkinMasterObject::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_MODULAR_SKIN_DISPLAY_LINKS_CHECKBOX
		, p_default, 1
	, end

	, ICModularSkinMasterObject::PI_HIDE_UNSELECTED_SKINS, _T("hide_unselected_skins"), TYPE_INT, 0, IDS_MODULAR_SKIN_HIDE_UNSELECTED_SKINS
		, p_ui, ICModularSkinMasterObject::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_MODULAR_SKIN_HIDE_UNSELECTED_SKINS_CHECKBOX
		, p_accessor, &ICModularSkinMasterObject::s_hideUnselectedSkinAccessor
		, p_default, 1
	, end

	// uncategorized skinned mesh list
	, ICModularSkinMasterObject::PI_SKINNED_MESH_LIST, _T("SkinnedMeshList"), TYPE_INODE_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
		, p_ui, ICModularSkinMasterObject::R_UNCATEGORIZED_SKINNED_MESHES, TYPE_NODELISTBOX, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_PICK_BUTTON
			, NULL
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_REMOVE_BUTTON
		, p_validator, &ICModularSkinMasterObject::s_skinnedMeshListValidator
		, p_accessor, &ICModularSkinMasterObject::s_skinnedMeshListAccessor
		, end

	// category list
	, ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, _T("SkinnedMeshCategoriesList"), TYPE_INODE_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
		, p_ui, ICModularSkinMasterObject::R_SKINNED_MESH_CATEGORIES, TYPE_NODELISTBOX, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_PICK_BUTTON
			, NULL
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_REMOVE_BUTTON
		, p_validator, &ICModularSkinMasterObject::s_skinnedMeshCategoriesListValidator
		, p_accessor, &ICModularSkinMasterObject::s_skinnedMeshListCategoriesAccessor
		, end

	// selected skinned mesh list ( one per category )
	, ICModularSkinMasterObject::PI_SELECTED_SKINNED_MESH_LIST, _T("SelectedSkinnedMeshList"), TYPE_INODE_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
		, p_accessor, &ICModularSkinMasterObject::s_selectedSkinnedMeshListAccessor
	, end

	// presets list
	//, ICModularSkinMasterObject::PI_PRESET_LIST, _T("Presets"), TYPE_REFTARG_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_MASTER_OBJECT_PRESETS_ROLLUP_LABEL, end

	// presets list
	//, ICModularSkinMasterObject::PI_PRESET_NAME_LIST, _T("Presets"), TYPE_STRING_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_MASTER_OBJECT_PRESETS_ROLLUP_LABEL
	//	, p_ui, ICModularSkinMasterObject::R_PRESETS, TYPE_STRINGLISTBOX, IDC_SP_SETLIST //IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
	//		, IDC_SP_SAVE //IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_PICK_BUTTON
	//		, NULL
	//		, IDC_SP_DELETE //IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_REMOVE_BUTTON
	//		, IDC_SP_SETEDIT
	//	, end

	, ICModularSkinMasterObject::PI_MIN_BOX, _T("min_box"), TYPE_POINT4, 0, IDS_MODULAR_SKIN_MIN_BOX, end
	, ICModularSkinMasterObject::PI_MAX_BOX, _T("max_box"), TYPE_POINT4, 0, IDS_MODULAR_SKIN_MAX_BOX, end

	, end
);


//--- ICModularSkinMasterObject Parameter map/block descriptors ------------------
//#define PARAMDESC_LENGTH	3
//
//ParamBlockDescID modularSkinDescVer0[] = {
//	{ TYPE_FLOAT, NULL, TRUE, FSDeflector::deflect_bounce },
//	{ TYPE_FLOAT, NULL, TRUE, FSDeflector::deflect_width },
//	{ TYPE_FLOAT, NULL, TRUE, FSDeflector::deflect_height }};
//
//#define PBLOCK_LENGTH	3
//
////#define CURRENT_VERSION	0
//#define NUM_OLDVERSIONS	1
//
//	// Array of old ParamBlock Ed. 1 versions
//static ParamVersionDesc versions[] = {
//	ParamVersionDesc (modularSkinDescVer0, 3, 0),
//};

//--- ModularSkinMaster object methods -----------------------------------------
ICModularSkinMasterObject::ICModularSkinMasterObject()
: m_bIsClosingFile( false )
{
	pblock2 = NULL;
	modularSkinMasterDesc.MakeAutoParamBlocks(this);
	assert(pblock2);

	// init selected skinned meshes combo box idc array
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_01_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_02_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_03_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_04_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_05_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_06_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_07_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_08_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_09_COMBOBOX );
	m_selectedSkinnedMeshesComboBoxIdcList.push_back( IDC_MODULAR_SKINNING_CATEGORY_10_COMBOBOX );

	// global notifications
	RegisterNotification( &ICModularSkinMasterObject::NotificationCallback, this, NOTIFY_SYSTEM_PRE_RESET );
	RegisterNotification( &ICModularSkinMasterObject::NotificationCallback, this, NOTIFY_SYSTEM_PRE_NEW );
	RegisterNotification( &ICModularSkinMasterObject::NotificationCallback, this, NOTIFY_FILE_PRE_OPEN );
	RegisterNotification( &ICModularSkinMasterObject::NotificationCallback, this, NOTIFY_SYSTEM_SHUTDOWN );
}

ICModularSkinMasterObject::~ICModularSkinMasterObject()
{	
	UnRegisterNotification( &ICModularSkinMasterObject::NotificationCallback, this );
}

RefTargetHandle ICModularSkinMasterObject::Clone(RemapDir& remap) 
{
	ICModularSkinMasterObject* newob = new ICModularSkinMasterObject();	
	
	CLONE_PARAMETER( newob, m_gizmoMesh );
	CLONE_PARAMETER( newob, m_categoryLinksMesh );
	CLONE_PARAMETER( newob, m_skinnedMeshLinksMesh );
	CLONE_PARAMETER( newob, m_scrollBarCurrentPos );
	CLONE_PARAMETER( newob, m_bIsClosingFile );
	
	for ( ModularSkinCategoryInfoMap::iterator modularSkinCategoryInfoMapIter = m_modularSkinCategoryInfoMap.begin(); modularSkinCategoryInfoMapIter != m_modularSkinCategoryInfoMap.end(); ++modularSkinCategoryInfoMapIter )
	{
		newob->m_modularSkinCategoryInfoMap[ modularSkinCategoryInfoMapIter->first ] = modularSkinCategoryInfoMapIter->second;
	}

	newob->ReplaceReference(0,remap.CloneRef(pblock2));

	BaseClone(this, newob, remap);
	return newob;
}

int ICModularSkinMasterObject::FindUnsortedCategoryId( int sortedId )
{
	int unsortedCategoryId = -1;
	for ( int categoryId = 0; categoryId < m_modularSkinCategoryInfoMap.size(); ++categoryId )
	{
		if ( m_modularSkinCategoryInfoMap[categoryId].m_sortedId == sortedId )
		{
			unsortedCategoryId = categoryId;
			break;
		}
	}
	return unsortedCategoryId;
}

// enables selected skinned mesh modifiers
// disables unselected skinned mesh modifiers
void ICModularSkinMasterObject::UpdateSkinnedMeshHiddenState( bool bForceUpdate )
{
	bool bHideUnselectedSkins = HideUnselectedSkinsEnabled();
	if ( !bHideUnselectedSkins && !bForceUpdate )
	{
		return;
	}

	bool bHiddenStatesModified = false;
	int numSkinnedMeshCategories = GetSkinnedMeshCategoryNodeCount();
	for ( int skinnedMeshCategoryId = 0; skinnedMeshCategoryId < numSkinnedMeshCategories; ++skinnedMeshCategoryId )
	{	
		INode *pSelectedSkinnedMeshNode = GetSelectedSkinnedMeshNode( skinnedMeshCategoryId );
		INode *pCategoryNode = GetSkinnedMeshCategoryNode( skinnedMeshCategoryId );
		if ( !pCategoryNode || !pSelectedSkinnedMeshNode )
		{
			continue;
		}
		
		Object *pObject = pCategoryNode->GetObjectRef();
		
		ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pObject;
		int skinnedMeshListCount = pModularSkinCategoryObject->GetSkinnedMeshNodeCount();
		for ( int skinnedMeshId = 0; skinnedMeshId < skinnedMeshListCount; ++skinnedMeshId )
		{
			INode *pSkinnedMeshNode = pModularSkinCategoryObject->GetSkinnedMeshNode( skinnedMeshId );

			bool bNewHideState = bHideUnselectedSkins && pSelectedSkinnedMeshNode != pSkinnedMeshNode;
			bool bOldHideState = pSkinnedMeshNode->IsHidden();
			bHiddenStatesModified |= ( bNewHideState != bOldHideState );

			pSkinnedMeshNode->Hide( bNewHideState );
		}
	}

	// redraw viewports
	
	// This method did not properly refresh the entirety of the viewports
	//{
	//	Interface *pInterface = GetCOREInterface();
	//	TimeValue t = pInterface->GetTime();
	//	pInterface->RedrawViews( t, REDRAW_BEGIN );
	//	pInterface->RedrawViews( t, REDRAW_NORMAL );
	//	pInterface->RedrawViews( t, REDRAW_END );
	//}

	// This method works but is guaranteed to be slow
	// There may be a way to flag nodes and/or their objects
	// as invalid, and then redraw only those invalid objects
	if ( bHiddenStatesModified )
	{
		Interface *pInterface = GetCOREInterface();
		pInterface->ForceCompleteRedraw( FALSE );
	}
}

INT_PTR CALLBACK SelectedSkinnedMeshRollupProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );
	ICModularSkinMasterObject *pModularSkinMasterObject = DLGetWindowLongPtr<ICModularSkinMasterObject*>( hWnd );

	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			//DLSetWindowLongPtr(hWnd, lParam);
			return TRUE;
		}

		// Handle the scrolling
		case WM_VSCROLL:
		{
			int code = loWord;
			int cpos = hiWord;

			int scrollBarCurrentPos = pModularSkinMasterObject->GetScrollBarCurrentPos();
			int scrollBarPageSize = pModularSkinMasterObject->GetComboBoxCount();

			// Handle the scroll bar
			{
				switch ( code )
				{
					case SB_LINEUP:
					{
						--scrollBarCurrentPos;
						break;
					}

					case SB_LINEDOWN:
					{
						++scrollBarCurrentPos;
						break;
					}

					case SB_PAGEUP:
					{
						scrollBarCurrentPos -= scrollBarPageSize;
						break;
					}

					case SB_PAGEDOWN:
					{
						scrollBarCurrentPos += scrollBarPageSize;
						break;
					}

					case SB_THUMBPOSITION:
					{
						scrollBarCurrentPos = cpos;
						break;
					}

					case SB_THUMBTRACK:
					{
						scrollBarCurrentPos = cpos;
						break;
					}
				}

				//NodeList categoryNodes;
				//pModularSkinMasterObject->GetSortedCategoryNodes( categoryNodes );
				//int maxScrollBarPos = categoryNodes.size() - scrollBarPageSize;
				int maxScrollBarPos = pModularSkinMasterObject->GetSkinnedMeshCategoryNodeCount() - scrollBarPageSize;

				scrollBarCurrentPos = scrollBarCurrentPos < 0 ? 0 : ( scrollBarCurrentPos > maxScrollBarPos ? maxScrollBarPos : scrollBarCurrentPos );
				
				pModularSkinMasterObject->SetScrollBarCurrentPos( scrollBarCurrentPos );
				UIHelpers::SetScrollPos( hWnd, IDC_MODULAR_SKINNING_CATEGORY_SCROLLBAR, scrollBarCurrentPos, true );
				pModularSkinMasterObject->RefreshSelectedSkinnedMeshRollup();
			}
			break;
		}

		case WM_COMMAND:
		{
			switch ( hiWord )
			{		
				case LBN_SELCHANGE:
				{
					int comboBoxIdc = loWord;
					int comboBoxId = pModularSkinMasterObject->GetComboBoxId( comboBoxIdc );
					if ( comboBoxId == -1 )
					{
						break;
					}

					int selectedIdx = UIHelpers::GetComboBoxSelectedIndex( hWnd, comboBoxIdc );

					// find unsorted category Id
					int unsortedCategoryId = comboBoxId;
					//int unsortedCategoryId = pModularSkinMasterObject->FindUnsortedCategoryId( comboBoxId );
					FUAssert( unsortedCategoryId != -1, return TRUE );

					//int bPreviousHoldingState = theHold.Holding();
					//if ( bPreviousHoldingState == 0 )
					//{
					//	theHold.Begin();
					//}
		
					//ICModularSkinMasterObject::SkinnedMeshSelectionRestoreObj *pRestoreObj == pModularSkinMasterObject->GetComboBoxRestoreObj( comboBoxIdc );
					//pRestoreObj->Set

					//theHold.Begin();
					
					//ICModularSkinMasterObject::SkinnedMeshSelectionRestoreObj *pRestoreObj = new ICModularSkinMasterObject::SkinnedMeshSelectionRestoreObj( pModularSkinMasterObject );
					//theHold.Put( pRestoreObj );
					//theHold.Accept( IDS_MODULAR_SKINNING_UNDO_SKINNED_MESH_SELECTION );

					//if ( bPreviousHoldingState == 0 )
					//{
					//}

					// get skinned mesh node corresponding to selected combo box item
					INode *pCategoryNode = pModularSkinMasterObject->GetSkinnedMeshCategoryNode( unsortedCategoryId );
					ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pCategoryNode->GetObjectRef();
					INode *pSkinnedMeshNode = pModularSkinCategoryObject->GetSkinnedMeshNode( selectedIdx );

					// update info map
					//ModularSkinCategoryInfoMap &modularSkinCategoryInfoMap = pModularSkinMasterObject->GetModularSkinCategoryInfoMap();
					//FUAssert( modularSkinCategoryInfoMap.find( comboBoxId ) != modularSkinCategoryInfoMap.end(),  );
					//modularSkinCategoryInfoMap[comboBoxId].m_pSelectedSkinnedMeshNode = pSkinnedMeshNode;

					// update param
					theHold.Begin();
					pModularSkinMasterObject->SetSelectedSkinnedMeshNode( unsortedCategoryId, pSkinnedMeshNode );
					theHold.Accept( GetString( IDS_MODULAR_SKINNING_UNDO_SKINNED_MESH_SELECTION ) );
					return TRUE;
				}
			}
			break;
		}

		//case WM_LBUTTONDOWN:
		//case WM_LBUTTONUP:
		//case WM_MOUSEMOVE:
		//{
		//	if ( ip )
		//	{
		//		ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
		//	}
		//	return FALSE;
		//}
	}
	return FALSE;
}

void ICModularSkinMasterObject::BeginEditParams( IObjParam *ip, ULONG flags, Animatable *prev )
{
	modularSkinMasterDesc.BeginEditParams(ip, this, flags, prev);

	if ( !s_selectedSkinnedMeshesRollupHWnd )
	{
		s_selectedSkinnedMeshesRollupHWnd = ip->AddRollupPage
		( 
			hInstance
			, MAKEINTRESOURCE(IDD_MODULAR_SKIN_MASTER_SELECTED_SKINNED_MESH_ROLLUP)
			, SelectedSkinnedMeshRollupProc
			, GetString(IDS_MODULAR_SKIN_MASTER_OBJECT_SELECTED_SKINNED_MESHES_ROLLUP_LABEL)
			, (LPARAM)ip
			, 0
			, ROLLUP_CAT_CUSTATTRIB
		);

		DLSetWindowLongPtr( s_selectedSkinnedMeshesRollupHWnd, this );	
		RefreshSelectedSkinnedMeshRollup();
	}

	//if ( !s_presetsRollupHWnd )
	//{
	//	s_presetsRollupHWnd = ip->AddRollupPage
	//	( 
	//		hInstance
	//		, MAKEINTRESOURCE(IDD_MODULAR_SKIN_MASTER_SELECTED_SKINNED_MESH_ROLLUP)
	//		, SelectedSkinnedMeshRollupProc
	//		, GetString(IDS_MODULAR_SKIN_MASTER_OBJECT_SELECTED_SKINNED_MESHES_ROLLUP_LABEL)
	//		, (LPARAM)ip
	//		, 0
	//		, ROLLUP_CAT_CUSTATTRIB
	//	);

	//	DLSetWindowLongPtr( s_presetsRollupHWnd, this );	
	//	RefreshSelectedSkinnedMeshRollup();
	//}

	m_scrollBarCurrentPos = 0;
}

void ICModularSkinMasterObject::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{		
	modularSkinMasterDesc.EndEditParams(ip, this, flags, next);

	if ( flags & END_EDIT_REMOVEUI ) 
	{		
		if ( s_selectedSkinnedMeshesRollupHWnd )
		{	
			ip->DeleteRollupPage( s_selectedSkinnedMeshesRollupHWnd );
			s_selectedSkinnedMeshesRollupHWnd = NULL;
		}
	}
}

void ICModularSkinMasterObject::BuildMesh( TimeValue t, INode *pNode )
{
	// cube gizmo
	{
		// vertices
		{
			Box3 box = ComputeGizmoBox( t, pNode );
			const int k_numBoxVertices = 8;
			m_gizmoMesh.setNumVerts( k_numBoxVertices );
			for ( int vertexId = 0; vertexId < k_numBoxVertices; ++vertexId )
			{
				m_gizmoMesh.setVert( vertexId, box[vertexId] );
			}
		}

		// faces
		{
			const int k_numFaces = 12;
			m_gizmoMesh.setNumFaces( k_numFaces );
			int currentFace = 0;

			m_gizmoMesh.faces[currentFace++].setVerts( 0, 1, 5 );
			m_gizmoMesh.faces[currentFace++].setVerts( 5, 4, 0 );
		
			m_gizmoMesh.faces[currentFace++].setVerts( 1, 3, 7 );
			m_gizmoMesh.faces[currentFace++].setVerts( 7, 5, 1 );
		
			m_gizmoMesh.faces[currentFace++].setVerts( 3, 2, 6 );
			m_gizmoMesh.faces[currentFace++].setVerts( 6, 7, 3 );
		
			m_gizmoMesh.faces[currentFace++].setVerts( 2, 0, 4 );
			m_gizmoMesh.faces[currentFace++].setVerts( 4, 6, 2 );
		
			m_gizmoMesh.faces[currentFace++].setVerts( 2, 3, 1 );
			m_gizmoMesh.faces[currentFace++].setVerts( 1, 0, 2 );
		
			m_gizmoMesh.faces[currentFace++].setVerts( 4, 5, 7 );
			m_gizmoMesh.faces[currentFace++].setVerts( 7, 6, 4 );

			for ( int faceId = 0; faceId < k_numFaces; ++faceId )
			{
				m_gizmoMesh.faces[faceId].setEdgeVisFlags( 1, 1, 0 );
				//m_gizmoMesh.faces[faceId].setSmGroup( 1 );
			}
		}

		m_gizmoMesh.InvalidateGeomCache();
	}

	// category links
	if ( IsDisplayLinksEnabled() )
	{
		const int numLinks = GetSkinnedMeshCategoryNodeCount();
		if ( numLinks <= 0 )
		{
			m_categoryLinksMesh.setNumVerts( 0 );
			m_categoryLinksMesh.setNumFaces( 0 );		
		}
		else
		{
			const int linksNumVerts = numLinks + 1;
			const int linksNumFaces = numLinks;

			m_categoryLinksMesh.setNumVerts( linksNumVerts );
			m_categoryLinksMesh.setNumFaces( linksNumFaces );

			const Matrix3 &nodeTm = pNode->GetObjectTM( t );
			//const Matrix3 &nodeTm = pNode->GetObjTMBeforeWSM( t );
			const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

			const int centerVertexId = 0;
			m_categoryLinksMesh.setVert( centerVertexId, Point3( 0.0f, 0.0f, 0.0f ) );

			int currentVertex = centerVertexId + 1;
			for ( int currentLinkId = 0; currentLinkId < numLinks; ++currentLinkId )
			{
				INode *pNode = GetSkinnedMeshCategoryNode( currentLinkId );

				// undo/redo case
				if ( !pNode )
				{
					m_categoryLinksMesh.setVert( currentVertex++, Point3( 0.0f, 0.0f, 0.0f ) );
					m_categoryLinksMesh.faces[currentLinkId].setEdgeVisFlags( 0, 0, 0 );
					m_categoryLinksMesh.faces[currentLinkId].setVerts( centerVertexId, currentVertex - 1, centerVertexId );					
				}
				else
				{
					Matrix3 currentLinkNodeTm = pNode->GetObjTMBeforeWSM( t );
					const Point3 &currentLinkTranslation = currentLinkNodeTm.GetRow( 3 );

					m_categoryLinksMesh.setVert( currentVertex++, currentLinkTranslation - nodeTranslation );
					m_categoryLinksMesh.faces[currentLinkId].setEdgeVisFlags( 1, 1, 1 );
					m_categoryLinksMesh.faces[currentLinkId].setVerts( centerVertexId, currentVertex - 1, centerVertexId );
				}
			}
		}

		m_categoryLinksMesh.InvalidateGeomCache();
	}

	// skinned mesh links
	if ( IsDisplayLinksEnabled() )
	{
		const int numLinks = pblock2->Count( PI_SKINNED_MESH_LIST );
		if ( numLinks <= 0 )
		{
			m_skinnedMeshLinksMesh.setNumVerts( 0 );
			m_skinnedMeshLinksMesh.setNumFaces( 0 );		
		}
		else
		{
			const int linksNumVerts = numLinks + 1;
			const int linksNumFaces = numLinks;

			m_skinnedMeshLinksMesh.setNumVerts( linksNumVerts );
			m_skinnedMeshLinksMesh.setNumFaces( linksNumFaces );

			const Matrix3 &nodeTm = pNode->GetObjectTM( t );
			//const Matrix3 &nodeTm = pNode->GetObjTMBeforeWSM( t );
			const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

			const int centerVertexId = 0;
			m_skinnedMeshLinksMesh.setVert( centerVertexId, Point3( 0.0f, 0.0f, 0.0f ) );

			int currentVertex = centerVertexId + 1;
			for ( int currentLinkId = 0; currentLinkId < numLinks; ++currentLinkId )
			{
				INode *pNode = NULL;
				pblock2->GetValue( PI_SKINNED_MESH_LIST, 0, pNode, FOREVER, currentLinkId );
				
				Matrix3 currentLinkNodeTm = pNode->GetObjTMBeforeWSM( t );
				const Point3 &currentLinkTranslation = currentLinkNodeTm.GetRow( 3 );

				m_skinnedMeshLinksMesh.setVert( currentVertex++, currentLinkTranslation - nodeTranslation );
				m_skinnedMeshLinksMesh.faces[currentLinkId].setEdgeVisFlags( 1, 1, 1 );
				m_skinnedMeshLinksMesh.faces[currentLinkId].setVerts( centerVertexId, currentVertex - 1, centerVertexId );
			}
		}

		m_skinnedMeshLinksMesh.InvalidateGeomCache();
	}
}

void ICModularSkinMasterObject::GetLinksTM( const Matrix3 &objectTM, Matrix3 &tm )
{
	tm.IdentityMatrix();
	tm.SetTrans( objectTM.GetTrans() );
}

int ICModularSkinMasterObject::HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt )
{
	GraphicsWindow *pGraphicsWindow = vpt->getGW();

	HitRegion hr; 
	MakeHitRegion( hr, type, crossing, 4, p ); 
	const Matrix3 objectTM = inode->GetObjectTM( t ); 

	// gizmo
	{
		pGraphicsWindow->setTransform( objectTM );
		if ( m_gizmoMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
		{
			return TRUE;
		}
	}

	// links
	if ( IsDisplayLinksEnabled() )
	{
		if ( m_categoryLinksMesh.getNumVerts() > 0 )
		{
			Matrix3 tm; 
			GetLinksTM( objectTM, tm );
			pGraphicsWindow->setTransform( tm );

			if ( m_categoryLinksMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
			{
				return TRUE;
			}
		}

		if ( m_skinnedMeshLinksMesh.getNumVerts() > 0 )
		{
			Matrix3 tm; 
			GetLinksTM( objectTM, tm );
			pGraphicsWindow->setTransform( tm );

			if ( m_skinnedMeshLinksMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int ICModularSkinMasterObject::Display( TimeValue t, INode* inode, ViewExp *vpt, int flags )
{
	BuildMesh( t, inode );

	GraphicsWindow *pGraphicsWindow = vpt->getGW();

	// render flags
	DWORD rlim  = pGraphicsWindow->getRndLimits();
	pGraphicsWindow->setRndLimits( GW_WIREFRAME | GW_EDGES_ONLY | /*GW_BACKCULL|*/ ( rlim & GW_Z_BUFFER ) );  // removed BC on 4/29/99
	
	// transform
	const Matrix3 objectTM = inode->GetObjectTM( t ); 

	// vertex color
	//if ( inode->Selected() )
	//{
	//	pGraphicsWindow->setColor( LINE_COLOR, GetSelColor() );
	//}
	//else if( !inode->IsFrozen() )
	//{
		
	//}

	// display gizmo mesh
	{
		pGraphicsWindow->setTransform( objectTM );
		pGraphicsWindow->setColor( LINE_COLOR, inode->Selected() ? GetSelColor() : k_gizmoColor );
		m_gizmoMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
	}

	// display links mesh
	if ( IsDisplayLinksEnabled() )
	{
		{
			Matrix3 tm;
			GetLinksTM( objectTM, tm );
			pGraphicsWindow->setTransform( tm );
			
			pGraphicsWindow->setColor( LINE_COLOR, inode->Selected() ? GetSelColor() : k_categoryLinksColor );
			m_categoryLinksMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
		}

		{
			Matrix3 tm;
			GetLinksTM( objectTM, tm );
			pGraphicsWindow->setTransform( tm );
			
			pGraphicsWindow->setColor( LINE_COLOR, inode->Selected() ? GetSelColor() : k_skinnedMeshLinksColor );
			m_skinnedMeshLinksMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
		}
	}
	
	// label
	//Point3 labelPos( 0.f, 0.f, 0.f );
	//pGraphicsWindow->setColor( TEXT_COLOR, 1.f, 1.f, 1.f );
	//pGraphicsWindow->text( &labelPos, _T("Category") );
	pGraphicsWindow->setRndLimits( rlim );

	return 0;
}

void ICModularSkinMasterObject::Snap( TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt )
{
	return;
}

Interval ICModularSkinMasterObject::ObjectValidity( TimeValue t )
{ 
	return FOREVER; 
}

ObjectState ICModularSkinMasterObject::Eval( TimeValue time )
{ 
	return ObjectState( this );
}

void ICModularSkinMasterObject::GetWorldBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	const Matrix3 objectTM = pNode->GetObjectTM( t );
	const int k_numBoxVertices = 8;
	const int k_numElements = 3;

	Box3 gizmoBox = ComputeGizmoBox( t, pNode );
	Box3 categoryLinksBox = ComputeCategoryLinksBox( t, pNode );
	Box3 skinnedMeshLinksBox = ComputeSkinnedMeshLinksBox( t, pNode );

	Point3 worldMin( FLT_MAX, FLT_MAX, FLT_MAX );
	Point3 worldMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	// gizmo
	{
		for ( int vertexId = 0; vertexId < k_numBoxVertices; ++vertexId )
		{
			Point3 p = objectTM.PointTransform( gizmoBox[vertexId] );
			for ( int elementId = 0; elementId < k_numElements; ++elementId )
			{
				worldMin[elementId] = min( worldMin[elementId], p[elementId] );
				worldMax[elementId] = max( worldMax[elementId], p[elementId] );
			}
		}
	}

	// category links
	{
		Matrix3 tm;
		GetLinksTM( objectTM, tm );

		for ( int vertexId = 0; vertexId < k_numBoxVertices; ++vertexId )
		{
			Point3 p = tm.PointTransform( categoryLinksBox[vertexId] );
			for ( int elementId = 0; elementId < k_numElements; ++elementId )
			{
				worldMin[elementId] = min( worldMin[elementId], p[elementId] );
				worldMax[elementId] = max( worldMax[elementId], p[elementId] );
			}
		}
	}	

	// skinned mesh links
	{
		Matrix3 tm;
		GetLinksTM( objectTM, tm );

		for ( int vertexId = 0; vertexId < k_numBoxVertices; ++vertexId )
		{
			Point3 p = tm.PointTransform( skinnedMeshLinksBox[vertexId] );
			for ( int elementId = 0; elementId < k_numElements; ++elementId )
			{
				worldMin[elementId] = min( worldMin[elementId], p[elementId] );
				worldMax[elementId] = max( worldMax[elementId], p[elementId] );
			}
		}
	}	

	box = Box3( worldMin, worldMax );
	return;
}

void ICModularSkinMasterObject::GetLocalBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	Box3 gizmoBox = ComputeGizmoBox( t, pNode );
	Box3 categoryLinksBox = ComputeCategoryLinksBox( t, pNode );
	Box3 skinnedMeshLinksBox = ComputeSkinnedMeshLinksBox( t, pNode );

	Point3 localMin = gizmoBox.Min();
	
	localMin = MAXHelpers::MinComponents( localMin, categoryLinksBox.Min() );
	localMin = MAXHelpers::MinComponents( localMin, skinnedMeshLinksBox.Min() );

	Point3 localMax = gizmoBox.Max();
	
	localMax = MAXHelpers::MaxComponents( localMax, categoryLinksBox.Max() );
	localMax = MAXHelpers::MaxComponents( localMax, skinnedMeshLinksBox.Max() );

	box = Box3( localMin, localMax );
}	

void ICModularSkinMasterObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel )
{
	return;
}

int ICModularSkinMasterObjCreateCallback::proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat )
{
	//if ( msg == MOUSE_INIT )
	//{
	//
	//}
	//else if ( msg == MOUSE_UNINIT )
	//{
	//
	//}
	//else
	if ( msg == MOUSE_POINT )
	{
		switch( point )
		{
			case MAXHelpers::CPC_FIRST :
			{
				sp0 = m;
				p0 = vpt->GetPointOnCP( sp0 );
				mat.SetTrans( p0 );
				ob->SetGizmoBoxMin( Point3( 0.f, 0.f, 0.f ) );
				ob->SetGizmoBoxMax( Point3( 0.f, 0.f, 0.f ) );
				break;
			}

			case MAXHelpers::CPC_SECOND :
			{
				sp1 = m;
				p1 = vpt->GetPointOnCP( sp1 );
				Box3 box;
				box.MakeCube( Point3( 0.f, 0.f, 0.f ), Length( p1 - p0 ) );
				ob->SetGizmoBoxMin( box.Min() );
				ob->SetGizmoBoxMax( box.Max() );

				if ( Length( sp1 - sp0 ) < 3 )
				{						
					return CREATE_ABORT;
				}
				else 
				{
					return CREATE_STOP;
				}

				break;
			}
		}
	}
	else if ( msg == MOUSE_MOVE )
	{
		switch( point )
		{
			//case MAXHelpers::CPC_FIRST:
			//{
			//	sp0 = m;
			//	break;
			//}

			case MAXHelpers::CPC_SECOND:
			{
				sp1 = m;
				p1 = vpt->GetPointOnCP( sp1 );
				Box3 box;
				box.MakeCube( Point3( 0.f, 0.f, 0.f ), Length( p1 - p0 ) );
				ob->SetGizmoBoxMin( box.Min() );
				ob->SetGizmoBoxMax( box.Max() );
				break;
			}
		}
	} 
	else if ( msg == MOUSE_ABORT )
	{
		return CREATE_ABORT;
	}	
	else if ( msg == MOUSE_FREEMOVE )
	{
		//vpt->SnapPreview( m, m );
	}

	return TRUE;
}

CreateMouseCallBack* ICModularSkinMasterObject::GetCreateMouseCallBack()
{
	static ICModularSkinMasterObjCreateCallback createCB;
	createCB.ob = this;
	return &createCB;
}

// JBW:  for loading old ParamBlock versions, register an Ed. 2 param block converter post-load callback
//       This one takes the ParamBlockDescID version array as before, but now takes the 
//       ParamBlockDesc2 for the block to describe the current version and will convert the loaded ParamBlock
//       into a corresponding ParamBlock2
IOResult
ICModularSkinMasterObject::Load(ILoad *iload) 
{	
	//ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, NUM_OLDVERSIONS, &s_modularSkinMasterParamBlockDesc, this, FSDeflector::PBLOCK_REF_NO);
	//iload->RegisterPostLoadCallback(plcb);
	iload->RegisterPostLoadCallback( new ICModularSkinMasterObjectPostLoadCallback( this ) );
	return IO_OK;
}

static int NodeNameCompareFunc( const void *elem1, const void *elem2 )
{
	INode *pNonConstA = *( INode ** )const_cast<void *>( elem1 );
	INode *pNonConstB = *( INode ** )const_cast<void *>( elem2 );

	if ( pNonConstA == NULL 
		&& pNonConstB != NULL )
	{
		return -1;
	}

	if ( pNonConstA != NULL 
		&& pNonConstB == NULL )
	{
		return 1;
	}

	if ( pNonConstA == NULL 
		&& pNonConstB == NULL )
	{
		return 0;
	}

	return _stricmp( pNonConstA->GetName(), pNonConstB->GetName() );
}

void ICModularSkinMasterObject::GetSortedCategoryNodes( NodeList &categoryNodes )
{
	// First copying into noteTab to ensure node names are sorted the same way as in widget
	Tab<INode *> nodeTab;
	int numSkinnedMeshCategories = pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST );
	for ( int skinnedMeshCategoryId = 0; skinnedMeshCategoryId < numSkinnedMeshCategories; ++skinnedMeshCategoryId )
	{	
		INode *pCategoryNode = NULL;
		pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, 0, pCategoryNode, FOREVER, skinnedMeshCategoryId );
		nodeTab.Append( 1, &pCategoryNode );
	}

	//NodeNameComparator nodeNameComparator;
	//categoryNodes.sort( nodeNameComparator );
	nodeTab.Sort( &NodeNameCompareFunc );

	// copy into NodeList
	for ( int nodeId = 0; nodeId < nodeTab.Count(); ++nodeId )
	{
		categoryNodes.push_back( nodeTab[nodeId] );
	}
}

// TODO : Implement scrollbar support
void ICModularSkinMasterObject::RefreshSelectedSkinnedMeshRollup()
{
	if ( !s_selectedSkinnedMeshesRollupHWnd )
	{
		return;
	}

	//// get array of categories, sorted by name in ascending order
	//NodeList categoryNodes;
	//GetSortedCategoryNodes( categoryNodes );
	//int numSkinnedMeshCategories = categoryNodes.size();
	int numSkinnedMeshCategories = GetSkinnedMeshCategoryNodeCount();
	int numSelectedSkinnedMeshNodes = GetSelectedSkinnedMeshNodeCount();

	// This method is called multiple times during undo/redo operations
	// This check eliminates times when pblock is in an invalid state
	//FUAssert( numSkinnedMeshCategories == numSelectedSkinnedMeshNodes, return );
	if ( numSkinnedMeshCategories != numSelectedSkinnedMeshNodes )
	{
		return;
	}

	// clear combo boxes
	for ( IdcList::iterator comboBoxIdcIter = m_selectedSkinnedMeshesComboBoxIdcList.begin(); comboBoxIdcIter != m_selectedSkinnedMeshesComboBoxIdcList.end(); ++comboBoxIdcIter )
	{
		int comboBoxIdc = *comboBoxIdcIter;
		UIHelpers::ClearComboBox( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc );
		UIHelpers::SetControlEnabled( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, false );
		//UIHelpers::SetControlVisible( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, false );
	}

	// set scrollbar
	if ( numSkinnedMeshCategories > m_selectedSkinnedMeshesComboBoxIdcList.size() )
	{
		SCROLLINFO scrollInfo;
		memset( &scrollInfo, 0, sizeof( scrollInfo ) );
		scrollInfo.cbSize = sizeof( scrollInfo );

		scrollInfo.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		scrollInfo.nMax = numSkinnedMeshCategories - 1;
		scrollInfo.nMin = 0;
		scrollInfo.nPage = m_selectedSkinnedMeshesComboBoxIdcList.size();
		scrollInfo.nPos = m_scrollBarCurrentPos;

		UIHelpers::SetScrollInfo( s_selectedSkinnedMeshesRollupHWnd, IDC_MODULAR_SKINNING_CATEGORY_SCROLLBAR, &scrollInfo );
		UIHelpers::SetControlEnabled( s_selectedSkinnedMeshesRollupHWnd, IDC_MODULAR_SKINNING_CATEGORY_SCROLLBAR, true );
		//UIHelpers::SetControlVisible( s_selectedSkinnedMeshesRollupHWnd, IDC_MODULAR_SKINNING_CATEGORY_SCROLLBAR, true );
	}
	else
	{
		m_scrollBarCurrentPos = 0;
		UIHelpers::SetControlEnabled( s_selectedSkinnedMeshesRollupHWnd, IDC_MODULAR_SKINNING_CATEGORY_SCROLLBAR, false );
		//UIHelpers::SetControlVisible( s_selectedSkinnedMeshesRollupHWnd, IDC_MODULAR_SKINNING_CATEGORY_SCROLLBAR, false );
	}

	// fill combo boxes
	{
		for ( int comboBoxId = 0; comboBoxId < ( int )m_selectedSkinnedMeshesComboBoxIdcList.size() && comboBoxId < numSkinnedMeshCategories; ++comboBoxId )
		{
			int comboBoxIdc = m_selectedSkinnedMeshesComboBoxIdcList[comboBoxId];
			int skinnedMeshCategoryId = m_scrollBarCurrentPos + comboBoxId;

			INode *pCategoryNode = GetSkinnedMeshCategoryNode( skinnedMeshCategoryId );
			//INode *pCategoryNode = categoryNodes[skinnedMeshCategoryId];

			// for undo/redo
			if ( pCategoryNode == NULL )
			{
				continue;
			}

			Object *pObject = pCategoryNode->GetObjectRef();
			
			ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pObject;
			int skinnedMeshListCount = pModularSkinCategoryObject->GetSkinnedMeshNodeCount();
			for ( int skinnedMeshId = 0; skinnedMeshId < skinnedMeshListCount; ++skinnedMeshId )
			{
				INode *pSkinnedMeshNode = pModularSkinCategoryObject->GetSkinnedMeshNode( skinnedMeshId );
				UIHelpers::AddComboBoxString( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, pSkinnedMeshNode->GetName(), skinnedMeshId );
			}

			//UIHelpers::SetControlVisible( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, true );
			//if ( skinnedMeshListCount > 0 )
			//{
				UIHelpers::SetControlEnabled( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, true );
			//}
		}
	}

	// set selected combobox items
	{
		for ( int comboBoxId = 0; comboBoxId < ( int )m_selectedSkinnedMeshesComboBoxIdcList.size() && comboBoxId < numSkinnedMeshCategories; ++comboBoxId )
		{
			int comboBoxIdc = m_selectedSkinnedMeshesComboBoxIdcList[comboBoxId];
			int skinnedMeshCategoryId = m_scrollBarCurrentPos + comboBoxId;

			INode *pCategoryNode = GetSkinnedMeshCategoryNode( skinnedMeshCategoryId );
			//INode *pCategoryNode = categoryNodes[selectedSkinnedMeshId];

			// for undo/redo
			if ( pCategoryNode == NULL )
			{
				continue;
			}

			// find real category id
			int realCategoryId = skinnedMeshCategoryId;
			//int realCategoryId = FindUnsortedCategoryId( selectedSkinnedMeshId );
			FUAssert( realCategoryId != -1, continue );

			INode *pSelectedSkinnedMeshNode = GetSelectedSkinnedMeshNode( realCategoryId );
			if ( pSelectedSkinnedMeshNode == NULL )
			{
				continue;
			}

			Object *pObject = pCategoryNode->GetObjectRef();
			ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pObject;

			int skinnedMeshListCount = pModularSkinCategoryObject->GetSkinnedMeshNodeCount();

			//if ( pSelectedSkinnedMeshNode == NULL )
			//{
			//	if ( skinnedMeshListCount > 0 )
			//	{
			//		UIHelpers::SetComboBoxSelectedIndex( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, 0 );
			//	}
			//}
			//else
			//{	
			//if ( pSelectedSkinnedMeshNode != NULL )
			//{
				int nodeId = pModularSkinCategoryObject->GetSkinnedMeshNodeId( pSelectedSkinnedMeshNode );
				UIHelpers::SetComboBoxSelectedIndex( s_selectedSkinnedMeshesRollupHWnd, comboBoxIdc, nodeId );
			//}
		}
	}
}

void ICModularSkinMasterObject::HideUnselectedSkinPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;
	pModularSkinMasterObject->UpdateSkinnedMeshHiddenState( true );
}

BOOL ICModularSkinMasterObject::SkinnedMeshCategoriesListPBValidator::Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex)
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;
		
	INode *pNode = ( INode * )v.r;

	Object *pObject = pNode->GetObjectRef();
	if ( !pObject )
	{
		return FALSE;
	}

	// Modular Skin Category object test
	{
		SClass_ID scid = pObject->SuperClassID();
		Class_ID cid = pObject->ClassID();
		Object *pTmpObject = pObject;
		while ( scid == GEN_DERIVOB_CLASS_ID || scid == DERIVOB_CLASS_ID || scid == GEN_DERIVOB_CLASS_ID )
		{
			IDerivedObject *pDerivedObject = ( IDerivedObject * )pTmpObject;
			pTmpObject = pDerivedObject->GetObjRef();
			scid = pTmpObject->SuperClassID();
			cid = pTmpObject->ClassID();
		}

		if ( cid != MODULAR_SKIN_CATEGORY_OBJECT_CLASSID )
		{
			return FALSE;
		}
	}

	// Duplicate test
	{
		int numCategories = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST );
		for ( int categoryId = 0; categoryId < numCategories; ++categoryId )
		{
			INode *pNodeInList = NULL;
			pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, 0, pNodeInList, FOREVER, categoryId );
			if ( pNode == pNodeInList )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void ICModularSkinMasterObject::SkinnedMeshListPBAccessor::TabChanged(tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count)
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;

	switch ( changeCode )
	{
		case tab_ref_deleted :
		{
			pModularSkinMasterObject->DeleteSkinnedMeshNodes( tabIndex, 1 );
			break;
		}

		case tab_setcount:
		{
			// Ensures that listbox widget is properly updated when undoing/redoing picking/remove operations
			s_modularSkinMasterParamBlockDesc.InvalidateUI();
			break;
		}
	}
}

BOOL ICModularSkinMasterObject::SkinnedMeshListPBValidator::Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex)
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;
		
	INode *pNode = ( INode * )v.r;

	Object *pObject = pNode->GetObjectRef();
	if ( !pObject )
	{
		return FALSE;
	}

	// Geometric object test
	{
		SClass_ID scid = pObject->SuperClassID();
		Object *pTmpObject = pObject;
		while ( scid == GEN_DERIVOB_CLASS_ID || scid == DERIVOB_CLASS_ID || scid == GEN_DERIVOB_CLASS_ID )
		{
			IDerivedObject *pDerivedObject = ( IDerivedObject * )pTmpObject;
			pTmpObject = pDerivedObject->GetObjRef();
			scid = pTmpObject->SuperClassID();
		}

		if ( scid != GEOMOBJECT_CLASS_ID )
		{
			return FALSE;
		}
	}

	// Skin test
	{
		bool bIsSkinnedMesh = false;
		ColladaModifierStack modifiers;
		ControllerExporter::ResolveSkinAndMorphModifiers( pNode, pObject, modifiers );
		for (ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end(); ++itM)
		{
			ColladaModifier* modifier = (*itM);
			if ( modifier->GetClassID() == COLLADA_SKIN_CLASS_ID )
			{
				bIsSkinnedMesh = true;
				break;
			}
		}
		CLEAR_POINTER_VECTOR( modifiers );

		if ( !bIsSkinnedMesh )
		{
			return FALSE;
		}
	}

	// Duplicate test
	{
		int numSkinnedMeshes = pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST );
		for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
		{
			INode *pNodeInList = NULL;
			pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_LIST, 0, pNodeInList, FOREVER, skinnedMeshId );
			if ( pNode == pNodeInList )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void ICModularSkinMasterObject::SynchronizeSelectedSkinnedMeshesWithCategories()
{
	//int numSkinnedMeshCategories = pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST );

	//// save old info map
	//ModularSkinCategoryInfoMap oldModularSkinCategoryInfoMap;
	//for ( ModularSkinCategoryInfoMap::iterator modularSkinCategoryInfoMapIter = m_modularSkinCategoryInfoMap.begin(); modularSkinCategoryInfoMapIter != m_modularSkinCategoryInfoMap.end(); ++modularSkinCategoryInfoMapIter )
	//{
	//	oldModularSkinCategoryInfoMap[ modularSkinCategoryInfoMapIter->first ] = modularSkinCategoryInfoMapIter->second;
	//}

	//// clear info map
	//m_modularSkinCategoryInfoMap.clear();

	//// get sorted category nodes
	//NodeList categoryNodes;
	//GetSortedCategoryNodes( categoryNodes );

	//// build new info map
	//for ( int categoryId = 0; categoryId < numSkinnedMeshCategories; ++categoryId )
	//{
	//	INode *pCategoryNode = NULL;
	//	pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, 0, pCategoryNode, FOREVER, categoryId );

	//	INode *pSelectedSkinnedMeshNode = NULL;
	//	{
	//		for ( ModularSkinCategoryInfoMap::iterator modularSkinCategoryInfoMapIter = oldModularSkinCategoryInfoMap.begin(); modularSkinCategoryInfoMapIter != oldModularSkinCategoryInfoMap.end(); ++modularSkinCategoryInfoMapIter )
	//		{	
	//			if ( modularSkinCategoryInfoMapIter->second.m_pCategoryNode == pCategoryNode )
	//			{
	//				pSelectedSkinnedMeshNode = modularSkinCategoryInfoMapIter->second.m_pSelectedSkinnedMeshNode;
	//			}
	//		}

	//		if ( pSelectedSkinnedMeshNode == NULL )
	//		{
	//			if ( pCategoryNode != NULL )
	//			{
	//				Object *pObject = pCategoryNode->GetObjectRef();
	//				ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pObject;

	//				int skinnedMeshListCount = pModularSkinCategoryObject->pblock2->Count( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST );
	//				if ( skinnedMeshListCount > 0 )
	//				{
	//					pModularSkinCategoryObject->pblock2->GetValue( ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST, 0, pSelectedSkinnedMeshNode, FOREVER, 0 );
	//				}
	//			}
	//		}
	//	}

	//	// get sorted id
	//	int sortedCategoryId = categoryNodes.find_index( pCategoryNode );

	//	m_modularSkinCategoryInfoMap.insert( categoryId, ModularSkinCategoryInfo( pCategoryNode, pSelectedSkinnedMeshNode, sortedCategoryId ) );
	//}

	//// clear selected skinned mesh param array
	//pblock2->ZeroCount( ICModularSkinMasterObject::PI_SELECTED_SKINNED_MESH_LIST );

	//// synchronize selected skinned mesh param with new info map
	//ModularSkinCategoryInfoMap &modularSkinCategoryInfoMap = m_modularSkinCategoryInfoMap;
	//for ( int categoryId = 0; categoryId < modularSkinCategoryInfoMap.size(); ++categoryId )
	//{
	//	pblock2->Append( ICModularSkinMasterObject::PI_SELECTED_SKINNED_MESH_LIST, 1, &modularSkinCategoryInfoMap[categoryId].m_pSelectedSkinnedMeshNode );
	//}

	//UpdateSkinnedMeshHiddenState();
}

void ICModularSkinMasterObject::SelectedSkinnedMeshListPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;
	//pModularSkinMasterObject->SynchronizeSelectedSkinnedMeshesWithCategories();
	pModularSkinMasterObject->RefreshSelectedSkinnedMeshRollup();
	pModularSkinMasterObject->UpdateSkinnedMeshHiddenState();
}

void ICModularSkinMasterObject::SelectedSkinnedMeshListPBAccessor::TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count )
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;

	switch ( changeCode )
	{
		case tab_ref_deleted :
		{
			// need to test for bad node reference because there is not guarantee that tab_ref_deleted
			// message has been received by pModularSkinCategoryObject prior to pModularSkinMasterObject receiving it
			INode *pSelectedSkinMeshNode = NULL;
			INode *pCategoryNode = pModularSkinMasterObject->GetSkinnedMeshCategoryNode( tabIndex );
			ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pCategoryNode->GetObjectRef();
			int skinnedMeshNodeCount = pModularSkinCategoryObject->GetSkinnedMeshNodeCount();
			for ( int skinnedMeshNodeId = 0; skinnedMeshNodeId < skinnedMeshNodeCount; ++skinnedMeshNodeId )
			{
				INode *pCandidateNode = pModularSkinCategoryObject->GetSkinnedMeshNode( skinnedMeshNodeId );
				if ( MAXHelpers::IsNodeInScene( pCandidateNode ) )
				{
					pSelectedSkinMeshNode = pCandidateNode;
					break;
				}
			}

			if ( !pModularSkinMasterObject->GetIsClosingFile() )
			{
				pModularSkinMasterObject->SetSelectedSkinnedMeshNode( tabIndex, pSelectedSkinMeshNode );
			}
			break;
		}

		case tab_insert :
		case tab_append :
		{
			pModularSkinMasterObject->UpdateSkinnedMeshHiddenState();
			break;
		}

		case tab_delete :
		{
			break;
		}

		case tab_setcount :
		{
			pModularSkinMasterObject->UpdateSkinnedMeshHiddenState();
			break;
		}
	}
}

void ICModularSkinMasterObject::SkinnedMeshCategoriesListPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;
	//pModularSkinMasterObject->SynchronizeSelectedSkinnedMeshesWithCategories();
	pModularSkinMasterObject->RefreshSelectedSkinnedMeshRollup();
	pModularSkinMasterObject->UpdateSkinnedMeshHiddenState();
}

void ICModularSkinMasterObject::SkinnedMeshCategoriesListPBAccessor::TabChanged(tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count)
{
	ICModularSkinMasterObject *pModularSkinMasterObject = (ICModularSkinMasterObject*)owner;

	switch ( changeCode )
	{
		case tab_ref_deleted :
		{
			pModularSkinMasterObject->DeleteSkinnedMeshCategories( tabIndex, 1 );
			break;
		}

		case tab_insert :
		case tab_append :
		{
			INode *pCategoryNode = (INode *)(*tab)[tabIndex].r;
			ICModularSkinCategoryObject *pModularSkinCategoryObject = ( ICModularSkinCategoryObject * )pCategoryNode->GetObjectRef();
			
			INode *pSelectedSkinMeshNode = NULL;
			int skinnedMeshNodeCount = pModularSkinCategoryObject->GetSkinnedMeshNodeCount();
			if ( skinnedMeshNodeCount > 0 )
			{
				pSelectedSkinMeshNode = pModularSkinCategoryObject->GetSkinnedMeshNode( 0 );
			}
			pModularSkinMasterObject->InsertSelectedSkinnedMeshNodes( tabIndex, 1, &pSelectedSkinMeshNode );
			pModularSkinMasterObject->RefreshSelectedSkinnedMeshRollup();
			break;
		}

		case tab_delete :
		{
			pModularSkinMasterObject->DeleteSelectedSkinnedMeshNodes( tabIndex, 1 );
			//pModularSkinMasterObject->SynchronizeSelectedSkinnedMeshesWithCategories();
			pModularSkinMasterObject->RefreshSelectedSkinnedMeshRollup();
			break;
		}

		case tab_setcount :
		{
			// Ensures that listbox widget is properly updated when undoing/redoing picking/remove operations
			s_modularSkinMasterParamBlockDesc.InvalidateUI();
			pModularSkinMasterObject->RefreshSelectedSkinnedMeshRollup();
			break;
		}
	}
}

void ICModularSkinMasterObject::ICModularSkinMasterObjectPostLoadCallback::proc( ILoad *iload )
{
	//NodeList categoryNodes;
	//m_pModularSkinMasterObject->GetSortedCategoryNodes( categoryNodes );

	//// fill modular skin category map
	//ModularSkinCategoryInfoMap &modularSkinCategoryInfoMap = m_pModularSkinMasterObject->GetModularSkinCategoryInfoMap();
	//int numSkinnedMeshCategories = m_pModularSkinMasterObject->pblock2->Count( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST );
	//for ( int skinnedMeshCategoryId = 0; skinnedMeshCategoryId < numSkinnedMeshCategories; ++skinnedMeshCategoryId )
	//{
	//	INode *pCategoryNode = NULL;
	//	m_pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SKINNED_MESH_CATEGORIES_LIST, 0, pCategoryNode, FOREVER, skinnedMeshCategoryId );

	//	INode *pSelectedSkinnedMeshNode = NULL;
	//	m_pModularSkinMasterObject->pblock2->GetValue( ICModularSkinMasterObject::PI_SELECTED_SKINNED_MESH_LIST, 0, pSelectedSkinnedMeshNode, FOREVER, skinnedMeshCategoryId );

	//	// get sorted id
	//	int sortedCategoryId = categoryNodes.find_index( pCategoryNode );

	//	modularSkinCategoryInfoMap.insert( skinnedMeshCategoryId, ModularSkinCategoryInfo( pCategoryNode, pSelectedSkinnedMeshNode, sortedCategoryId ) );
	//}

	m_pModularSkinMasterObject->UpdateSkinnedMeshHiddenState();

	delete this; 
}

//bool ICModularSkinMasterObject::NodeNameComparator::compare(const void *& a, const void *& b)
//{
//	void *&pNonConstVoidA = const_cast<void *&>( a );
//	void *&pNonConstVoidB = const_cast<void *&>( b );
//	INode *&pNonConstA = ( INode *& )( pNonConstVoidA );
//	INode *&pNonConstB = ( INode *& )( pNonConstVoidB );
//
//	return _stricmp( pNonConstA->GetName(), pNonConstB->GetName() ) < 0;
//}

Point3 ICModularSkinMasterObject::GetGizmoBoxMin()
{
	Point4 minBox;
	pblock2->GetValue( PI_MIN_BOX, 0, minBox, FOREVER );
	return Point3( minBox[0], minBox[1], minBox[2] );
}

Point3 ICModularSkinMasterObject::GetGizmoBoxMax()
{
	Point4 maxBox;
	pblock2->GetValue( PI_MAX_BOX, 0, maxBox, FOREVER );
	return Point3( maxBox[0], maxBox[1], maxBox[2] );
}

void ICModularSkinMasterObject::SetGizmoBoxMin( const Point3 &p )
{
	Point4 p4( p, 1.f );
	pblock2->SetValue( PI_MIN_BOX, 0, p4 );
}

void ICModularSkinMasterObject::SetGizmoBoxMax( const Point3 &p )
{
	Point4 p4( p, 1.f );
	pblock2->SetValue( PI_MAX_BOX, 0, p4 );
}

Box3 ICModularSkinMasterObject::ComputeGizmoBox( TimeValue t, INode *pNode )
{
	return Box3( GetGizmoBoxMin(), GetGizmoBoxMax() );
}

Box3 ICModularSkinMasterObject::ComputeCategoryLinksBox( TimeValue t, INode *pNode )
{
	const int numLinkVertices = pblock2->Count( PI_SKINNED_MESH_CATEGORIES_LIST );
	const int k_numElements = 3;

	if ( numLinkVertices <= 0 )
	{
		return Box3( Point3( 0.f, 0.f, 0.f ), Point3( 0.f, 0.f, 0.f ) );
	}
	else
	{
		const Matrix3 &nodeTm = pNode->GetObjectTM( t );
		const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

		Point3 linksMin( FLT_MAX, FLT_MAX, FLT_MAX );
		Point3 linksMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		for ( int vertexId = 0; vertexId < numLinkVertices; ++vertexId )
		{
			INode *pNode = GetSkinnedMeshCategoryNode( vertexId );
			if ( !pNode )
			{
				continue;
			}

			Matrix3 currentLinkNodeTm = pNode->GetObjTMBeforeWSM( t );
			const Point3 &currentLinkTranslation = currentLinkNodeTm.GetRow( 3 );

			const Point3 p = currentLinkTranslation - nodeTranslation;
			for ( int elementId = 0; elementId < k_numElements; ++elementId )
			{
				linksMin[elementId] = min( linksMin[elementId], p[elementId] );
				linksMax[elementId] = max( linksMax[elementId], p[elementId] );
			}
		}

		return Box3( linksMin, linksMax );
	}
}



Box3 ICModularSkinMasterObject::ComputeSkinnedMeshLinksBox( TimeValue t, INode *pNode )
{
	const int numLinkVertices = pblock2->Count( PI_SKINNED_MESH_LIST );
	const int k_numElements = 3;

	if ( numLinkVertices <= 0 )
	{
		return Box3( Point3( 0.f, 0.f, 0.f ), Point3( 0.f, 0.f, 0.f ) );
	}
	else
	{
		const Matrix3 &nodeTm = pNode->GetObjectTM( t );
		const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

		Point3 linksMin( FLT_MAX, FLT_MAX, FLT_MAX );
		Point3 linksMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		for ( int vertexId = 0; vertexId < numLinkVertices; ++vertexId )
		{
			INode *pNode = NULL;
			pblock2->GetValue( PI_SKINNED_MESH_LIST, 0, pNode, FOREVER, vertexId );

			Matrix3 currentLinkNodeTm = pNode->GetObjTMBeforeWSM( t );
			const Point3 &currentLinkTranslation = currentLinkNodeTm.GetRow( 3 );

			const Point3 p = currentLinkTranslation - nodeTranslation;
			for ( int elementId = 0; elementId < k_numElements; ++elementId )
			{
				linksMin[elementId] = min( linksMin[elementId], p[elementId] );
				linksMax[elementId] = max( linksMax[elementId], p[elementId] );
			}
		}

		return Box3( linksMin, linksMax );
	}
}

bool ICModularSkinMasterObject::IsDisplayLinksEnabled()
{
	int val = 0;
	pblock2->GetValue( ICModularSkinMasterObject::PI_DISPLAY_LINKS, 0, val, FOREVER );
	return val != 0;
}

bool ICModularSkinMasterObject::HideUnselectedSkinsEnabled()
{
	int val = 0;
	pblock2->GetValue( ICModularSkinMasterObject::PI_HIDE_UNSELECTED_SKINS, 0, val, FOREVER );
	return val != 0;
}

void ICModularSkinMasterObject::SkinnedMeshSelectionRestoreObj::Restore( int isUndo )
{
	//if ( isUndo )
	//{ 
	//	rtimes = m_pModularSkinMaster->llist; 
	//}
	//m_pModularSkinMaster->llist=utimes;
	//if ( m_pModularSkinMaster->ip )
	//{
	//	m_pModularSkinMaster->SetUpLifeList();
	//}
}

void ICModularSkinMasterObject::SkinnedMeshSelectionRestoreObj::Redo() 
{
	//m_pModularSkinMaster->llist = rtimes;
	//if ( m_pModularSkinMaster->ip )
	//{
	//	m_pModularSkinMaster->SetUpLifeList();
	//}
}

int ICModularSkinMasterObject::GetSkinnedMeshCategoryNodeId( INode *pNode )
{
	int categoryNodeId = -1;

	for ( int id = 0; id < GetSkinnedMeshCategoryNodeCount(); ++id )
	{
		if ( pNode == GetSkinnedMeshCategoryNode( id ) )
		{
			categoryNodeId = id;
			break;
		}
	}

	return categoryNodeId;
}

int ICModularSkinMasterObject::GetSkinnedMeshCategoryNodeId( ICModularSkinCategoryObject *pObject )
{
	int categoryNodeId = -1;

	for ( int id = 0; id < GetSkinnedMeshCategoryNodeCount(); ++id )
	{
		if ( pObject == GetSkinnedMeshCategoryNode( id )->GetObjectRef() )
		{
			categoryNodeId = id;
			break;
		}
	}

	return categoryNodeId;
}

INT_PTR ICModularSkinMasterObject::CategoriesDlgProc::DlgProc( TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG: 
		{
			return TRUE;
		}
		//default:
		//{
		//	//if ( m_pWidget->DialogProc( hWnd, message, wParam, lParam ) )
		//	//{
		//	//	fm::pvector<INode> &selectedNodes = GetIrrColladaPtr()->GetSelectedNodes();
		//	//	SetDataChunkFromWidgetValue( hWnd, selectedNodes );
		//	//	break;
		//	//}
		//	return FALSE;
		//}
	}
	return FALSE;
}

void ICModularSkinMasterObject::NotificationCallback( void *pParam, NotifyInfo *pInfo )
{
	ICModularSkinMasterObject *pModularSkinMasterObject = ( ICModularSkinMasterObject * ) pParam;
	switch ( pInfo->intcode )
	{
		case NOTIFY_SYSTEM_PRE_RESET :
		case NOTIFY_SYSTEM_PRE_NEW :
		case NOTIFY_FILE_PRE_OPEN :
		case NOTIFY_SYSTEM_SHUTDOWN :
		{
			pModularSkinMasterObject->SetIsClosingFile( true );
			break;
		}
	}
}