/**********************************************************************
 *<
   FILE: gravity.cpp

   DESCRIPTION:  Gravity World Space Modifier

   CREATED BY: Rolf Berteig

   Separated out from Wind, add range hoops by Eric Peterson

   HISTORY: 10-30-95

   Modified: 9/18/01 Bayboro: separate mods from EDP dependency

 *>   Copyright (c) 1994, All Rights Reserved.

  PB2 conversion '00 ECP
 **********************************************************************/
#include "StdAfx.h"
#include "ICGravity.h"
#include "ICSuprPrts.h"
#include "MAXHelpers.h"
#include "FUtils/FUDaeEnum.h"
#include "ICParticleSystemBridge.h"

static ICGravityMtl swMtl;

#define GRAVITY_R float(.7)
#define GRAVITY_G float(0)
#define GRAVITY_B float(0)

const Point3 HOOPCOLOR(0.0f,0.0f,0.0f);

ICGravityMtl::ICGravityMtl():Material() 
{  Kd[0] = GRAVITY_R;
   Kd[1] = GRAVITY_G;
   Kd[2] = GRAVITY_B;
   Ks[0] = GRAVITY_R;
   Ks[1] = GRAVITY_G;
   Ks[2] = GRAVITY_B;
   shininess = (float)0.0;
   shadeLimit = GW_WIREFRAME|GW_BACKCULL;
   selfIllum = (float)1.0;
}

IObjParam *ICForceObject::ip        = NULL;
// IParamMap *ICForceObject::pmapParam = NULL;
HWND       ICForceObject::hSot      = NULL;

static ICGravityClassDesc gravityDesc;
ClassDesc* GetICGravityObjDesc() {return &gravityDesc;}

static ICGravityModClassDesc gravityModDesc;
ClassDesc* GetICGravityModDesc() {return &gravityModDesc;}

//--- ICGravityObject Parameter map/block descriptors ------------------
//
//
// Parameters
static ParamBlockDesc2 gravity_param_blk ( FSForceGravity::gravity_params, _T("GravityParameters"),  0, &gravityDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, FSForceGravity::PBLOCK_REF_NO, 
   //rollout
   IDD_GRAVITYPARAM, IDS_RB_PARAMETERS, 0, 0, NULL,
   // params
   FSForceGravity::PB_STRENGTH,  _T("strength"),       TYPE_FLOAT,    P_ANIMATABLE + P_RESET_DEFAULT,  IDS_RB_STRENGTH2, 
      p_default,     1.0f, 
      p_ms_default,  1.0f,
      p_range,       -9999999.0f, 9999999.0f,
      p_ui,          TYPE_SPINNER,EDITTYPE_FLOAT,IDC_GRAV_STRENGTH,IDC_GRAV_STRENGTHSPIN,SPIN_AUTOSCALE, 
      end, 

   FSForceGravity::PB_DECAY,     _T("decay"),          TYPE_FLOAT,    P_ANIMATABLE + P_RESET_DEFAULT,  IDS_RB_DECAY, 
      p_default,     0.0f, 
      p_ms_default,  0.0f,
      p_range,       0.0f, 9999999.0f,
      p_ui,          TYPE_SPINNER,EDITTYPE_FLOAT,IDC_GRAV_DECAY,IDC_GRAV_DECAYSPIN,SPIN_AUTOSCALE, 
      end, 

   FSForceGravity::PB_TYPE,   _T("gravitytype"), TYPE_RADIOBTN_INDEX, 0,    IDS_ECP_GRAVITYTYPE,
      p_default,        0,
      p_range,       0, 1, 
      p_ui,          TYPE_RADIO,    2, IDC_FORCE_PLANAR, IDC_FORCE_SPHERICAL,
      end,

   FSForceGravity::PB_DISPLENGTH,  _T("iconsize"),     TYPE_FLOAT,    P_ANIMATABLE + P_RESET_DEFAULT,  IDS_ECP_GRAVITYICONSIZE, 
      p_default,     10.0f,   
      p_ms_default,  10.0f,
      p_range,       0.0f, 9999999.0f,
      p_ui,          TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_DISPLENGTH,IDC_DISPLENGTHSPIN, SPIN_AUTOSCALE, 
      end, 

   FSForceGravity::PB_HOOPSON,   _T("hoopson"),     TYPE_BOOL, 0, IDS_ECP_HOOPSON,
      p_default,     TRUE, 
      p_ui,       TYPE_SINGLECHEKBOX, IDC_EP_HOOPSON, 
      end,

   end
   );

