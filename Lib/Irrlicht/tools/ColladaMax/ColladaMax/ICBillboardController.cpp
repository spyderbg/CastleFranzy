/**********************************************************************
 *
	FILE: lookat_cnstrnt.cpp

	DESCRIPTION: A rotation constraint controller that controls the lookat 
				of an object based on the lookat of the target objects

	CREATED BY: Ambarish Goswami 5/2000
	            

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "stdafx.h"
#include "ICBillboardController.h"
#include "ICBillboardHelper.h"
#include "MAXHelpers.h"
#include "UIHelpers.h"
#include "FUtils/FUDaeEnum.h"
#include "FUtils/FUParameter.h"

void ICBillboardController::MyTimeChangeCallback::TimeChanged(TimeValue t)
{
}

static ICBillboardController::MyClassDesc lookAtCD;
ClassDesc* GetICBillboardControllerDesc()
{
	return &lookAtCD;
}

#pragma warning(disable:4355)

ICBillboardController::ICBillboardController(BOOL loading) 
: m_preViewportDisplayCallback( this )
, m_pBillboardHelper( NULL )
{
	curRot.Identity();
	
	initialLookAtFlag = 0;
	baseRotQuatLocal.Identity();
	baseRotQuatWorld.Identity();
	userRotQuat.Identity();
	sourceTM.IdentityMatrix();

	GetCOREInterface()->RegisterViewportDisplayCallback( true, &m_preViewportDisplayCallback );
}

#pragma warning(default:4355)

ICBillboardController::~ICBillboardController()
{
	GetCOREInterface()->UnRegisterViewportDisplayCallback( true, &m_preViewportDisplayCallback );
}

RefTargetHandle ICBillboardController::Clone(RemapDir& remap)
{
	ICBillboardController *p = new ICBillboardController(TRUE);

	p->curRot     = curRot;
	p->baseRotQuatLocal  = baseRotQuatLocal;
	p->baseRotQuatWorld  = baseRotQuatWorld;
	p->userRotQuat = userRotQuat;
	CLONE_PARAMETER( p, m_pBillboardHelper );

	if ( m_pBillboardHelper )
	{
		RegisterNotification( &ICBillboardController::NotificationCallback, p, NOTIFY_NODE_CLONED );
	}

	BaseClone(this, p, remap);
	return p;
}

void ICBillboardController::NotificationCallback( void *param, NotifyInfo *info )
{
	switch ( info->intcode )
	{
		case NOTIFY_NODE_CLONED :
		{
			ICBillboardController *pBillboardController = ( ICBillboardController * )param;
			ICBillboardHelper *pBillboardHelper = pBillboardController->GetHelperPtr();
			pBillboardHelper->AppendBillboardNodes( 1, ( INode ** )&info->callParam );

			UnRegisterNotification( &ICBillboardController::NotificationCallback, param, NOTIFY_NODE_CLONED );
			break;
		}
	}
}

void ICBillboardController::Copy(Control *from)
{
	Quat fvalRot;
	if (from->ClassID()==ClassID())
	{
		ICBillboardController *ctrl = (ICBillboardController*)from;
		curRot = ctrl->curRot;
	} else 
	{
		from->GetValue(GetCOREInterface()->GetTime(), &fvalRot, Interval(0, 0));	//  to know the object lookat before the
		initialLookAtFlag = 1;
		baseRotQuatLocal = fvalRot;
		baseRotQuatLocal.Normalize(); // current controller was active
	}
	
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
}

Quat ICBillboardController::ComputeLookAtQuat(TimeValue t)
{
	Matrix3 temp_mat(1);
	Point3 xdir, ydir, zdir;
	Point3 source_to_upnode_vector;

	temp_mat.IdentityMatrix();

	if ( GetHelperPtr() && 
		 IsDisplayBillboardedTargetsEnabled() )
	{
		int billboardType;
		GetBillboardType( billboardType );

		int billboardOrientation;
		GetBillboardOrientation( billboardOrientation );

		Point3 origFrontVector;
		GetFrontVector( t, origFrontVector );
		origFrontVector = origFrontVector.Normalize();

		Point3 origUpVector;
		GetUpVector( t, origUpVector );
		origUpVector = origUpVector.Normalize();

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

		if ( billboardType == FUDaeBillboard::BT_FREE || billboardType == FUDaeBillboard::BT_ROTATION )
		{
			// orientation
			Point3 billboardFrontV;
			Point3 billboardUpV;
			Point3 billboardRightV;

			Matrix3 billboardTM( 1 );

			Point3 origRightVector = ( origUpVector ^ origFrontVector ).Normalize();
			Point3 origUpPrimeVector = ( origFrontVector ^ origRightVector ).Normalize();

			temp_mat.SetRow( 0, origRightVector );
			temp_mat.SetRow( 1, origUpPrimeVector );
			temp_mat.SetRow( 2, origFrontVector );

			temp_mat.Invert();

			if ( billboardOrientation == FUDaeBillboard::OT_PARALLEL_CAM_AXIS || !bIsPerspectiveProjection )
			{
				billboardFrontV = camBackV;
				billboardUpV = camUpV;
				billboardRightV = camRightV;
			}
			else if ( billboardOrientation == FUDaeBillboard::OT_TOWARD_CAMERA )
			{
				billboardFrontV = ( camPosition - sourceTM.GetTrans() ).Normalize();
				billboardRightV = ( camUpV ^ billboardFrontV ).Normalize();
				billboardUpV = ( billboardFrontV ^ billboardRightV ).Normalize();
			}			

			billboardTM.SetRow( 0, billboardRightV );
			billboardTM.SetRow( 1, billboardUpV );
			billboardTM.SetRow( 2, billboardFrontV );

			temp_mat = temp_mat * billboardTM;
		}
		//else if ( billboardType == FUDaeBillboard::BT_ROTATION )
		//{
		//}
		else if ( billboardType == FUDaeBillboard::BT_UP_ONLY )
		{
			Point3 billboardFrontV;
			if ( billboardOrientation == FUDaeBillboard::OT_PARALLEL_CAM_AXIS || !bIsPerspectiveProjection )
			{
				billboardFrontV = camBackV;
			}
			else if ( billboardOrientation == FUDaeBillboard::OT_TOWARD_CAMERA )
			{
				billboardFrontV = ( camPosition - sourceTM.GetTrans() ).Normalize();
			}

			Point3 billboardFrontVOnPlane = ( origUpVector ^ ( billboardFrontV ^ origUpVector ) ).Normalize();
			Point3 origFrontVectorOnPlane = ( origUpVector ^ ( origFrontVector ^ origUpVector ) ).Normalize();

			float radians = acos( billboardFrontVOnPlane % origFrontVectorOnPlane );
			radians = origUpVector % ( billboardFrontVOnPlane ^ origFrontVectorOnPlane ) < 0 ? radians : -radians;

			temp_mat.SetAngleAxis( origUpVector, radians );
		}
	}

	Quat temp_quat;
	temp_quat.Set(temp_mat);
	temp_quat.MakeClosest(IdentQuat());
	temp_quat.Normalize();
	return temp_quat;
}

void ICBillboardController::Update( TimeValue t )
{
	curRot = ComputeLookAtQuat( t );
	curRot.Normalize();
}

void ICBillboardController::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
{
	HoldQuat();			// this is where the RotationRestoreObj is called
						// to properly manage the the object rotation in the set
						// orientation mode. Works in concert with RestoreObj 
						// and HoldQuat functions
	Quat deltaRotQuat;
	deltaRotQuat.Identity();
	if (method==CTRL_RELATIVE) {
		AngAxis *AngAx = (AngAxis*)val;
		deltaRotQuat = QFromAngAxis(AngAx->angle, AngAx->axis);
		userRotQuat *= deltaRotQuat;
		NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);	
	} 
	else //CTRL_ABSOLUTE
	{
		*((Quat*)val) = curRot;
	}
}


void ICBillboardController::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
	//ViewExp *pViewport = GetCOREInterface()->GetActiveViewport();
	//pViewport->GetAffineTM( 

 	sourceTM = * ( Matrix3 * )val;		// has only the relevant position part. The rotation part 
										// will be set by this constraint controller
	if( initialLookAtFlag )
	{
		Matrix3 tempMat( 1 );

		// CAL-9/26/2002: in absolute mode the value could be un-initialized (random value).
		if ( method == CTRL_RELATIVE )
		{
			tempMat = *(Matrix3*)val;
		}

		AffineParts comps;		// Requires header decomp.h
		decomp_affine(tempMat, &comps);
		baseRotQuatWorld = baseRotQuatLocal * comps.q;
		baseRotQuatWorld.Normalize();
		initialLookAtFlag = 0;
	}

	Update(t);
		
	Interval validityInterval = FOREVER;
	//if ( pblock2 )
	//{
	//	pblock2->GetValidity( t, validityInterval );
	//}

	valid &= validityInterval;	 
	if ( method == CTRL_RELATIVE )
	{
//			{
//				Matrix3 *mat = (Matrix3*)val;		// this is source_Parent_TM
//				Quat q = baseRotQuatLocal;			// this is target_world_TM
//				PreRotateMatrix(*mat, q);
//			}
//			else{
			Matrix3 *mat = (Matrix3*)val;
			// CAL-06/24/02: preserve all components and replace with the new orientation.
			AffineParts ap;
			decomp_affine( *mat, &ap );
			if ( GetHelperPtr() && !IsDisplayBillboardedTargetsEnabled() )
			{
				ap.q *= curRot;
			}
			else
			{
				ap.q = curRot;				// target world rotation
			}
			MAXHelpers::ComposeMatrix( ap, *mat );
			// CAL-06/24/02: this only preserve the translation component
			// Point3 tr; 
			// tr = mat->GetTrans();
			// Quat q = curRot;			
			// q.MakeMatrix(*mat);
			// mat->SetTrans(tr);	
//			}
	} 
	else 
	{
		*(( Quat* )val) = curRot;
	}

}

void ICBillboardController::GetFrontVector( TimeValue t, Point3 &v )
{
	ICBillboardHelper *pBillboardHelper = GetHelperPtr();
	FUAssert( pBillboardHelper, return );
	
	pBillboardHelper->GetFrontVector( t, v );
}

void ICBillboardController::GetUpVector( TimeValue t, Point3 &v )
{
	ICBillboardHelper *pBillboardHelper = GetHelperPtr();
	FUAssert( pBillboardHelper, return );
	
	pBillboardHelper->GetUpVector( t, v );
}

void ICBillboardController::GetBillboardType( int &type )
{
	ICBillboardHelper *pBillboardHelper = GetHelperPtr();
	FUAssert( pBillboardHelper, return );
	
	pBillboardHelper->GetBillboardType( type );
}

void ICBillboardController:: GetBillboardOrientation( int &orientation )
{
	ICBillboardHelper *pBillboardHelper = GetHelperPtr();
	FUAssert( pBillboardHelper, return );
	
	pBillboardHelper->GetBillboardOrientation( orientation );
}

bool ICBillboardController::IsDisplayBillboardedTargetsEnabled()
{
	ICBillboardHelper *pBillboardHelper = GetHelperPtr();
	FUAssert( pBillboardHelper, return false );
	
	return pBillboardHelper->IsDisplayBillboardedTargetsEnabled();
}

void ICBillboardController::HoldQuat()
{
	if ( theHold.Holding() && !TestAFlag( A_HELD ) ) 
	{		
		theHold.Put(new RotationRestoreObj(this));
		SetAFlag(A_HELD);
	}
}

#define OFFSETQUAT_LOCAL_CHUNK		0x1001
#define OFFSETQUAT_WORLD_CHUNK		0x1002
#define USEROTQUAT_CHUNK			0x1003

IOResult ICBillboardController::Save(ISave *isave)
{	
	ULONG nb;

	// Save baseRotQuat
	isave->BeginChunk(OFFSETQUAT_LOCAL_CHUNK);
	isave->Write(&baseRotQuatLocal, sizeof(baseRotQuatLocal), &nb);
	isave->EndChunk();

	// Save baseRotQuat
	isave->BeginChunk(OFFSETQUAT_WORLD_CHUNK);
	isave->Write(&baseRotQuatWorld, sizeof(baseRotQuatWorld), &nb);
	isave->EndChunk();

	// Save userRotQuat
	isave->BeginChunk(USEROTQUAT_CHUNK);
	isave->Write(&userRotQuat,sizeof(userRotQuat),&nb);	
	isave->EndChunk();

	return IO_OK;
}

IOResult ICBillboardController::Load(ILoad *iload)
{
	ULONG nb;
	IOResult res = IO_OK;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
			case OFFSETQUAT_LOCAL_CHUNK:
				// Read a block of bytes from the output stream.
				res=iload->Read(&baseRotQuatLocal, sizeof(baseRotQuatLocal), &nb);
			break;

			case OFFSETQUAT_WORLD_CHUNK:
				// Read a block of bytes from the output stream.
				res=iload->Read(&baseRotQuatWorld, sizeof(baseRotQuatWorld), &nb);
			break;

			case USEROTQUAT_CHUNK:
				// Read a block of bytes from the output stream.
				res=iload->Read(&userRotQuat, sizeof(userRotQuat), &nb);
			break;
		}		
		iload->CloseChunk();
		if (res!=IO_OK)  return res;
	}
	
	return IO_OK;
}

void ICBillboardController::ViewportDisplayCallbackObject::Display( TimeValue t, ViewExp *vpt, int flags )
{
	if ( !m_pOwner )
	{
		return;
	}

	m_pOwner->Update( t );
	m_pOwner->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
}

void ICBillboardController::ViewportDisplayCallbackObject::GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect )
{

}

BOOL ICBillboardController::ViewportDisplayCallbackObject::Foreground()
{
	return TRUE;
}

ICBillboardController::RotationRestoreObj::RotationRestoreObj( ICBillboardController *c )
{
	cont = c;
	undo   = c->userRotQuat;
}

void ICBillboardController::RotationRestoreObj::Restore(int isUndo)
{
//			if (cont->editCont == cont) {
//				lookAt_const_paramblk.InvalidateUI();
//			}
	redo   = cont->userRotQuat;
	cont->userRotQuat  = undo;
	cont->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
}

void ICBillboardController::RotationRestoreObj::Redo()
{
	cont->userRotQuat = redo;
	cont->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
}

void ICBillboardController::RotationRestoreObj::EndHold()
{ 
	cont->ClearAFlag( A_HELD ); 
}