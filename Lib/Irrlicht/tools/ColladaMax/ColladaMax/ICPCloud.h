/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PCLOUD_H
#define _IC_PCLOUD_H

#include "ICSuprPrts.h"
#include "ICBridgeableParticleSystem.h"

// ----------------------------------------------------------------------
// Config macros
// ----------------------------------------------------------------------
#define IC_PCLOUD_ENABLE_UNUSED_PARAMETERS DEBUG_BUILD

#define IC_PCLOUD_ENABLE_ROLLOUT_TYPE                      IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_ROLLOUT_SPIN                      IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_ROLLOUT_OBJECT_MOTION_INHERITANCE IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_ROLLOUT_BUBBLE_MOTION             IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_ROLLOUT_PARTICLE_SPAWN            IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_ROLLOUT_PRESETS                   IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

// Particle Type Rollout Widgets
// TODO ( only if necessary ) : Add define for standard particle type
#define IC_PCLOUD_ENABLE_META_PARTICLES                    IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_INSTANCED_GEOMETRY_PARTICLES      IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_TRIANGLE        IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_CUBE            IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SPECIAL         IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_FACING          ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_CONSTANT        IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_TETRA           IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SIXPOINT        IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_STANDARD_PARTICLE_SPHERE          IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

// Particle Generation Rollout Widgets
#define IC_PCLOUD_ENABLE_UNIQUENESS_GROUPBOX               IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

#define IC_PCLOUD_ENABLE_SPEEDDIR_RANDOM                   IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_SPEEDDIR_VECTOR                   ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_SPEEDDIR_REFERENCE_OBJECT         IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

// Basic Parameters Rollout Widgets
#define IC_PCLOUD_ENABLE_PERCENTAGE_PARTICLES_DISPLAYED    IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

#define IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_DOTS             ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_TICKS            ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_MESH             ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_PARTICLE_DISPLAY_BOUNDING_BOX     IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

#define IC_PCLOUD_ENABLE_EMITTER_SHAPE_BOX                 ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_EMITTER_SHAPE_SPHERE              ( DEBUG_BUILD | RELEASE_BUILD )
#define IC_PCLOUD_ENABLE_EMITTER_SHAPE_CYLINDER            IC_PCLOUD_ENABLE_UNUSED_PARAMETERS
#define IC_PCLOUD_ENABLE_EMITTER_SHAPE_OBJECT              IC_PCLOUD_ENABLE_UNUSED_PARAMETERS

#define ICPCLOUD_CLASS_ID Class_ID(0x22100afd, 0x110d0c8a)

#define PBLK      0
#define CUSTNODE     1
#define DISTNODE     2
#define MBASE        3
#define BASER        4
#define shownormname 6

typedef struct
{
	float Ts // Determines LamTs
		, Ts0 // Determines Ts
		, LamTs // Related to spin, used for rendering <==
		, Mltvar; // Related to particle spawn, NOT used for rendering

	TimeValue L // Particle Life, used in ParticleLife(...) func, NOT used for rendering
		, showframe // ?, used for rendering, boundingbox, GetTimes <==
		, DL // ?, NOT used for rendering
		, persist; // NOT used for rendering

	Point3 V // particle speed vector, NOT used for rendering
		, W // partcicle spin axis, used for rendering <==
		, RV; // ?, NOT used for rendering

	int themtl // used in GetRenderMesh <==
		, gennum // probably related to particle spawn, used for rendering <==
		, SpVar; // NOT used for rendering

	float Vsz // NOT used for rendering
		, A // NOT used for rendering
		, LamA // NOT used for rendering
		, To // NOT used for rendering
		, pvar; // NOT used for rendering

} PCSavePt;

// used in update only
typedef struct
{
	float Size
		, VSz
		, VSpin
		, Phase
		, VPhase
		, Speed
		, VSpeed;

	float bstr
		, bstrvar
		, ToAmp
		, VToAmp;

	float ToPhase
		, VToPhase
		, VToPeriod
		, DirVar;

	int axisentered
		, direntered
		, sym;

	TimeValue Spin
		, ToPeriod
		, Life
		, Vl
		, persist;

	float len
		, width
		, depth
		, axisvar
		, pvar;

	Point3 Ve
		, Dir
		, Axis;

	BirthPositionSpeed bps;

} VelDir;

