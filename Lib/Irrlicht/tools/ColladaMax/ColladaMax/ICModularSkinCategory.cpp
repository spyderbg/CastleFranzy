/**********************************************************************
 *<
	FILE: ModularSkinCategory.cpp

	DESCRIPTION: A simple planar deflector

	CREATED BY: Rolf Berteig
			

	HISTORY: Peter Watje 3-15-00

 *>	Copyright (c) 200, All Rights Reserved.
 **********************************************************************/

#include "StdAfx.h"
#include "ICModularSkinCategory.h"
#include "ICModularSkinMaster.h"
#include "ControllerExporter.h"
#include "ColladaModifiers.h"
#include "FUtils/FUParameter.h"
#include "MAXHelpers.h"

static ICModularSkinCategoryClassDesc modularSkinCategoryDesc;
ClassDesc* GetICModularSkinCategoryObjDesc() {return &modularSkinCategoryDesc;}

ICModularSkinCategoryObject::SkinnedMeshListPBValidator ICModularSkinCategoryObject::s_skinnedMeshListValidator;
ICModularSkinCategoryObject::SkinnedMeshListPBAccessor ICModularSkinCategoryObject::s_skinnedMeshListAccessor;

ICModularSkinCategoryObject::SkinnedMeshReferenceListPBAccessor ICModularSkinCategoryObject::s_skinnedMeshReferenceListAccessor;

const Point3 ICModularSkinCategoryObject::k_gizmoColor( 1.f, 1.f, 0.f );
const Point3 ICModularSkinCategoryObject::k_linksColor( 0.8f, 0.8f, 0.f );

static ParamBlockDesc2 s_modularSkinCategoryParamBlockDesc ( 
	
	ICModularSkinCategoryObject::PB_MAIN_ID
	, _T("ModularSkinCategoryParameters")
	,  0
	, &modularSkinCategoryDesc
	, P_AUTO_CONSTRUCT + P_AUTO_UI + P_MULTIMAP + P_VERSION
	, 0
	, ICModularSkinCategoryObject::PB_MAIN_REF_ID
	
	//rollout
	, ICModularSkinCategoryObject::R_COUNT
	, ICModularSkinCategoryObject::R_DISPLAY_OPTIONS, IDD_MODULAR_SKIN_CATEGORY_DISPLAY_OPTIONS, IDS_MODULAR_SKIN_DISPLAY_OPTIONS_ROLLUP_LABEL, 0, 0, NULL
	, ICModularSkinCategoryObject::R_SKINNED_MESHES, IDD_MODULAR_SKIN_CATEGORY_OBJECT, IDS_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_ROLLUP_LABEL, 0, 0, NULL

	// display rollup
	, ICModularSkinCategoryObject::PI_DISPLAY_LINKS, _T("display_links"), TYPE_INT, 0, IDS_MODULAR_SKIN_DISPLAY_LINKS
		, p_ui, ICModularSkinCategoryObject::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_MODULAR_SKIN_DISPLAY_LINKS_CHECKBOX
		, p_default, 1
	, end

	// skinned meshes rollup
	, ICModularSkinCategoryObject::PI_SKINNED_MESH_LIST, _T("SkinnedMeshList"), TYPE_INODE_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
		, p_ui, ICModularSkinCategoryObject::R_SKINNED_MESHES, TYPE_NODELISTBOX, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_PICK_BUTTON
			, NULL
			, IDC_MODULAR_SKIN_OBJECT_SKINNED_MESH_LIST_REMOVE_BUTTON
		, p_validator, &ICModularSkinCategoryObject::s_skinnedMeshListValidator
		, p_accessor, &ICModularSkinCategoryObject::s_skinnedMeshListAccessor
	, end
	//, ICModularSkinCategoryObject::PI_SKINNED_MESH_REFERENCE_LIST, _T("SkinnedMeshReferenceList"), TYPE_REFTARG_TAB, 0, P_VARIABLE_SIZE, IDS_MODULAR_SKIN_OBJECT_SKINNED_MESH_REFERENCE_LIST
	//	, p_accessor, &ICModularSkinCategoryObject::s_skinnedMeshReferenceListAccessor
	//, end

	, ICModularSkinCategoryObject::PI_MIN_BOX, _T("min_box"), TYPE_POINT4, 0, IDS_MODULAR_SKIN_MIN_BOX, end
	, ICModularSkinCategoryObject::PI_MAX_BOX, _T("max_box"), TYPE_POINT4, 0, IDS_MODULAR_SKIN_MAX_BOX, end

	, end
);

