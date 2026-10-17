#ifndef _MAX_HELPERS_H_
#define _MAX_HELPERS_H_

namespace MAXHelpers
{
	// for Max's CreateMouseCallBack proc
	enum CREATE_PROC_CLICK
	{
		CPC_FIRST
		, CPC_SECOND
	};

	struct NotifyModifierObject
	{ 
		INode *pNode; 
		Modifier *pModifier; 
		ModContext *pModifierContext;
	};

	// @params:
	//	- defaultData : if not NULL, app data chunk will be automatically created - and returned - with the defaultData.  defaultData size must be 'size'.
	AppDataChunk *GetAppDataChunk( INode *pNode, Class_ID cid, SClass_ID sid, DWORD sbid, size_t size, void *defaultData = NULL );
	//template <class T> T GetAppDataChunkData( INode *pNode, Class_ID cid, SClass_ID sid, DWORD sbid, size_t size, void *defaultData );
	void SetAppDataChunkData( INode *pNode, Class_ID cid, SClass_ID sid, DWORD sbid, size_t size, void *defaultData, void *data );

	Point3 MinComponents( const Point3 &pA, const Point3 &pB );
	Point3 MaxComponents( const Point3 &pA, const Point3 &pB );

	bool IsNodeInScene( INode *pNode );

	void GetAllNodes( INode* root, INodeTab& nodeList );
	void GetNodesByObjectClassID( INode* root, INodeTab& nodeList, const Class_ID &cid );
	void GetNodesByColladaMaxType( INode* root, INodeTab& nodeList, ColladaMaxTypes::MaxType maxType );
	INode *GetNodeByObjectRef( INode *root, Object *pObject );
	INode *GetNodeBySubAnimatable( INode *root, Animatable *pAnimatable );
	
	bool GetAnimatableParent( Animatable *pRootAnimatable, Animatable *pRequestedAnimatable, Animatable *&pParentAnimatable, int &subAnimatableId );

	void ComposeMatrix( const AffineParts &ap, Matrix3 &mat );

	const TCHAR *GetTmpDir();

	void DBG_PrintActionTables();
	ActionItem *FindActionItemByDescriptionText( ActionTable *pActionTable, const fm::string &descriptionText );
	
}

#endif // _MAX_HELPERS_H_
