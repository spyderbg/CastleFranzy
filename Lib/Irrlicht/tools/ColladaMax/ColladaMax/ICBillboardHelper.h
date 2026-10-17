#ifndef __IC_BILLBOARD_HELPER__H
#define __IC_BILLBOARD_HELPER__H

#include "resource.h"
#include "ICReferenceMaker.h"
#include "ICBillboardController.h"

#define BILLBOARD_HELPER_CLASSID_PARTA 0x44fe0608
#define BILLBOARD_HELPER_OBJECT_CLASSID Class_ID( BILLBOARD_HELPER_CLASSID_PARTA, 0 )

#define ENABLE_BILLBOARD_CONTROLLERS 1

typedef fm::pvector<INode> NodeList;
typedef fm::pvector<ICBillboardController> BillboardControllerList;

class ICBillboardHelper : public HelperObject
{	
public:
	enum ParamBlock
	{
		PB_MAIN_ID
		, PB_MAIN_REF_ID = PB_MAIN_ID
		, PB_COUNT
	};

	enum ParamId
	{
		PI_MIN_BOX
		, PI_MAX_BOX
		, PI_DISPLAY_LINKS
		, PI_DISPLAY_BILLBOARDED_TARGETS
		, PI_DISPLAY_VECTORS_ON_TARGETS
		, PI_BILLBOARD_NODE_LIST
		, PI_BILLBOARD_TYPE
		, PI_BILLBOARD_ORIENTATION
		, PI_BILLBOARD_FRONT
		, PI_BILLBOARD_UP
		, PI_COUNT
	};

	enum Rollup
	{
		R_DISPLAY_OPTIONS
		, R_BILLBOARDS
		, R_BILLBOARD_CONTROLLER
		, R_COUNT
	};

	class BillboardListPBValidator : public PBValidator
	{ 
	public:

		inline BOOL Validate(PB2Value& v){ return TRUE; }
		BOOL Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex);
	};

	class BillboardListPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		void TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count );
	};

	class BillboardControllerPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		//void TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count );
	};

	class ICBillboardHelperPostLoadCallback : public PostLoadCallback 
	{
		public:
			ICBillboardHelper *m_pOwner;
			ICBillboardHelperPostLoadCallback( ICBillboardHelper *pOwner )
				: m_pOwner( pOwner )
				{}

			void proc( ILoad *iload );
	};

	class ICBillboardHelperCreateCallback : public CreateMouseCallBack {
		public:
			ICBillboardHelper *ob;	
			Point3 p0, p1;
			IPoint2 sp0, sp1;
			int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	};

	class SubObject : public GenSubObjType
	{
	public:
		SubObject( MCHAR *nm, MCHAR* pFilePrefix, int idx ) 
			: GenSubObjType( nm, pFilePrefix, idx )
		{}

		SubObject( int idx )
			: GenSubObjType( idx )
		{}
	};

private:
	Mesh m_gizmoMesh;
	Mesh m_linksMesh;
	Mesh m_frontVectorMesh;
	Mesh m_upVectorMesh;

	SubObject m_frontVectorSubObject;
	SubObject m_upVectorSubObject;

	NodeList m_previousBillboardNodeList;

	static MCHAR *k_frontVectorSubObjectName;
	static MCHAR *k_upVectorSubObjectName;

	static MoveModBoxCMode *s_pMoveModBox;
	static RotateModBoxCMode *s_pRotateModBox;
	static UScaleModBoxCMode *s_pUScaleModBox;
	static NUScaleModBoxCMode *s_pNUScaleModBox;
	static SquashModBoxCMode *s_pSquashModBox;
	static SelectModBoxCMode *s_pSelectModBox;
	
public:
	IParamBlock2 *pblock2;

	static BillboardListPBValidator s_billboardListValidator;
	static BillboardListPBAccessor s_billboardListAccessor;
	static BillboardControllerPBAccessor s_billboardControllerAccessor;

	static const Point3 k_gizmoColor;
	static const Point3 k_linksColor;
	static const Point3 k_frontVectorColor;
	static const Point3 k_upVectorColor;

private:
	void BuildVectorMeshes( TimeValue t, ViewExp *vpt );
	Matrix3 GetObjectTM( INode *pNode, TimeValue t );

	static void BuildVectorMesh( TimeValue t, ViewExp *vpt, Mesh &mesh, const Point3 &vector );
	static void TransformToDisplayedVector( Point3 &v, ViewExp *vpt );