typedef fm::pvector<Modifier> ModifierReferenceList;
typedef fm::pvector<Modifier> ModifierList;
typedef fm::map<Modifier *, int> ICForceModifierMap;

class ICPCPickOperand;
class ICPCPickNorm;
class ICPCloudParticle;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//       WARNING - a copy of this class description is in maxscrpt\maxnode.cpp
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class ICPCloudParticleDraw : public CustomParticleDisplay {
   public:
      BOOL firstpart;
      ICPCloudParticle *obj;
      int disptype,ptype,bb,anifr,anioff;
      boxlst *bboxpt;
      TimeValue t;
      InDirInfo indir;

      ICPCloudParticleDraw() {obj=NULL;}
      BOOL DrawParticle(GraphicsWindow *gw,ParticleSys &parts,int i);
   };

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//       WARNING - a copy of this class description is in maxscrpt\maxnode.cpp
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class ICPCloudParticle : public SimpleParticle, IParamArray, RandGeneratorParticles, public ICBridgeableParticleSystem
{
public:
	ICPCloudParticleDraw thePCloudDraw;
	ICPCloudParticle();
	~ICPCloudParticle();
	static IParamMap *pmapParam;
	static IParamMap *pmapPGen;
	static IParamMap *pmapPType;
	static IParamMap *pPMapAnimationKeying;
	static IParamMap *pmapPSpin;
	static IParamMap *pmapEmitV;
	static IParamMap *pmapBubl;
	static IParamMap *pmapSpawn;
	static int createmeth;
	static HWND hbubl,hexts,hParams2,hptype,hAnimationKeying,hgen,hparam,hrot,spawn;
	int stepSize,size,maincount,fragflags;
	static int custsettings;
	Mesh *cmesh,*dispmesh;
	Box3 *cmbb;
	TimeValue dispt;
	INode *custnode,*distnode,*cnode,*mbase;
	TSTR custname,distname,normname;
	DWORD flags;
	ULONG dflags;
	BOOL cancelled,wasmulti;
	static BOOL creating;
	static ICPCPickOperand pickCB;
	static ICPCPickNorm pickNorm;
	Mtl *origmtl;

	ICForceModifierMap m_icForceModifiers;
	ModifierReferenceList m_modifierReferences;

	// hack variables
	void *m_pRadiusLengthSystemValue;
	void *m_pHeightSystemValue;
	IICBridgeableParticleSystemUIParam *m_pCreationMethodParam;
	IICBridgeableParticleSystemUIParam *m_pEmitterLengthParam;
	IICBridgeableParticleSystemUIParam *m_pEmitterHeightParam;

	Point3 boxcenter;
	int CustMtls;
	Tab<int> nmtls;
	TimeLst times;
	void GetTimes(TimeLst &times,TimeValue t,int anifr,int ltype);
	void TreeDoGroup(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
	void CheckTree(INode *node,Matrix3 tspace,Mesh *cmesh,Box3 *cmbb,int *numV,int *numF,int *tvnum,int *ismapped,TimeValue t,int subtree,int custmtl);
	void GetMesh(TimeValue t,int subtree,int custmtl);
	void GetNextBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
	void DoGroupBB(INode *node,int subtree,int *count,int *tabmax,Point3 boxcenter,TimeValue t,int tcount,INode *onode);
	void GetallBB(INode *custnode,int subtree,TimeValue t);

	void SetUpList();
	void AddToList(INode *newnode,int i,BOOL add);
	void DeleteFromList(int nnum,BOOL all);
	Tab<INode*> nlist;
	Tab<int> llist;
	int deftime;
	int NumRefs();
	RefTargetHandle GetReference(int i);
	void SetReference(int i, RefTargetHandle rtarg);      
	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
	 PartID& partID, RefMessage message);
	void SetUpLifeList();
	void AddToLifeList(int newlife);
	void DeleteFromLifeList(int nnum);
	void ShowName(int dist);
	static AName *NameLst;
	static ICustEdit *custCtrlEdit;

	void GetWorldBoundBox(TimeValue t, INode *inode, ViewExp* vpt, Box3& box);
	int CountLive();
	int rcounter,vcounter;
	oldipc lc;
	static IObjParam *ip;
	// Animatable methods      
	void DeleteThis() {delete this;}
	void MapKeys(TimeMap *map,DWORD flags);
	int RenderBegin(TimeValue t, ULONG flags);      
	int RenderEnd(TimeValue t);

	// From SimpleParticle
	Interval GetValidity(TimeValue t);     
	BOOL EmitterVisible();     
	MarkerType GetMarkerType();   
	PCSavePt *sdata;
	//    unsigned int rseed; // don't need it anymore because of RandGenerator

	// From BaseObject
	int IsInstanceDependent() {return 1;}
	void GetFilename(TCHAR *filename);
	void SetupTargetList();
	int SaveSettings(int overwrite,TCHAR *newname);
	int GetSettings(int setnum,TCHAR *newname);
	int RemSettings(int setnum,TCHAR *newname);
	IOResult Load(ILoad *iload);
	IOResult Save(ISave *isave);
	void AssignMtl(INode *node,INode *topnode,int subtree,TimeValue t);
	void DoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
	void RetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
	void GetSubs(INode *node,INode *topnode,int subtree,TimeValue t);
	void CntDoGroupMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
	void CntRetrieveMtls(INode *node,int subtree,int *numsubs,int *numtabs,int *tabmax,TimeValue t);
	BOOL backpatch;
	Mtllst mttab;
	TimeValue ParticleLife(TimeValue t, int i);
	Point3 ParticlePosition(TimeValue t,int i);
	Point3 ParticleVelocity(TimeValue t,int i);     
	float ParticleSize(TimeValue t,int i);
	int ParticleCenter(TimeValue t,int i);
	void DoSpawn(int j,int spmult,SpawnVars spvars,TimeValue lvar,BOOL emits);

	CreateMouseCallBack* GetCreateMouseCallBack();

	TCHAR *GetObjectName();
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);     
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);    
	void BuildEmitter(TimeValue t, Mesh& amesh);
	void BirthParticle(INode *node,TimeValue bt,int num,VelDir *ptvel,Mesh* amesh,Point3* fnorms,Matrix3 disttm);
	BOOL ComputeParticleStart(TimeValue t0,int c);
	// From GeomObject
	Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);
	// Animatable methods      
	Class_ID ClassID() {return ICPCLOUD_CLASS_ID;} 
	// From ref

	CLONE_REF_TARGET_HANDLE;

	// From Simple Particle
	void UpdateParticles(TimeValue t,INode *node);
	ParamDimension *GetParameterDim(int pbIndex);
	TSTR GetParameterName(int pbIndex);
	void InvalidateUI();
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	int HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, 
	IPoint2 *p, ViewExp *vpt);
	void MovePart(int j,TimeValue dt,BOOL fullframe,int tpf);
	void ResetSystem(TimeValue t,BOOL full=TRUE);

	// bridgeable particle system
	void ApplyOutParams( const ICBridgeableParticleSystemOutParams &outParams );
	void FetchInParams( ICBridgeableParticleSystemInParams &inParams, TimeValue t, INode *pNode );
	virtual void InitParams();
	void InitUIParams();

	int GetNListBaser();
	void RegisterModifierReference( Modifier *pModifier );
	void UpdateICForceModifiers( TimeValue t,INode *node );
	void GetICForceModifiers( INode *pNode, ModifierList &modifiers );
	void UpdateICForceModifierBindedParameterValues( Modifier *pModifier, TimeValue t );

	void ForceValidParameterBlockValues();
	void UpdateParameterMapping();
};
//--- ClassDescriptor and class vars ---------------------------------

