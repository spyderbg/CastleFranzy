/**********************************************************************
 *<
	FILE: ModularSkinCategory.cpp

	DESCRIPTION: A simple planar deflector

	CREATED BY: Rolf Berteig
			

	HISTORY: Peter Watje 3-15-00

 *>	Copyright (c) 200, All Rights Reserved.
 **********************************************************************/

#include "StdAfx.h"
#include "ICBillboardHelper.h"
#include "ICModifierBillboard.h"
#include "ControllerExporter.h"
#include "ColladaModifiers.h"
#include "FUtils/FUParameter.h"
#include "MAXHelpers.h"
#include "FUtils/FUDaeEnum.h"

static ICBillboardHelperClassDesc modularSkinCategoryDesc;
ClassDesc* GetICBillboardHelperDesc() {return &modularSkinCategoryDesc;}

ICBillboardHelper::BillboardListPBValidator ICBillboardHelper::s_billboardListValidator;
ICBillboardHelper::BillboardListPBAccessor ICBillboardHelper::s_billboardListAccessor;

ICBillboardHelper::BillboardControllerPBAccessor ICBillboardHelper::s_billboardControllerAccessor;

const Point3 ICBillboardHelper::k_gizmoColor( 1.f, 1.f, 0.f );
const Point3 ICBillboardHelper::k_linksColor( 1.f, 0.f, 1.f );
const Point3 ICBillboardHelper::k_frontVectorColor( 0.0f, 0.0f, 1.f );
const Point3 ICBillboardHelper::k_upVectorColor( 0.0f, 1.0f, 0.f );

MCHAR *ICBillboardHelper::k_frontVectorSubObjectName = _T( "Front Vector" );
MCHAR *ICBillboardHelper::k_upVectorSubObjectName = _T( "Up Vector" );

MoveModBoxCMode *ICBillboardHelper::s_pMoveModBox = NULL;
RotateModBoxCMode *ICBillboardHelper::s_pRotateModBox = NULL;
UScaleModBoxCMode *ICBillboardHelper::s_pUScaleModBox = NULL;
NUScaleModBoxCMode *ICBillboardHelper::s_pNUScaleModBox = NULL;
SquashModBoxCMode *ICBillboardHelper::s_pSquashModBox = NULL;
SelectModBoxCMode *ICBillboardHelper::s_pSelectModBox = NULL;

static ParamBlockDesc2 s_billboardHelperParamBlockDesc ( 
	
	ICBillboardHelper::PB_MAIN_ID
	, _T("BillboardHelperParameters")
	,  0
	, &modularSkinCategoryDesc
	, P_AUTO_CONSTRUCT + P_AUTO_UI + P_MULTIMAP + P_VERSION
	, 0
	, ICBillboardHelper::PB_MAIN_REF_ID
	
	//rollout
	, ICBillboardHelper::R_COUNT
	, ICBillboardHelper::R_DISPLAY_OPTIONS, IDD_BILLBOARD_HELPER_DISPLAY_OPTIONS, IDS_MODULAR_SKIN_DISPLAY_OPTIONS_ROLLUP_LABEL, 0, 0, NULL
	, ICBillboardHelper::R_BILLBOARDS, IDD_MODULAR_SKIN_CATEGORY_OBJECT, IDS_BILLBOARD_LIST_ROLLUP_LABEL, 0, 0, NULL
	, ICBillboardHelper::R_BILLBOARD_CONTROLLER, IDD_BILLBOARD_CONTROLLER_PARAMS, IDS_BILLBOARD_CONTROLLER_ROLLUP_LABEL, 0, 0, NULL

	// display rollup
	, ICBillboardHelper::PI_DISPLAY_LINKS, _T("display_links"), TYPE_INT, 0, IDS_MODULAR_SKIN_DISPLAY_LINKS
		, p_ui, ICBillboardHelper::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_BILLBOARD_HELPER_DISPLAY_LINKS_CHECKBOX
		, p_range, 0, 1
		, p_default, 1
	, end

	, ICBillboardHelper::PI_DISPLAY_BILLBOARDED_TARGETS, _T("display_billboarded_targets"), TYPE_INT, 0, IDS_BILLBOARD_DISPLAY_BILLBOARDED_TARGETS
		, p_ui, ICBillboardHelper::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_BILLBOARD_HELPER_DISPLAY_BILLBOARDED_TARGETS_CHECKBOX
		, p_accessor, &ICBillboardHelper::s_billboardControllerAccessor
		, p_range, 0, 1
		, p_default, 1
	, end

	, ICBillboardHelper::PI_DISPLAY_VECTORS_ON_TARGETS, _T("display_vectors_on_targets"), TYPE_INT, 0, IDS_BILLBOARD_DISPLAY_VECTORS_ON_TARGETS
		, p_ui, ICBillboardHelper::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_BILLBOARD_HELPER_DISPLAY_VECTORS_ON_TARGETS_CHECKBOX
		, p_range, 0, 1
		, p_default, 0
	, end

	// skinned meshes rollup
	, ICBillboardHelper::PI_BILLBOARD_NODE_LIST, _T("BillboardList"), TYPE_INODE_TAB, 0, P_VARIABLE_SIZE, IDS_BILLBOARD_NODE_LIST
		, p_ui, ICBillboardHelper::R_BILLBOARDS, TYPE_NODELISTBOX, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_PICK_BUTTON
			, NULL
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_REMOVE_BUTTON
		, p_validator, &ICBillboardHelper::s_billboardListValidator
		, p_accessor, &ICBillboardHelper::s_billboardListAccessor
	, end

	// billboard controller
	, ICBillboardHelper::PI_BILLBOARD_TYPE, _T("Billboard Type"), TYPE_INT, P_RESET_DEFAULT, IDS_BILLBOARD_TYPE
		, p_default, 		FUDaeBillboard::BT_FREE
		, p_range, 			FUDaeBillboard::BT_FREE, FUDaeBillboard::BT_UP_ONLY
		, p_ui, 			ICBillboardHelper::R_BILLBOARD_CONTROLLER, TYPE_RADIO, FUDaeBillboard::BT_COUNT
			, IDC_BILLBOARD_TYPE_FREE_RADIOBUTTON
			, IDC_BILLBOARD_TYPE_ROTATION_RADIOBUTTON
			, IDC_BILLBOARD_TYPE_UP_ONLY_RADIOBUTTON
		, p_accessor, &ICBillboardHelper::s_billboardControllerAccessor
	, end

	, ICBillboardHelper::PI_BILLBOARD_ORIENTATION, _T("Billboard Orientation"), TYPE_INT, P_RESET_DEFAULT, IDS_BILLBOARD_ORIENTATION
		, p_default, 		FUDaeBillboard::OT_TOWARD_CAMERA
		, p_range, 			FUDaeBillboard::OT_TOWARD_CAMERA, FUDaeBillboard::OT_PARALLEL_CAM_AXIS
		, p_ui, 			ICBillboardHelper::R_BILLBOARD_CONTROLLER, TYPE_RADIO, FUDaeBillboard::OT_COUNT
			, IDC_BILLBOARD_ORIENTATION_TOWARD_CAMERA_RADIOBUTTON
			, IDC_BILLBOARD_ORIENTATION_PARALLEL_CAM_AXIS_RADIOBUTTON
		, p_accessor, &ICBillboardHelper::s_billboardControllerAccessor
	, end

	// billboard front vector ( vector ui version )
	, ICBillboardHelper::PI_BILLBOARD_FRONT, _T("Billboard Front Vector"), TYPE_POINT3, P_ANIMATABLE | P_RESET_DEFAULT, IDS_BILLBOARD_FRONT_VECTOR
		, p_default, 		Point3( 0.f, 0.f, 1.f )
		, p_range, 			-FLT_MAX, FLT_MAX
		, p_ui, 			ICBillboardHelper::R_BILLBOARD_CONTROLLER, TYPE_SPINNER, EDITTYPE_FLOAT
			, IDC_BILLBOARD_FRONT_VECTOR_X_EDITTEXT, IDC_BILLBOARD_FRONT_VECTOR_X_SPIN
			, IDC_BILLBOARD_FRONT_VECTOR_Y_EDITTEXT, IDC_BILLBOARD_FRONT_VECTOR_Y_SPIN
			, IDC_BILLBOARD_FRONT_VECTOR_Z_EDITTEXT, IDC_BILLBOARD_FRONT_VECTOR_Z_SPIN
			, SPIN_AUTOSCALE
		, p_accessor, &ICBillboardHelper::s_billboardControllerAccessor
	, end

	// billboard up vector ( vector ui version )
	, ICBillboardHelper::PI_BILLBOARD_UP, _T("Billboard Up Vector"), TYPE_POINT3, P_ANIMATABLE | P_RESET_DEFAULT, IDS_BILLBOARD_UP_VECTOR
		, p_default, 		Point3( 0.f, 1.f, 0.f )
		, p_range, 			-FLT_MAX, FLT_MAX
		, p_ui, 			ICBillboardHelper::R_BILLBOARD_CONTROLLER, TYPE_SPINNER, EDITTYPE_UNIVERSE
			, IDC_BILLBOARD_UP_VECTOR_X_EDITTEXT, IDC_BILLBOARD_UP_VECTOR_X_SPIN
			, IDC_BILLBOARD_UP_VECTOR_Y_EDITTEXT, IDC_BILLBOARD_UP_VECTOR_Y_SPIN
			, IDC_BILLBOARD_UP_VECTOR_Z_EDITTEXT, IDC_BILLBOARD_UP_VECTOR_Z_SPIN
			, SPIN_AUTOSCALE
		, p_accessor, &ICBillboardHelper::s_billboardControllerAccessor
	, end

	// box gizmo
	, ICBillboardHelper::PI_MIN_BOX, _T("min_box"), TYPE_POINT4, 0, IDS_MODULAR_SKIN_MIN_BOX, end
	, ICBillboardHelper::PI_MAX_BOX, _T("max_box"), TYPE_POINT4, 0, IDS_MODULAR_SKIN_MAX_BOX, end

	, end
);

