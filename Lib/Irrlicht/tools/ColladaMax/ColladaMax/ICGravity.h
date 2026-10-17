#ifndef __GRAVITY__H
#define __GRAVITY__H

#include "resource.h"
#include "MaxForceDefinitions.h"

#undef snprintf
#include "PForceImpl.h"

class ICGravityMod;

class ICGravityMtl: public Material 
{
   public:
   ICGravityMtl();
};

class ICForceObject : public SimpleWSMObject2 
{  
   public:     
//    static IParamMap *pmapParam;
      static IObjParam *ip;
      static HWND hSot;
               
      ICForceObject() {}
      BOOL SupportsDynamics() {return TRUE;}

      // From Animatable      
      void DeleteThis() {delete this;}    
      void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev );
      void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);    
            
      // from object    
      CreateMouseCallBack* GetCreateMouseCallBack();     
      
      // From SimpleWSMObject    
      void InvalidateUI();    
      void BuildMesh(TimeValue t);

      virtual int DialogID()=0;
//    virtual ParamUIDesc *UIDesc()=0;
//    virtual int UIDescLength()=0;
      int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
      void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
};

class ICGravityObject : public ICForceObject //,IOperatorInterface // Bayboro 9/18/01
{  
public:                          
	ICGravityObject();     
	~ICGravityObject();    

	// From Animatable      
	void DeleteThis() {delete this;}    
	Class_ID ClassID() {return FSForceGravity::IC_OBJECT_CLASSID;}
	CLONE_REF_TARGET_HANDLE;
	TCHAR *GetObjectName() {return GetString(IDS_RB_GRAVITY);}
	          
	// From WSMObject
	Modifier *CreateWSMMod(INode *node);
	ForceField *GetForceField(INode *node);

	// From SimpleWSMObject          
	ParamDimension *GetParameterDim(int pbIndex);
	//    TSTR GetParameterName(int pbIndex);

	int DialogID() {return IDD_GRAVITYPARAM;}
	//    ParamUIDesc *UIDesc();
	//    int UIDescLength();

	// Direct paramblock access
	int   NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int i) { return pblock2; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

	// from ref
	IOResult Load(ILoad *iload);

	//    int NPOpInterface(TimeValue t,ParticleData *part,float dt,INode *node,int index); // Bayboro 9/18/01
	//    void* GetInterface(ULONG id); // Bayboro 9/18/01
	ForceField *ff;
	//void SetUpModifier(TimeValue t,INode *node);
	ICGravityMod *mf;
};

//--- ClassDescriptor and class vars ---------------------------------
class ICGravityClassDesc: public ClassDesc2 
{
   public:
   int         IsPublic() {return 1;}
   void *         Create(BOOL loading = FALSE) { return new ICGravityObject;}
   const TCHAR *  ClassName() {return GetString(IDS_RB_GRAVITY_CLASS);}
   SClass_ID      SuperClassID() {return WSM_OBJECT_CLASS_ID; }
   Class_ID    ClassID() {return FSForceGravity::IC_OBJECT_CLASSID;}
   const TCHAR*   Category() {return GetString(IDS_EP_SW_FORCES);}

   // Hardwired name, used by MAX Script as unique identifier
   const TCHAR*   InternalName()          { return _T("Gravity"); }
   HINSTANCE      HInstance()             { return hInstance; }
};

ClassDesc* GetICGravityObjDesc();

class ICGravityField : public ForceField 
{
public:
	ICGravityObject *obj;
	INode *node;
	Matrix3 tm;
	Interval tmValid;
	Point3 force;
	Interval fValid;
	int type;

public:
	Point3 Force(TimeValue t,const Point3 &pos, const Point3 &vel,int index);
	void DeleteThis() {delete this;} // RB 5/12/99
	void UpdateBindedParameters( irr::ps::PGravity::parameters_type &params, irr::core::matrix4 &irrMat, TimeValue t );
};

class ICGravityMod : public SimpleWSMMod 
{
public:           
	ICGravityField force;

	ICGravityMod(){}
	ICGravityMod(INode *node,ICGravityObject *obj);     
	~ICGravityMod();

	// From Animatable
	void GetClassName(TSTR& s) {s= GetString(IDS_RB_GRAVITYMOD);}
	SClass_ID SuperClassID() {return WSM_CLASS_ID;}
	void DeleteThis() {delete this;}
	Class_ID ClassID() { return FSForceGravity::IC_MODIFIER_CLASSID;}
	CLONE_REF_TARGET_HANDLE;
	TCHAR *GetObjectName() {return GetString(IDS_RB_GRAVITYBINDING);}

	void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

	// From SimpleWSMMod    
	Interval GetValidity(TimeValue t);     
	Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);

	irr::ps::PGravity::parameters_type m_params;
	irr::core::matrix4 tm;
};

//--- ClassDescriptor and class vars ---------------------------------
class ICGravityModClassDesc:public ClassDesc 
{
   public:
   int         IsPublic() { return 0; }
   void *         Create(BOOL loading = FALSE) {return new ICGravityMod;}
   const TCHAR *  ClassName() { return GetString(IDS_RB_GRAVITYMOD);}
   SClass_ID      SuperClassID() {return WSM_CLASS_ID;}
   Class_ID    ClassID() {return FSForceGravity::IC_MODIFIER_CLASSID;}
   const TCHAR*   Category() {return _T("");}
};

ClassDesc* GetICGravityModDesc();

class ICForceObjCreateCallback : public CreateMouseCallBack 
{
   public:
      ICForceObject *ob;  
      Point3 p0, p1;
      IPoint2 sp0;
      int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
};

class ICGravityDeformer : public Deformer 
{
   public:     
      Point3 Map(int i, Point3 p) {return p;}
};

class ICGravityPostLoadCallback : public PostLoadCallback 
{
   public:
      ParamBlock2PLCB *cb;
      ICGravityPostLoadCallback(ParamBlock2PLCB *c) {cb=c;}
      void proc(ILoad *iload) 
      {
         DWORD oldVer = ((ICGravityObject*)(cb->targ))->pblock2->GetVersion();
         ReferenceTarget *targ = cb->targ;
         cb->proc(iload);
         if (oldVer<1) 
         {  
            ((ICGravityObject*)targ)->pblock2->SetValue(FSForceGravity::PB_HOOPSON,0,0);
         }
         delete this;
      }
};


#endif // __GRAVITY__H