#define GRAVPARAMDESC_LENGTH  5

// parameter block for version zero
ParamBlockDescID descGravVer0[] = 
{
   { TYPE_FLOAT, NULL, TRUE,  0 },
   { TYPE_FLOAT, NULL, TRUE,  1 },
   { TYPE_INT,   NULL, FALSE, 2 },
   { TYPE_FLOAT, NULL, FALSE, 3 },
};

//parameter block for version 1
ParamBlockDescID descGravVer1[] = 
{
   { TYPE_FLOAT, NULL, TRUE,  0 },
   { TYPE_FLOAT, NULL, TRUE,  1 },
   { TYPE_INT,   NULL, FALSE, 2 },
   { TYPE_FLOAT, NULL, FALSE, 3 },
   { TYPE_INT,   NULL, FALSE, 4 },
};

#define GRAVPBLOCK_LENGTH  5

//#define CURRENT_GRAVVERSION 1
//#define NUM_OLDVERSIONS     1
#define NUM_OLDVERSIONS    2

static ParamVersionDesc gravversions[] = 
{
   ParamVersionDesc(descGravVer0,4,0),
   ParamVersionDesc(descGravVer1,5,1),
};

//--- ICForceObject Methods ---------------------------------------------
void ICForceObject::BeginEditParams(
      IObjParam *ip,ULONG flags,Animatable *prev)
{
   if (!hSot)
      hSot = ip->AddRollupPage( 
         hInstance, 
         MAKEINTRESOURCE(IDD_WINDGRAV_SOT),
         DefaultSOTProc,
         GetString(IDS_RB_SOT), 
         (LPARAM)ip,APPENDROLL_CLOSED);

   SimpleWSMObject::BeginEditParams(ip,flags,prev);
   this->ip = ip;

   gravityDesc.BeginEditParams(ip, this, flags, prev);
}

void ICForceObject::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{     
	SimpleWSMObject::EndEditParams(ip,flags,next);
	this->ip = NULL;

	gravityDesc.EndEditParams(ip, this, flags, next);

	if ( flags & END_EDIT_REMOVEUI ) 
	{     
		if ( hSot )
		{  
			ip->DeleteRollupPage( hSot );
			hSot = NULL;
		}
	}  
}

static void DrawPlaneRangeHoops(float range,float len3, ICModDrawLineProc& lp)
{  
	Point3 pt[5];
	lp.SetLineColor(GetUIColor(COLOR_END_RANGE));      
	pt[0] = Point3(len3, len3, range); 
	pt[1] = Point3(-len3, len3, range); 
	pt[2] = Point3(-len3,-len3, range); 
	pt[3] = Point3(len3,-len3, range); 
	lp.Closed(pt,4);
	pt[0].z =-range; 
	pt[1].z =-range;
	pt[2].z =-range;
	pt[3].z =-range;
	lp.Closed(pt,4);
}

#define NUM_SEGS  16

static void DrawFalloffSphere(float range, ICModDrawLineProc& lp)
{  
   float u;
   Point3 pt[3],pty[3],ptz[3],first,firsty;
   
   lp.SetLineColor(GetUIColor(COLOR_END_RANGE));   
   pt[0]=(first= Point3(range,0.0f,0.0f));
   pty[0] =(firsty=Point3(0.0f,range,0.0f));
   ptz[0] = pt[0];
   for (int i=0; i<NUM_SEGS; i++)
   {  u = float(i)/float(NUM_SEGS) * TWOPI;
      float crange=(float)cos(u)*range,srange=(float)sin(u)*range;
      pt[1]=Point3(crange, srange, 0.0f);
      lp.proc(pt,2); pt[0]=pt[1];
      pty[1]=Point3(0.0f, crange, srange);
      lp.proc(pty,2); pty[0]=pty[1];
      ptz[1]=Point3(crange, 0.0f, srange);
      lp.proc(ptz,2); ptz[0]=ptz[1];
   }
   pt[1]=first;lp.proc(pt,2); 
   pty[1]=firsty;lp.proc(pty,2); 
   ptz[1]=first;lp.proc(ptz,2); 
}

int ICForceObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
{
	GraphicsWindow *gw = vpt->getGW();
	Material *mtl = &swMtl;
	Matrix3 mat = inode->GetObjectTM(t);
	DWORD rlim = gw->getRndLimits();
	
	gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|/*GW_BACKCULL|*/ (rlim&GW_Z_BUFFER?GW_Z_BUFFER:0) );//removed BC 2/16/99 DB
	gw->setTransform(mat);

	if ( inode->Selected() )
	{
		gw->setColor( LINE_COLOR, GetSelColor() );
	}
	else if( !inode->IsFrozen() )
	{
		gw->setColor( LINE_COLOR, GetUIColor( COLOR_SPACE_WARPS ) );
	}

	mesh.render( gw, mtl, NULL, COMP_ALL );
	
	int hoopson;
	float decay;
	pblock2->GetValue(FSForceGravity::PB_HOOPSON,t,hoopson,FOREVER);
	pblock2->GetValue(FSForceGravity::PB_DECAY,t,decay,FOREVER);
	
	if ( hoopson && ( decay > 0.0f ))
	{
		int type;
		pblock2->GetValue( FSForceGravity::PB_TYPE, t, type, FOREVER );
		float range;
		range = ( decay > 0.0f?0.6931472f / decay:0.0f );

		ICModDrawLineProc lp( gw );
		if ( type == FUDaeForce::FORCE_PARAMS_PLANAR )
		{ 
			float length;
			pblock2->GetValue( FSForceGravity::PB_DISPLENGTH, t, length, FOREVER );
			DrawPlaneRangeHoops( range, 3.0f * length, lp );
		}
		else
		{
			DrawFalloffSphere( range, lp );
		}
	}
	gw->setRndLimits( rlim );
	return 0;
}

void ICForceObject::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{   
   Box3 meshBox;
    Matrix3 mat = inode->GetObjectTM(t);
   box.Init();
   int hoopson;
   pblock2->GetValue(FSForceGravity::PB_HOOPSON,t,hoopson,FOREVER);
   if (hoopson)
   {  float decay; pblock2->GetValue(FSForceGravity::PB_DECAY,t,decay,FOREVER);
      if (decay>0.0f)
      {  float range,xy; range=2.0f*(decay > 0.0f?0.6931472f / decay:0.0f);
         int type;pblock2->GetValue(FSForceGravity::PB_TYPE,t,type,ivalid);
         if (type==FUDaeForce::FORCE_PARAMS_PLANAR) 
         {  pblock2->GetValue(FSForceGravity::PB_DISPLENGTH,t,xy,FOREVER);
            xy*=3.0f;
         }  
         else 
            xy=range;
         Box3 rangeBox(Point3(-xy,-xy,-range),Point3(xy,xy,range)); 
         for(int i = 0; i < 8; i++) 
            box += mat * rangeBox[i];
      }
   }
   GetLocalBoundBox(t,inode,vpt,meshBox); 
   for(int i = 0; i < 8; i++)
      box += mat * meshBox[i];
}