class ICPCloudClassDesc:public ClassDesc {
   public:
   int         IsPublic() {return 1;}
   void *         Create(BOOL loading = FALSE) {return new ICPCloudParticle;}
   const TCHAR *  ClassName(); 
   SClass_ID      SuperClassID() {return GEOMOBJECT_CLASS_ID;}
   Class_ID    ClassID() {return ICPCLOUD_CLASS_ID;}
   const TCHAR*   Category(); 
   int         BeginCreate(Interface *i);
   int         EndCreate(Interface *i);
   void        ResetClassParams(BOOL fileReset);
   };

class ICPCPickOperand : 
      public PickModeCallback,
      public PickNodeCallback {
   public:     
      ICPCloudParticle *po;
      int dodist,repi;
      
      ICPCPickOperand() {po=NULL;}

      BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
      BOOL Pick(IObjParam *ip,ViewExp *vpt);

      void EnterMode(IObjParam *ip);
      void ExitMode(IObjParam *ip);

      BOOL RightClick(IObjParam *ip, ViewExp *vpt) { return TRUE; }
      BOOL Filter(INode *node);
      
      PickNodeCallback *GetFilter() {return this;}
   };
class ICPCPickNorm : 
      public PickModeCallback,
      public PickNodeCallback {
   public:     
      ICPCloudParticle *po;
      
      ICPCPickNorm() {po=NULL;}

      BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
      BOOL Pick(IObjParam *ip,ViewExp *vpt);

      void EnterMode(IObjParam *ip);
      void ExitMode(IObjParam *ip);

      BOOL RightClick(IObjParam *ip, ViewExp *vpt) { return TRUE; }
      BOOL Filter(INode *node);
      
      PickNodeCallback *GetFilter() {return this;}
   };

