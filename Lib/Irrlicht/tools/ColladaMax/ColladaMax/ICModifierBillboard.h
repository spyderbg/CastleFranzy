#ifndef _IC_MODIFIER_BILLBOARD_H_
#define _IC_MODIFIER_BILLBOARD_H_

#include "MAXHelpers.h"
#include "resource.h"

#define IC_BILLBOARD_CLASSID_PARTA 0x44fe0608
#define IC_BILLBOARD_OBJECT_CLASSID Class_ID( IC_BILLBOARD_CLASSID_PARTA, 0 )
#define IC_BILLBOARD_MODIFIER_CLASSID Class_ID( IC_BILLBOARD_CLASSID_PARTA, 1 )

class ICBillboardWSM;

typedef fm::pvector<INode> NodeList;
typedef fm::pvector<ICBillboardWSM> BillboardModifierList;

class ICBillboardWSMObject : public WSMObject
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
		PI_DISPLAY_LINKS
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
		, R_BILLBOARD_CONTROLLER
		, R_COUNT
	};

public:

	//--- ClassDescriptor and class vars ---------------------------------
	class MyClassDesc : public ClassDesc2 
	{
	public:
		int IsPublic(){ return 1; }
		void *Create( BOOL loading = FALSE ){ return new ICBillboardWSMObject; }
		const TCHAR *ClassName(){ return GetString( IDS_BILLBOARD_HELPER_CLASS_NAME ); }
		SClass_ID SuperClassID(){ return WSM_OBJECT_CLASS_ID; }
		Class_ID ClassID(){ return IC_BILLBOARD_OBJECT_CLASSID; }
		const TCHAR *Category(){ return GetString( IDS_IRRLICHT_STANDARD_CATEGORY ); }

		// Hardwired name, used by MAX Script as unique identifier
		const TCHAR *InternalName(){ return GetString( IDS_BILLBOARD_HELPER_OBJECT_NAME ); }
		HINSTANCE HInstance(){ return hInstance; }
	};

	class CreateMouseCB : public CreateMouseCallBack {
		public:
			ICBillboardWSMObject *ob;	
			Point3 p0, p1;
			IPoint2 sp0, sp1;
			int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	};

	class BillboardControllerPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		//void TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count );
	};

	class ICBillboardWSMObjectPostLoadCallback : public PostLoadCallback 
	{
		public:
			ICBillboardWSMObject *m_pOwner;
			ICBillboardWSMObjectPostLoadCallback( ICBillboardWSMObject *pOwner )
				: m_pOwner( pOwner )
				{}

			void proc( ILoad *iload );
	};

	class ICBillboardWSMObjectCreateCallback : public CreateMouseCallBack {
		public:
			ICBillboardWSMObject *ob;	
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
	static ICBillboardWSM *GetBillboardModifier( INode *pNode );

public:     
	void BuildMesh( TimeValue t, INode *pNode, ViewExp *vpt );

	ICBillboardWSMObject();
	~ICBillboardWSMObject();
	CLONE_REF_TARGET_HANDLE;

	// From BaseObject
	void DeleteThis() { delete this; }
	void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
	
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);		
	void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );
	void GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box );

	// From Object
	ObjectState Eval( TimeValue time );
	Interval ObjectValidity(TimeValue t);		

	int IsRenderable() { return 0; }
	//int UsesWireColor() { return FALSE; }   // TRUE if the object color is used for display
	BOOL NormalAlignVector(TimeValue t,Point3 &pt, Point3 &norm) {pt=Point3(0,0,0);norm=Point3(0,0,-1);return TRUE;}

	// From ref
	int NumRefs(){ return 1; }
	RefTargetHandle GetReference(int i){ return pblock2; }
	void SetReference( int i, RefTargetHandle rtarg ){ pblock2 = ( IParamBlock2 * )rtarg; }
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message ){ return REF_DONTCARE; }

	// from object    
	CreateMouseCallBack* GetCreateMouseCallBack();

	// From Animatable      

	// From WSMObject
	Modifier *CreateWSMMod( INode *node );

	// Direct paramblock access
	int   NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int i) { return pblock2; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

	// from ref
	IOResult Load(ILoad *iload);

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
	void GetBillboardNodeIds( INode *pNode, UInt32List &indices );
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

	void GetBillboardModifiers( BillboardModifierList &billboardModifiers );


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
		return IC_BILLBOARD_OBJECT_CLASSID; 
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
	static bool ValidateNotifyModifierObject( MAXHelpers::NotifyModifierObject *pNotifyModifierObject );

	static ClassDesc2 *GetMyClassDesc();
};