//--- ICBillboardHelper Parameter map/block descriptors ------------------
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

//--- ModularSkinCategory object methods -----------------------------------------
ICBillboardHelper::ICBillboardHelper()
: m_frontVectorSubObject( 13 )
, m_upVectorSubObject( 13 )
{
	pblock2 = NULL;
	modularSkinCategoryDesc.MakeAutoParamBlocks(this);
	assert(pblock2);

	m_frontVectorSubObject.SetName( k_frontVectorSubObjectName );
	m_upVectorSubObject.SetName( k_upVectorSubObjectName );

	RegisterNotification( &ICBillboardHelper::NotificationCallback, this, NOTIFY_SCENE_PRE_DELETED_NODE );
}

RefTargetHandle ICBillboardHelper::Clone(RemapDir& remap) 
{
	ICBillboardHelper* newob = new ICBillboardHelper();	

	CLONE_PARAMETER( newob, m_gizmoMesh );
	CLONE_PARAMETER( newob, m_linksMesh );
	CLONE_PARAMETER( newob, m_frontVectorMesh );
	CLONE_PARAMETER( newob, m_upVectorMesh );
	CLONE_PARAMETER( newob, m_previousBillboardNodeList );

	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	newob->ClearBillboardNodes();

	BaseClone(this, newob, remap);
	return newob;
}

