#include "stdafx.h"
#include "ICModifierBillboard.h"
#include "FUtils/FUParameter.h"
#include "FUtils/FUDaeEnum.h"
#include "ColladaModifierManager.h"
#include "ColladaModifiers.h"

// ----------------------------------------------------------------------
// - Config
// ----------------------------------------------------------------------
// Also need to set the enabled state of IDD_BILLBOARD_CONTROLLER_PARAMS vector widgets when enabling/disabling this switch
#define ENABLE_BILLBOARD_VECTOR_UI 0

ICBillboardWSMObject::BillboardControllerPBAccessor ICBillboardWSMObject::s_billboardControllerAccessor;

const Point3 ICBillboardWSMObject::k_gizmoColor( 1.f, 1.f, 0.f );
const Point3 ICBillboardWSMObject::k_linksColor( 1.f, 0.f, 1.f );
const Point3 ICBillboardWSMObject::k_frontVectorColor( 0.0f, 0.0f, 1.f );
const Point3 ICBillboardWSMObject::k_upVectorColor( 0.0f, 1.0f, 0.f );

MCHAR *ICBillboardWSMObject::k_frontVectorSubObjectName = _T( "Front Vector" );
MCHAR *ICBillboardWSMObject::k_upVectorSubObjectName = _T( "Up Vector" );

MoveModBoxCMode *ICBillboardWSMObject::s_pMoveModBox = NULL;
RotateModBoxCMode *ICBillboardWSMObject::s_pRotateModBox = NULL;
UScaleModBoxCMode *ICBillboardWSMObject::s_pUScaleModBox = NULL;
NUScaleModBoxCMode *ICBillboardWSMObject::s_pNUScaleModBox = NULL;
SquashModBoxCMode *ICBillboardWSMObject::s_pSquashModBox = NULL;
SelectModBoxCMode *ICBillboardWSMObject::s_pSelectModBox = NULL;

ClassDesc2* ICBillboardWSMObject::GetMyClassDesc()
{
	static MyClassDesc myClassDesc;
	return &myClassDesc;
}

static ParamBlockDesc2 s_billboardParamBlockDesc
( 
	ICBillboardWSMObject::PB_MAIN_ID
	, _T("BillboardParameters")
	,  0
	, ICBillboardWSMObject::GetMyClassDesc()
	, P_AUTO_CONSTRUCT + P_AUTO_UI + P_MULTIMAP + P_VERSION
	, 0
	, ICBillboardWSMObject::PB_MAIN_REF_ID
	
	//rollout
	, ICBillboardWSMObject::R_COUNT
	, ICBillboardWSMObject::R_DISPLAY_OPTIONS, IDD_BILLBOARD_HELPER_DISPLAY_OPTIONS, IDS_MODULAR_SKIN_DISPLAY_OPTIONS_ROLLUP_LABEL, 0, 0, NULL
	, ICBillboardWSMObject::R_BILLBOARD_CONTROLLER, IDD_BILLBOARD_CONTROLLER_PARAMS, IDS_BILLBOARD_CONTROLLER_ROLLUP_LABEL, 0, 0, NULL
	
	// display rollup
	, ICBillboardWSMObject::PI_DISPLAY_LINKS, _T("display_links"), TYPE_INT, P_RESET_DEFAULT, IDS_MODULAR_SKIN_DISPLAY_LINKS
		, p_ui, ICBillboardWSMObject::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_BILLBOARD_HELPER_DISPLAY_LINKS_CHECKBOX
		, p_range, 0, 1
		, p_default, 1
	, end

	, ICBillboardWSMObject::PI_DISPLAY_BILLBOARDED_TARGETS, _T("display_billboarded_targets"), TYPE_INT, P_RESET_DEFAULT, IDS_BILLBOARD_DISPLAY_BILLBOARDED_TARGETS
		, p_ui, ICBillboardWSMObject::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_BILLBOARD_HELPER_DISPLAY_BILLBOARDED_TARGETS_CHECKBOX
		, p_accessor, &ICBillboardWSMObject::s_billboardControllerAccessor
		, p_range, 0, 1
		, p_default, 1
	, end

	, ICBillboardWSMObject::PI_DISPLAY_VECTORS_ON_TARGETS, _T("display_vectors_on_targets"), TYPE_INT, P_RESET_DEFAULT, IDS_BILLBOARD_DISPLAY_VECTORS_ON_TARGETS
		, p_ui, ICBillboardWSMObject::R_DISPLAY_OPTIONS, TYPE_SINGLECHEKBOX, IDC_BILLBOARD_HELPER_DISPLAY_VECTORS_ON_TARGETS_CHECKBOX
		, p_range, 0, 1
		, p_default, 0
	, end

	// skinned meshes rollup
	, ICBillboardWSMObject::PI_BILLBOARD_NODE_LIST, _T("BillboardList"), TYPE_INODE_TAB, 0, P_VARIABLE_SIZE, IDS_BILLBOARD_NODE_LIST
	, end

	// billboard controller
	, ICBillboardWSMObject::PI_BILLBOARD_TYPE, _T("Billboard Type"), TYPE_INT, P_RESET_DEFAULT, IDS_BILLBOARD_TYPE
		, p_default, 		FUDaeBillboard::BT_FREE
		, p_range, 			FUDaeBillboard::BT_FREE, FUDaeBillboard::BT_UP_ONLY
		, p_ui, 			ICBillboardWSMObject::R_BILLBOARD_CONTROLLER, TYPE_RADIO, FUDaeBillboard::BT_COUNT
			, IDC_BILLBOARD_TYPE_FREE_RADIOBUTTON
			, IDC_BILLBOARD_TYPE_ROTATION_RADIOBUTTON
			, IDC_BILLBOARD_TYPE_UP_ONLY_RADIOBUTTON
		, p_accessor, &ICBillboardWSMObject::s_billboardControllerAccessor
	, end

	, ICBillboardWSMObject::PI_BILLBOARD_ORIENTATION, _T("Billboard Orientation"), TYPE_INT, P_RESET_DEFAULT, IDS_BILLBOARD_ORIENTATION
		, p_default, 		FUDaeBillboard::OT_TOWARD_CAMERA
		, p_range, 			FUDaeBillboard::OT_TOWARD_CAMERA, FUDaeBillboard::OT_PARALLEL_CAM_AXIS
		, p_ui, 			ICBillboardWSMObject::R_BILLBOARD_CONTROLLER, TYPE_RADIO, FUDaeBillboard::OT_COUNT
			, IDC_BILLBOARD_ORIENTATION_TOWARD_CAMERA_RADIOBUTTON
			, IDC_BILLBOARD_ORIENTATION_PARALLEL_CAM_AXIS_RADIOBUTTON
		, p_accessor, &ICBillboardWSMObject::s_billboardControllerAccessor
	, end

	// billboard front vector ( vector ui version )
	, ICBillboardWSMObject::PI_BILLBOARD_FRONT, _T("Billboard Front Vector"), TYPE_POINT3, P_ANIMATABLE | P_RESET_DEFAULT, IDS_BILLBOARD_FRONT_VECTOR
		, p_default, 		Point3( 0.f, 0.f, 1.f )
		, p_range, 			-FLT_MAX, FLT_MAX
#if !ENABLE_BILLBOARD_VECTOR_UI
		, p_enabled,		FALSE
#endif
		, p_ui, 			ICBillboardWSMObject::R_BILLBOARD_CONTROLLER, TYPE_SPINNER, EDITTYPE_UNIVERSE
			, IDC_BILLBOARD_FRONT_VECTOR_X_EDITTEXT, IDC_BILLBOARD_FRONT_VECTOR_X_SPIN
			, IDC_BILLBOARD_FRONT_VECTOR_Y_EDITTEXT, IDC_BILLBOARD_FRONT_VECTOR_Y_SPIN
			, IDC_BILLBOARD_FRONT_VECTOR_Z_EDITTEXT, IDC_BILLBOARD_FRONT_VECTOR_Z_SPIN
			, SPIN_AUTOSCALE
		, p_accessor, &ICBillboardWSMObject::s_billboardControllerAccessor
	, end

	// billboard up vector ( vector ui version )
	, ICBillboardWSMObject::PI_BILLBOARD_UP, _T("Billboard Up Vector"), TYPE_POINT3, P_ANIMATABLE | P_RESET_DEFAULT, IDS_BILLBOARD_UP_VECTOR
		, p_default, 		Point3( 0.f, 1.f, 0.f )
		, p_range, 			-FLT_MAX, FLT_MAX
#if !ENABLE_BILLBOARD_VECTOR_UI
		, p_enabled,		FALSE
#endif
		, p_ui, 			ICBillboardWSMObject::R_BILLBOARD_CONTROLLER, TYPE_SPINNER, EDITTYPE_UNIVERSE
			, IDC_BILLBOARD_UP_VECTOR_X_EDITTEXT, IDC_BILLBOARD_UP_VECTOR_X_SPIN
			, IDC_BILLBOARD_UP_VECTOR_Y_EDITTEXT, IDC_BILLBOARD_UP_VECTOR_Y_SPIN
			, IDC_BILLBOARD_UP_VECTOR_Z_EDITTEXT, IDC_BILLBOARD_UP_VECTOR_Z_SPIN
			, SPIN_AUTOSCALE
		, p_accessor, &ICBillboardWSMObject::s_billboardControllerAccessor
	, end

	, end
);

