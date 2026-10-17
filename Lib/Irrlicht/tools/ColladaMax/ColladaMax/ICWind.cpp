/**********************************************************************
 *<
	FILE: wind.cpp

	DESCRIPTION:  Wind World Space Modifier

	CREATED BY: Rolf Berteig (as part of gravity.CPP)
	Split out, range added, by Eric Peterson

	HISTORY: 10-30-95

	Modified: 9/18/01 Bayboro: separate mods from EDP dependency

 *>	Copyright (c) 1994, All Rights Reserved.

  PB2 conversion '00 ECP
 **********************************************************************/

#include "StdAfx.h"
#include "ICWind.h"
#include "ICSuprPrts.h"
#include "MAXHelpers.h"
#include "FUtils/FUDaeEnum.h"
#include "ICParticleSystemBridge.h"

static ICWindMtl swMtl;
const Point3 WHOOPCOLOR(0.0f,0.0f,0.0f);

#define WIND_R	float(.7)
#define WIND_G	float(0)
#define WIND_B	float(0)

ICWindMtl::ICWindMtl():Material() 
{	Kd[0] = WIND_R;
	Kd[1] = WIND_G;
	Kd[2] = WIND_B;
	Ks[0] = WIND_R;
	Ks[1] = WIND_G;
	Ks[2] = WIND_B;
	shininess = (float)0.0;
	shadeLimit = GW_WIREFRAME|GW_BACKCULL;
	selfIllum = (float)1.0;
}

IObjParam *ICWindObject::ip        = NULL;
HWND       ICWindObject::hSot      = NULL;


//--- ClassDescriptor and class vars ---------------------------------
static ICWindClassDesc windDesc;
ClassDesc* GetICWindObjDesc() {return &windDesc;}

//--- ICWindMod -----------------------------------------------------
static ICWindModClassDesc windModDesc;
ClassDesc* GetICWindModDesc() {return &windModDesc;}

//
//
// Parameters
static ParamBlockDesc2 wind_param_blk ( FSForceWind::wind_params, _T("WindParameters"),  0, &windDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, FSForceWind::PBLOCK_REF_NO, 
	//rollout
	IDD_WINDPARAM, IDS_RB_PARAMETERS, 0, 0, NULL,

	// params
	FSForceWind::PB_STRENGTH,  _T("strength"), 		TYPE_FLOAT, 	P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_RB_STRENGTH2, 
		p_default, 		1.0f,	
		p_ms_default,	1.0f,
		p_range, 		-9999999.0f, 9999999.0f,
		p_ui, 			TYPE_SPINNER,EDITTYPE_FLOAT,IDC_WIND_STRENGTH,IDC_WIND_STRENGTHSPIN,SPIN_AUTOSCALE, 
		end, 

	FSForceWind::PB_DECAY,	  _T("decay"), 			TYPE_FLOAT, 	P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_RB_DECAY, 
		p_default, 		0.0f,	
		p_ms_default,	1.0f,
		p_range, 		0.0f, 9999999.0f,
		p_ui, 			TYPE_SPINNER,EDITTYPE_FLOAT,IDC_WIND_DECAY,IDC_WIND_DECAYSPIN,SPIN_AUTOSCALE, 
		end, 

	FSForceWind::PB_TYPE,	  _T("windtype"),		TYPE_RADIOBTN_INDEX,	0,		IDS_ECP_WINDTYPE,
		p_default,			0,
		p_range,			0, 1, 
		p_ui,				TYPE_RADIO,		2,	IDC_FORCE_PLANAR, IDC_FORCE_SPHERICAL,
		end,

	FSForceWind::PB_TURBULENCE,  _T("turbulence"), 	TYPE_FLOAT, 	P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_RB_TURBULENCE, 
		p_default, 		0.0f,	
		p_ms_default,	1.0f,
		p_range, 		0.0f, 9999999.0f,
		p_ui, 			TYPE_SPINNER,EDITTYPE_FLOAT,IDC_WIND_TURB,IDC_WIND_TURBSPIN,SPIN_AUTOSCALE, 
		end, 

	FSForceWind::PB_FREQUENCY,  _T("frequency"), 	TYPE_FLOAT, 	P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_RB_FREQUENCY, 
		p_default, 		0.0f,	
		p_ms_default,	1.0f,
		p_range, 		0.0f, 9999999.0f,
		p_ui, 			TYPE_SPINNER,EDITTYPE_FLOAT,IDC_WIND_FREQ,IDC_WIND_FREQSPIN,SPIN_AUTOSCALE, 
		end, 

	FSForceWind::PB_SCALE,	  _T("scale"), 			TYPE_FLOAT, 	P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_RB_SCALE, 
		p_default, 		1.0f,	
		p_ms_default,	1.0f,
		p_range, 		0.0f, 9999999.0f,
		p_ui, 			TYPE_SPINNER,EDITTYPE_FLOAT,IDC_WIND_SCALE,IDC_WIND_SCALESPIN,SPIN_AUTOSCALE, 
		end, 

	FSForceWind::PB_DISPLENGTH,  _T("iconsize"),		TYPE_FLOAT, 	P_ANIMATABLE + P_RESET_DEFAULT, 	IDS_ECP_GRAVITYICONSIZE, 
		p_default, 		10.0f,	
		p_ms_default,	10.0f,
		p_range, 		0.0f, 9999999.0f,
		p_ui, 			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_DISPLENGTH,IDC_DISPLENGTHSPIN, SPIN_AUTOSCALE, 
		end, 

	FSForceWind::PB_HOOPSON,   _T("hoopson"),		TYPE_BOOL, 0, IDS_ECP_HOOPSON,
		p_default, 		FALSE, 
		p_ui,			TYPE_SINGLECHEKBOX, IDC_EP_HOOPSON, 
		end,

	end
	);

