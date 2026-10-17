#ifndef _IC_MODULAR_SKIN_MASTER_H_
#define _IC_MODULAR_SKIN_MASTER_H_

#include "resource.h"
#include "ICModularSkinCategory.h"
#include "UIHelpers.h"

// This is the Class_ID for the world space modifier version of Bend
#define MODULAR_SKIN_MASTER_CLASSID_PARTA 0x71b12eb0
#define MODULAR_SKIN_MASTER_OBJECT_CLASSID Class_ID( MODULAR_SKIN_MASTER_CLASSID_PARTA, 0 )

class ModularSkinCategoryInfo
	{
	public:
		INode *m_pCategoryNode;
		INode *m_pSelectedSkinnedMeshNode;
		int m_sortedId;

	public:
		ModularSkinCategoryInfo( INode *pCategoryNode = NULL, INode *pSelectedSkinnedMeshNode = NULL, int sortedId = -1 )
			: m_pCategoryNode( pCategoryNode )
			, m_pSelectedSkinnedMeshNode( pSelectedSkinnedMeshNode )
			, m_sortedId( sortedId )
		{}
	};

typedef fm::map<int, ModularSkinCategoryInfo> ModularSkinCategoryInfoMap;
typedef fm::pvector<INode> NodeList;

class ICModularSkinMasterObject : public HelperObject
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
		, PI_HIDE_UNSELECTED_SKINS

		, PI_SKINNED_MESH_LIST
		, PI_SKINNED_MESH_CATEGORIES_LIST
		, PI_SELECTED_SKINNED_MESH_LIST // one selected skinned mesh per category

		//, PI_PRESET_LIST
		//, PI_PRESET_NAME_LIST
		
		, PI_COUNT
	};

	enum Rollup
	{
		R_DISPLAY_OPTIONS
		, R_UNCATEGORIZED_SKINNED_MESHES
		, R_SKINNED_MESH_CATEGORIES
		//, R_PRESETS
		, R_COUNT
	};