//--- ICModularSkinCategoryObject Parameter map/block descriptors ------------------
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
ICModularSkinCategoryObject::ICModularSkinCategoryObject()
//: m_skinnedMeshReferenceMaker( this )
{
	pblock2 = NULL;
	modularSkinCategoryDesc.MakeAutoParamBlocks(this);
	assert(pblock2);
}

RefTargetHandle ICModularSkinCategoryObject::Clone(RemapDir& remap) 
{
	ICModularSkinCategoryObject* newob = new ICModularSkinCategoryObject();	

	CLONE_PARAMETER( newob, m_gizmoMesh );
	CLONE_PARAMETER( newob, m_linksMesh );
	//CLONE_PARAMETER( newob, dumFlags );
	//CLONE_PARAMETER( newob, color );
	//CLONE_PARAMETER( newob, valid );
	newob->ReplaceReference(0,remap.CloneRef(pblock2));

	BaseClone(this, newob, remap);
	return newob;
}

void ICModularSkinCategoryObject::BeginEditParams( IObjParam *ip, ULONG flags, Animatable *prev )
{
	modularSkinCategoryDesc.BeginEditParams(ip, this, flags, prev);
}

void ICModularSkinCategoryObject::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{		
	modularSkinCategoryDesc.EndEditParams(ip, this, flags, next);
}

void ICModularSkinCategoryObject::BuildMesh( TimeValue t, INode *pNode )
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

	// links
	if ( IsDisplayLinksEnabled() )
	{
		const int numLinks = GetSkinnedMeshNodeCount();
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

			const Matrix3 &nodeTm = pNode->GetObjectTM( t );
			//const Matrix3 &nodeTm = pNode->GetObjTMBeforeWSM( t );
			const Point3 &nodeTranslation = nodeTm.GetRow( 3 );

			const int centerVertexId = 0;
			m_linksMesh.setVert( centerVertexId, Point3( 0.0f, 0.0f, 0.0f ) );

			int currentVertex = centerVertexId + 1;
			for ( int currentLinkId = 0; currentLinkId < numLinks; ++currentLinkId )
			{
				INode *pNode = GetSkinnedMeshNode( currentLinkId );
				
				Matrix3 currentLinkNodeTm = pNode->GetObjTMBeforeWSM( t );
				const Point3 &currentLinkTranslation = currentLinkNodeTm.GetRow( 3 );

				m_linksMesh.setVert( currentVertex++, currentLinkTranslation - nodeTranslation );
				m_linksMesh.faces[currentLinkId].setEdgeVisFlags( 1, 1, 1 );
				m_linksMesh.faces[currentLinkId].setVerts( centerVertexId, currentVertex - 1, centerVertexId );
			}
		}

		m_linksMesh.InvalidateGeomCache();
	}
}

void ICModularSkinCategoryObject::GetLinksTM( const Matrix3 &objectTM, Matrix3 &tm )
{
	tm.IdentityMatrix();
	tm.SetTrans( objectTM.GetTrans() );
}

int ICModularSkinCategoryObject::HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt )
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

	return FALSE;
}

int ICModularSkinCategoryObject::Display( TimeValue t, INode* inode, ViewExp *vpt, int flags )
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
		Matrix3 tm;
		GetLinksTM( objectTM, tm );
		pGraphicsWindow->setTransform( tm );
		
		pGraphicsWindow->setColor( LINE_COLOR, inode->Selected() ? GetSelColor() : k_linksColor );
		m_linksMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
	}
	
	// label
	//Point3 labelPos( 0.f, 0.f, 0.f );
	//pGraphicsWindow->setColor( TEXT_COLOR, 1.f, 1.f, 1.f );
	//pGraphicsWindow->text( &labelPos, _T("Category") );
	pGraphicsWindow->setRndLimits( rlim );

	return 0;
}