void ICBillboardHelper::BeginEditParams( IObjParam *ip, ULONG flags, Animatable *prev )
{
	// deleting invalid nodes whose billboard controller has been removed
	{
		int billboardNodeCount = GetBillboardNodeCount();
		NodeList invalidBillboardNodes;
		for ( int billboardId = 0; billboardId < billboardNodeCount; ++billboardId )
		{
			INode *pNode = GetBillboardNode( billboardId );

#if ENABLE_BILLBOARD_CONTROLLERS
			Control *pPRSController = ControllerExporter::FindController( pNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
			if ( !pPRSController ||
				!pPRSController->GetRotationController() ||
				pPRSController->GetRotationController()->ClassID() != BILLBOARD_CONTROLLER_CLASS_ID )
			{
				invalidBillboardNodes.push_back( pNode );
			}
#else
#endif // ENABLE_BILLBOARD_CONTROLLERS
		}

		for ( NodeList::iterator nodeIter = invalidBillboardNodes.begin(); nodeIter != invalidBillboardNodes.end(); ++nodeIter )
		{
			// For some reason, cannot seem to get undo to work here ( using theHold )
			DeleteBillboardNode( *nodeIter );
		}
	}

	modularSkinCategoryDesc.BeginEditParams( ip, this, flags, prev );

	// sub object selection
	{
		s_pMoveModBox = new MoveModBoxCMode( this, ip );
		s_pRotateModBox = new RotateModBoxCMode( this, ip );
		s_pUScaleModBox = new UScaleModBoxCMode( this, ip );
		s_pNUScaleModBox = new NUScaleModBoxCMode( this, ip );
		s_pSquashModBox = new SquashModBoxCMode( this, ip );
		s_pSelectModBox = new SelectModBoxCMode( this, ip );

		const MCHAR *types[] = 
		{ 
			k_frontVectorSubObjectName
			, k_upVectorSubObjectName
		};

		ip->RegisterSubObjectTypes( types, 2 );
	}
}

void ICBillboardHelper::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{		
	modularSkinCategoryDesc.EndEditParams( ip, this, flags, next );

	ip->DeleteMode( s_pMoveModBox );
	SAFE_DELETE( s_pMoveModBox );

	ip->DeleteMode( s_pRotateModBox );
	SAFE_DELETE( s_pRotateModBox );

	ip->DeleteMode( s_pUScaleModBox );
	SAFE_DELETE( s_pUScaleModBox );

	ip->DeleteMode( s_pNUScaleModBox );
	SAFE_DELETE( s_pNUScaleModBox );

	ip->DeleteMode( s_pSquashModBox );
	SAFE_DELETE( s_pSquashModBox );

	ip->DeleteMode( s_pSelectModBox );
	SAFE_DELETE( s_pSelectModBox );
}

void ICBillboardHelper::TransformToDisplayedVector( Point3 &v, ViewExp *vpt )
{
	v = v.Normalize();
}

void ICBillboardHelper::BuildVectorMesh( TimeValue t, ViewExp *vpt, Mesh &mesh, const Point3 &vector )
{
	// vertices
	{
		Point3 transformedVector = vector;
		TransformToDisplayedVector( transformedVector, vpt );

		const int numVertex = 14;
		mesh.setNumVerts( numVertex );

		const Point3 xAxis( 1.0f, 0.0f, 0.0f );
		const Point3 yAxis( 0.0f, 1.0f, 0.0f );

		Point3 upVector;
		FMVector3 transformedVectorFM( ( float * )&transformedVector );
		FMVector3 xAxisFM( ( float * )&xAxis );
		if ( IsEquivalent( transformedVectorFM, xAxisFM ) )
		{
			upVector = yAxis;
		}
		else
		{
			upVector = xAxis ^ transformedVector;
		}

		upVector = upVector.Normalize();
		Point3 frontVector = upVector ^ transformedVector;

		const float arrowHalfWidth = 0.1f;
		int currentVert = 0;

		{
			mesh.setVert( currentVert++, transformedVector );
			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) - ( upVector * arrowHalfWidth ) );
			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) + ( upVector * arrowHalfWidth ) );

			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) - ( upVector * ( arrowHalfWidth / 3.f ) ) );
			mesh.setVert( currentVert++, Point3( 0.f, 0.f, 0.f ) - ( upVector * ( arrowHalfWidth / 3.f ) ) );
			mesh.setVert( currentVert++, Point3( 0.f, 0.f, 0.f ) + ( upVector * ( arrowHalfWidth / 3.f ) ) );
			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) + ( upVector * ( arrowHalfWidth / 3.f ) ) );
		}

		{
			mesh.setVert( currentVert++, transformedVector );
			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) - ( frontVector * arrowHalfWidth ) );
			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) + ( frontVector * arrowHalfWidth ) );

			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) - ( frontVector * ( arrowHalfWidth / 3.f ) ) );
			mesh.setVert( currentVert++, Point3( 0.f, 0.f, 0.f ) - ( frontVector * ( arrowHalfWidth / 3.f ) ) );
			mesh.setVert( currentVert++, Point3( 0.f, 0.f, 0.f ) + ( frontVector * ( arrowHalfWidth / 3.f ) ) );
			mesh.setVert( currentVert++, ( transformedVector * 2.f / 3.f ) + ( frontVector * ( arrowHalfWidth / 3.f ) ) );
		}
	}

	// faces
	{
		const int k_numFaces = 6;
		mesh.setNumFaces( k_numFaces );
		
		int currentFace = 0;

		{
			mesh.faces[currentFace].setVerts( 0, 1, 2 );
			mesh.faces[currentFace++].setEdgeVisFlags( 1, 1, 1 );

			mesh.faces[currentFace].setVerts( 6, 3, 4 );
			mesh.faces[currentFace++].setEdgeVisFlags( 1, 1, 0 );

			mesh.faces[currentFace].setVerts( 4, 5, 6 );
			mesh.faces[currentFace++].setEdgeVisFlags( 1, 1, 0 );
		}
		
		{
			mesh.faces[currentFace].setVerts( 7, 8, 9 );
			mesh.faces[currentFace++].setEdgeVisFlags( 1, 1, 1 );

			mesh.faces[currentFace].setVerts( 13, 10, 11 );
			mesh.faces[currentFace++].setEdgeVisFlags( 1, 1, 0 );

			mesh.faces[currentFace].setVerts( 11, 12, 13 );
			mesh.faces[currentFace++].setEdgeVisFlags( 1, 1, 0 );
		}
	}

	mesh.InvalidateGeomCache();
}

void ICBillboardHelper::BuildVectorMeshes( TimeValue t, ViewExp *vpt )
{
	Point3 frontVector;
	GetFrontVector( t, frontVector );
	BuildVectorMesh( t, vpt, m_frontVectorMesh, frontVector );

	Point3 upVector;
	GetUpVector( t, upVector );
	BuildVectorMesh( t, vpt, m_upVectorMesh, upVector );
}

void ICBillboardHelper::BuildMesh( TimeValue t, INode *pNode, ViewExp *vpt )
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
			}
		}

		m_gizmoMesh.InvalidateGeomCache();
	}

	// links
	if ( IsDisplayLinksEnabled() )
	{
		int numLinks = 0;
		int billboardNodeCount = GetBillboardNodeCount();
		for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
		{
			INode *pNode = GetBillboardNode( billboardNodeId );
			if ( pNode && !pNode->IsHidden() )
			{
				++numLinks;
			}
		}

		if ( numLinks <= 0 )
		{
			m_linksMesh.setNumVerts( 0 );
			m_linksMesh.setNumFaces( 0 );		
		}
		else
		{
			const int linksNumVerts = numLinks + 1;
			const int linksNumFaces = numLinks;

			m_linksMesh.setNumVerts( linksNumVerts );
			m_linksMesh.setNumFaces( linksNumFaces );

			const Matrix3 &nodeTm = GetObjectTM( pNode, t );
			const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

			const int centerVertexId = 0;
			m_linksMesh.setVert( centerVertexId, Point3( 0.0f, 0.0f, 0.0f ) );

			int currentVertex = centerVertexId + 1;
			int billboardNodeCount = GetBillboardNodeCount();
			int currentLinkId = 0;
			for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
			{
				INode *pNode = GetBillboardNode( billboardNodeId );
				if ( !pNode || pNode->IsHidden() )
				{
					continue;
				}
				
				Matrix3 currentLinkNodeTm = pNode->GetObjTMBeforeWSM( t );
				const Point3 &currentLinkTranslation = currentLinkNodeTm.GetRow( 3 );

				m_linksMesh.setVert( currentVertex++, currentLinkTranslation - nodeTranslation );
				m_linksMesh.faces[currentLinkId].setEdgeVisFlags( 1, 1, 1 );
				m_linksMesh.faces[currentLinkId].setVerts( centerVertexId, currentVertex - 1, centerVertexId );

				currentLinkId++;
			}
		}

		m_linksMesh.InvalidateGeomCache();
	}

	// billboard vectors
	BuildVectorMeshes( t, vpt );
}

void ICBillboardHelper::GetLinksTM( const Matrix3 &objectTM, Matrix3 &tm )
{
	tm.IdentityMatrix();
	tm.SetTrans( objectTM.GetTrans() );
}

void ICBillboardHelper::GetVectorsGizmoTM( ViewExp *vpt, const Matrix3 &objectTM, Matrix3 &tm )
{
	tm.IdentityMatrix();
	float screenScaleFactor = vpt->GetScreenScaleFactor( objectTM.GetTrans() );
	screenScaleFactor /= 5;
	tm.SetScale( Point3( screenScaleFactor, screenScaleFactor, screenScaleFactor ) );
	tm.SetTrans( objectTM.GetTrans() );
	//float vpWorldWidth = vpt->GetVPWorldWidth( tm.GetTrans() );
}