class ICPCObjectListRestore : public RestoreObj
{
public:        
	ICPCloudParticle *po;
	Tab<INode*> unodes;
	Tab<INode*> rnodes;
	int lnum,lnum2;

	ICPCObjectListRestore(ICPCloudParticle *p) 
	{
		po = p;
		unodes = p->nlist;
	}

	void Restore(int isUndo)
	{
		if ( isUndo )
		{
			rnodes = po->nlist;
		}
		po->nlist = unodes;
		po->NotifyDependents( FOREVER, 0, REFMSG_CHANGE );
	}

	void Redo()
	{
		po->nlist = rnodes;
		po->NotifyDependents( FOREVER, 0, REFMSG_CHANGE );
	}
};

class ICPCLifeListRestore : public RestoreObj
{
public:        
	ICPCloudParticle *po;
	Tab<int> utimes;
	Tab<int> rtimes;
	ICPCLifeListRestore( ICPCloudParticle *p ) 
	{
		po = p;
		utimes = p->llist;
	}

	void Restore(int isUndo)
	{
		if ( isUndo )
		{ 
			rtimes = po->llist; 
		}
		po->llist=utimes;
		if ( po->ip )
		{
			po->SetUpLifeList();
		}
	}

	void Redo() 
	{
		po->llist = rtimes;
		if ( po->ip )
		{
			po->SetUpLifeList();
		}
	}
};

class ICCreatePCPartDelNode : public RestoreObj
{
public:        
	ICPCloudParticle *obj;
	TSTR name;
	int dist;

	ICCreatePCPartDelNode(ICPCloudParticle *o, TSTR n,int d)
	{
		obj = o; 
		name = TSTR( n ); 
		dist = d;
	}

	void Restore( int isUndo )
	{
		if ( dist == shownormname )
		{
			obj->normname = name;
		}
		else if ( dist )
		{
			obj->distname = name;
		}
		else 
		{
			obj->custname = name;
		}
		obj->ShowName( dist );
	}