void ICModularSkinCategoryObject::Snap( TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt )
{
	return;
}

Interval ICModularSkinCategoryObject::ObjectValidity( TimeValue t )
{ 
	return FOREVER; 
}

ObjectState ICModularSkinCategoryObject::Eval( TimeValue time )
{ 
	return ObjectState( this );
}

void ICModularSkinCategoryObject::GetWorldBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	const Matrix3 objectTM = pNode->GetObjectTM( t );
	const int k_numBoxVertices = 8;
	const int k_numElements = 3;

	Box3 gizmoBox = ComputeGizmoBox( t, pNode );
	Box3 linksBox = ComputeLinksBox( t, pNode );

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

	// links
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

	box = Box3( worldMin, worldMax );
	return;
}

void ICModularSkinCategoryObject::GetLocalBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	Box3 gizmoBox = ComputeGizmoBox( t, pNode );
	Box3 linksBox = ComputeLinksBox( t, pNode );

	Point3 localMin = MAXHelpers::MinComponents( gizmoBox.Min(), linksBox.Min() );
	Point3 localMax = MAXHelpers::MaxComponents( gizmoBox.Max(), linksBox.Max() );

	box = Box3( localMin, localMax );
}	

void ICModularSkinCategoryObject::GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel )
{
	return;
}

int ICModularSkinCategoryObjCreateCallback::proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat )
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

CreateMouseCallBack* ICModularSkinCategoryObject::GetCreateMouseCallBack()
{
	static ICModularSkinCategoryObjCreateCallback createCB;
	createCB.ob = this;
	return &createCB;
}

// JBW:  for loading old ParamBlock versions, register an Ed. 2 param block converter post-load callback
//       This one takes the ParamBlockDescID version array as before, but now takes the 
//       ParamBlockDesc2 for the block to describe the current version and will convert the loaded ParamBlock
//       into a corresponding ParamBlock2
IOResult
ICModularSkinCategoryObject::Load(ILoad *iload)
{	
	// TODO
	//ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, NUM_OLDVERSIONS, &s_modularSkinCategoryParamBlockDesc, this, FSDeflector::PBLOCK_REF_NO);
	//iload->RegisterPostLoadCallback(plcb);
	iload->RegisterPostLoadCallback( new ICModularSkinCategoryObjectPostLoadCallback( this ) );
	return IO_OK;
}

ICModularSkinCategoryObject::~ICModularSkinCategoryObject()
{	
}

void ICModularSkinCategoryObject::SynchronizeSkinnedMeshReferences()
{
	// remove all references
	//m_skinnedMeshReferenceMaker.UnregisterAllReferences();

	//int skinnedMeshNodeCount = GetSkinnedMeshNodeCount();
	//for ( int skinnedMeshNodeId = 0; skinnedMeshNodeId < skinnedMeshNodeCount; ++skinnedMeshNodeId )
	//{
	//	INode *pNode = GetSkinnedMeshNode( skinnedMeshNodeId );
	//	m_skinnedMeshReferenceMaker.RegisterReference( pNode );
	//}
}

void ICModularSkinCategoryObject::SkinnedMeshReferenceListPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	int tmp = 0;
}

void ICModularSkinCategoryObject::SkinnedMeshReferenceListPBAccessor::TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count )
{
	ICModularSkinCategoryObject *pModularSkinCategoryObject = (ICModularSkinCategoryObject*)owner;

	switch ( changeCode )
	{
		case tab_insert :
		case tab_append :
		//case tab_delete :
		//case tab_ref_deleted :
		{
			int tmp = 0;
			break;
		}

		case tab_delete :
		case tab_ref_deleted :
		{
			int tmp = 0;
			break;
		}

		case tab_setcount:
		{
			int tmp = 0;
			break;
		}
	}
}

void ICModularSkinCategoryObject::SkinnedMeshListPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICModularSkinCategoryObject *pModularSkinCategoryObject = (ICModularSkinCategoryObject*)owner;
	//pModularSkinCategoryObject->SynchronizeSkinnedMeshReferences();
}