void ICBillboardHelper::GetBillboardTargetVectorsTM( ViewExp *vpt, const Matrix3 &objectTM, Matrix3 &tm )
{
	tm = objectTM;
	tm.NoScale();
	tm.NoTrans();
	
	float screenScaleFactor = vpt->GetScreenScaleFactor( objectTM.GetTrans() );
	screenScaleFactor /= 5;
	tm.Scale( Point3( screenScaleFactor, screenScaleFactor, screenScaleFactor ) );
	tm.SetTrans( objectTM.GetTrans() );
}

Matrix3 ICBillboardHelper::GetObjectTM( INode *pNode, TimeValue t )
{
	if ( !pNode )
	{
		return Matrix3( 1 );
	}


	Matrix3 objectTM = pNode->GetObjectTM( t );
	objectTM.NoRot();
	objectTM.NoScale();

	return objectTM;
}

int ICBillboardHelper::HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt )
{
	GraphicsWindow *pGraphicsWindow = vpt->getGW();

	HitRegion hr; 
	MakeHitRegion( hr, type, crossing, 4, p ); 
	Matrix3 objectTM = GetObjectTM( inode, t );

	// gizmo
	//{
	//	pGraphicsWindow->setTransform( objectTM );
	//	if ( m_gizmoMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
	//	{
	//		return TRUE;
	//	}
	//}

	// links
	if ( IsDisplayLinksEnabled() )
	{
		if ( m_linksMesh.getNumVerts() > 0 )
		{
			Matrix3 tm; 
			GetLinksTM( objectTM, tm );
			pGraphicsWindow->setTransform( tm );

			if ( m_linksMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
			{
				return TRUE;
			}
		}
	}

	// billboard vectors
	{
		Matrix3 tm; 
		GetVectorsGizmoTM( vpt, objectTM, tm );
		pGraphicsWindow->setTransform( tm );
		if ( m_frontVectorMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
		{
			return TRUE;
		}
		if ( m_upVectorMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
		{
			return TRUE;
		}
	}

	if ( IsDisplayVectorsOnTargetsEnabled() )
	{
		int billboardNodeCount = GetBillboardNodeCount();
		for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
		{
			INode *pNode = GetBillboardNode( billboardNodeId );
			if ( !pNode || pNode->IsHidden() )
			{
				continue;
			}

			//Matrix3 currentObjectTM = pNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pNode->GetNodeTM( t );
			Matrix3 tm;
			GetBillboardTargetVectorsTM( vpt, currentObjectTM, tm );
			pGraphicsWindow->setTransform( tm );
			if ( m_frontVectorMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
			{
				return TRUE;
			}
			if ( m_upVectorMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int ICBillboardHelper::Display( TimeValue t, INode* inode, ViewExp *vpt, int flags )
{
	BuildMesh( t, inode, vpt );

	GraphicsWindow *pGraphicsWindow = vpt->getGW();

	// render flags
	DWORD rlim  = pGraphicsWindow->getRndLimits();
	pGraphicsWindow->setRndLimits( /*GW_WIREFRAME | GW_EDGES_ONLY | *//*GW_BACKCULL|*/ ( rlim & GW_Z_BUFFER ) );  // removed BC on 4/29/99
	
	// transform
	const Matrix3 objectTM = GetObjectTM( inode, t );

	// vertex color
	//if ( inode->Selected() )
	//{
	//	pGraphicsWindow->setColor( LINE_COLOR, GetSelColor() );
	//}
	//else if( !inode->IsFrozen() )
	//{
		
	//}

	// display gizmo mesh
	//{
	//	pGraphicsWindow->setTransform( objectTM );
	//	pGraphicsWindow->setColor( LINE_COLOR, inode->Selected() ? GetSelColor() : k_gizmoColor );
	//	m_gizmoMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
	//}

	// display links mesh
	if ( IsDisplayLinksEnabled() )
	{
		Matrix3 tm;
		GetLinksTM( objectTM, tm );
		pGraphicsWindow->setTransform( tm );
		
		pGraphicsWindow->setColor( LINE_COLOR, inode->Selected() ? GetSelColor() : k_linksColor );
		m_linksMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
	}

	// Display billboard vectors
	{
		Matrix3 tm;
		GetVectorsGizmoTM( vpt, objectTM, tm );
		pGraphicsWindow->setTransform( tm );
		int subObjectLevel = GetSubObjectLevel();

		{
			Point3 color = ( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :
				(( inode->Selected() && subObjectLevel == 1 ) ? GetSubSelColor() :  
				(( inode->Selected() && subObjectLevel != 1 ) ? GetFreezeColor() :  
				k_frontVectorColor ));

			pGraphicsWindow->setColor( LINE_COLOR, color );
			m_frontVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
		}

		{
			Point3 color = ( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :
				(( inode->Selected() && subObjectLevel == 2 ) ? GetSubSelColor() :  
				(( inode->Selected() && subObjectLevel != 2 ) ? GetFreezeColor() :  
				k_upVectorColor ));

			pGraphicsWindow->setColor( LINE_COLOR, color );
			m_upVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
		}
	}

	// vectors on targets
	if ( IsDisplayVectorsOnTargetsEnabled() )
	{
		int billboardNodeCount = GetBillboardNodeCount();
		for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
		{
			INode *pNode = GetBillboardNode( billboardNodeId );
			if ( !pNode || pNode->IsHidden() )
			{
				continue;
			}

			//Matrix3 currentObjectTM = pNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pNode->GetNodeTM( t );
			Matrix3 tm;
			GetBillboardTargetVectorsTM( vpt, currentObjectTM, tm );
			//tm->
			pGraphicsWindow->setTransform( tm );
			int subObjectLevel = GetSubObjectLevel();

			{
				Point3 color = ( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :
					(( inode->Selected() && subObjectLevel == 1 ) ? GetSubSelColor() :  
					(( inode->Selected() && subObjectLevel != 1 ) ? GetFreezeColor() :  
					k_frontVectorColor ));

				pGraphicsWindow->setColor( LINE_COLOR, color );
				m_frontVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
			}

			{
				Point3 color = ( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :
					(( inode->Selected() && subObjectLevel == 2 ) ? GetSubSelColor() :  
					(( inode->Selected() && subObjectLevel != 2 ) ? GetFreezeColor() :  
					k_upVectorColor ));

				pGraphicsWindow->setColor( LINE_COLOR, color );
				m_upVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
			}
		}
	}

	pGraphicsWindow->setRndLimits( rlim );

	return 0;
}

void ICBillboardHelper::Snap( TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt )
{
	return;
}

Interval ICBillboardHelper::ObjectValidity( TimeValue t )
{ 
	Interval validityInterval = FOREVER;
	
	if ( pblock2 )
	{
		pblock2->GetValidity( t, validityInterval );
	}

	return validityInterval;
}

ObjectState ICBillboardHelper::Eval( TimeValue time )
{ 
	return ObjectState( this );
}

void ICBillboardHelper::GetWorldBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	Matrix3 objectTM = GetObjectTM( pNode, t );
	const int k_numBoxVertices = 8;
	const int k_numElements = 3;

	Box3 gizmoBox = ComputeGizmoBox( t, pNode );
	Box3 linksBox = ComputeLinksBox( t, pNode );

	Point3 worldMin( FLT_MAX, FLT_MAX, FLT_MAX );
	Point3 worldMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	// gizmo
	//{
	//	for ( int vertexId = 0; vertexId < k_numBoxVertices; ++vertexId )
	//	{
	//		Point3 p = objectTM.PointTransform( gizmoBox[vertexId] );
	//		for ( int elementId = 0; elementId < k_numElements; ++elementId )
	//		{
	//			worldMin[elementId] = min( worldMin[elementId], p[elementId] );
	//			worldMax[elementId] = max( worldMax[elementId], p[elementId] );
	//		}
	//	}
	//}

	// links
	if ( IsDisplayLinksEnabled() )
	{
		Matrix3 tm;
		GetLinksTM( objectTM, tm );

		for ( int vertexId = 0; vertexId < k_numBoxVertices; ++vertexId )
		{
			Point3 p = tm.PointTransform( linksBox[vertexId] );
			for ( int elementId = 0; elementId < k_numElements; ++elementId )
			{
				worldMin[elementId] = min( worldMin[elementId], p[elementId] );
				worldMax[elementId] = max( worldMax[elementId], p[elementId] );
			}
		}
	}	

	// billboard vectors
	{
		Matrix3 tm;
		GetVectorsGizmoTM( vpt, objectTM, tm );

		BuildVectorMeshes( t, vpt );

		m_frontVectorMesh.buildBoundingBox();
		Box3 frontVectorBoundingBox = m_frontVectorMesh.getBoundingBox( &tm );
		
		m_upVectorMesh.buildBoundingBox();
		Box3 upVectorBoundingBox = m_upVectorMesh.getBoundingBox( &tm );

		worldMin = MAXHelpers::MinComponents( worldMin, frontVectorBoundingBox.Min() );
		worldMin = MAXHelpers::MinComponents( worldMin, upVectorBoundingBox.Min() );

		worldMax = MAXHelpers::MaxComponents( worldMax, frontVectorBoundingBox.Max() );
		worldMax = MAXHelpers::MaxComponents( worldMax, upVectorBoundingBox.Max() );
	}

	// vectors on targets
	// not calling BuildVectorMeshes again because it has been called when determining bounding box for billboard vectors
	if ( IsDisplayVectorsOnTargetsEnabled() )
	{
		int billboardNodeCount = GetBillboardNodeCount();
		for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
		{
			INode *pCurrentNode = GetBillboardNode( billboardNodeId );
			if ( !pCurrentNode || pCurrentNode->IsHidden() )
			{
				continue;
			}

			Matrix3 tm;
			//Matrix3 currentObjectTM = pCurrentNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pCurrentNode->GetNodeTM( t );
			GetBillboardTargetVectorsTM( vpt, currentObjectTM, tm );

			BuildVectorMeshes( t, vpt );

			m_frontVectorMesh.buildBoundingBox();
			Box3 frontVectorBoundingBox = m_frontVectorMesh.getBoundingBox( &tm );
			
			m_upVectorMesh.buildBoundingBox();
			Box3 upVectorBoundingBox = m_upVectorMesh.getBoundingBox( &tm );

			worldMin = MAXHelpers::MinComponents( worldMin, frontVectorBoundingBox.Min() );
			worldMin = MAXHelpers::MinComponents( worldMin, upVectorBoundingBox.Min() );

			worldMax = MAXHelpers::MaxComponents( worldMax, frontVectorBoundingBox.Max() );
			worldMax = MAXHelpers::MaxComponents( worldMax, upVectorBoundingBox.Max() );
		}
	}

	box = Box3( worldMin, worldMax );
	return;
}

void ICBillboardHelper::GetLocalBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	Matrix3 objectTM = GetObjectTM( pNode, t );

	Point3 localMin( FLT_MAX, FLT_MAX, FLT_MAX );
	Point3 localMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

	// gizmo
	{
		Box3 gizmoBox = ComputeGizmoBox( t, pNode );
		//localMin = MAXHelpers::MinComponents( localMin, gizmoBox.Min() );
		//localMax = MAXHelpers::MaxComponents( localMin, gizmoBox.Max() );
	}

	// links
	if ( IsDisplayLinksEnabled() )
	{
		Box3 linksBox = ComputeLinksBox( t, pNode );
		localMin = MAXHelpers::MinComponents( localMin, linksBox.Min() );
		localMax = MAXHelpers::MaxComponents( localMin, linksBox.Max() );
	}

	// vector bounding boxes
	{
		BuildVectorMeshes( t, vpt );

		Matrix3 tm;
		GetVectorsGizmoTM( vpt, objectTM, tm );
		tm.SetTrans( Point3( 0.f, 0.f, 0.f ) );

		m_frontVectorMesh.buildBoundingBox();
		Box3 frontVectorBoundingBox = m_frontVectorMesh.getBoundingBox( &tm );
		localMin = MAXHelpers::MinComponents( localMin, frontVectorBoundingBox.Min() );
		localMax = MAXHelpers::MaxComponents( localMax, frontVectorBoundingBox.Max() );

		m_upVectorMesh.buildBoundingBox();
		Box3 upVectorBoundingBox = m_upVectorMesh.getBoundingBox( &tm );
		localMin = MAXHelpers::MinComponents( localMin, upVectorBoundingBox.Min() );
		localMax = MAXHelpers::MaxComponents( localMax, upVectorBoundingBox.Max() );
	}

	// vectors on targets
	// not building vector meshes or their bounding boxes because they have been previously built when determining bounding box for billboard vectors
	if ( IsDisplayVectorsOnTargetsEnabled() )
	{
		int billboardNodeCount = GetBillboardNodeCount();
		for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
		{
			INode *pCurrentNode = GetBillboardNode( billboardNodeId );
			if ( !pCurrentNode  || pCurrentNode->IsHidden() )
			{
				continue;
			}

			Matrix3 tm;
			//Matrix3 currentObjectTM = pCurrentNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pCurrentNode->GetNodeTM( t );
			GetBillboardTargetVectorsTM( vpt, currentObjectTM, tm );
			tm.SetTrans( currentObjectTM.GetTrans() - objectTM.GetTrans() );

			Box3 frontVectorBoundingBox = m_frontVectorMesh.getBoundingBox( &tm );
			localMin = MAXHelpers::MinComponents( localMin, frontVectorBoundingBox.Min() );
			localMax = MAXHelpers::MaxComponents( localMax, frontVectorBoundingBox.Max() );

			Box3 upVectorBoundingBox = m_upVectorMesh.getBoundingBox( &tm );
			localMin = MAXHelpers::MinComponents( localMin, upVectorBoundingBox.Min() );
			localMax = MAXHelpers::MaxComponents( localMax, upVectorBoundingBox.Max() );
		}
	}

	box = Box3( localMin, localMax );
}	

void ICBillboardHelper::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel )
{
	return;
}

int ICBillboardHelper::ICBillboardHelperCreateCallback::proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat )
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
				return CREATE_STOP;
			}
		}
	}
	else if ( msg == MOUSE_MOVE )
	{
		//switch( point )
		//{
		//	case MAXHelpers::CPC_SECOND:
		//	{
		//		break;
		//	}
		//}
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

CreateMouseCallBack* ICBillboardHelper::GetCreateMouseCallBack()
{
	static ICBillboardHelperCreateCallback createCB;
	createCB.ob = this;
	return &createCB;
}

// JBW:  for loading old ParamBlock versions, register an Ed. 2 param block converter post-load callback
//       This one takes the ParamBlockDescID version array as before, but now takes the 
//       ParamBlockDesc2 for the block to describe the current version and will convert the loaded ParamBlock
//       into a corresponding ParamBlock2
IOResult
ICBillboardHelper::Load(ILoad *iload)
{	
	// TODO
	//ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, NUM_OLDVERSIONS, &s_billboardHelperParamBlockDesc, this, FSDeflector::PBLOCK_REF_NO);
	//iload->RegisterPostLoadCallback(plcb);
	iload->RegisterPostLoadCallback( new ICBillboardHelperPostLoadCallback( this ) );
	return IO_OK;
}

ICBillboardHelper::~ICBillboardHelper()
{	
	UnRegisterNotification( &ICBillboardHelper::NotificationCallback, this );
}

void ICBillboardHelper::BillboardListPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICBillboardHelper *pBillboardHelper = (ICBillboardHelper*)owner;

	NodeList &previousBillboardNodeList = pBillboardHelper->GetPreviousBillboardNodeList();
	previousBillboardNodeList.clear();
	pBillboardHelper->GetCurrentBillboardNodeList( previousBillboardNodeList );
}

//#define BEGIN_HOLD() \
//{ \
//	bool __bIsHolding = theHold.Holding() == TRUE; \
//	if ( !bIsHolding ) \
//	{ \
//		theHold.Begin(); \
//	}
//
//#define END_HOLD( __undoOpName ) \
//	if ( !__bIsHolding ) \
//	{ \
//		theHold.Accept( __undoOpName ); \
//	} \
//}

void ICBillboardHelper::BillboardListPBAccessor::TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count )
{
	ICBillboardHelper *pBillboardHelper = (ICBillboardHelper*)owner;

	switch ( changeCode )
	{
		case tab_insert :
		case tab_append :
		{
			INode *pNode = (INode *)(*tab)[tabIndex].r;

#if ENABLE_BILLBOARD_CONTROLLERS
			Control *pPRSController = ControllerExporter::FindController( pNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
			FUAssert( pPRSController, break );
#else
#endif // ENABLE_BILLBOARD_CONTROLLERS

			// if node is a target of another billboard helper, remove it from that billboard manager
			{
				INodeTab billboardHelperNodes;
				MAXHelpers::GetNodesByObjectClassID( GetCOREInterface()->GetRootNode(), billboardHelperNodes, BILLBOARD_HELPER_OBJECT_CLASSID );
				for ( int nodeId = 0; nodeId < billboardHelperNodes.Count(); ++nodeId )
				{
					ICBillboardHelper *pCurrentBillboardHelper = ( ICBillboardHelper * )billboardHelperNodes[nodeId]->GetObjectRef();
					if ( pCurrentBillboardHelper == pBillboardHelper )
					{
						continue;
					}

					int currentNodeId = pCurrentBillboardHelper->GetBillboardNodeId( pNode );
					if ( currentNodeId != -1 )
					{
						pCurrentBillboardHelper->DeleteBillboardNode( currentNodeId );
						break;
					}
				}
			}

			{
				//ICBillboardHelper *pPreviousHelper = NULL;
				//{
				//	Control *pPreviousRotationController = pPRSController->GetRotationController();
				//	if ( pPreviousRotationController 
				//		&& pPreviousRotationController->ClassID() == BILLBOARD_CONTROLLER_CLASS_ID )
				//	{
				//		ICBillboardController *pBillboardController = ( ICBillboardController * )pPreviousRotationController;
				//		pPreviousHelper = pBillboardController->GetHelperPtr();
				//	}
				//}

#if ENABLE_BILLBOARD_CONTROLLERS
				ClassDesc *pBillboardControllerClassDesc = GetICBillboardControllerDesc();
				ICBillboardController *pBillboardController = ( ICBillboardController * )pBillboardControllerClassDesc->Create();
				pPRSController->SetRotationController( pBillboardController );
				pBillboardController->SetHelperPtr( pBillboardHelper );
#else
				//Object *pObject = pNode->GetObjectRef();
				//Class_ID cid = pObject->ClassID();
				//SClass_ID scid = pObject->SuperClassID();
				//int tmp = 0;

				ICBillboardWSM *pBillboardModifier = ( ICBillboardWSM * )GetMyClassDesc()->Create();
				//Modifier *pBillboardModifier = GetMyClassDesc()->Create();
				GetCOREInterface7()->AddModifier( *pNode, *pBillboardModifier );

#endif // ENABLE_BILLBOARD_CONTROLLERS

				//BEGIN_HOLD();
				//SetBillboardControllerRestoreObj pRestoreObj = new SetBillboardControllerRestoreObj( );
				//theHold.Put( pRestoreObj );
				//END_HOLD( "Set Billboard" );

				// get this objects node
				//INode *pNode = MAXHelpers::GetNodeByObjectRef( GetCOREInterface()->GetRootNode(), pBillboardHelper );
				//FUAssert( pNode, break );

				//pBillboardController->SetReference( ICBillboardController::RI_BILLBOARD_HELPER, pNode );
				//pBillboardController->SetReference( ICBillboardController::RI_BILLBOARD_HELPER, pBillboardHelper );
			}

			break;
		}

		case tab_ref_deleted :
		{
			pBillboardHelper->DeleteBillboardNode( tabIndex );
			int tmp = 0;
			break;
		}

		case tab_delete :
		{
			NodeList currentBillboardNodeList;
			NodeList &previousBillboardNodeList = pBillboardHelper->GetPreviousBillboardNodeList();
			pBillboardHelper->GetCurrentBillboardNodeList( currentBillboardNodeList );

			NodeList removedNodes;
			for ( NodeList::iterator nodeIter = previousBillboardNodeList.begin(); nodeIter != previousBillboardNodeList.end(); ++nodeIter )
			{
				INode *pPreviousNode = *nodeIter;
				if ( pPreviousNode
					&& MAXHelpers::IsNodeInScene( pPreviousNode )
					&& currentBillboardNodeList.find_index( *nodeIter ) == -1 )
				{
					removedNodes.push_back( pPreviousNode );
				}
			}

			for ( NodeList::iterator nodeIter = removedNodes.begin(); nodeIter != removedNodes.end(); ++nodeIter )
			{
				INode *pNode = *nodeIter;
				Control *pPRSController = ControllerExporter::FindController( pNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
				if ( !pPRSController )
				{
					continue;
				}

				{
					Control *pEulerRotationController = NewDefaultRotationController();
					pPRSController->SetRotationController( pEulerRotationController );
				}
			}
			
			break;
		}

		case tab_setcount:
		{
			s_billboardHelperParamBlockDesc.InvalidateUI();
			break;
		}
	}

	NodeList &previousBillboardNodeList = pBillboardHelper->GetPreviousBillboardNodeList();
	previousBillboardNodeList.clear();
	pBillboardHelper->GetCurrentBillboardNodeList( previousBillboardNodeList );
}

BOOL ICBillboardHelper::BillboardListPBValidator::Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex)
{
	ICBillboardHelper *pBillboardHelper = (ICBillboardHelper*)owner;
		
	INode *pNode = ( INode * )v.r;

	Object *pObject = pNode->GetObjectRef();
	if ( !pObject )
	{
		return FALSE;
	}

	// billboard manager test
	if ( pObject->ClassID() == BILLBOARD_HELPER_OBJECT_CLASSID )
	{
		return FALSE;
	}

	// must have a PRS Controller
	Control *pPRSController = ControllerExporter::FindController( pNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
	if ( !pPRSController )
	{
		return FALSE;
	}

	// Duplicate test
	{
		if ( pBillboardHelper->GetBillboardNodeId( pNode ) != -1 )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void ICBillboardHelper::GetBillboardControllers( BillboardControllerList &billboardControllers )
{
	for ( size_t id = 0; id < GetBillboardNodeCount(); ++id )
	{
		INode *pNode = GetBillboardNode( id );
		Control *pPRSController = ControllerExporter::FindController( pNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
		Control *pRotationController = pPRSController->GetRotationController();
		if ( pRotationController->ClassID() != BILLBOARD_CONTROLLER_CLASS_ID )
		{
			continue;
		}

		billboardControllers.push_back( ( ICBillboardController * )pRotationController );
	}
}

void ICBillboardHelper::BillboardControllerPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICBillboardHelper *pBillboardHelper = (ICBillboardHelper*)owner;

	//if ( id == PI_BILLBOARD_FRONT
	//	|| id == PI_BILLBOARD_UP )
	//{
	//	Point3 &vector = *v.p;
	//	float vectorLength = vector.Length();
	//	if ( !IsEquivalent( vectorLength, 1.f ) )
	//	{
	//		Point3 normalizedVector = vector.Normalize();
	//		pBillboardHelper->pblock2->SetValue( id, t, normalizedVector );
	//		s_billboardHelperParamBlockDesc.InvalidateUI( id );
	//		return;
	//	}
	//}

#if ENABLE_BILLBOARD_CONTROLLERS
	BillboardControllerList billboardControllers;
	pBillboardHelper->GetBillboardControllers( billboardControllers );

	for ( BillboardControllerList::iterator controlIter = billboardControllers.begin(); controlIter != billboardControllers.end(); ++controlIter )
	{
		ICBillboardController *pBillboardController = *controlIter;
		pBillboardController->Update( t );
		pBillboardController->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
	}
#else
#endif // ENABLE_BILLBOARD_CONTROLLERS
}

Point3 ICBillboardHelper::GetGizmoBoxMin()
{
	Point4 minBox;
	pblock2->GetValue( PI_MIN_BOX, 0, minBox, FOREVER );
	return Point3( minBox[0], minBox[1], minBox[2] );
}

Point3 ICBillboardHelper::GetGizmoBoxMax()
{
	Point4 maxBox;
	pblock2->GetValue( PI_MAX_BOX, 0, maxBox, FOREVER );
	return Point3( maxBox[0], maxBox[1], maxBox[2] );
}

void ICBillboardHelper::SetGizmoBoxMin( const Point3 &p )
{
	Point4 p4( p, 1.f );
	pblock2->SetValue( PI_MIN_BOX, 0, p4 );
}

void ICBillboardHelper::SetGizmoBoxMax( const Point3 &p )
{
	Point4 p4( p, 1.f );
	pblock2->SetValue( PI_MAX_BOX, 0, p4 );
}

Box3 ICBillboardHelper::ComputeGizmoBox( TimeValue t, INode *pNode )
{
	return Box3( GetGizmoBoxMin(), GetGizmoBoxMax() );
}

Box3 ICBillboardHelper::ComputeLinksBox( TimeValue t, INode *pNode )
{
	const int numLinkVertices = GetBillboardNodeCount();
	const int k_numElements = 3;

	if ( numLinkVertices <= 0 )
	{
		return Box3( Point3( 0.f, 0.f, 0.f ), Point3( 0.f, 0.f, 0.f ) );
	}
	else
	{
		const Matrix3 &nodeTm = GetObjectTM( pNode, t );
		const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

		Point3 linksMin( FLT_MAX, FLT_MAX, FLT_MAX );
		Point3 linksMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		for ( int vertexId = 0; vertexId < numLinkVertices; ++vertexId )
		{
			INode *pNode = GetBillboardNode( vertexId );

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

bool ICBillboardHelper::IsDisplayLinksEnabled()
{
	int val = 0;
	pblock2->GetValue( PI_DISPLAY_LINKS, 0, val, FOREVER );
	return val != 0;
}

bool ICBillboardHelper::IsDisplayBillboardedTargetsEnabled()
{
	int val = 0;
	pblock2->GetValue( PI_DISPLAY_BILLBOARDED_TARGETS, 0, val, FOREVER );
	return val != 0;
}

bool ICBillboardHelper::IsDisplayVectorsOnTargetsEnabled()
{
	int val = 0;
	pblock2->GetValue( PI_DISPLAY_VECTORS_ON_TARGETS, 0, val, FOREVER );
	return val != 0;
}

int ICBillboardHelper::GetBillboardNodeCount()
{
	return pblock2->Count( PI_BILLBOARD_NODE_LIST );
}

INode *ICBillboardHelper::GetBillboardNode( int id )
{
	INode *pNode = NULL;
	pblock2->GetValue( PI_BILLBOARD_NODE_LIST, 0, pNode, FOREVER, id );
	return pNode;
}

int ICBillboardHelper::GetBillboardNodeId( INode *pNode )
{
	int requestedId = -1;
	int count = GetBillboardNodeCount();
	
	for ( int id = 0; id < count; ++id )
	{
		if ( pNode == GetBillboardNode( id ) )
		{
			requestedId = id;
			break;
		}
	}
	
	return requestedId;
}

void ICBillboardHelper::DeleteBillboardNode( INode *pNode )
{
	int billboardNodeCount = GetBillboardNodeCount();
	for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
	{
		INode *pCurrentNode = GetBillboardNode( billboardNodeId );
		if ( pNode == pCurrentNode )
		{
			pblock2->Delete( PI_BILLBOARD_NODE_LIST, billboardNodeId, 1 );
			break;
		}
	}
}

void ICBillboardHelper::DeleteBillboardNode( int id )
{
	pblock2->Delete( PI_BILLBOARD_NODE_LIST, id, 1 );
}

void ICBillboardHelper::GetCurrentBillboardNodeList( NodeList &nodeList )
{
	for ( int id = 0; id < GetBillboardNodeCount(); ++id )
	{
		nodeList.push_back( GetBillboardNode( id ) );
	}
}

void ICBillboardHelper::InsertBillboardNodes( int start, int count, INode **pNodes )
{
	pblock2->Insert( PI_BILLBOARD_NODE_LIST, start, count, pNodes );
}

void ICBillboardHelper::AppendBillboardNodes( int count, INode **pNodes )
{
	pblock2->Append( PI_BILLBOARD_NODE_LIST, count, pNodes );
}

void ICBillboardHelper::ClearBillboardNodes()
{
	pblock2->ZeroCount( PI_BILLBOARD_NODE_LIST );
}

void ICBillboardHelper::ICBillboardHelperPostLoadCallback::proc( ILoad *iload )
{
	if ( !m_pOwner )
	{
		return;
	}

	{
		int billboardNodeCount = m_pOwner->GetBillboardNodeCount();
		for ( int billboardId = 0; billboardId < billboardNodeCount; ++billboardId )
		{
			INode *pNode = m_pOwner->GetBillboardNode( billboardId );
			if ( !pNode )
			{
				continue;
			}

#if ENABLE_BILLBOARD_CONTROLLERS
			Control *pPRSController = ControllerExporter::FindController( pNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
			Control *pRotationController = pPRSController->GetRotationController();
			FUAssert( pRotationController->ClassID() == BILLBOARD_CONTROLLER_CLASS_ID, continue );

			ICBillboardController *pBillboardController = ( ICBillboardController * )pRotationController;
			pBillboardController->SetHelperPtr( m_pOwner );
#else
#endif // ENABLE_BILLBOARD_CONTROLLERS
		}
	}
}

int ICBillboardHelper::NumSubObjTypes()
{
	return 2;
}

ISubObjType *ICBillboardHelper::GetSubObjType( int id )
{
	if ( id == -1 )
	{
		return NULL;
	}
	else if ( id == 0 )
	{
		return &m_frontVectorSubObject;
	}
	else if ( id == 1 )
	{
		return &m_upVectorSubObject;
	}
	return NULL;
}

void ICBillboardHelper::ActivateSubobjSel( int level, XFormModes &modes )
{
	switch ( level )
	{
		case 1: // front vector
		case 2: // up vector
		{
			modes = XFormModes( NULL, s_pRotateModBox, NULL, NULL, NULL, NULL );
			//modes = XFormModes( s_pMoveModBox, s_pRotateModBox, s_pNUScaleModBox, s_pUScaleModBox, s_pSquashModBox, s_pSelectModBox );
			break;
		}
	}
	
	NotifyDependents( FOREVER, PART_DISPLAY, REFMSG_CHANGE );
}

void ICBillboardHelper::GetSubObjectTMs( SubObjAxisCallback *cb, TimeValue  t, INode *node, ModContext *mc )
{
	Matrix3 objectTM = GetObjectTM( node, t );
	for ( int i = 0; i < COORDS_GIMBAL + 1; ++i )
	{
		cb->TM( objectTM, i );
	}
}

void ICBillboardHelper::GetSubObjectCenters( SubObjAxisCallback *cb, TimeValue  t, INode *node, ModContext *mc )
{
	Matrix3 objectTM = GetObjectTM( node, t );
	Point3 center = objectTM.GetTrans();
	for ( int i = 0; i < COORDS_GIMBAL + 1; ++i )
	{
		cb->Center( center, i );
	}
}

void ICBillboardHelper::Rotate( TimeValue t, Matrix3 &partm, Matrix3 &tmAxis, Quat &val, BOOL localOrigin )
{
	Matrix3 tm;
	val.MakeMatrix( tm );

	int subObjectLevel = GetSubObjectLevel();
	//SetXFormPacket pckt( val, localOrigin, partm, tmAxis );

	if ( subObjectLevel == 1 )
	{
		Point3 frontVector;
		GetFrontVector( t, frontVector );

		frontVector = tm.VectorTransform( frontVector );
		frontVector = frontVector.Normalize();
		SetFrontVector( t, frontVector );

		s_billboardHelperParamBlockDesc.InvalidateUI();
	}
	else if ( subObjectLevel == 2 )
	{
		Point3 upVector;
		GetUpVector( t, upVector );

		upVector = tm.VectorTransform( upVector );
		upVector = upVector.Normalize();
		SetUpVector( t, upVector );

		s_billboardHelperParamBlockDesc.InvalidateUI();	
	}

	//tmControl->SetValue( t, &pckt, TRUE, CTRL_RELATIVE );
	//macroRecorder->OpAssign(_T("+="), mr_prop, _T("gizmo.rotation"), mr_reftarg, this, mr_quat, &val);
}

void ICBillboardHelper::GetFrontVector( TimeValue t, Point3 &v )
{
	pblock2->GetValue( PI_BILLBOARD_FRONT, t, v, FOREVER );
}

void ICBillboardHelper::GetUpVector( TimeValue t, Point3 &v )
{
	pblock2->GetValue( PI_BILLBOARD_UP, t, v, FOREVER );
}

void ICBillboardHelper::SetFrontVector( TimeValue t, Point3 v )
{
	pblock2->SetValue( PI_BILLBOARD_FRONT, t, v );
}

void ICBillboardHelper::SetUpVector( TimeValue t, Point3 v )
{
	pblock2->SetValue( PI_BILLBOARD_UP, t, v );
}

void ICBillboardHelper::SetBillboardType( int type )
{
	pblock2->SetValue( PI_BILLBOARD_TYPE, 0, type );
}

void ICBillboardHelper::GetBillboardType( int &type )
{
	pblock2->GetValue( PI_BILLBOARD_TYPE, 0, type, FOREVER );
}

void ICBillboardHelper::SetBillboardOrientation( int orientation )
{
	pblock2->SetValue( PI_BILLBOARD_ORIENTATION, 0, orientation );
}

void ICBillboardHelper::GetBillboardOrientation( int &orientation )
{
	pblock2->GetValue( PI_BILLBOARD_ORIENTATION, 0, orientation, FOREVER );
}

void ICBillboardHelper::NotificationCallback( void *param, NotifyInfo *info )
{
	ICBillboardHelper *pBillboardHelper = ( ICBillboardHelper * )param;

	switch ( info->intcode )
	{
		case NOTIFY_SCENE_PRE_DELETED_NODE :
		{
			INode *pDeletedNode = ( INode * )info->callParam;
			INode *pBillboardHelperNode = MAXHelpers::GetNodeByObjectRef( GetCOREInterface()->GetRootNode(), pBillboardHelper );
			if ( pDeletedNode == pBillboardHelperNode )
			{
				int billboardNodeCount = pBillboardHelper->GetBillboardNodeCount();
				for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
				{
					INode *pBillboardNode = pBillboardHelper->GetBillboardNode( billboardNodeId );

#if ENABLE_BILLBOARD_CONTROLLERS
					Control *pPRSController = ControllerExporter::FindController( pBillboardNode->GetTMController(), Class_ID( PRS_CONTROL_CLASS_ID, 0 ), true );
					if ( !pPRSController )
					{
						continue;
					}

					Control *pPreviousRotationController = pPRSController->GetRotationController();
					if ( !pPreviousRotationController )
					{
						continue;
					}

					if ( pPreviousRotationController->ClassID() != BILLBOARD_CONTROLLER_CLASS_ID )
					{
						continue;
					}

					Control *pEulerRotationController = NewDefaultRotationController();
					pPRSController->SetRotationController( pEulerRotationController );
#else
#endif // ENABLE_BILLBOARD_CONTROLLERS
				}
			}
			break;
		}
	}
}