	void Redo() 
	{ 
		if (dist==shownormname) obj->normname=TSTR(_T(""));
		else if (dist) obj->distname=TSTR(_T(""));
		else obj->custname = TSTR(_T(""));
		TSTR name=TSTR(GetString(IDS_AP_OBJECTSTR)) + TSTR(GetString(IDS_AP_NONE));
		if (dist==shownormname)
		{ 
			if (obj->hgen) SetWindowText(GetDlgItem(obj->hgen, IDC_AP_PCLOUDOBJDIRNAME), name);
		}
		if (dist)
		{ 
			if (obj->hparam)SetWindowText(GetDlgItem(obj->hparam, IDC_AP_FILLPICKOBJECT), name);
		}
		else
		{ 
			if (obj->hptype) SetWindowText(GetDlgItem(obj->hptype, IDC_AP_INSTANCESRCNAME), name);
		}
	}	

	TSTR Description() {return GetString(IDS_AP_COMPICK);}
};

class ICCreatePCPartPickNode : public RestoreObj
{
public:        
	ICPCloudParticle *obj;
	TSTR name,name2;
	int dist;
	
	ICCreatePCPartPickNode( ICPCloudParticle *o, TSTR n, TSTR n1, int d )
	{
		obj = o; 
		name = TSTR( n );
		name2 = TSTR( n1 );
		dist = d;
	}
	
	void Restore( int isUndo )
	{
		if ( dist == shownormname )
		{
			if ( obj->normname )
				obj->normname=name;
			else obj->normname=TSTR(_T(""));
		}
		else if (dist)
		{ 
			if (obj->distnode) obj->distname = name;
			else  obj->distname=TSTR(_T(""));
		}
		else
		{ 
			if (obj->custnode) obj->custname = name;
			else  obj->custname=TSTR(_T(""));
		}
		obj->ShowName(dist);
	}

	void Redo() 
	{ 
		if (((dist)&&(obj->hptype))||((!dist)&&(obj->hparam)))
		{ 
			TSTR name;
			if ( dist == shownormname ) obj->normname=name2;
			else if (dist) obj->distname=name2;
			else  obj->custname = name2;
			name=TSTR(GetString(IDS_AP_OBJECTSTR))+(_tcslen(name2)>0 ? name2 : TSTR(GetString(IDS_AP_NONE)));
			if (dist==shownormname)
			{
				if (obj->hgen) SetWindowText(GetDlgItem(obj->hgen, IDC_AP_PCLOUDOBJDIRNAME), name);
			}
			else if (dist)
			{  
				if (obj->hparam) SetWindowText(GetDlgItem(obj->hparam, IDC_AP_FILLPICKOBJECT), name);
			}
			else if (obj->hptype) SetWindowText(GetDlgItem(obj->hptype, IDC_AP_INSTANCESRCNAME), name);
		}
	}

	TSTR Description()
	{
		return GetString( IDS_AP_COMPICK );
	}
};

class ICPCParticleDisableDlgProc : public ParamMapUserDlgProc {
   public:
      ICPCloudParticle *po;
      ICustButton *iBut;

      ICPCParticleDisableDlgProc(ICPCloudParticle *p) {po=p;}
      INT_PTR DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
      void Update(TimeValue t);
      void DeleteThis() {delete this;}
   };

class ICPCAnimationKeyingDlgProc : public ParamMapUserDlgProc 
{
public:
	ICPCloudParticle *po;
	ICustButton *iBut;

public:
	ICPCAnimationKeyingDlgProc(ICPCloudParticle *p) {po=p;}
	INT_PTR DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void Update(TimeValue t);
	void DeleteThis() {delete this;}

private:
	void SetAnimationOffsetWidgetsEnabled( HWND hWnd, BOOL bEnabled );
	void SetAnimationScalingWidgetsEnabled( HWND hWnd, BOOL bEnabled );
};

class ICPCParticleParmDlgProc : public ParamMapUserDlgProc {
   public:
      ICPCloudParticle *po;
      ICustButton *iBut;

      ICPCParticleParmDlgProc(ICPCloudParticle *p) {po=p;}
      INT_PTR DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
      void Update(TimeValue t);
      void DeleteThis() {delete this;}
   };

