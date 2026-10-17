#ifndef __IC_MODULAR_SKIN_CATEGORY__H
#define __IC_MODULAR_SKIN_CATEGORY__H

#include "resource.h"
#include "ICReferenceMaker.h"

#define MODULAR_SKIN_CATEGORY_CLASSID_PARTA 0x64e40ff6
#define MODULAR_SKIN_CATEGORY_OBJECT_CLASSID Class_ID( MODULAR_SKIN_CATEGORY_CLASSID_PARTA, 0 )

class ICModularSkinCategoryObject : public HelperObject
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
		, PI_SKINNED_MESH_LIST
		//, PI_SKINNED_MESH_REFERENCE_LIST
		, PI_COUNT
	};

	enum Rollup
	{
		R_DISPLAY_OPTIONS
		, R_SKINNED_MESHES
		, R_COUNT
	};

	class SkinnedMeshReferenceListPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		void TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count );
	};

	class SkinnedMeshListPBValidator : public PBValidator
	{ 
	public:

		inline BOOL Validate(PB2Value& v){ return TRUE; }
		BOOL Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex);
	};

	class SkinnedMeshListPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		void TabChanged( tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count );
	};

	//class SkinnedMeshReferenceMaker : public ReferenceMaker
	//{
	//private:
	//	ICModularSkinCategoryObject *m_pModularSkinCategoryObject;

	//public:
	//	SkinnedMeshReferenceMaker( ICModularSkinCategoryObject *pModularSkinCategoryObject )
	//		: m_pModularSkinCategoryObject( pModularSkinCategoryObject )
	//	{}

	//	int NumRefs();
	//	RefTargetHandle GetReference( int i );
	//	void SetReference( int i, RefTargetHandle rtarg );

	//	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message );
	//};

	class ICModularSkinCategoryObjectPostLoadCallback : public PostLoadCallback 
	{
		public:
			ICModularSkinCategoryObject *m_pModularSkinCategoryObject;
			ICModularSkinCategoryObjectPostLoadCallback(ICModularSkinCategoryObject *pModularSkinCategoryObject){ m_pModularSkinCategoryObject = pModularSkinCategoryObject; }
			void proc( ILoad *iload );
	};

	//class SkinnedMeshDeleteRestoreObj : public RestoreObj
	//{
	//public:        
	//	ICModularSkinCategoryObject *pModularSkinCategory;
	//	Tab<int> utimes;
	//	Tab<int> rtimes;
	//	ICPCLifeListRestore( ICModularSkinCategoryObject *p ) 
	//	{
	//		pModularSkinCategory = p;
	//		utimes = p->llist;
	//	}

	//	void Restore(int isUndo)
	//	{
	//		if ( isUndo )
	//		{ 
	//			rtimes = pModularSkinCategory->llist; 
	//		}
	//		pModularSkinCategory->llist=utimes;
	//		if ( pModularSkinCategory->ip )
	//		{
	//			pModularSkinCategory->SetUpLifeList();
	//		}
	//	}

	//	void Redo() 
	//	{
	//		pModularSkinCategory->llist = rtimes;
	//		if ( pModularSkinCategory->ip )
	//		{
	//			pModularSkinCategory->SetUpLifeList();
	//		}
	//	}
	//};

private:
	Mesh m_gizmoMesh;
	Mesh m_linksMesh;
	//long dumFlags;	
	//Point3 color;
	//Interval valid;

public:
	IParamBlock2 *pblock2;
	//SkinnedMeshReferenceMaker m_skinnedMeshReferenceMaker;

	static SkinnedMeshListPBValidator s_skinnedMeshListValidator;
	static SkinnedMeshListPBAccessor s_skinnedMeshListAccessor;

	static SkinnedMeshReferenceListPBAccessor s_skinnedMeshReferenceListAccessor;

	static const Point3 k_gizmoColor;
	static const Point3 k_linksColor;

