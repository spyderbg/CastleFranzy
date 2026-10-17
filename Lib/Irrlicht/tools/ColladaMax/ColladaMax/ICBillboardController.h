/**********************************************************************
 *
	FILE: lookat_cnstrnt.cpp

	DESCRIPTION: A rotation constraint controller that controls the lookat 
				of an object based on the lookat of the target objects

	CREATED BY: Ambarish Goswami 5/2000
	            

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef _IC_BILLBOARD_CONTROLLER_H_
#define _IC_BILLBOARD_CONTROLLER_H_

#include "resource.h"
#include "ICReferenceMaker.h"

#define LOOKAT_CONSTRAINT_CNAME		"Billboard Controller"
#define BILLBOARD_CONTROLLER_CLASS_ID Class_ID(0x4e390738, 0x7c9d01f8)

class ICBillboardHelper;

class ICBillboardController : public Control
{
public:
	enum ReferenceId
	{
		RI_BILLBOARD_HELPER
		, RI_COUNT
	};

public:
	//********************************************************
	// LOOKAT CONSTRAINT
	//********************************************************
	class MyClassDesc : public ClassDesc2 
	{
		public:
		int 			IsPublic() { return 0; }
		void *			Create(BOOL loading) { return new ICBillboardController(loading); }
		const TCHAR *	ClassName() { return LOOKAT_CONSTRAINT_CNAME; }
		SClass_ID		SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
		Class_ID		ClassID() { return BILLBOARD_CONTROLLER_CLASS_ID; }
		const TCHAR* 	Category() { return _T("");  }
		const TCHAR*	InternalName() { return _T("Lookat_Constraint"); }// returns fixed parsable name (scripter-visible name)
		HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
	};

	class ViewportDisplayCallbackObject : public ViewportDisplayCallback
	{
	private:
		ICBillboardController *m_pOwner;

	public:
		ViewportDisplayCallbackObject( ICBillboardController *pOwner = NULL )
			: m_pOwner( pOwner )
		{}

		void Display( TimeValue t, ViewExp *vpt, int flags );
		void GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect );
		BOOL Foreground(); // return TRUE if the object changes a lot or FALSE if it doesn't change much
	};

	class MyTimeChangeCallback : public TimeChangeCallback
	{
		public:
			ICBillboardController* lookAt_controller;
			void TimeChanged( TimeValue t );
	};

	class RotationRestoreObj : public RestoreObj 
	{
	public:
		Quat undo, redo;
		ICBillboardController *cont;
		RotationRestoreObj( ICBillboardController *c );
		void Restore(int isUndo);
		void Redo();
		void EndHold();
	};

private:

	Matrix3 sourceTM;
	Quat curRot, baseRotQuatLocal, baseRotQuatWorld;
	int initialLookAtFlag;

	ViewportDisplayCallbackObject m_preViewportDisplayCallback;
	ICBillboardHelper *m_pBillboardHelper;

public:

	Quat userRotQuat;

	ICBillboardController(const ICBillboardController &ctrl);
	ICBillboardController(BOOL loading=FALSE);

	~ICBillboardController();

	Quat ComputeLookAtQuat(TimeValue t);
	void HoldQuat();
	void Update( TimeValue t );

	// Animatable methods
	Class_ID ClassID() { return BILLBOARD_CONTROLLER_CLASS_ID; }  
	SClass_ID SuperClassID() { return CTRL_ROTATION_CLASS_ID; }  		
	
	void GetClassName(TSTR& s) {s = LOOKAT_CONSTRAINT_CNAME;}
	void DeleteThis() {delete this;} // intellgent destructor
	int IsKeyable() {return 0;}		// if the controller is keyable (for trackview)?

	//void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev ); //to display controller params on the motion panel
	//void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next ); // to undisplay

	inline RefResult NotifyRefChanged( Interval, RefTargetHandle, PartID&, RefMessage )
	{
		return REF_DONTCARE;
	}

	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
	
	void RescaleWorldUnits(float f) {}

	// Control methods
	void Copy(Control *from);
	RefTargetHandle Clone(RemapDir& remap);
	BOOL IsLeaf() {return FALSE;}
	void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method);
	void SetValue(TimeValue t, void *val, int commit, GetSetMethod method);
	void CommitValue(TimeValue t) { }
	void RestoreValue(TimeValue t) { }
	BOOL InheritsParentTransform() {return FALSE;}

	void GetBillboardType( int &type );
	void GetBillboardOrientation( int &orientation );
	void GetFrontVector( TimeValue t, Point3 &v );
	void GetUpVector( TimeValue t, Point3 &v );
	bool IsDisplayBillboardedTargetsEnabled();

	inline ICBillboardHelper *GetHelperPtr()
	{
		return m_pBillboardHelper;
	}

	inline void SetHelperPtr( ICBillboardHelper *pBillboardHelper )
	{
		m_pBillboardHelper = pBillboardHelper;
	}

	inline BOOL IsReplaceable()
	{
		return FALSE;
	}

	static void NotificationCallback( void *param, NotifyInfo *info );
};

ClassDesc *GetICBillboardControllerDesc();

#endif // _IC_BILLBOARD_CONTROLLER_H_