#define WINDPARAMDESC_LENGTH	8

ParamBlockDescID descWindVer0[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, FALSE, 2 },
	{ TYPE_FLOAT, NULL, FALSE, 3 },
	{ TYPE_FLOAT, NULL, TRUE, 4 },
	{ TYPE_FLOAT, NULL, TRUE, 5 },
	{ TYPE_FLOAT, NULL, TRUE, 6 },
};

ParamBlockDescID descWindVer1[] = {
	{ TYPE_FLOAT, NULL, TRUE, 0 },
	{ TYPE_FLOAT, NULL, TRUE, 1 },
	{ TYPE_INT, NULL, FALSE, 2 },
	{ TYPE_FLOAT, NULL, FALSE, 3 },
	{ TYPE_FLOAT, NULL, TRUE, 4 },
	{ TYPE_FLOAT, NULL, TRUE, 5 },
	{ TYPE_FLOAT, NULL, TRUE, 6 },
	{ TYPE_INT, NULL, FALSE, 7 },
};

#define WINDPBLOCK_LENGTH		8

//#define CURRENT_WINDVERSION		1
#define NUM_OLDVERSIONS			2

static ParamVersionDesc windversions[] = 
{
	ParamVersionDesc(descWindVer0,7,0),
	ParamVersionDesc(descWindVer1,8,1),
};

//--- ICWindObject Methods ---------------------------------------------

void ICWindObject::BeginEditParams(
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

	windDesc.BeginEditParams(ip, this, flags, prev);
}

void ICWindObject::EndEditParams(
		IObjParam *ip, ULONG flags,Animatable *next)
{		
	SimpleWSMObject::EndEditParams(ip,flags,next);
	this->ip = NULL;

	windDesc.EndEditParams(ip, this, flags, next);

	if (flags&END_EDIT_REMOVEUI ) 
	{		
		if (hSot)
//		DestroyCPParamMap(pmapParam);
		{	ip->DeleteRollupPage(hSot);
			hSot = NULL;
		}
//		pmapParam = NULL;		
	}	
}