class ICCreatePCloudProc : public MouseCallBack, ReferenceMaker
{
private:
	IObjParam *ip;
	void Init( IObjParam *i ){ ip=i; }
	CreateMouseCallBack *createCB;   
	INode *CloudNode;
	ICPCloudParticle *CloudObject;
	int attachedToNode;
	IObjCreate *createInterface;
	ClassDesc *cDesc;
	Matrix3 mat;  // the nodes TM relative to the CP
	Point3 p0,p1;
	IPoint2 sp0, sp1;
	BOOL square;

	int ignoreSelectionChange;

	int lastPutCount;
	void CreateNewObject(); 

	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int i) { return (RefTargetHandle)CloudNode; } 
	void SetReference(int i, RefTargetHandle rtarg) { CloudNode = (INode *)rtarg; }

	// StdNotifyRefChanged calls this, which can change the partID to new value 
	// If it doesnt depend on the particular message& partID, it should return
	// REF_DONTCARE
	BOOL SupportAutoGrid(){ return TRUE; }
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message );

public:
	void Begin( IObjCreate *ioc, ClassDesc *desc );
	void End();

	ICCreatePCloudProc()
	{
		ignoreSelectionChange = FALSE;
	}
	
	// int createmethod(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	int proc( HWND hwnd, int msg, int point, int flag, IPoint2 m );
};


#define CID_CREATEDBGPCLOUDMODE  CID_USER +14

class ICCreatePCloudMode : public CommandMode {      
   public:     
      ICCreatePCloudProc proc;
      IObjParam *ip;
      ICPCloudParticle *obj;
      void Begin( IObjCreate *ioc, ClassDesc *desc ) { proc.Begin( ioc, desc ); }
      void End() { proc.End(); }
      void JumpStart(IObjParam *i,ICPCloudParticle*o);

      int Class() {return CREATE_COMMAND;}
      int ID() { return CID_CREATEDBGPCLOUDMODE; }
      MouseCallBack *MouseProc(int *numPoints) {*numPoints = 10000; return &proc;}
      ChangeForegroundCallback *ChangeFGProc() {return CHANGE_FG_SELECTED;}
      BOOL ChangeFG( CommandMode *oldMode ) { return (oldMode->ChangeFGProc() != CHANGE_FG_SELECTED); }
      void EnterMode() 
      { GetCOREInterface()->PushPrompt(GetString(IDS_AP_CREATEMODE));
        SetCursor(LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CROSS_HAIR)));
      }
      void ExitMode() {GetCOREInterface()->PopPrompt();SetCursor(LoadCursor(NULL, IDC_ARROW));}
   };

class ICPCloudEmitterCreateCallback : public CreateMouseCallBack {
   public:
      ICPCloudParticle *po;
      Point3 p0,p1;
      IPoint2 sp0, sp1;
      BOOL square;
      int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
   };

class ICPCParticleGenDlgProc : public ParamMapUserDlgProc {
   public:
      ICPCloudParticle *po;
      ICustButton *iBut;

      ICPCParticleGenDlgProc(ICPCloudParticle *p) {po=p;}
      INT_PTR DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
      void Update(TimeValue t);
      void DeleteThis() {delete this;}
   };

class ICPCParticleSpinDlgProc : public ParamMapUserDlgProc {
   public:
      ICPCloudParticle *po;

      ICPCParticleSpinDlgProc(ICPCloudParticle *p) {po=p;}
      INT_PTR DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
      void Update(TimeValue t);
      void DeleteThis() {delete this;}
   };


class ICPCParticleSpawnDlgProc : public ParamMapUserDlgProc 
{
public:
	ICPCloudParticle *po;
	ICustButton *iBut,*iButrep;

	ICPCParticleSpawnDlgProc(ICPCloudParticle *p) {po=p;}
	INT_PTR DlgProc(TimeValue t,IParamMap *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void Update(TimeValue t);
	void DeleteThis() {delete this;}
};

class ICPCloudPostLoadCallback : public PostLoadCallback {
	public:
		ParamBlockPLCB *cb;
		ICPCloudPostLoadCallback(ParamBlockPLCB *c) {cb=c;}
		void proc(ILoad *iload);
	};

#endif // _IC_PCLOUD_H