void ICForceObject::BuildMesh(TimeValue t)
{  ivalid = FOREVER;
   float length;
   int type;   
   pblock2->GetValue(FSForceGravity::PB_DISPLENGTH,t,length,ivalid);
   pblock2->GetValue(FSForceGravity::PB_TYPE,t,type,ivalid);

   int norvs,norfs,extraverts,extrafaces;
   float decay,range;
   pblock2->GetValue(FSForceGravity::PB_DECAY,t,decay,ivalid);

   if (decay > 0.0f)
      range = 0.6931472f / decay;
   else
      range = 0.0f;

   if (type==FUDaeForce::FORCE_PARAMS_PLANAR)
   {  extrafaces = 0;
      extraverts = 0;
      mesh.setNumVerts((norvs=15) + extraverts);
      mesh.setNumFaces((norfs=5) + extrafaces);
      mesh.setVert(0, Point3(-length,-length, 0.0f));
      mesh.setVert(1, Point3( length,-length, 0.0f));
      mesh.setVert(2, Point3( length, length, 0.0f));
      mesh.setVert(3, Point3(-length, length, 0.0f));
      mesh.setVert(4, Point3(   0.0f,   0.0f, 0.0f));
      mesh.setVert(5, Point3(   0.0f,   0.0f, 3*length));
      mesh.setVert(6, Point3(   0.0f,   0.0f, 0.0f));
      length *= 0.3f;
      mesh.setVert(7, Point3(-length,-length, 7.0f*length));
      mesh.setVert(8, Point3( length,-length, 7.0f*length));
      mesh.setVert(9, Point3( length, length, 7.0f*length));
      mesh.setVert(10, Point3(-length, length, 7.0f*length));

      mesh.setVert(11, Point3(-length,-length, range));
      mesh.setVert(12, Point3( length,-length, range));
      mesh.setVert(13, Point3( length, length, range));
      mesh.setVert(14, Point3(-length, length, range));

      mesh.faces[0].setEdgeVisFlags(1,0,1);
      mesh.faces[0].setSmGroup(0);
      mesh.faces[0].setVerts(0,1,3);

      mesh.faces[1].setEdgeVisFlags(1,1,0);
      mesh.faces[1].setSmGroup(0);
      mesh.faces[1].setVerts(1,2,3);

      mesh.faces[2].setEdgeVisFlags(1,1,0);
      mesh.faces[2].setSmGroup(0);
      mesh.faces[2].setVerts(4,5,6);

      mesh.faces[3].setEdgeVisFlags(0,1,1);
      mesh.faces[3].setSmGroup(0);
      mesh.faces[3].setVerts(7,9,5);

      mesh.faces[4].setEdgeVisFlags(0,1,1);
      mesh.faces[4].setSmGroup(0);
      mesh.faces[4].setVerts(10,8,5);
   }
   else
   {  extrafaces = 0;
      extraverts = 0;
      float u;

      mesh.setNumVerts((norvs = 3*NUM_SEGS + 1) + extraverts);
      mesh.setNumFaces((norfs = 3*NUM_SEGS) + extrafaces);

      for (int i=0; i<NUM_SEGS; i++)
      {  u = float(i)/float(NUM_SEGS) * TWOPI;
         mesh.setVert(i           , Point3((float)cos(u) * length, (float)sin(u) * length, 0.0f));
         mesh.setVert(i + NUM_SEGS, Point3(0.0f, (float)cos(u) * length, (float)sin(u) * length));
         mesh.setVert(i+2*NUM_SEGS, Point3((float)cos(u) * length, 0.0f, (float)sin(u) * length));
      }
      mesh.setVert(3*NUM_SEGS, Point3(0.0f, 0.0f, 0.0f));
      
      int i1,lastvertex = 3*NUM_SEGS;
      for (int i=0; i<lastvertex; i++)
      {  i1 = i + 1;
         if (i1%NUM_SEGS == 0) 
            i1 -= NUM_SEGS;
         mesh.faces[i].setEdgeVisFlags(1,0,0);
         mesh.faces[i].setSmGroup(0);
         mesh.faces[i].setVerts(i,i1,lastvertex);
      }
   }
   mesh.InvalidateGeomCache();
}

int ICForceObjCreateCallback::proc(
      ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat)
{
	float PI = 3.1415926535f;
   if (msg==MOUSE_POINT||msg==MOUSE_MOVE) 
   {
#ifdef _3D_CREATE
      DWORD snapdim = SNAP_IN_3D;
#else
      DWORD snapdim = SNAP_IN_PLANE;
#endif
      switch(point) 
      {
         case 0:                       
            sp0 = m;
            p0  = vpt->SnapPoint(m,m,NULL,snapdim);
            mat.SetTrans(p0);
            break;
         case 1:
			 if (ob->ClassID()==FSForceGravity::IC_OBJECT_CLASSID) 
            {
               mat.IdentityMatrix();
               mat.RotateX(PI);
               mat.SetTrans(p0);
            }
            p1  = vpt->GetPointOnCP(m);
            ob->pblock2->SetValue(FSForceGravity::PB_DISPLENGTH,0,Length(p1-p0)/2.0f);
//          ob->pmapParam->Invalidate();
            gravity_param_blk.InvalidateUI();
            if (msg==MOUSE_POINT) 
            {
               if (Length(m-sp0)<3) 
                  return CREATE_ABORT;
               else 
                  return CREATE_STOP;
            }
            break;
         }
      } 
      else 
      {
      if (msg == MOUSE_ABORT) 
      {
         return CREATE_ABORT;
      }
      else
      if (msg == MOUSE_FREEMOVE) 
      {
         vpt->SnapPreview(m,m);
      }
   }
   
   return TRUE;
}

