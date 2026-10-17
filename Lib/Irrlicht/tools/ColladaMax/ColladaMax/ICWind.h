#ifndef __WIND__H
#define __WIND__H

#include "resource.h"
#include "MaxForceDefinitions.h"

#undef snprintf
#include "PForceImpl.h"

class ICWindMtl: public Material 
{
	public:
	ICWindMtl();
};

class ICWindMod;

class ICWindObject : public SimpleWSMObject2 // ,IOperatorInterface // Bayboro 9/18/01
{	
	public:		
		static IObjParam *ip;
		static HWND hSot;

		ICWindObject();		
		~ICWindObject();		
		TCHAR *GetObjectName() {return GetString(IDS_RB_WIND);}
					
		BOOL SupportsDynamics() {return TRUE;}

		// From Animatable		
		void DeleteThis() {delete this;}		
		Class_ID ClassID() {return FSForceWind::IC_OBJECT_CLASSID;}		
		CLONE_REF_TARGET_HANDLE;
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
				
		// from object		
		CreateMouseCallBack* GetCreateMouseCallBack();	
		
		// From WSMObject
		Modifier *CreateWSMMod(INode *node);
		ForceField *GetForceField(INode *node);
		
		// From SimpleWSMObject		
		ParamDimension *GetParameterDim(int pbIndex);
		void InvalidateUI();		
		void BuildMesh(TimeValue t);

		int DialogID() {return IDD_WINDPARAM;}

		// Direct paramblock access
		int	NumParamBlocks() { return 1; }
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

		// from ref
		IOResult Load(ILoad *iload);
		int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
		void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );

//		int NPOpInterface(TimeValue t,ParticleData *part,float dt,INode *node,int index); // Bayboro 9/18/01
//		void* GetInterface(ULONG id); // Bayboro 9/18/01
		ForceField *ff;
		//void SetUpModifier(TimeValue t,INode *node);
		ICWindMod *mf;
};

class ICWindClassDesc:public ClassDesc2 
{
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new ICWindObject;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_WIND_CLASS);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return FSForceWind::IC_OBJECT_CLASSID;}
	const TCHAR* 	Category() {return GetString(IDS_EP_SW_FORCES);}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("Wind"); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

ClassDesc* GetICWindObjDesc();

class ICWindMod;

class ICWindField : public ForceField 
{
public:
	ICWindObject *obj;
	INode *node;
	Matrix3 tm;
	Interval tmValid;
	Point3 force;
	Interval fValid;
	int type;

public:
	Point3 Force(TimeValue t,const Point3 &pos, const Point3 &vel,int index);
	void DeleteThis() {delete this;} // RB 5/12/99
	void UpdateBindedParameters( irr::ps::PWind::parameters_type &params, irr::core::matrix4 &irrMat, TimeValue t );
};

class ICWindMod : public SimpleWSMMod 
{
	public:				
		ICWindField force;

		ICWindMod() {}
		ICWindMod(INode *node,ICWindObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_RB_WINDMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return FSForceWind::IC_MODIFIER_CLASSID; }
		CLONE_REF_TARGET_HANDLE;
		TCHAR *GetObjectName() {return GetString(IDS_RB_WINDBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);

		irr::ps::PWind::parameters_type m_params;
		irr::core::matrix4 tm;
};

//--- ClassDescriptor and class vars ---------------------------------

class ICWindModClassDesc:public ClassDesc 
{
	public:
	int 			IsPublic() { return 0; }
	void *			Create(BOOL loading = FALSE) {return new ICWindMod;}
	const TCHAR *	ClassName() { return GetString(IDS_RB_WINDMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID;}
	Class_ID		ClassID() {return FSForceWind::IC_MODIFIER_CLASSID; }
	const TCHAR* 	Category() {return _T("");}
};

ClassDesc* GetICWindModDesc();

class ICWindObjCreateCallback : public CreateMouseCallBack {
	public:
		ICWindObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
};

class ICWindDeformer : public Deformer 
{
	public:		
		Point3 Map(int i, Point3 p) {return p;}
};

class ICWindPostLoadCallback : public PostLoadCallback 
{
	public:
		ParamBlock2PLCB *cb;
		ICWindPostLoadCallback(ParamBlock2PLCB *c) {cb=c;}
		void proc(ILoad *iload) 
		{
			DWORD oldVer = ((ICWindObject*)(cb->targ))->pblock2->GetVersion();
			ReferenceTarget *targ = cb->targ;
			cb->proc(iload);
			if (oldVer<1) 
			{	
 				((ICWindObject*)targ)->pblock2->SetValue(FSForceWind::PB_HOOPSON,0,0);
			}
			delete this;
		}
};

#endif // __WIND__H