CreateMouseCallBack *ICBillboardWSMObject::GetCreateMouseCallBack()
{
	static ICBillboardWSMObject::CreateMouseCB createCB;
	createCB.ob = this;
	return &createCB;
}

int ICBillboardWSMObject::CreateMouseCB::proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat )
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

bool ICBillboardWSMObject::ValidateNotifyModifierObject( MAXHelpers::NotifyModifierObject *pNotifyModifierObject )
{
	if ( !pNotifyModifierObject->pModifier )
	{
		return false;
	}

	Class_ID cid = pNotifyModifierObject->pModifier->ClassID();
	if ( cid != IC_BILLBOARD_MODIFIER_CLASSID )
	{
		return false;
	}

	if ( !pNotifyModifierObject->pNode )
	{
		return false;
	}

	return true;
}

bool ICBillboardWSM::ValidateNotifyModifierObject( MAXHelpers::NotifyModifierObject *pNotifyModifierObject )
{
	if ( !ICBillboardWSMObject::ValidateNotifyModifierObject( pNotifyModifierObject ) )
	{
		return false;
	}

	ICBillboardWSMObject *pBillboardWSMObject = (( ICBillboardWSM * )pNotifyModifierObject->pModifier )->GetBillboardWSMObjectPtr();
	if ( !pBillboardWSMObject )
	{
		return false;
	}

	return true;
}

void ICBillboardWSMObject::NotificationCallback( void *param, NotifyInfo *info )
{
	ICBillboardWSMObject *pBillboardWSMObject = ( ICBillboardWSMObject * )param;

	switch ( info->intcode )
	{
		case NOTIFY_SCENE_ADDED_NODE :
		{
			INode *pAddedNode = ( INode * )info->callParam;
			INode *pBillboardWSMObjectNode = MAXHelpers::GetNodeByObjectRef( GetCOREInterface()->GetRootNode(), pBillboardWSMObject );

			if ( pAddedNode == pBillboardWSMObjectNode )
			{
				pBillboardWSMObjectNode->SetTransformLock( INODE_LOCKROT, INODE_LOCK_X, true );
				pBillboardWSMObjectNode->SetTransformLock( INODE_LOCKROT, INODE_LOCK_Y, true );
				pBillboardWSMObjectNode->SetTransformLock( INODE_LOCKROT, INODE_LOCK_Z, true );

				pBillboardWSMObjectNode->SetTransformLock( INODE_LOCKSCL, INODE_LOCK_X, true );
				pBillboardWSMObjectNode->SetTransformLock( INODE_LOCKSCL, INODE_LOCK_Y, true );
				pBillboardWSMObjectNode->SetTransformLock( INODE_LOCKSCL, INODE_LOCK_Z, true );
			}
			
			break;
		}
		case NOTIFY_SCENE_PRE_DELETED_NODE :
		{
			INode *pDeletedNode = ( INode * )info->callParam;

			// on delete BillboardWSMObject
			{
				INode *pBillboardWSMObjectNode = MAXHelpers::GetNodeByObjectRef( GetCOREInterface()->GetRootNode(), pBillboardWSMObject );
				if ( pDeletedNode == pBillboardWSMObjectNode )
				{
					int billboardNodeCount = pBillboardWSMObject->GetBillboardNodeCount();
					NodeModifierMappingList toDelete;
					for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
					{
						INode *pBillboardNode = pBillboardWSMObject->GetBillboardNode( billboardNodeId );
						FUAssert( pBillboardNode, continue );

						ColladaModifierStack foundModifiers;
						{
							ColladaModifierStack targetModifiers;
							targetModifiers.push_back( new ICBillboardModifier() );
							ColladaModifierManager::ResolveModifiers( pBillboardNode, pBillboardNode->GetWSMDerivedObject(), targetModifiers, foundModifiers );
							CLEAR_POINTER_VECTOR( targetModifiers );
						}

						ICBillboardWSMObject *pBillboardModifierObject = NULL;
						ICBillboardWSM *pBillboardModifier = NULL;
						for ( ColladaModifierStack::iterator colladaModifierIter = foundModifiers.begin(); colladaModifierIter != foundModifiers.end() && pBillboardModifierObject != pBillboardWSMObject; ++colladaModifierIter )
						{
							ICBillboardModifier *pBillboardColladaModifier = ( ICBillboardModifier * )*colladaModifierIter;
							pBillboardModifier = pBillboardColladaModifier->GetInterface();
							pBillboardModifierObject = pBillboardModifier->GetBillboardWSMObjectPtr();
							
						}
						CLEAR_POINTER_VECTOR( foundModifiers );

						if ( pBillboardModifierObject != pBillboardWSMObject )
						{
							continue;
						}

						toDelete.push_back( NodeModifierMapping( pBillboardNode, pBillboardModifier ) );
					}

					for ( NodeModifierMappingList::iterator mappingIter = toDelete.begin(); mappingIter != toDelete.end(); ++mappingIter )
					{
						GetCOREInterface7()->DeleteModifier( *mappingIter->GetNode(), *mappingIter->GetModifier() );
					}
					break;
				}
			}

			// on delete node with a billboard modifier
			{
				int billboardId = pBillboardWSMObject->GetBillboardNodeId( pDeletedNode );
				if ( billboardId != -1 )
				{
					pBillboardWSMObject->DeleteBillboardNode( billboardId );
					break;
				}
			}
			break;
		}
	}
}