class ICBillboardWSM: public Modifier
{	
public:
	class MyClassDesc:public ClassDesc2 
	{
	public:
		int IsPublic(){ return 0; }
		void *Create(BOOL loading = FALSE){ return new ICBillboardWSM; }
		const TCHAR *ClassName(){ return GetString( IDS_BILLBOARD_HELPER_CLASS_NAME ); }
		SClass_ID SuperClassID(){ return WSM_CLASS_ID; }
		Class_ID ClassID(){ return IC_BILLBOARD_MODIFIER_CLASSID; }
		const TCHAR *Category(){return GetString( IDS_IRRLICHT_STANDARD_CATEGORY ); }

		const TCHAR *InternalName()	{ return _T("IrrBillboardWSM"); }
		HINSTANCE HInstance() { return hInstance; }
	};

	class ViewportDisplayCallbackObject : public ViewportDisplayCallback
	{
	private:
		ICBillboardWSM *m_pOwner;

	public:
		ViewportDisplayCallbackObject( ICBillboardWSM *pOwner = NULL )
			: m_pOwner( pOwner )
		{}

		void Display( TimeValue t, ViewExp *vpt, int flags );
		void GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect );
		BOOL Foreground(); // return TRUE if the object changes a lot or FALSE if it doesn't change much
	};

private:
	ViewportDisplayCallbackObject m_preViewportDisplayCallback;
	Matrix3 m_rotationMatrix;
	ICBillboardWSMObject *m_pBillboardWSMObject;

	// hack
	bool m_bIsClosingFile;

public:
	ICBillboardWSM();
	~ICBillboardWSM();

	Class_ID ClassID(){ return IC_BILLBOARD_MODIFIER_CLASSID; }
	SClass_ID SuperClassID(){ return WSM_CLASS_ID; }

	ChannelMask ChannelsUsed()  { return GEOM_CHANNEL | TM_CHANNEL; }
	ChannelMask ChannelsChanged() { return GEOM_CHANNEL | TM_CHANNEL; }
	void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
	Class_ID InputType() {return defObjectClassID;}
	Interval LocalValidity(TimeValue t);
	BOOL ChangeTopology() {return FALSE;}
	CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

	inline int NumRefs(){ return 0; }
	RefTargetHandle GetReference(int i){ return NULL; }
	void SetReference(int i, RefTargetHandle rtarg){}
	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message){ return REF_DONTCARE; }

	CLONE_REF_TARGET_HANDLE;

	// Clients of simpmod probably want to override these. If they do
	// the should call these from within thier methods.
	void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev){}
	void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next){}

	// Clients of simpmod need to implement this method
	//Deformer& GetDeformer( TimeValue t, ModContext &mc, Matrix3& mat, Matrix3& invmat );
	void InvalidateUI() {}
	Interval GetValidity(TimeValue t){ return FOREVER;}

	// --- Interhited virtual methods of ReferenceMaker
	//IOResult Load(ILoad *iload);

	void GetBillboardType( int &type );
	void GetBillboardOrientation( int &orientation );
	void GetFrontVector( TimeValue t, Point3 &v );
	void GetUpVector( TimeValue t, Point3 &v );
	bool IsDisplayBillboardedTargetsEnabled();

	inline ICBillboardWSMObject *GetBillboardWSMObjectPtr()
	{
		return m_pBillboardWSMObject;
	}

	inline void SetBillboardWSMObjectPtr( ICBillboardWSMObject *pBillboardWSMObject )
	{
		m_pBillboardWSMObject = pBillboardWSMObject;
	}

	const Matrix3 &GetRotationMatrix() const
	{
		return m_rotationMatrix;
	}

	inline TCHAR *GetObjectName()
	{
		return GetString( IDS_BILLBOARD_HELPER_OBJECT_NAME );
	}

	inline void GetClassName( TSTR& s )
	{
		s = GetString( IDS_BILLBOARD_HELPER_CLASS_NAME );
	}

	inline void SetIsClosingFile( bool bIsClosing )
	{
		m_bIsClosingFile = bIsClosing;
	}

	inline bool GetIsClosingFile()
	{
		return m_bIsClosingFile;
	}

	static void NotificationCallback( void *param, NotifyInfo *info );
	static bool ValidateNotifyModifierObject( MAXHelpers::NotifyModifierObject *pNotifyModifierObject );

	static ClassDesc2 *GetMyClassDesc();
};

class NodeModifierMapping
{
private:
	INode *m_pNode;
	Modifier *m_pModifier;

public:
	NodeModifierMapping( INode *pNode = NULL, Modifier *pModifier = NULL )
		: m_pNode( pNode )
		, m_pModifier( pModifier )
	{}

	inline INode *GetNode() const
	{
		return m_pNode;
	}

	inline void SetNode( INode *pNode )
	{
		m_pNode = pNode;
	}

	inline Modifier *GetModifier() const
	{
		return m_pModifier;
	}

	inline void SetModifier( Modifier *pModifier )
	{
		m_pModifier = pModifier;
	}
};

typedef fm::vector<NodeModifierMapping> NodeModifierMappingList;


#endif // _IC_MODIFIER_BILLBOARD_H_