static ICForceObjCreateCallback forceCreateCB;

CreateMouseCallBack* ICForceObject::GetCreateMouseCallBack()
{
   forceCreateCB.ob = this;
   return &forceCreateCB;
}

void ICForceObject::InvalidateUI() 
{
// if (pmapParam) pmapParam->Invalidate();
   gravity_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}

//--- ICGravityObject methods ---------------------------------------

ICGravityObject::ICGravityObject()
{
	gravityDesc.MakeAutoParamBlocks(this);
	assert(pblock2);  
	ff = NULL;
	mf = NULL;
}

Modifier *ICGravityObject::CreateWSMMod(INode *node)
{
	return new ICGravityMod(node,this);
}

ForceField *ICGravityObject::GetForceField(INode *node)
{
	ICGravityField *gf = new ICGravityField;   
	gf->obj  = this;
	gf->node = node;
	gf->tmValid.SetEmpty();
	gf->fValid.SetEmpty();
	gf->obj->pblock2->GetValue(FSForceGravity::PB_TYPE,0,gf->type,FOREVER);
	return gf;
}

RefTargetHandle ICGravityObject::Clone(RemapDir& remap) 
{
	ICGravityObject* newob = new ICGravityObject();  
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	BaseClone(this, newob, remap);
	return newob;
}


ParamDimension *ICGravityObject::GetParameterDim(int pbIndex) 
{
   switch (pbIndex) 
   {     
      case 0:
      default: return defaultDim;
   }
}

ICGravityMod::ICGravityMod(INode *node,ICGravityObject *obj)
{
	ReplaceReference( SIMPWSMMOD_NODEREF, node );
}

ICGravityMod::~ICGravityMod()
{
}

Interval ICGravityMod::GetValidity(TimeValue t) 
{
   if (nodeRef) 
   {
      Interval valid = FOREVER;
      Matrix3 tm;
      float f;    
      ((ICGravityObject*)GetWSMObject(t))->pblock2->GetValue(FSForceGravity::PB_STRENGTH,t,f,valid);
      tm = nodeRef->GetObjectTM(t,&valid);
      return valid;
   } 
   else 
   {
      return FOREVER;
   }
}

static ICGravityDeformer gdeformer;

Deformer& ICGravityMod::GetDeformer(
      TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
{
   return gdeformer;
}

RefTargetHandle ICGravityMod::Clone(RemapDir& remap) 
{
   ICGravityMod *newob = new ICGravityMod(nodeRef,(ICGravityObject*)obRef);   
   newob->SimpleWSMModClone(this, remap);
   BaseClone(this, newob, remap);
   return newob;
}

void ICGravityMod::ModifyObject( TimeValue t, ModContext &mc, ObjectState *os, INode *node )
{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) 
	{
		force.obj  = (ICGravityObject*)GetWSMObject(t);
		force.node = nodeRef;
		force.tmValid.SetEmpty();
		force.fValid.SetEmpty();

		if (force.obj != NULL)
		{
			force.obj->pblock2->GetValue(FSForceGravity::PB_TYPE,t,force.type,FOREVER);
		}

		obj->ApplyForceField(&force);
	}
}