public:		
	ICBillboardHelper();
	~ICBillboardHelper();

	// From Animatable		
	void DeleteThis() {delete this;}		
	void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
	CLONE_REF_TARGET_HANDLE;
			
	// from object		
	CreateMouseCallBack* GetCreateMouseCallBack();		
	
	void BuildMesh( TimeValue t, INode *pNode, ViewExp *vpt );
	
	// Direct paramblock access
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int i) { return pblock2; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

	// inherited virtual methods for Reference-management
	// Need ref on pblock2 for MakeAutoParamBlocks to work
	int NumRefs(){ return 1; }
	RefTargetHandle GetReference( int i ){ return pblock2; }
	void SetReference( int i, RefTargetHandle rtarg ){ pblock2 = ( IParamBlock2 * ) rtarg; }
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message ){ return REF_DONTCARE; }

	//IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
	
	//  inherited virtual methods:
	// From BaseObject
	int HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt );
	void Snap( TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt );
	int Display( TimeValue t, INode* inode, ViewExp *vpt, int flags );
	Interval ObjectValidity(TimeValue t);

	int NumSubObjTypes();
	ISubObjType *GetSubObjType( int id );
	void ActivateSubobjSel( int level, XFormModes &modes );
	void GetSubObjectTMs( SubObjAxisCallback *cb, TimeValue  t, INode *  node, ModContext *mc );
	void GetSubObjectCenters( SubObjAxisCallback *cb, TimeValue  t, INode *  node, ModContext *mc );

	//void ClearSelection( int selLevel ){}
	//void SelectAll( int selLevel ){}
	//void SelectSubComponent( HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert = FALSE ){}
	//int SubObjectIndex( HitRecord *hitRec ){ return 0; }
	//void CloneSelSubComponents( TimeValue t ){}
	//void Move( TimeValue t, Matrix3 &partm, Matrix3 &tmAxis, Point3 &val, BOOL localOrigin = FALSE ){}
	void Rotate( TimeValue t, Matrix3 &partm, Matrix3 &tmAxis, Quat &val, BOOL localOrigin = FALSE );
	//void Scale( TimeValue t, Matrix3 &partm, Matrix3 &tmAxis,  Point3 &val, BOOL localOrigin = FALSE ){}

	//  From Object
	ObjectState Eval( TimeValue time );
	void GetWorldBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box );
	void GetLocalBoundBox(TimeValue t, INode *pNode, ViewExp *vpt, Box3& box );
	void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel=FALSE );

	void SetGizmoBoxMin( const Point3 &p );
	void SetGizmoBoxMax( const Point3 &p );
	Point3 GetGizmoBoxMin();
	Point3 GetGizmoBoxMax();
	Box3 ComputeGizmoBox( TimeValue t, INode *pNode );

	Box3 ComputeLinksBox( TimeValue t, INode *pNode );

	void GetLinksTM( const Matrix3 &objectTM, Matrix3 &tm );
	void GetVectorsGizmoTM( ViewExp *vpt, const Matrix3 &objectTM, Matrix3 &tm );
	void GetBillboardTargetVectorsTM( ViewExp *vpt, const Matrix3 &objectTM, Matrix3 &tm );

	bool IsDisplayLinksEnabled();
	bool IsDisplayBillboardedTargetsEnabled();
	bool IsDisplayVectorsOnTargetsEnabled();

	int GetBillboardNodeCount();
	INode *GetBillboardNode( int id );
	int GetBillboardNodeId( INode *pNode );
	void DeleteBillboardNode( INode *pNode );
	void DeleteBillboardNode( int id );
	void GetCurrentBillboardNodeList( NodeList &nodeList );
	void InsertBillboardNodes( int start, int count, INode **pNodes );
	void AppendBillboardNodes( int count, INode **pNodes );
	void ClearBillboardNodes();

	void SetBillboardType( int type );
	void GetBillboardType( int &type );

	void SetBillboardOrientation( int orientation );
	void GetBillboardOrientation( int &orientation );

	void GetFrontVector( TimeValue t, Point3 &v );
	void GetUpVector( TimeValue t, Point3 &v );

	void SetFrontVector( TimeValue t, Point3 v );
	void SetUpVector( TimeValue t, Point3 v );

	void GetBillboardControllers( BillboardControllerList &billboardControllers );

	inline int NumSubs()
	{ 
		return 1; 
	}

	inline Animatable* SubAnim( int i )
	{ 
		return pblock2; 
	}
	
	inline TSTR SubAnimName( int i )
	{ 
		return _T( "Parameters" ); 
	}
	
	inline int SubNumToRefNum( int subNum )
	{ 
		return subNum == 0 ? PB_MAIN_REF_ID : -1;
	}

	inline NodeList &GetPreviousBillboardNodeList()
	{
		return m_previousBillboardNodeList;
	}

	inline Class_ID ClassID()
	{ 
		return BILLBOARD_HELPER_OBJECT_CLASSID; 
	}

	inline TCHAR *GetObjectName()
	{
		return GetString( IDS_BILLBOARD_HELPER_OBJECT_NAME );
	}

	inline void InitNodeName( TSTR& s )
	{
		s = GetString( IDS_BILLBOARD_HELPER_OBJECT_NAME );
	}

	inline void GetClassName( TSTR& s )
	{
		s = GetString( IDS_BILLBOARD_HELPER_CLASS_NAME );
	}

	static void NotificationCallback( void *param, NotifyInfo *info );
};

class ICBillboardHelperClassDesc : public ClassDesc2 
{
public:
	int 			IsPublic(){ return 1; }
	void *			Create( BOOL loading = FALSE ) { return new ICBillboardHelper; }
	const TCHAR *	ClassName(){ return GetString( IDS_BILLBOARD_HELPER_CLASS_NAME ); }
	SClass_ID		SuperClassID(){ return HELPER_CLASS_ID; }
	Class_ID		ClassID(){ return BILLBOARD_HELPER_OBJECT_CLASSID; }
	const TCHAR* 	Category(){ return GetString( IDS_IRRLICHT_STANDARD_CATEGORY ); }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return GetString( IDS_BILLBOARD_HELPER_OBJECT_NAME ); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

ClassDesc* GetICBillboardHelperDesc();

#endif // __IC_BILLBOARD_HELPER__H