ICBillboardWSMObject::ICBillboardWSMObject()
: m_frontVectorSubObject( 13 )
, m_upVectorSubObject( 13 )
{
	pblock2 = NULL;
	GetMyClassDesc()->MakeAutoParamBlocks( this );
	assert( pblock2 );

	m_frontVectorSubObject.SetName( k_frontVectorSubObjectName );
	m_upVectorSubObject.SetName( k_upVectorSubObjectName );

	RegisterNotification( &ICBillboardWSMObject::NotificationCallback, this, NOTIFY_SCENE_PRE_DELETED_NODE );
	RegisterNotification( &ICBillboardWSMObject::NotificationCallback, this, NOTIFY_SCENE_ADDED_NODE );
}

ICBillboardWSMObject::~ICBillboardWSMObject()
{
	UnRegisterNotification( &ICBillboardWSMObject::NotificationCallback, this );
}

Modifier *ICBillboardWSMObject::CreateWSMMod( INode *node )
{
	ICBillboardWSM *pBillboardWSM = new ICBillboardWSM;
	pBillboardWSM->SetBillboardWSMObjectPtr( this );
	return pBillboardWSM;
}

RefTargetHandle ICBillboardWSMObject::Clone(RemapDir& remap) 
{
	ICBillboardWSMObject* newob = new ICBillboardWSMObject();

	CLONE_PARAMETER( newob, m_linksMesh );
	CLONE_PARAMETER( newob, m_frontVectorMesh );
	CLONE_PARAMETER( newob, m_upVectorMesh );
	CLONE_PARAMETER( newob, m_previousBillboardNodeList );

	newob->ReplaceReference(0, remap.CloneRef( pblock2 ) );
	newob->ClearBillboardNodes();

	BaseClone(this, newob, remap);
	return newob;
}

void ICBillboardWSMObject::GetWorldBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	Matrix3 objectTM = GetObjectTM( pNode, t );
	const int k_numBoxVertices = 8;
	const int k_numElements = 3;

	Box3 linksBox = ComputeLinksBox( t, pNode );

	Point3 worldMin( FLT_MAX, FLT_MAX, FLT_MAX );
	Point3 worldMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

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

			ICBillboardWSM *pBillboardWSM = GetBillboardModifier( pCurrentNode );
			FUAssert( pBillboardWSM, continue );
			Matrix3 rotMatrix = pBillboardWSM->GetRotationMatrix();
			
			Matrix3 tm;
			//Matrix3 currentObjectTM = pCurrentNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pCurrentNode->GetNodeTM( t );

			if ( IsDisplayBillboardedTargetsEnabled() )
			{
				currentObjectTM = rotMatrix * currentObjectTM;
			}

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

int ICBillboardWSMObject::HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt )
{
	GraphicsWindow *pGraphicsWindow = vpt->getGW();

	HitRegion hr; 
	MakeHitRegion( hr, type, crossing, 4, p ); 
	Matrix3 objectTM = GetObjectTM( inode, t );

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

	//if ( IsDisplayVectorsOnTargetsEnabled() )
	//{
	//	int billboardNodeCount = GetBillboardNodeCount();
	//	for ( int billboardNodeId = 0; billboardNodeId < billboardNodeCount; ++billboardNodeId )
	//	{
	//		INode *pNode = GetBillboardNode( billboardNodeId );
	//		if ( !pNode || pNode->IsHidden() )
	//		{
	//			continue;
	//		}

	//		ICBillboardWSM *pBillboardWSM = GetBillboardModifier( pNode );
	//		FUAssert( pBillboardWSM, continue );
	//		Matrix3 rotMatrix = pBillboardWSM->GetRotationMatrix();
	//		
	//		//Matrix3 currentObjectTM = pNode->GetObjectTM( t );
	//		Matrix3 currentObjectTM = pNode->GetNodeTM( t );

	//		if ( IsDisplayBillboardedTargetsEnabled() )
	//		{
	//			currentObjectTM = rotMatrix * currentObjectTM;
	//		}

	//		Matrix3 tm;
	//		GetBillboardTargetVectorsTM( vpt, currentObjectTM, tm );
	//		pGraphicsWindow->setTransform( tm );
	//		if ( m_frontVectorMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
	//		{
	//			return TRUE;
	//		}
	//		if ( m_upVectorMesh.select( pGraphicsWindow, pGraphicsWindow->getMaterial(), &hr ) )
	//		{
	//			return TRUE;
	//		}
	//	}
	//}

	return FALSE;
}

ICBillboardWSM *ICBillboardWSMObject::GetBillboardModifier( INode *pNode )
{
	if ( !pNode )
	{
		return NULL;
	}

	ICBillboardWSM *pBillboardWSM = NULL;
	IDerivedObject *pDerivedObject = pNode->GetWSMDerivedObject();
	int modifiersCount = pDerivedObject->NumModifiers();
	for ( int modifierId = 0; modifierId < modifiersCount; ++modifierId )
	{
		Modifier *pModifier = pDerivedObject->GetModifier( modifierId );
		if ( !pModifier )
		{
			continue;
		}

		if ( pModifier->ClassID() == IC_BILLBOARD_MODIFIER_CLASSID )
		{
			pBillboardWSM = ( ICBillboardWSM * )pModifier;	
		}
	}

	return pBillboardWSM;
}