void ICGravityField::UpdateBindedParameters( irr::ps::PGravity::parameters_type &params, irr::core::matrix4 &irrMat, TimeValue t )
{
	if ( !obj )
	{
		return;
	}

	obj->pblock2->GetValue( FSForceGravity::PB_DECAY, t, params.decay, fValid );
	//obj->pblock2->GetValue( FSForceGravity::PB_TYPE, t, type, FOREVER );

	bool bUpdateForce = !fValid.InInterval( t ) || type == FUDaeForce::FORCE_PARAMS_SPHERICAL || params.decay != 0.0f;
	if ( bUpdateForce )
	{
		fValid = FOREVER;    
		if ( !tmValid.InInterval(t) )
		{
			tmValid = FOREVER;
			tm = node->GetObjectTM(t,&tmValid);
		}
		fValid &= tmValid;
		obj->pblock2->GetValue( FSForceGravity::PB_STRENGTH, t, params.strength, fValid );
	}
	else
	{
		obj->pblock2->GetValue( FSForceGravity::PB_STRENGTH, t, params.strength, FOREVER );
	}

	params.type = ( irr::ps::PGravity::PGravityType )type;
	irrMat = ICParticleSystemBridge::MaxMatrix3ToIrrCoreMatrix4( tm );
}

// This is an adjustment to forces to make the independent of time scale.
// They were previously dependent on the old 1200 ticks per second constant.
// Note that the constants are being squared because we are dealing with acceleration not velocity.
static float forceScaleFactor = float(1200*1200)/float(TIME_TICKSPERSEC*TIME_TICKSPERSEC);

Point3 ICGravityField::Force( TimeValue t, const Point3 &pos, const Point3 &vel, int index )
{  
	if ( obj == NULL )
	{
		return Point3( 0.0f, 0.0f, 0.0f );  //667105 watje xrefs can change the base object type through proxies which will cause this to be null or the user can copy a new object type over ours
	}

	float strength, decay;
	//int type;

	obj->pblock2->GetValue( FSForceGravity::PB_DECAY, t, decay, fValid );
	//obj->pblock2->GetValue( FSForceGravity::PB_TYPE, t, type, FOREVER );

	bool bUpdateForce = !fValid.InInterval( t ) || type == FUDaeForce::FORCE_PARAMS_SPHERICAL || decay != 0.0f;
	if ( bUpdateForce )
	{
		fValid = FOREVER;    
		if ( !tmValid.InInterval(t) )
		{
			tmValid = FOREVER;
			tm = node->GetObjectTM(t,&tmValid);
		}
		fValid &= tmValid;
		obj->pblock2->GetValue( FSForceGravity::PB_STRENGTH, t, strength, fValid );
	}

	if ( decay < 0.0f )
	{
		decay = 0.0f; 
	}

	if ( bUpdateForce )
	{
		if ( type == FUDaeForce::FORCE_PARAMS_PLANAR )
		{
			force = Normalize( tm.GetRow(2) );
			if ( decay != 0.0f )
			{
				float dist = (float)fabs(DotProd( force, pos-tm.GetTrans() ));
				strength *= (float)exp( -decay * dist );
			}        
		} 
		else 
		{
			float dist;
			force = tm.GetTrans() - pos;
			dist  = Length(force);
			if (dist!=0.0f) force /= dist;
			if (decay!=0.0f)
			{
				strength *= (float)exp(-decay*dist);
			}        
		}

		force *= strength * 0.0001f * forceScaleFactor;
	}

	return force;
}

IOResult ICGravityObject::Load(ILoad *iload) 
{  
   ParamBlock2PLCB* plcb = new ParamBlock2PLCB(gravversions, NUM_OLDVERSIONS, &gravity_param_blk, this, FSForceGravity::PBLOCK_REF_NO);
   iload->RegisterPostLoadCallback(plcb);
   return IO_OK;
}

ICGravityObject::~ICGravityObject()
{  
   if (ff)
      delete ff;  
   DeleteAllRefsFromMe();
}

//void ICGravityObject::SetUpModifier(TimeValue t,INode *node)
//{
//   mf->force.obj  = (ICGravityObject*)(mf->GetWSMObject(t));
//   mf->force.node = mf->nodeRef;
//   mf->force.tmValid.SetEmpty();
//   mf->force.fValid.SetEmpty();
//   pblock2->GetValue(FSForceGravity::PB_TYPE,t,mf->force.type,FOREVER);
//}