static void DrawPlaneRangeHoops(float range,float len3, ICModDrawLineProc& lp)
{	Point3 pt[5];
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

#define NUM_SEGS	16

static void DrawFalloffSphere(float range, ICModDrawLineProc& lp)
{	float u;
	Point3 pt[3],pty[3],ptz[3],first,firsty;
	
	lp.SetLineColor(GetUIColor(COLOR_END_RANGE));	
	pt[0]=(first= Point3(range,0.0f,0.0f));
	pty[0] =(firsty=Point3(0.0f,range,0.0f));
	ptz[0] = pt[0];
	for (int i=0; i<NUM_SEGS; i++)
	{	u = float(i)/float(NUM_SEGS) * TWOPI;
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

int ICWindObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags)
{	GraphicsWindow *gw = vpt->getGW();
	Material *mtl = &swMtl;
	Matrix3 mat = inode->GetObjectTM(t);
 	DWORD rlim = gw->getRndLimits();
	gw->setRndLimits(GW_WIREFRAME|GW_EDGES_ONLY|/*GW_BACKCULL|*/ (rlim&GW_Z_BUFFER?GW_Z_BUFFER:0) );//removed BC 2/16/99 DB

	gw->setTransform(mat);
	if (inode->Selected()) 
		gw->setColor( LINE_COLOR, GetSelColor());
	else if(!inode->IsFrozen())
		gw->setColor(LINE_COLOR,GetUIColor(COLOR_SPACE_WARPS));
	mesh.render(gw, mtl, NULL, COMP_ALL);
	int hoopson;float decay;
	pblock2->GetValue(FSForceWind::PB_HOOPSON,t,hoopson,FOREVER);
	pblock2->GetValue(FSForceWind::PB_DECAY,t,decay,FOREVER);
	if (hoopson && (decay > 0.0f))
	{	int type;pblock2->GetValue(FSForceWind::PB_TYPE,t,type,FOREVER);
		float range;
		range=(decay > 0.0f?0.6931472f / decay:0.0f);
		ICModDrawLineProc lp(gw);
		if (type==FUDaeForce::FORCE_PARAMS_PLANAR)
		{	float length;
			pblock2->GetValue(FSForceWind::PB_DISPLENGTH,t,length,FOREVER);
			DrawPlaneRangeHoops(range,3.0f*length,lp);
		}
		else 
			DrawFalloffSphere(range,lp);
	}
	gw->setRndLimits(rlim);
	return(0);
}

void ICWindObject::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{
	Box3 meshBox;
	Matrix3 mat = inode->GetObjectTM(t);
	box.Init();
	int hoopson;
	pblock2->GetValue(FSForceWind::PB_HOOPSON,t,hoopson,FOREVER);
	if (hoopson)
	{	float decay; pblock2->GetValue(FSForceWind::PB_DECAY,t,decay,FOREVER);
		if (decay>0.0f)
		{	float range,xy; range=2.0f*(decay > 0.0f?0.6931472f / decay:0.0f);
			int type;pblock2->GetValue(FSForceWind::PB_TYPE,t,type,ivalid);
			if (type==FUDaeForce::FORCE_PARAMS_PLANAR)	
			{	pblock2->GetValue(FSForceWind::PB_DISPLENGTH,t,xy,FOREVER);
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

void ICWindObject::BuildMesh(TimeValue t)
{	ivalid = FOREVER;
	float length;
	int type;	
	pblock2->GetValue(FSForceWind::PB_DISPLENGTH,t,length,ivalid);
	pblock2->GetValue(FSForceWind::PB_TYPE,t,type,ivalid);
	int norvs,norfs,extraverts,extrafaces;
	float decay,range;
	pblock2->GetValue(FSForceWind::PB_DECAY,t,decay,ivalid);
	range=decay;
	int hoopson;
	pblock2->GetValue(FSForceWind::PB_HOOPSON,t,hoopson,FOREVER);
	range=(decay > 0.0f?0.6931472f / decay:0.0f);
	if (type==FUDaeForce::FORCE_PARAMS_PLANAR)
	{	extrafaces = 0;
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
	{	extrafaces = 0;
		extraverts = 0;
		float u;

		mesh.setNumVerts((norvs = 3*NUM_SEGS + 1) + extraverts);
		mesh.setNumFaces((norfs = 3*NUM_SEGS) + extrafaces);

		for (int i=0; i<NUM_SEGS; i++)
		{	u = float(i)/float(NUM_SEGS) * TWOPI;
			mesh.setVert(i           , Point3((float)cos(u) * length, (float)sin(u) * length, 0.0f));
			mesh.setVert(i + NUM_SEGS, Point3(0.0f, (float)cos(u) * length, (float)sin(u) * length));
			mesh.setVert(i+2*NUM_SEGS, Point3((float)cos(u) * length, 0.0f, (float)sin(u) * length));
		}
		mesh.setVert(3*NUM_SEGS, Point3(0.0f, 0.0f, 0.0f));
		
		int i1,lastvertex = 3*NUM_SEGS;
		for (int i=0; i<lastvertex; i++)
		{	i1 = i + 1;
			if (i1%NUM_SEGS == 0) 
				i1 -= NUM_SEGS;
			mesh.faces[i].setEdgeVisFlags(1,0,0);
			mesh.faces[i].setSmGroup(0);
			mesh.faces[i].setVerts(i,i1,lastvertex);
		}

	}
	mesh.InvalidateGeomCache();
}

int ICWindObjCreateCallback::proc(
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
				if (ob->ClassID()==Class_ID(GRAVITYOBJECT_CLASS_ID,0)) 
				{
					mat.IdentityMatrix();
					mat.RotateX(PI);
					mat.SetTrans(p0);
				}
				p1  = vpt->GetPointOnCP(m);
				ob->pblock2->SetValue(FSForceWind::PB_DISPLENGTH,0,Length(p1-p0)/2.0f);
//				ob->pmapParam->Invalidate();
				wind_param_blk.InvalidateUI();
				if (msg==MOUSE_POINT) 
				{
					if (Length(m-sp0)<3) return CREATE_ABORT;
					else return CREATE_STOP;
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

static ICWindObjCreateCallback forceCreateCB;

CreateMouseCallBack* ICWindObject::GetCreateMouseCallBack()
{
	forceCreateCB.ob = this;
	return &forceCreateCB;
}

void ICWindObject::InvalidateUI() 
{
//	if (pmapParam) pmapParam->Invalidate();
	wind_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}

//--- ICWindObject methods ---------------------------------------

ICWindObject::ICWindObject()
	{
	pblock2 = NULL;
	windDesc.MakeAutoParamBlocks(this);
	assert(pblock2);	
	ff = NULL;
	mf = NULL;
}

Modifier *ICWindObject::CreateWSMMod(INode *node)
{
	return new ICWindMod(node,this);
}

ForceField *ICWindObject::GetForceField(INode *node)
{
	ICWindField *wf = new ICWindField;	
	wf->obj  = this;
	wf->node = node;
	wf->tmValid.SetEmpty();
	wf->fValid.SetEmpty();
	wf->obj->pblock2->GetValue(FSForceWind::PB_TYPE,0,wf->type,FOREVER);
	return wf;
}

RefTargetHandle ICWindObject::Clone(RemapDir& remap) 
{
	ICWindObject* newob = new ICWindObject();
	newob->ReplaceReference(0,remap.CloneRef(pblock2));
	BaseClone(this, newob, remap);
	return newob;
}

ParamDimension *ICWindObject::GetParameterDim(int pbIndex) 
{
	switch (pbIndex) 
	{		
		case 0:
		default: return defaultDim;
	}
}


//--- ICWindMod methods -----------------------------------------
ICWindMod::ICWindMod(INode *node,ICWindObject *obj)
	{	
	ReplaceReference(SIMPWSMMOD_NODEREF,node);	
	}

Interval ICWindMod::GetValidity(TimeValue t) 
{
	if (nodeRef) 
	{
		Interval valid = FOREVER;
		Matrix3 tm;
		float f;		
		((ICWindObject*)GetWSMObject(t))->pblock2->GetValue(FSForceWind::PB_STRENGTH,t,f,valid);		
		tm = nodeRef->GetObjectTM(t,&valid);
		return valid;
	} 
	else 
	{
		return FOREVER;
	}
}

static ICWindDeformer wdeformer;

Deformer& ICWindMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
{
	return wdeformer;
}

RefTargetHandle ICWindMod::Clone(RemapDir& remap) 
{
	ICWindMod *newob = new ICWindMod(nodeRef,(ICWindObject*)obRef);	
	newob->SimpleWSMModClone(this, remap);
	BaseClone(this, newob, remap);
	return newob;
}

void ICWindMod::ModifyObject(
		TimeValue t, ModContext &mc, ObjectState *os, INode *node)
{
	ParticleObject *obj = GetParticleInterface(os->obj);
	if (obj) 
	{
		force.obj  = (ICWindObject*)GetWSMObject(t);
		force.node = nodeRef;
		force.tmValid.SetEmpty();
		force.fValid.SetEmpty();
		if (force.obj != NULL)
			force.obj->pblock2->GetValue(FSForceWind::PB_TYPE,t,force.type,FOREVER);
		obj->ApplyForceField(&force);
	}
}

static float RTurbulence(Point3 p,float freq)
{
	return noise3(p*freq);
}

void ICWindField::UpdateBindedParameters( irr::ps::PWind::parameters_type &params, irr::core::matrix4 &irrMat, TimeValue t )
{
	if ( !obj )
	{
		return;
	}

	//obj->pblock2->GetValue( FSForceWind::PB_TYPE, t, type, FOREVER );
	obj->pblock2->GetValue( FSForceWind::PB_DECAY, t, params.decay, fValid );
	obj->pblock2->GetValue( FSForceWind::PB_TURBULENCE, t, params.turbulence, FOREVER );
	obj->pblock2->GetValue( FSForceWind::PB_FREQUENCY, t, params.frequency, FOREVER );
	obj->pblock2->GetValue( FSForceWind::PB_SCALE, t, params.scale, FOREVER );

	bool bUpdate = !fValid.InInterval( t ) || type == FUDaeForce::FORCE_PARAMS_SPHERICAL || params.decay != 0.0f;
	if ( bUpdate )
	{
		fValid = FOREVER;		
		if ( !tmValid.InInterval( t ) )
		{
			tmValid = FOREVER;
			tm = node->GetObjectTM( t, &tmValid );
		}
		fValid &= tmValid;
		obj->pblock2->GetValue( FSForceWind::PB_STRENGTH, t, params.strength, fValid );
	}
	else
	{
		obj->pblock2->GetValue( FSForceGravity::PB_STRENGTH, t, params.strength, FOREVER );
	}

	params.type = ( irr::ps::PWind::PWindType )type;
	irrMat = ICParticleSystemBridge::MaxMatrix3ToIrrCoreMatrix4( tm );
}


static float forceScaleFactor = float(1200*1200)/float(TIME_TICKSPERSEC*TIME_TICKSPERSEC);

Point3 ICWindField::Force( TimeValue t, const Point3 &pos, const Point3 &vel, int index )
{
	if ( !obj )
	{
		return Point3(0.0f, 0.0f, 0.0f);
	}

	float strength, decay;
	float turb, freq, scale;
	//int type;
	
	//obj->pblock2->GetValue( FSForceWind::PB_TYPE, t, type, FOREVER );
	obj->pblock2->GetValue( FSForceWind::PB_DECAY, t, decay, fValid );
	obj->pblock2->GetValue( FSForceWind::PB_TURBULENCE, t, turb, FOREVER );
	obj->pblock2->GetValue( FSForceWind::PB_FREQUENCY, t, freq, FOREVER );
	obj->pblock2->GetValue( FSForceWind::PB_SCALE, t, scale, FOREVER );

	bool bUpdate = !fValid.InInterval( t ) || type == FUDaeForce::FORCE_PARAMS_SPHERICAL || decay != 0.0f;
	if ( bUpdate )
	{
		fValid = FOREVER;		
		if ( !tmValid.InInterval( t ) )
		{
			tmValid = FOREVER;
			tm = node->GetObjectTM( t, &tmValid );
		}
		fValid &= tmValid;
		obj->pblock2->GetValue( FSForceWind::PB_STRENGTH, t, strength, fValid );
	}

	if ( decay < 0.0f )
	{
		decay = 0.0f;
	}

	if ( bUpdate )
	{
		if ( type == FUDaeForce::FORCE_PARAMS_PLANAR )
		{
			force = Normalize( tm.GetRow(2) );
			if (decay!=0.0f)
			{
				float dist = (float)fabs(DotProd(force,pos-tm.GetTrans()));
				strength *= (float)exp(-decay*dist);
			}
			force *= strength * 0.0001f * forceScaleFactor;
		}
		else 
		{
			float dist;
			force = pos-tm.GetTrans();
			dist  = Length(force);
			if ( dist != 0.0f )
			{
				force /= dist;
			}
			if ( decay != 0.0f )
			{				
				strength *= (float)exp(-decay*dist);
			}			
			force *= strength * 0.0001f * forceScaleFactor;
		}
	}

	if ( turb != 0.0f )
	{
		Point3 tf, pt = pos-tm.GetTrans(), p;
		freq *= 0.01f;
		turb *= 0.0001f * forceScaleFactor;

		p    = pt;
		p.x  = freq * float(t);
		tf.x = RTurbulence(p,scale);
		p    = pt;
		p.y  = freq * float(t);
		tf.y = RTurbulence(p,scale);
		p    = pt;
		p.z  = freq * float(t);
		tf.z = RTurbulence(p,scale);

		return force + (turb*tf);
	} 
	else
	{
		return force;
	}

	return force;
}

IOResult ICWindObject::Load(ILoad *iload) 
{	iload->RegisterPostLoadCallback(new ParamBlock2PLCB(windversions,NUM_OLDVERSIONS,&wind_param_blk,this,FSForceWind::PBLOCK_REF_NO));
	return IO_OK;
}

ICWindObject::~ICWindObject()
{	
	if (mf)
		delete mf;	
	if (ff)
		delete ff;	
	DeleteAllRefsFromMe();
}

//void ICWindObject::SetUpModifier(TimeValue t,INode *node)
//{
//	mf->force.obj  = (ICWindObject*)(mf->GetWSMObject(t));
//	mf->force.node = mf->nodeRef;
//	mf->force.tmValid.SetEmpty();
//	mf->force.fValid.SetEmpty();
//	pblock2->GetValue(FSForceWind::PB_TYPE,t,mf->force.type,FOREVER);
//}