public:		
	ICModularSkinCategoryObject();
	~ICModularSkinCategoryObject();

	// From Animatable		
	void DeleteThis() {delete this;}		
	void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);		
	CLONE_REF_TARGET_HANDLE;
			
	// from object		
	CreateMouseCallBack* GetCreateMouseCallBack();		
	
	void BuildMesh( TimeValue t, INode *pNode );
	
	// Direct paramblock access
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int i) { return pblock2; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock2->ID() == id) ? pblock2 : NULL; }

	// inherited virtual methods for Reference-management
	// Need ref on pblock2 for MakeAutoParamBlocks to work
	int NumRefs(){ return 1; }
	RefTargetHandle GetReference( int i ){ return pblock2; }
	void SetReference( int i, RefTargetHandle rtarg ){ pblock2 = ( IParamBlock2 * ) rtarg; }
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message ){ return REF_SUCCEED; }

	//IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);
	
	//  inherited virtual methods:
	// From BaseObject
	int HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt );
	void Snap( TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt );
	int Display( TimeValue t, INode* inode, ViewExp *vpt, int flags );
	Interval ObjectValidity(TimeValue t);

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

	bool IsDisplayLinksEnabled();

	int GetSkinnedMeshNodeCount();
	INode *GetSkinnedMeshNode( int id );
	int GetSkinnedMeshNodeId( INode *pNode );
	void DeleteSkinnedMeshNode( INode *pNode );
	void DeleteSkinnedMeshNode( int id );

	void SynchronizeSkinnedMeshReferences();

	inline Class_ID ClassID()
	{ 
		return MODULAR_SKIN_CATEGORY_OBJECT_CLASSID; 
	}

	inline TCHAR *GetObjectName()
	{
		return GetString( IDS_MODULAR_SKIN_CATEGORY_OBJECT_NAME );
	}

	inline void InitNodeName( TSTR& s )
	{
		s = GetString( IDS_MODULAR_SKIN_CATEGORY_OBJECT_NAME );
	}

	inline void GetClassName( TSTR& s )
	{
		s = GetString( IDS_MODULAR_SKIN_CATEGORY_CLASS_NAME );
	}

	//inline void ICModularSkinCategoryObject::InsertSkinnedMeshReferences( int start, int count, ReferenceTarget **pReferenceTargets )
	//{
	//	pblock2->Insert( PI_SKINNED_MESH_REFERENCE_LIST, start, count, pReferenceTargets );
	//}

	//inline void ICModularSkinCategoryObject::DeleteSkinnedMeshReferences( int start, int count )
	//{
	//	pblock2->Delete( PI_SKINNED_MESH_REFERENCE_LIST, start, count );
	//}

	//inline int ICModularSkinCategoryObject::GetSkinnedMeshReferenceCount()
	//{
	//	return pblock2->Count( PI_SKINNED_MESH_REFERENCE_LIST );
	//}

	//inline ReferenceTarget *ICModularSkinCategoryObject::GetSkinnedMeshReference( int id )
	//{
	//	return pblock2->GetReferenceTarget( PI_SKINNED_MESH_REFERENCE_LIST, 0, id );
	//}

	//inline void ICModularSkinCategoryObject::SetSkinnedMeshReference( int id, ReferenceTarget *pReferenceTarget )
	//{
	//	pblock2->SetValue( PI_SKINNED_MESH_REFERENCE_LIST, 0, pReferenceTarget, id );
	//}

	// ----------------------------------------------------------------------
	// Methods from dummy.h
	// ----------------------------------------------------------------------
	//void SetColor(Point3 color);
	//void EnableDisplay();
	//void DisableDisplay();

	// ----------------------------------------------------------------------
	// Useless methods
	// ----------------------------------------------------------------------
	//void SetValidity(Interval v);

	//int IsKeyable(){ return 1;}
	//LRESULT CALLBACK TrackViewWinProc( HWND hwnd,  UINT message, WPARAM wParam,   LPARAM lParam ){return(0);}

	//inline int DoOwnSelectHilite()
	//{
	//	return 1; 
	//}

	//// From ReferenceMaker
	//void RescaleWorldUnits(float f);
};

class ICModularSkinCategoryClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic(){ return 1; }
	void *			Create( BOOL loading = FALSE ) { return new ICModularSkinCategoryObject; }
	const TCHAR *	ClassName(){ return GetString( IDS_MODULAR_SKIN_CATEGORY_CLASS_NAME ); }
	SClass_ID		SuperClassID(){ return HELPER_CLASS_ID; }
	Class_ID		ClassID(){ return MODULAR_SKIN_CATEGORY_OBJECT_CLASSID; }
	const TCHAR* 	Category(){ return GetString( IDS_MODULAR_SKIN_MAX_CATEGORY ); }

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return GetString( IDS_MODULAR_SKIN_CATEGORY_OBJECT_NAME ); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

ClassDesc* GetICModularSkinCategoryObjDesc();

class ICModularSkinCategoryObjCreateCallback : public CreateMouseCallBack {
	public:
		ICModularSkinCategoryObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
};

#endif // __IC_MODULAR_SKIN_CATEGORY__H