public:
	class HideUnselectedSkinPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
	};

	class SkinnedMeshCategoriesListPBValidator : public PBValidator
	{ 
	public:
		inline BOOL Validate(PB2Value& v){ return TRUE; }
		BOOL Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex);
	};

	class SkinnedMeshListPBAccessor : public PBAccessor
	{ 
	public:
		void TabChanged(tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count);
	};

	class SkinnedMeshListPBValidator : public PBValidator
	{ 
	public:

		inline BOOL Validate(PB2Value& v){ return TRUE; }
		BOOL Validate(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex);
	};

	class SkinnedMeshCategoriesListPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		void TabChanged(tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count);
	};

	class SelectedSkinnedMeshListPBAccessor : public PBAccessor
	{ 
	public:
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
		void TabChanged(tab_changes changeCode, Tab<PB2Value>* tab, ReferenceMaker* owner, ParamID id, int tabIndex, int count);
	};

	class ICModularSkinMasterObjectPostLoadCallback : public PostLoadCallback {
		public:
			ICModularSkinMasterObject *m_pModularSkinMasterObject;
			ICModularSkinMasterObjectPostLoadCallback(ICModularSkinMasterObject *pModularSkinMasterObject){ m_pModularSkinMasterObject = pModularSkinMasterObject; }
			void proc( ILoad *iload );
	};

	
	class SkinnedMeshSelectionRestoreObj : public RestoreObj
	{
	private:
		ICModularSkinMasterObject *m_pModularSkinMaster;

	public:        
		SkinnedMeshSelectionRestoreObj( ICModularSkinMasterObject *pModularSkinMaster ) 
			: m_pModularSkinMaster( pModularSkinMaster )
		{}

		void Restore(int isUndo);
		void Redo();
		
		inline int Size()
		{
			return sizeof( SkinnedMeshSelectionRestoreObj );
		}
	};

	class CategoriesDlgProc : public ParamMap2UserDlgProc
	{
	private:
		//ICModularSkinMasterObject *m_pOwner;

	public:
		//CategoriesDlg

		INT_PTR DlgProc( TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
		void DeleteThis(){ /*delete this;*/ }
		void SetThing( ReferenceTarget *m ) { }
		void Update( TimeValue t ) { }
		void SetParamBlock( IParamBlock2 *pb ) { }
		// alternative update add to R4, for extra context & influence over pmap validity interval - jbw 9.9.0
		//virtual void Update(TimeValue t, Interval& valid, IParamMap2* pmap) { this->Update(t); }
	};

	// currently unused
	// Using void pointers so comparator is compatible with pvector
	// Can only be used for INode pointers
	//class NodeNameComparator : public fm::comparator<const void *>
	//{
	//	/** Compares two elements in ascending order, using operator<.
	//		@param a The first element.
	//		@param b The second element.
	//		@return True if a is smaller than b, false otherwise.*/
	//	virtual bool compare(const void *& a, const void *& b);
	//};

public:
	IParamBlock2 *pblock2;	

	static HideUnselectedSkinPBAccessor s_hideUnselectedSkinAccessor;

	static SkinnedMeshListPBValidator s_skinnedMeshListValidator;
	static SkinnedMeshListPBAccessor s_skinnedMeshListAccessor;

	static SkinnedMeshCategoriesListPBValidator s_skinnedMeshCategoriesListValidator;
	static SkinnedMeshCategoriesListPBAccessor s_skinnedMeshListCategoriesAccessor;

	static SelectedSkinnedMeshListPBAccessor s_selectedSkinnedMeshListAccessor;

	static CategoriesDlgProc s_categoriesDlgProc;

	static const Point3 k_gizmoColor;
	static const Point3 k_categoryLinksColor;
	static const Point3 k_skinnedMeshLinksColor;

private:
	Mesh m_gizmoMesh;
	Mesh m_categoryLinksMesh;
	Mesh m_skinnedMeshLinksMesh;
	int m_scrollBarCurrentPos;

	IdcList m_selectedSkinnedMeshesComboBoxIdcList;
	ModularSkinCategoryInfoMap m_modularSkinCategoryInfoMap;

	// hack
	bool m_bIsClosingFile;

private:
	static HWND s_selectedSkinnedMeshesRollupHWnd;
	static HWND s_presetsRollupHWnd;

public:		
	ICModularSkinMasterObject();
	~ICModularSkinMasterObject();

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


	void RefreshSelectedSkinnedMeshRollup();
	void SynchronizeSelectedSkinnedMeshesWithCategories();

	void GetSortedCategoryNodes( NodeList &categoryNodes );
	int FindUnsortedCategoryId( int sortedId );

	void UpdateSkinnedMeshHiddenState( bool bForceUpdate = false );

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

	Box3 ComputeCategoryLinksBox( TimeValue t, INode *pNode );
	Box3 ComputeSkinnedMeshLinksBox( TimeValue t, INode *pNode );

	void GetLinksTM( const Matrix3 &objectTM, Matrix3 &tm );

	bool IsDisplayLinksEnabled();
	bool HideUnselectedSkinsEnabled();

	int GetSkinnedMeshCategoryNodeId( INode *pNode );
	int GetSkinnedMeshCategoryNodeId( ICModularSkinCategoryObject *pObject );

	// ---------------------------------------------------------------
	// Skinned Mesh List Accessors
	// ---------------------------------------------------------------
	inline void DeleteSkinnedMeshNodes( int startIndex, int count )
	{
		pblock2->Delete( PI_SKINNED_MESH_LIST, startIndex, count );
	}

	// ---------------------------------------------------------------
	// Skinned Mesh Category List Inlined Accessors
	// ---------------------------------------------------------------
	inline int GetSkinnedMeshCategoryNodeCount()
	{
		return pblock2->Count( PI_SKINNED_MESH_CATEGORIES_LIST );
	}

	inline INode *GetSkinnedMeshCategoryNode( int id )
	{
		return pblock2->GetINode( PI_SKINNED_MESH_CATEGORIES_LIST, 0, id );
	}

	inline void DeleteSkinnedMeshCategories( int startIndex, int count )
	{
		pblock2->Delete( PI_SKINNED_MESH_CATEGORIES_LIST, startIndex, count );
	}

	inline void DeleteSelectedSkinnedMeshNodes( int startIndex, int count )
	{
		pblock2->Delete( PI_SELECTED_SKINNED_MESH_LIST, startIndex, count );
	}

	// ---------------------------------------------------------------
	// Selected Skinned Mesh List Inlined Accessors
	// ---------------------------------------------------------------
	inline int GetSelectedSkinnedMeshNodeCount()
	{
		return pblock2->Count( PI_SELECTED_SKINNED_MESH_LIST );
	}

	inline INode *GetSelectedSkinnedMeshNode( int id )
	{
		return pblock2->GetINode( PI_SELECTED_SKINNED_MESH_LIST, 0, id );
	}

	inline void InsertSelectedSkinnedMeshNodes( int startIndex, int count, INode **pNodes )
	{
		pblock2->Insert( PI_SELECTED_SKINNED_MESH_LIST, startIndex, count, pNodes );
	}

	inline void SetSelectedSkinnedMeshNode( int id, INode *pNode )
	{
		pblock2->SetValue( PI_SELECTED_SKINNED_MESH_LIST, 0, pNode, id );
	}

	// ---------------------------------------------------------------
	// Selected Skinned Mesh Rollup Widget Inlined Methods
	// ---------------------------------------------------------------
	inline int GetComboBoxId( int idc )
	{
		return m_selectedSkinnedMeshesComboBoxIdcList.find_index( idc );
	}

	inline int GetScrollBarCurrentPos()
	{
		return m_scrollBarCurrentPos;
	}

	inline void SetScrollBarCurrentPos( int pos )
	{
		m_scrollBarCurrentPos = pos;
	}

	inline size_t GetComboBoxCount()
	{
		return m_selectedSkinnedMeshesComboBoxIdcList.size();
	}

	//inline const IdcList &GetSelectedSkinnedMeshesComboBoxIdcList() const
	//{
	//	return m_selectedSkinnedMeshesComboBoxIdcList;
	//}

	inline ModularSkinCategoryInfoMap &GetModularSkinCategoryInfoMap()
	{
		return m_modularSkinCategoryInfoMap;
	}

	inline Class_ID ClassID()
	{ 
		return MODULAR_SKIN_MASTER_OBJECT_CLASSID;
	}

	inline TCHAR *GetObjectName()
	{
		return GetString( IDS_MODULAR_SKIN_MASTER_OBJECT_NAME );
	}

	inline void InitNodeName( TSTR& s )
	{
		s = GetString( IDS_MODULAR_SKIN_MASTER_OBJECT_NAME );
	}

	inline void GetClassName( TSTR& s )
	{
		s = GetString( IDS_MODULAR_SKIN_MASTER_CLASS_NAME );
	}

	inline void SetIsClosingFile( bool bIsClosing )
	{
		m_bIsClosingFile = bIsClosing;
	}

	inline bool GetIsClosingFile()
	{
		return m_bIsClosingFile;
	}

public:

	static void NotificationCallback( void *pParam, NotifyInfo *pInfo );

	//static HWND GetSelectedSkinnedMeshesRollupHWnd()
	//{
	//	return s_selectedSkinnedMeshesRollupHWnd;
	//}
};

class ICModularSkinMasterClassDesc:public ClassDesc2 
{
public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) { return new ICModularSkinMasterObject;}
	const TCHAR *	ClassName() {return GetString( IDS_MODULAR_SKIN_MASTER_CLASS_NAME );}
	SClass_ID		SuperClassID() {return HELPER_CLASS_ID; }
	Class_ID		ClassID() {return MODULAR_SKIN_MASTER_OBJECT_CLASSID;}
	const TCHAR* 	Category() {return GetString( IDS_MODULAR_SKIN_MAX_CATEGORY );}

	// Hardwired name, used by MAX Script as unique identifier
	const TCHAR*	InternalName()				{ return GetString( IDS_MODULAR_SKIN_MASTER_OBJECT_NAME ); }
	HINSTANCE		HInstance()					{ return hInstance; }
};

ClassDesc* GetICModularSkinMasterObjDesc();

class ICModularSkinMasterObjCreateCallback : public CreateMouseCallBack {
	public:
		ICModularSkinMasterObject *ob;	
		Point3 p0, p1;
		IPoint2 sp0, sp1;
		int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
};

#endif // _IC_MODULAR_SKIN_MASTER_H_