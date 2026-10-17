#ifndef __DEFLECTOR__H
#define __DEFLECTOR__H

#include "resource.h"
#include "MaxForceDefinitions.h"
#include "ICollision.h"

#undef snprintf
#include "PForceImpl.h"

class ICDeflectObject : public SimpleWSMObject2 {	
	public:		
//		static IParamMap *pmapParam;
		static IObjParam *ip;
		static HWND hSot;
					
		ICDeflectObject();
		~ICDeflectObject();

		// From Animatable		
		void DeleteThis() {delete this;}		
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
		Class_ID ClassID() {return FSDeflector::IC_OBJECT_CLASSID;}
		CLONE_REF_TARGET_HANDLE;
		TCHAR *GetObjectName() {return GetString(IDS_RB_DEFLECTOR);}
				
		// from object		
		CreateMouseCallBack* GetCreateMouseCallBack();		
		
		// From SimpleWSMObject		
		void InvalidateUI();		
		void BuildMesh(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
//		TSTR GetParameterName(int pbIndex);		
		
		// From WSMObject
		Modifier *CreateWSMMod(INode *node);	
		
		// Direct paramblock access
		int	NumParamBlocks() { return 1; }
		IParamBlock2* GetParamBlock(int i) { return pblock2; }
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

		IOResult Load(ILoad *iload);
//watje
		CollisionObject *GetCollisionObject(INode *node);
};

class ICDeflectorClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new ICDeflectObject;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_DEFLECTOR_CLASS);}
	SClass_ID		SuperClassID() {return WSM_OBJECT_CLASS_ID; }
	Class_ID		ClassID() {return FSDeflector::IC_OBJECT_CLASSID;}
	const TCHAR* 	Category() {return GetString(IDS_EP_SW_DEFLECTORS);}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return _T("Deflector"); }
	HINSTANCE		HInstance()					{ return hInstance; }
	};

ClassDesc* GetICDeflectObjDesc();

//--- ICDeflectMod -----------------------------------------------------
class ICDeflectorField : public CollisionObject {
	public:	
		ICDeflectorField()
		{
			srand(1264873);
			for (int i =0;i < 500; i++)
				{
				randomFloat[i] = (float)rand()/(float)RAND_MAX;
				}
			colp=NULL;
		}

		void DeleteThis() 
			{	 
			if (colp) colp->DeleteThis();
			colp=NULL;
			delete this;

			}

		~ICDeflectorField()
		{
			if (colp) colp->DeleteThis();
			colp=NULL;
		}

		ICDeflectObject *obj;
		INode *node;
		Matrix3 tm, invtm;
		Interval tmValid;		
		CollisionPlane *colp;

		float randomFloat[500];

		BOOL CheckCollision(TimeValue t,Point3 &pos, Point3 &vel, float dt, int index, float *ct,BOOL UpdatePastCollide);
		Object *GetSWObject();

//hmm moved these so they are only gotten at one tick in time
		float width, height, at, bounce;
		float friction, chaos, variation, inherit;
		int quality;

		void UpdateBindedParameters( irr::ps::PDeflector::parameters_type &params, irr::core::matrix4 &irrMat, TimeValue t );
};

class ICDeflectMod : public SimpleWSMMod {
	public:				
		ICDeflectorField deflect;

		ICDeflectMod() {}
		ICDeflectMod(INode *node,ICDeflectObject *obj);		

		// From Animatable
		void GetClassName(TSTR& s) {s= GetString(IDS_RB_DEFLECTMOD);}
		SClass_ID SuperClassID() {return WSM_CLASS_ID;}
		void DeleteThis() {delete this;}
		Class_ID ClassID() { return FSDeflector::IC_MODIFIER_CLASSID;}
		CLONE_REF_TARGET_HANDLE;
		TCHAR *GetObjectName() {return GetString(IDS_RB_DEFLECTORBINDING);}

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);

		// From SimpleWSMMod		
		Interval GetValidity(TimeValue t);		
		Deformer& GetDeformer(TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat);

		irr::ps::PDeflector::parameters_type m_params;
		irr::core::matrix4 tm;
};