int ICBillboardWSMObject::Display( TimeValue t, INode* inode, ViewExp *vpt, int flags )
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
			Point3 color = /*( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :*/
				(( inode->Selected() && subObjectLevel == 1 ) ? GetSubSelColor() :  
				(( inode->Selected() && subObjectLevel == 2 ) ? GetFreezeColor() :  
				k_frontVectorColor ));

			pGraphicsWindow->setColor( LINE_COLOR, color );
			m_frontVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
		}

		{
			Point3 color = /*( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :*/
				(( inode->Selected() && subObjectLevel == 2 ) ? GetSubSelColor() :  
				(( inode->Selected() && subObjectLevel == 1 ) ? GetFreezeColor() :  
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

			ICBillboardWSM *pBillboardWSM = GetBillboardModifier( pNode );
			FUAssert( pBillboardWSM, continue );
			Matrix3 rotMatrix = pBillboardWSM->GetRotationMatrix();

			//Matrix3 currentObjectTM = pNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pNode->GetNodeTM( t );

			if ( IsDisplayBillboardedTargetsEnabled() )
			{
				Point3 currentObjectPivotPos = currentObjectTM.GetTrans();
				currentObjectTM.NoTrans();
				currentObjectTM.NoScale();

				currentObjectTM = currentObjectTM * rotMatrix;
				currentObjectTM.SetTrans( currentObjectPivotPos );
			}

			Matrix3 tm;
			GetBillboardTargetVectorsTM( vpt, currentObjectTM, tm );
			//tm->
			pGraphicsWindow->setTransform( tm );
			int subObjectLevel = GetSubObjectLevel();

			{
				Point3 color = /*( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :*/
					(( inode->Selected() && subObjectLevel == 1 ) ? GetSubSelColor() :  
					(( inode->Selected() && subObjectLevel == 2 ) ? GetFreezeColor() :  
					k_frontVectorColor ));

				pGraphicsWindow->setColor( LINE_COLOR, color );
				m_frontVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
			}

			{
				Point3 color = /*( inode->Selected() && subObjectLevel == 0 ) ? GetSelColor() :*/
					(( inode->Selected() && subObjectLevel == 2 ) ? GetSubSelColor() :  
					(( inode->Selected() && subObjectLevel == 1 ) ? GetFreezeColor() :  
					k_upVectorColor ));

				pGraphicsWindow->setColor( LINE_COLOR, color );
				m_upVectorMesh.render( pGraphicsWindow, pGraphicsWindow->getMaterial(), TEST_FLAG( flags, USE_DAMAGE_RECT ) ? &vpt->GetDammageRect() : NULL, COMP_ALL );
			}
		}
	}

	pGraphicsWindow->setRndLimits( rlim );

	return 0;
}

void ICBillboardWSMObject::TransformToDisplayedVector( Point3 &v, ViewExp *vpt )
{
	v = v.Normalize();
}

void ICBillboardWSMObject::BuildVectorMesh( TimeValue t, ViewExp *vpt, Mesh &mesh, const Point3 &vector )
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

void ICBillboardWSMObject::BuildVectorMeshes( TimeValue t, ViewExp *vpt )
{
	Point3 frontVector;
	GetFrontVector( t, frontVector );
	BuildVectorMesh( t, vpt, m_frontVectorMesh, frontVector );

	Point3 upVector;
	GetUpVector( t, upVector );
	BuildVectorMesh( t, vpt, m_upVectorMesh, upVector );
}