void ICModularSkinCategoryObject::SkinnedMeshListPBAccessor::TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count )
{
	ICModularSkinCategoryObject *pModularSkinCategoryObject = (ICModularSkinCategoryObject*)owner;

	switch ( changeCode )
	{
		case tab_insert :
		case tab_append :
		{
			int tmp = 0;
			
			// associated modular skin masters skinned mesh node auto-select
			if ( (*tab).Count() == 1 )
			{
				INodeTab modularSkinMasterNodes;
				MAXHelpers::GetNodesByObjectClassID( GetCOREInterface()->GetRootNode(), modularSkinMasterNodes, GetICModularSkinMasterObjDesc()->ClassID() );
				
				for ( int modularSkinMasterNodeId = 0; modularSkinMasterNodeId < modularSkinMasterNodes.Count(); ++modularSkinMasterNodeId )
				{
					ICModularSkinMasterObject *pModularSkinMasterObject = ( ICModularSkinMasterObject * )modularSkinMasterNodes[modularSkinMasterNodeId]->GetObjectRef();
					
					int categoryNodeId = pModularSkinMasterObject->GetSkinnedMeshCategoryNodeId( pModularSkinCategoryObject );
					if (  categoryNodeId == -1 )
					{
						continue;
					}

					if ( pModularSkinMasterObject->GetSelectedSkinnedMeshNode( categoryNodeId ) == NULL )
					{
						pModularSkinMasterObject->SetSelectedSkinnedMeshNode( categoryNodeId, (INode *)(*tab)[tabIndex].r );
					}
				}
			}
			//pModularSkinCategoryObject->InsertSkinnedMeshReferences( tabIndex, 1, &(*tab)[tabIndex].r );
			//pModularSkinCategoryObject->m_skinnedMeshReferenceMaker.SetReference( tabIndex, NULL );
			//pModularSkinCategoryObject->ReplaceReference( tabIndex, (*tab)[tabIndex].r );
			//pModularSkinCategoryObject->SynchronizeSkinnedMeshReferences();
			break;
		}

		case tab_ref_deleted :
		{
			pModularSkinCategoryObject->DeleteSkinnedMeshNode( tabIndex );
			int tmp = 0;
			break;
		}

		case tab_delete :
		{
			int tmp = 0;
			//INode *pNode = NULL;
			//pModularSkinCategoryObject->DeleteSkinnedMeshNode( pNode );

			//pModularSkinCategoryObject->SetReference( tabIndex, NULL );
			//pModularSkinCategoryObject->ReplaceReference( tabIndex, (*tab)[tabIndex].r );
			//pModularSkinCategoryObject->DeleteSkinnedMeshReferences( tabIndex, 1 );
			//pModularSkinCategoryObject->SynchronizeSkinnedMeshReferences();
			break;
		}

		case tab_setcount:
		{
			//pModularSkinCategoryObject->SynchronizeSkinnedMeshReferences();
			s_modularSkinCategoryParamBlockDesc.InvalidateUI();
			break;
		}
	}
}