//--- ClassDescriptor and class vars ---------------------------------
class ICDeflectorModClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 0;}
	void *			Create(BOOL loading = FALSE) { return new ICDeflectMod;}
	const TCHAR *	ClassName() {return GetString(IDS_RB_DEFLECTMOD);}
	SClass_ID		SuperClassID() {return WSM_CLASS_ID; }
	Class_ID		ClassID() {return FSDeflector::IC_MODIFIER_CLASSID;}
	const TCHAR* 	Category() {return _T("");}
};

ClassDesc* GetICDeflectModDesc();

class ICDeflectObjCreateCallback : public CreateMouseCallBack {
	public:
		ICDeflectObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
};

//need a post load call back to create a planar collision engine for our pblock
class ICDeflectObjectLoad : public PostLoadCallback {
	public:
		ICDeflectObject *n;
		ICDeflectObjectLoad(ICDeflectObject *ns) {n = ns;}
		void proc(ILoad *iload)
		{  
			ReferenceTarget *rt;
			Interval iv;
			n->pblock2->GetValue(FSDeflector::deflect_collider,0,rt,iv);
			if (rt == NULL)
				{
				macroRecorder->Disable();
				CollisionPlane *colp = (CollisionPlane*)CreateInstance(REF_MAKER_CLASS_ID, PLANAR_COLLISION_ID);
				if (colp)
					n->pblock2->SetValue(FSDeflector::deflect_collider,0,(ReferenceTarget*)colp);
				macroRecorder->Enable();
				}

			// Bayboro
			// Parameters Variation, Chaos, Friction and Inherit Vel are readjusted to be
			// consistent with corresponding parameters in Omni- and DynaFlectors.
			if (HIWORD(iload->GetFileSaveVersion() < 3900)) // MAX before Magma=v.3.900
			{ ; // do nothing since Variation, Chaos, Friction and Inherit Vel aren't present in Shiva and before
			}
			else if ((HIWORD(iload->GetFileSaveVersion()) < 4000) && 
					 (LOWORD(iload->GetFileSaveVersion()) < 48)) // MAX v.3.9, build 48
			{
				Control *cont, *mult;
				float pcnt = 0.01f;
	
				int defl_chan[3] = {FSDeflector::deflect_variation, FSDeflector::deflect_chaos, FSDeflector::deflect_friction};
				for( int i=0; i<3; i++)
				{	cont = n->pblock2->GetController(defl_chan[i]);
					if (cont == NULL)
						n->pblock2->SetValue(defl_chan[i], 0, 0.01f*n->pblock2->GetFloat(defl_chan[i], 0));
					else
					{	mult = NewDefaultFloatController();
						mult->SetValue(0, &pcnt);
						cont->AppendMultCurve(mult);
					}
				}

				cont = n->pblock2->GetController(FSDeflector::deflect_inherit_vel);
				if (cont != NULL) // user's done some manipulations with the parameters; let's rescale
				{	mult = NewDefaultFloatController();
					mult->SetValue(0, &pcnt);
					cont->AppendMultCurve(mult);
				}
				else
				{	if (n->pblock2->GetFloat(FSDeflector::deflect_inherit_vel,0) == 1.0f)
					{	; // do nothing since user probably haven't adjusted the parameter and left it as default value
						// therefore it should be left as 1.0 multiplier
					}
					else	// user's done some manipulations with the parameters; let's rescale
						n->pblock2->SetValue(FSDeflector::deflect_inherit_vel, 0, 0.01f*n->pblock2->GetFloat(FSDeflector::deflect_inherit_vel, 0));
				}
			}

			delete this; 
		} 
};

class ICDeflectDeformer : public Deformer {
	public:		
		Point3 Map(int i, Point3 p) {return p;}
};







#endif // __DEFLECTOR__H