void ICBillboardWSMObject::BuildMesh( TimeValue t, INode *pNode, ViewExp *vpt )
{
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



void ICBillboardWSMObject::GetLinksTM( const Matrix3 &objectTM, Matrix3 &tm )
{
	tm.IdentityMatrix();
	tm.SetTrans( objectTM.GetTrans() );
}

void ICBillboardWSMObject::GetVectorsGizmoTM( ViewExp *vpt, const Matrix3 &objectTM, Matrix3 &tm )
{
	tm.IdentityMatrix();
	float screenScaleFactor = vpt->GetScreenScaleFactor( objectTM.GetTrans() );
	screenScaleFactor /= 5;
	tm.SetScale( Point3( screenScaleFactor, screenScaleFactor, screenScaleFactor ) );
	tm.SetTrans( objectTM.GetTrans() );
	//float vpWorldWidth = vpt->GetVPWorldWidth( tm.GetTrans() );
}

void ICBillboardWSMObject::GetBillboardTargetVectorsTM( ViewExp *vpt, const Matrix3 &objectTM, Matrix3 &tm )
{
	tm = objectTM;
	tm.NoScale();
	tm.NoTrans();
	
	float screenScaleFactor = vpt->GetScreenScaleFactor( objectTM.GetTrans() );
	screenScaleFactor /= 5;
	tm.Scale( Point3( screenScaleFactor, screenScaleFactor, screenScaleFactor ) );
	tm.SetTrans( objectTM.GetTrans() );
}

Matrix3 ICBillboardWSMObject::GetObjectTM( INode *pNode, TimeValue t )
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

void ICBillboardWSMObject::GetLocalBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box )
{
	Matrix3 objectTM = GetObjectTM( pNode, t );

	Point3 localMin( FLT_MAX, FLT_MAX, FLT_MAX );
	Point3 localMax( -FLT_MAX, -FLT_MAX, -FLT_MAX );

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

			ICBillboardWSM *pBillboardWSM = GetBillboardModifier( pCurrentNode );
			FUAssert( pBillboardWSM, continue );
			Matrix3 rotMatrix = pBillboardWSM->GetRotationMatrix();
			
			Matrix3 tm;
			//Matrix3 currentObjectTM = pCurrentNode->GetObjectTM( t );
			Matrix3 currentObjectTM = pCurrentNode->GetNodeTM( t );

			if ( IsDisplayBillboardedTargetsEnabled() )
			{
				currentObjectTM = rotMatrix * currentObjectTM;
			}

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

void ICBillboardWSMObject::BeginEditParams( IObjParam *ip, ULONG flags, Animatable *prev )
{
	GetMyClassDesc()->BeginEditParams( ip, this, flags, prev );

	// sub object selection
	{
		s_pMoveModBox = new MoveModBoxCMode( this, ip );
		s_pRotateModBox = new RotateModBoxCMode( this, ip );
		s_pUScaleModBox = new UScaleModBoxCMode( this, ip );
		s_pNUScaleModBox = new NUScaleModBoxCMode( this, ip );
		s_pSquashModBox = new SquashModBoxCMode( this, ip );
		s_pSelectModBox = new SelectModBoxCMode( this, ip );

		//const MCHAR *types[] = 
		//{ 
		//	k_frontVectorSubObjectName
		//	, k_upVectorSubObjectName
		//};

		//ip->RegisterSubObjectTypes( types, 2 );
	}
}

void ICBillboardWSMObject::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{		
	GetMyClassDesc()->EndEditParams( ip, this, flags, next );

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

Interval ICBillboardWSMObject::ObjectValidity( TimeValue t )
{ 
	Interval validityInterval = WSMObject::ObjectValidity( t );
	
	if ( pblock2 )
	{
		pblock2->GetValidity( t, validityInterval );
	}

	return validityInterval;
}

ObjectState ICBillboardWSMObject::Eval( TimeValue time )
{ 
	return ObjectState( this );
}

IOResult ICBillboardWSMObject::Load(ILoad *iload)
{
	//Modifier::Load(iload);
	//ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, NUM_OLDVERSIONS, &bend_param_blk, this, SIMPMOD_PBLOCKREF);
	iload->RegisterPostLoadCallback( new ICBillboardWSMObjectPostLoadCallback( this ) );
	return IO_OK;
}

void ICBillboardWSMObject::GetBillboardModifiers( BillboardModifierList &billboardModifiers )
{
	for ( size_t id = 0; id < GetBillboardNodeCount(); ++id )
	{
		INode *pNode = GetBillboardNode( id );
		ICBillboardWSM *pBillboardModifier = GetBillboardModifier( pNode );
		if ( !pBillboardModifier )
		{
			continue;
		}

		billboardModifiers.push_back( pBillboardModifier );
	}
}

void ICBillboardWSMObject::BillboardControllerPBAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	ICBillboardWSMObject *pBillboardWSMObject = (ICBillboardWSMObject*)owner;

//	//if ( id == PI_BILLBOARD_FRONT
//	//	|| id == PI_BILLBOARD_UP )
//	//{
//	//	Point3 &vector = *v.p;
//	//	float vectorLength = vector.Length();
//	//	if ( !IsEquivalent( vectorLength, 1.f ) )
//	//	{
//	//		Point3 normalizedVector = vector.Normalize();
//	//		pBillboardWSMObject->pblock2->SetValue( id, t, normalizedVector );
//	//		s_billboardWSMObjectParamBlockDesc.InvalidateUI( id );
//	//		return;
//	//	}
//	//}

	BillboardModifierList billboardModifiers;
	pBillboardWSMObject->GetBillboardModifiers( billboardModifiers );

	for ( BillboardModifierList::iterator modifierIter = billboardModifiers.begin(); modifierIter != billboardModifiers.end(); ++modifierIter )
	{
		ICBillboardWSM *pBillboardModifier = *modifierIter;
		pBillboardModifier->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
	}

	// This has the effect of ObjectValidity begin called, which will update the object's validity interval
	// As a consequence, the system will know when to redisplay the object.
	pBillboardWSMObject->ForceNotify( FOREVER );
}

Box3 ICBillboardWSMObject::ComputeLinksBox( TimeValue t, INode *pNode )
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

bool ICBillboardWSMObject::IsDisplayLinksEnabled()
{
	int val = 0;
	pblock2->GetValue( PI_DISPLAY_LINKS, 0, val, FOREVER );
	return val != 0;
}

bool ICBillboardWSMObject::IsDisplayBillboardedTargetsEnabled()
{
	int val = 0;
	pblock2->GetValue( PI_DISPLAY_BILLBOARDED_TARGETS, 0, val, FOREVER );
	return val != 0;
}

bool ICBillboardWSMObject::IsDisplayVectorsOnTargetsEnabled()
{
	int val = 0;
	pblock2->GetValue( PI_DISPLAY_VECTORS_ON_TARGETS, 0, val, FOREVER );
	return val != 0;
}

int ICBillboardWSMObject::GetBillboardNodeCount()
{
	//return 0;
	return pblock2->Count( PI_BILLBOARD_NODE_LIST );
}

INode *ICBillboardWSMObject::GetBillboardNode( int id )
{
	INode *pNode = NULL;
	pblock2->GetValue( PI_BILLBOARD_NODE_LIST, 0, pNode, FOREVER, id );
	return pNode;
	//return NULL;
}

int ICBillboardWSMObject::GetBillboardNodeId( INode *pNode )
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

void ICBillboardWSMObject::GetBillboardNodeIds( INode *pNode, UInt32List &indices )
{
	int count = GetBillboardNodeCount();
	
	for ( int id = 0; id < count; ++id )
	{
		if ( pNode == GetBillboardNode( id ) )
		{
			indices.push_back( id );
		}
	}
}

void ICBillboardWSMObject::DeleteBillboardNode( INode *pNode )
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

void ICBillboardWSMObject::DeleteBillboardNode( int id )
{
	pblock2->Delete( PI_BILLBOARD_NODE_LIST, id, 1 );
}

void ICBillboardWSMObject::GetCurrentBillboardNodeList( NodeList &nodeList )
{
	for ( int id = 0; id < GetBillboardNodeCount(); ++id )
	{
		nodeList.push_back( GetBillboardNode( id ) );
	}
}

void ICBillboardWSMObject::InsertBillboardNodes( int start, int count, INode **pNodes )
{
	pblock2->Insert( PI_BILLBOARD_NODE_LIST, start, count, pNodes );
}

void ICBillboardWSMObject::AppendBillboardNodes( int count, INode **pNodes )
{
	pblock2->Append( PI_BILLBOARD_NODE_LIST, count, pNodes );
}

void ICBillboardWSMObject::ClearBillboardNodes()
{
	pblock2->ZeroCount( PI_BILLBOARD_NODE_LIST );
}

void ICBillboardWSMObject::ICBillboardWSMObjectPostLoadCallback::proc( ILoad *iload )
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

			ColladaModifierStack foundModifiers;
			{
				ColladaModifierStack targetModifiers;
				targetModifiers.push_back( new ICBillboardModifier() );
				ColladaModifierManager::ResolveModifiers( pNode, pNode->GetWSMDerivedObject(), targetModifiers, foundModifiers );
				CLEAR_POINTER_VECTOR( targetModifiers );
			}

			FUAssert( foundModifiers.size() == 1, );

			for ( ColladaModifierStack::iterator colladaModifierIter = foundModifiers.begin(); colladaModifierIter != foundModifiers.end(); ++colladaModifierIter )
			{
				ICBillboardModifier *pBillboardColladaModifier = ( ICBillboardModifier * )*colladaModifierIter;
				ICBillboardWSM *pBillboardModifier = pBillboardColladaModifier->GetInterface();
				pBillboardModifier->SetBillboardWSMObjectPtr( m_pOwner );
			}

			CLEAR_POINTER_VECTOR( foundModifiers );
		}
	}
}

int ICBillboardWSMObject::NumSubObjTypes()
{
#if ENABLE_BILLBOARD_VECTOR_UI
	return 2;
#else
	return 0;
#endif
}

ISubObjType *ICBillboardWSMObject::GetSubObjType( int id )
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

void ICBillboardWSMObject::ActivateSubobjSel( int level, XFormModes &modes )
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