BOOL ICModularSkinCategoryObject::SkinnedMeshListPBValidator::Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex)
{
	ICModularSkinCategoryObject *pModularSkinCategoryObject = (ICModularSkinCategoryObject*)owner;
		
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
		int numSkinnedMeshes = pModularSkinCategoryObject->GetSkinnedMeshNodeCount();
		for ( int skinnedMeshId = 0; skinnedMeshId < numSkinnedMeshes; ++skinnedMeshId )
		{
			INode *pNodeInList = pModularSkinCategoryObject->GetSkinnedMeshNode( skinnedMeshId );
			if ( pNode == pNodeInList )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

Point3 ICModularSkinCategoryObject::GetGizmoBoxMin()
{
	Point4 minBox;
	pblock2->GetValue( PI_MIN_BOX, 0, minBox, FOREVER );
	return Point3( minBox[0], minBox[1], minBox[2] );
}

Point3 ICModularSkinCategoryObject::GetGizmoBoxMax()
{
	Point4 maxBox;
	pblock2->GetValue( PI_MAX_BOX, 0, maxBox, FOREVER );
	return Point3( maxBox[0], maxBox[1], maxBox[2] );
}

void ICModularSkinCategoryObject::SetGizmoBoxMin( const Point3 &p )
{
	Point4 p4( p, 1.f );
	pblock2->SetValue( PI_MIN_BOX, 0, p4 );
}

void ICModularSkinCategoryObject::SetGizmoBoxMax( const Point3 &p )
{
	Point4 p4( p, 1.f );
	pblock2->SetValue( PI_MAX_BOX, 0, p4 );
}

Box3 ICModularSkinCategoryObject::ComputeGizmoBox( TimeValue t, INode *pNode )
{
	return Box3( GetGizmoBoxMin(), GetGizmoBoxMax() );
}

Box3 ICModularSkinCategoryObject::ComputeLinksBox( TimeValue t, INode *pNode )
{
	const int numLinkVertices = GetSkinnedMeshNodeCount();
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
			INode *pNode = GetSkinnedMeshNode( vertexId );

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

bool ICModularSkinCategoryObject::IsDisplayLinksEnabled()
{
	int val = 0;
	pblock2->GetValue( ICModularSkinCategoryObject::PI_DISPLAY_LINKS, 0, val, FOREVER );
	return val != 0;
}

int ICModularSkinCategoryObject::GetSkinnedMeshNodeCount()
{
	return pblock2->Count( PI_SKINNED_MESH_LIST );
}

INode *ICModularSkinCategoryObject::GetSkinnedMeshNode( int id )
{
	INode *pNode = NULL;
	pblock2->GetValue( PI_SKINNED_MESH_LIST, 0, pNode, FOREVER, id );
	return pNode;
}

int ICModularSkinCategoryObject::GetSkinnedMeshNodeId( INode *pNode )
{
	int requestedId = -1;
	int count = GetSkinnedMeshNodeCount();
	
	for ( int id = 0; id < count; ++id )
	{
		if ( pNode == GetSkinnedMeshNode( id ) )
		{
			requestedId = id;
			break;
		}
	}
	
	return requestedId;
}

void ICModularSkinCategoryObject::DeleteSkinnedMeshNode( INode *pNode )
{
	int skinnedMeshNodeCount = GetSkinnedMeshNodeCount();
	for ( int skinnedMeshNodeId = 0; skinnedMeshNodeId < skinnedMeshNodeCount; ++skinnedMeshNodeId )
	{
		INode *pCurrentNode = GetSkinnedMeshNode( skinnedMeshNodeId );
		if ( pNode == pCurrentNode )
		{
			pblock2->Delete( PI_SKINNED_MESH_LIST, skinnedMeshNodeId, 1 );
			break;
		}
	}
}

void ICModularSkinCategoryObject::DeleteSkinnedMeshNode( int id )
{
	pblock2->Delete( PI_SKINNED_MESH_LIST, id, 1 );
}

//inline int ICModularSkinCategoryObject::SkinnedMeshReferenceMaker::NumRefs()
//{
//	return m_pModularSkinCategoryObject->GetSkinnedMeshReferenceCount();
//}
//
//RefTargetHandle ICModularSkinCategoryObject::SkinnedMeshReferenceMaker::GetReference( int i )
//{
//	return m_pModularSkinCategoryObject->GetSkinnedMeshReference( i );
//}
//
//void ICModularSkinCategoryObject::SkinnedMeshReferenceMaker::SetReference( int i, RefTargetHandle rtarg )
//{
//	m_pModularSkinCategoryObject->SetSkinnedMeshReference( i, rtarg );
//}
//
//RefResult ICModularSkinCategoryObject::SkinnedMeshReferenceMaker::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message )
//{           
//	switch (message)
//	{     
//		//case REFMSG_CHANGE:
//		//{
//		//	int tmp = 0;
//		//	break;
//		//}
//		case REFMSG_TARGET_DELETED:
//		{
//			INode *pNode = ( INode * )hTarget;
//			m_pModularSkinCategoryObject->DeleteSkinnedMeshNode( pNode );
//			return REF_SUCCEED;
//		}
//	}
//
//	return REF_DONTCARE;
//}

void ICModularSkinCategoryObject::ICModularSkinCategoryObjectPostLoadCallback::proc( ILoad *iload )
{
	//m_pModularSkinCategoryObject->SynchronizeSkinnedMeshReferences();
}