void ICBillboardWSMObject::GetSubObjectTMs( SubObjAxisCallback *cb, TimeValue  t, INode *node, ModContext *mc )
{
	Matrix3 objectTM = GetObjectTM( node, t );
	for ( int i = 0; i < COORDS_GIMBAL + 1; ++i )
	{
		cb->TM( objectTM, i );
	}
}

void ICBillboardWSMObject::GetSubObjectCenters( SubObjAxisCallback *cb, TimeValue  t, INode *node, ModContext *mc )
{
	Matrix3 objectTM = GetObjectTM( node, t );
	Point3 center = objectTM.GetTrans();
	for ( int i = 0; i < COORDS_GIMBAL + 1; ++i )
	{
		cb->Center( center, i );
	}
}

void ICBillboardWSMObject::Rotate( TimeValue t, Matrix3 &partm, Matrix3 &tmAxis, Quat &val, BOOL localOrigin )
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

		s_billboardParamBlockDesc.InvalidateUI();
	}
	else if ( subObjectLevel == 2 )
	{
		Point3 upVector;
		GetUpVector( t, upVector );

		upVector = tm.VectorTransform( upVector );
		upVector = upVector.Normalize();
		SetUpVector( t, upVector );

		s_billboardParamBlockDesc.InvalidateUI();	
	}

	//tmControl->SetValue( t, &pckt, TRUE, CTRL_RELATIVE );
	//macroRecorder->OpAssign(_T("+="), mr_prop, _T("gizmo.rotation"), mr_reftarg, this, mr_quat, &val);
}

void ICBillboardWSMObject::GetFrontVector( TimeValue t, Point3 &v )
{
	pblock2->GetValue( PI_BILLBOARD_FRONT, t, v, FOREVER );
}

void ICBillboardWSMObject::GetUpVector( TimeValue t, Point3 &v )
{
	pblock2->GetValue( PI_BILLBOARD_UP, t, v, FOREVER );
}

void ICBillboardWSMObject::SetFrontVector( TimeValue t, Point3 v )
{
	pblock2->SetValue( PI_BILLBOARD_FRONT, t, v );
}

void ICBillboardWSMObject::SetUpVector( TimeValue t, Point3 v )
{
	pblock2->SetValue( PI_BILLBOARD_UP, t, v );
}

void ICBillboardWSMObject::SetBillboardType( int type )
{
	pblock2->SetValue( PI_BILLBOARD_TYPE, 0, type );
}

void ICBillboardWSMObject::GetBillboardType( int &type )
{
	pblock2->GetValue( PI_BILLBOARD_TYPE, 0, type, FOREVER );
}

void ICBillboardWSMObject::SetBillboardOrientation( int orientation )
{
	pblock2->SetValue( PI_BILLBOARD_ORIENTATION, 0, orientation );
}

void ICBillboardWSMObject::GetBillboardOrientation( int &orientation )
{
	pblock2->GetValue( PI_BILLBOARD_ORIENTATION, 0, orientation, FOREVER );
}

ICBillboardWSM::ICBillboardWSM()
: m_preViewportDisplayCallback( this )
, m_rotationMatrix( 1 )
, m_pBillboardWSMObject( NULL )
, m_bIsClosingFile( false )
{	
	//obRef   = NULL;
	//nodeRef = NULL;
	//pblock2  = NULL;
	GetCOREInterface()->RegisterViewportDisplayCallback( true, &m_preViewportDisplayCallback );

	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_PRE_MODIFIER_ADDED );
	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_POST_MODIFIER_ADDED );
	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_PRE_MODIFIER_DELETED );
	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_NODE_CLONED );

	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_SYSTEM_PRE_RESET );
	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_SYSTEM_PRE_NEW );
	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_FILE_PRE_OPEN );
	RegisterNotification( &ICBillboardWSM::NotificationCallback, this, NOTIFY_SYSTEM_SHUTDOWN );
}

ICBillboardWSM::~ICBillboardWSM()
{	
	//DeleteAllRefsFromMe();
	GetCOREInterface()->UnRegisterViewportDisplayCallback( true, &m_preViewportDisplayCallback );
	UnRegisterNotification( &ICBillboardWSM::NotificationCallback, this );
}

//IParamArray *ICBillboardWSM::GetParamBlock()
//{
//	return pblock2;
//}
//
//int ICBillboardWSM::GetParamBlockIndex(int id)
//	{
//	if (pblock2 && id>=0 && id<pblock2->NumParams()) return id;
//	else return -1;
//	}

//RefTargetHandle ICBillboardWSM::GetReference(int i) 
//	{ 
//	switch (i) {		
//		case IC_BILLBOARD_WSM_MOD_OBREF: 		return obRef;
//		case IC_BILLBOARD_WSM_MOD_NODEREF:	return nodeRef;
//		case IC_BILLBOARD_WSM_MOD_PBLOCKREF: 	return pblock2;
//		default: return NULL;
//		}
//	}
//
//void ICBillboardWSM::SetReference(int i, RefTargetHandle rtarg) 
//	{ 
//	switch (i) {
//		case IC_BILLBOARD_WSM_MOD_OBREF:		obRef   = (WSMObject*)rtarg; break;
//		case IC_BILLBOARD_WSM_MOD_NODEREF:	nodeRef = (INode*)rtarg; break;
//		case IC_BILLBOARD_WSM_MOD_PBLOCKREF: 	pblock2  = (IParamBlock2*)rtarg; break;
//		}
//	}
//
//Animatable* ICBillboardWSM::SubAnim(int i) 
//	{ 
//	switch (i) {
//		case 0: return pblock2;		
//		default: return NULL;
//		}
//	}
//
//TSTR ICBillboardWSM::SubAnimName(int i) 
//	{ 
//	switch (i) {
//		case 0: return TSTR("Parameters");
//		//case 0: return TSTR(GetResString(IDS_RB_PARAMETERS));
//		default: return TSTR(_T(""));
//		}	
//	}

Interval ICBillboardWSM::LocalValidity(TimeValue t)
{
	// if being edited, return NEVER forces a cache to be built 
	// after previous modifier.
	if ( TestAFlag( A_MOD_BEING_EDITED ) )
	{
		return NEVER;
	}

	return GetValidity(t);
}

RefTargetHandle ICBillboardWSM::Clone(RemapDir& remap) 
{
	ICBillboardWSM* newob = new ICBillboardWSM();

	CLONE_PARAMETER( newob, m_rotationMatrix );
	CLONE_PARAMETER( newob, m_pBillboardWSMObject );
	CLONE_PARAMETER( newob, m_bIsClosingFile );

	//newob->ReplaceReference(0, remap.CloneRef( pblock2 ) );
	//if ( smodSource->pblock2 ) ReplaceReference(IC_BILLBOARD_WSM_MOD_PBLOCKREF,remap.CloneRef(smodSource->pblock2));
	//ReplaceReference(IC_BILLBOARD_WSM_MOD_OBREF,smodSource->obRef);
	//ReplaceReference(IC_BILLBOARD_WSM_MOD_NODEREF,smodSource->nodeRef);

	BaseClone(this, newob, remap);
	return newob;
}

void ICBillboardWSM::NotificationCallback( void *param, NotifyInfo *info )
{
	ICBillboardWSM *pBillboardModifier = ( ICBillboardWSM * )param;

	switch ( info->intcode )
	{
		case NOTIFY_PRE_MODIFIER_ADDED:
		{
			MAXHelpers::NotifyModifierObject *pNotifyModifierObject = ( MAXHelpers::NotifyModifierObject *)info->callParam;
			if ( !ValidateNotifyModifierObject( pNotifyModifierObject ) )
			{
				break;
			}

			if ( pBillboardModifier != pNotifyModifierObject->pModifier )
			{
				break;
			}

			ICBillboardWSMObject *pBillboardWSMObject = pBillboardModifier->GetBillboardWSMObjectPtr();
			pBillboardWSMObject->AppendBillboardNodes( 1, &pNotifyModifierObject->pNode );
			break;
		}
		case NOTIFY_POST_MODIFIER_ADDED:
		{
			MAXHelpers::NotifyModifierObject *pNotifyModifierObject = ( MAXHelpers::NotifyModifierObject *)info->callParam;
			if ( !ValidateNotifyModifierObject( pNotifyModifierObject ) )
			{
				break;
			}

			if ( pBillboardModifier != pNotifyModifierObject->pModifier )
			{
				break;
			}

			// if more than one billboard modifier, remove this one
			INode *pBillboardNode = pNotifyModifierObject->pNode;

			ColladaModifierStack foundModifiers;
			{
				ColladaModifierStack targetModifiers;
				targetModifiers.push_back( new ICBillboardModifier() );
				ColladaModifierManager::ResolveModifiers( pBillboardNode, pBillboardNode->GetWSMDerivedObject(), targetModifiers, foundModifiers );
				CLEAR_POINTER_VECTOR( targetModifiers );
			}

			size_t foundModifiersCount = foundModifiers.size();
			CLEAR_POINTER_VECTOR( foundModifiers );

			if ( foundModifiersCount <= 1 )
			{
				break;
			}

			GetCOREInterface7()->DeleteModifier( *pBillboardNode, *pNotifyModifierObject->pModifier );
			break;
		}
		case NOTIFY_PRE_MODIFIER_DELETED:
		{
			MAXHelpers::NotifyModifierObject *pNotifyModifierObject = ( MAXHelpers::NotifyModifierObject *)info->callParam;
			if ( !ValidateNotifyModifierObject( pNotifyModifierObject ) )
			{
				break;
			}

			if ( pBillboardModifier != pNotifyModifierObject->pModifier )
			{
				break;
			}

			ICBillboardWSMObject *pBillboardWSMObject = pBillboardModifier->GetBillboardWSMObjectPtr();
			pBillboardWSMObject->DeleteBillboardNode( pNotifyModifierObject->pNode );
			break;
		}
		case NOTIFY_NODE_CLONED:
		{
			INode *pNode = ( INode * )info->callParam;
			if ( !pNode )
			{
				break;
			}

			ColladaModifierStack foundModifiers;
			{
				ColladaModifierStack targetModifiers;
				targetModifiers.push_back( new ICBillboardModifier() );
				ColladaModifierManager::ResolveModifiers( pNode, pNode->GetWSMDerivedObject(), targetModifiers, foundModifiers );
				CLEAR_POINTER_VECTOR( targetModifiers );
			}

			ICBillboardWSMObject *pBillboardModifierObject = NULL;
			ICBillboardWSM *pCurrentBillboardModifier = NULL;
			for ( ColladaModifierStack::iterator colladaModifierIter = foundModifiers.begin(); colladaModifierIter != foundModifiers.end() && pBillboardModifier != pCurrentBillboardModifier; ++colladaModifierIter )
			{
				ICBillboardModifier *pBillboardColladaModifier = ( ICBillboardModifier * )*colladaModifierIter;
				pCurrentBillboardModifier = pBillboardColladaModifier->GetInterface();
				pBillboardModifierObject = pCurrentBillboardModifier->GetBillboardWSMObjectPtr();
			}
			CLEAR_POINTER_VECTOR( foundModifiers );

			if ( pBillboardModifier != pCurrentBillboardModifier 
				|| !pBillboardModifierObject )
			{
				break;
			}

			pBillboardModifierObject->AppendBillboardNodes( 1, &pNode );
			break;
		}
		case NOTIFY_SYSTEM_PRE_RESET :
		case NOTIFY_SYSTEM_PRE_NEW :
		case NOTIFY_FILE_PRE_OPEN :
		case NOTIFY_SYSTEM_SHUTDOWN :
		{
			pBillboardModifier->SetIsClosingFile( true );
			break;
		}
	}
}

void ICBillboardWSM::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{	
	m_rotationMatrix.IdentityMatrix();

	if ( !GetBillboardWSMObjectPtr() ||
		 !IsDisplayBillboardedTargetsEnabled() 
		 || !node )
	{
		return;
	}

	Interval valid = GetValidity( t );

	Matrix3 pivotTM = node->GetNodeTM( t );
	Point3 pivotNodeTranslation = pivotTM.GetTrans();
	pivotTM.NoTrans();
	pivotTM.NoScale();

	Matrix3 nodeWorldTM = node->GetObjTMBeforeWSM( t );
	//Matrix3 nodeInverseWorldTM = nodeWorldTM;
	//nodeInverseWorldTM.Invert();
	Point3 nodeTranslation = nodeWorldTM.GetTrans();
	nodeWorldTM.NoScale();
	nodeWorldTM.NoTrans();
	nodeWorldTM.Invert();

	Matrix3 camTM;
	bool bIsPerspectiveProjection = false;
	Point3 camRightV,camUpV,camBackV,camPosition;
	{
		ViewExp *pViewport = GetCOREInterface()->GetActiveViewport();
		bIsPerspectiveProjection = pViewport->IsPerspView();
		pViewport->GetAffineTM( camTM );
		GetCOREInterface()->ReleaseViewport( pViewport );

		camTM.Invert();
		camRightV = camTM.GetRow(0);
		camUpV = camTM.GetRow(1);
		camBackV = camTM.GetRow(2);
		camPosition = camTM.GetRow(3);
	}

	// orientation
	Point3 billboardFrontV;
	Point3 billboardUpV;
	Point3 billboardRightV;

	{
		Point3 xdir, ydir, zdir;
		Point3 source_to_upnode_vector;

		if ( GetBillboardWSMObjectPtr() && 
			 IsDisplayBillboardedTargetsEnabled() )
		{
			int billboardType;
			GetBillboardType( billboardType );

			int billboardOrientation;
			GetBillboardOrientation( billboardOrientation );

			Point3 origFrontVector;
			GetFrontVector( t, origFrontVector );
			origFrontVector = pivotTM.VectorTransform( origFrontVector );
			origFrontVector = origFrontVector.Normalize();

			Point3 origUpVector;
			GetUpVector( t, origUpVector );
			origUpVector = pivotTM.VectorTransform( origUpVector );
			origUpVector = origUpVector.Normalize();

			if ( billboardType == FUDaeBillboard::BT_FREE || billboardType == FUDaeBillboard::BT_ROTATION )
			{
				Matrix3 billboardTM( 1 );

				Point3 origRightVector = ( origUpVector ^ origFrontVector ).Normalize();
				Point3 origUpPrimeVector = ( origFrontVector ^ origRightVector ).Normalize();

				m_rotationMatrix.SetRow( 0, origRightVector );
				m_rotationMatrix.SetRow( 1, origUpPrimeVector );
				m_rotationMatrix.SetRow( 2, origFrontVector );

				m_rotationMatrix.Invert();

				if ( billboardOrientation == FUDaeBillboard::OT_PARALLEL_CAM_AXIS || !bIsPerspectiveProjection )
				{
					billboardFrontV = camBackV;
					billboardUpV = camUpV;
					billboardRightV = camRightV;
				}
				else if ( billboardOrientation == FUDaeBillboard::OT_TOWARD_CAMERA )
				{
					//billboardFrontV = ( camPosition - nodeTranslation ).Normalize();
					billboardFrontV = ( camPosition - pivotNodeTranslation ).Normalize();
					//billboardRightV = ( billboardFrontV ^ camUpV ).Normalize();
					billboardRightV = ( camUpV ^ billboardFrontV ).Normalize();
					billboardUpV = ( billboardFrontV ^ billboardRightV ).Normalize();
					//billboardUpV = ( billboardRightV ^ billboardFrontV ).Normalize();
				}			

				billboardTM.SetRow( 0, billboardRightV );
				billboardTM.SetRow( 1, billboardUpV );
				billboardTM.SetRow( 2, billboardFrontV );

				m_rotationMatrix = m_rotationMatrix * billboardTM;
			}
			//else if ( billboardType == FUDaeBillboard::BT_ROTATION )
			//{
			//}
			else if ( billboardType == FUDaeBillboard::BT_UP_ONLY )
			{
				if ( billboardOrientation == FUDaeBillboard::OT_PARALLEL_CAM_AXIS || !bIsPerspectiveProjection )
				{
					billboardFrontV = camBackV;
				}
				else if ( billboardOrientation == FUDaeBillboard::OT_TOWARD_CAMERA )
				{
					//billboardFrontV = ( camPosition - nodeTranslation ).Normalize();
					billboardFrontV = ( camPosition - pivotNodeTranslation ).Normalize();
				}

				Point3 billboardFrontVOnPlane = ( origUpVector ^ ( billboardFrontV ^ origUpVector ) ).Normalize();
				Point3 origFrontVectorOnPlane = ( origUpVector ^ ( origFrontVector ^ origUpVector ) ).Normalize();

				float radians = acos( billboardFrontVOnPlane % origFrontVectorOnPlane );
				radians = origUpVector % ( billboardFrontVOnPlane ^ origFrontVectorOnPlane ) < 0 ? radians : -radians;

				m_rotationMatrix.SetAngleAxis( origUpVector, radians );
			}
		}
	}

	{
		int nv = os->obj->NumPoints(); 
		for ( int i = 0;  i < nv; i++ )
		{
			Point3 origPoint = os->obj->GetPoint( i );

			Point3 destPoint = origPoint - pivotNodeTranslation;
			//Point3 destPoint = origPoint - nodeTranslation;
			destPoint = m_rotationMatrix.PointTransform( destPoint );
			destPoint += pivotNodeTranslation;
			//destPoint += nodeTranslation;

			//destPoint += pivotNodeTranslation - nodeTranslation;
			//destPoint += billboardFrontV * ( pivotNodeTranslation - nodeTranslation ).Length();
			
			os->obj->SetPoint( i, destPoint );
		}
		os->obj->PointsWereChanged();
	}

	os->obj->UpdateValidity( GEOM_CHAN_NUM, valid );
}

ClassDesc2 *ICBillboardWSM::GetMyClassDesc()
{ 
	static MyClassDesc myClassDesc;
	return &myClassDesc;
}

void ICBillboardWSM::ViewportDisplayCallbackObject::Display( TimeValue t, ViewExp *vpt, int flags )
{
	if ( !m_pOwner || m_pOwner->GetIsClosingFile() )
	{
		return;
	}

	m_pOwner->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
	ICBillboardWSMObject *pBillboardModifierObject = m_pOwner->GetBillboardWSMObjectPtr();
	if ( pBillboardModifierObject )
	{
		pBillboardModifierObject->ForceNotify( FOREVER );
	}
}

void ICBillboardWSM::ViewportDisplayCallbackObject::GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect )
{

}

BOOL ICBillboardWSM::ViewportDisplayCallbackObject::Foreground()
{
	return TRUE;
}

void ICBillboardWSM::GetFrontVector( TimeValue t, Point3 &v )
{
	ICBillboardWSMObject *pBillboardWSMObject = GetBillboardWSMObjectPtr();
	FUAssert( pBillboardWSMObject, return );
	
	pBillboardWSMObject->GetFrontVector( t, v );
}

void ICBillboardWSM::GetUpVector( TimeValue t, Point3 &v )
{
	ICBillboardWSMObject *pBillboardWSMObject = GetBillboardWSMObjectPtr();
	FUAssert( pBillboardWSMObject, return );
	
	pBillboardWSMObject->GetUpVector( t, v );
}

void ICBillboardWSM::GetBillboardType( int &type )
{
	ICBillboardWSMObject *pBillboardWSMObject = GetBillboardWSMObjectPtr();
	FUAssert( pBillboardWSMObject, return );
	
	pBillboardWSMObject->GetBillboardType( type );
}

void ICBillboardWSM:: GetBillboardOrientation( int &orientation )
{
	ICBillboardWSMObject *pBillboardWSMObject = GetBillboardWSMObjectPtr();
	FUAssert( pBillboardWSMObject, return );
	
	pBillboardWSMObject->GetBillboardOrientation( orientation );
}

bool ICBillboardWSM::IsDisplayBillboardedTargetsEnabled()
{
	ICBillboardWSMObject *pBillboardWSMObject = GetBillboardWSMObjectPtr();
	FUAssert( pBillboardWSMObject, return false );
	
	return pBillboardWSMObject->IsDisplayBillboardedTargetsEnabled();
}