#include "stdafx.h"
#include "MAXHelpers.h"
#include "ICAppDataChunkMemManager.h"
#include "IPathConfigMgr.h"

extern FUStringBuilder g_warningLog;

namespace MAXHelpers
{
	AppDataChunk *GetAppDataChunk( INode *pNode, Class_ID cid, SClass_ID sid, DWORD sbid, size_t size, void *defaultData )
	{
		FUAssert( pNode, return NULL );

		AppDataChunk *pDataChunk = pNode->GetAppDataChunk( cid, sid, sbid );
		
		if ( defaultData
			&& ( pDataChunk == NULL
			|| pDataChunk->length != size
			|| !pDataChunk->data ))
		{
			// MAX will take care of deallocation when it no longer needs the data
			char *newData = ( char * )MAX_MALLOC( size );
			memcpy( newData, defaultData, size );
			pNode->AddAppDataChunk( cid, sid, sbid, size, newData );
			pDataChunk = pNode->GetAppDataChunk( cid, sid, sbid );
			SetSaveRequiredFlag();
		}

		return pDataChunk;
	}

	//template <class T> 
	//T GetAppDataChunkData( INode *pNode, Class_ID cid, SClass_ID sid, DWORD sbid, size_t size, T &defaultData )
	//{
	//	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
	//		, cid
	//		, sid
	//		, sbid
	//		, ObjectProperty::CHUNK_SIZE_DRAW_LAYER
	//		, &defaultData
	//	);

	//	int drawLayer = *( int * )pDataChunk->data;
	//}

	void SetAppDataChunkData( INode *pNode, Class_ID cid, SClass_ID sid, DWORD sbid, size_t size, void *defaultData, void *data )
	{
		AppDataChunk *pDataChunk = GetAppDataChunk( pNode, cid, sid, sbid, size, defaultData );
		FUAssert( pDataChunk, return );
		memcpy( pDataChunk->data, data, size );
		SetSaveRequiredFlag();
	}

	Point3 MinComponents( const Point3 &pA, const Point3 &pB )
	{
		Point3 minComponents = pA;

		const int k_numElements = 3;
		for ( int elementId = 0; elementId < k_numElements; ++elementId )
		{
			minComponents[elementId] = min( pA[elementId], pB[elementId] );
		}

		return minComponents;
	}

	Point3 MaxComponents( const Point3 &pA, const Point3 &pB )
	{
		Point3 maxComponents = pA;

		const int k_numElements = 3;
		for ( int elementId = 0; elementId < k_numElements; ++elementId )
		{
			maxComponents[elementId] = max( pA[elementId], pB[elementId] );
		}

		return maxComponents;
	}

	bool GetAnimatableParent( Animatable *pRootAnimatable, Animatable *pRequestedAnimatable, Animatable *&pParentAnimatable, int &subAnimatableId )
	{
		if ( !pRootAnimatable || !pRequestedAnimatable )
		{
			return false;
		}

		int numSubAnims = pRootAnimatable->NumSubs();
		for ( int id = 0; id < numSubAnims; ++id )
		{
			Animatable *pSubAnimatable = pRootAnimatable->SubAnim( id );
			if ( pSubAnimatable == pRequestedAnimatable )
			{
				pParentAnimatable = pRootAnimatable;
				subAnimatableId = id;
				return true;
			}
		}

		for ( int id = 0; id < numSubAnims; ++id )
		{
			Animatable *pSubAnimatable = pRootAnimatable->SubAnim( id );
			bool bSuccess = GetAnimatableParent( pSubAnimatable, pRequestedAnimatable, pParentAnimatable, subAnimatableId );
			if ( bSuccess )
			{
				return true;
			}
		}

		return false;
	}

	void GetAllNodes( INode* root, INodeTab& nodeList )
	{
		nodeList.Append(1, &root);
		for (int i = 0; i < root->NumChildren(); ++i)
		{
			GetAllNodes( root->GetChildNode( i ), nodeList );
		}
	}

	void GetNodesByObjectClassID( INode* root, INodeTab& nodeList, const Class_ID &cid )
	{
		Object *pObject = root->GetObjectRef();
		if ( pObject && pObject->ClassID() == cid )
		{
			nodeList.Append(1, &root);
		}

		for (int i = 0; i < root->NumChildren(); ++i)
		{
			GetNodesByObjectClassID( root->GetChildNode( i ), nodeList, cid );
		}
	}

	void GetNodesByColladaMaxType( INode* root, INodeTab& nodeList, ColladaMaxTypes::MaxType maxType )
	{
		Object *pObject = root->GetObjectRef();
		if ( ColladaMaxTypes::Get( root, pObject ) == maxType )
		{
			nodeList.Append(1, &root);
		}

		for (int i = 0; i < root->NumChildren(); ++i)
		{
			GetNodesByColladaMaxType( root->GetChildNode( i ), nodeList, maxType );
		}
	}

	bool IsNodeInScene( INode *pNode )
	{
		INodeTab nodeList;
		GetAllNodes( GetCOREInterface()->GetRootNode(), nodeList );
		return nodeList.IndexOf( pNode ) != -1;
	}

	// TODO : xRef support
	INode *GetNodeByObjectRef( INode *root, Object *pObject )
	{
		if ( !root || !pObject )
		{
			return NULL;
		}

		INodeTab nodeList;
		GetNodesByObjectClassID( GetCOREInterface()->GetRootNode(), nodeList, pObject->ClassID() );

		INode *pNode = NULL;
		for ( int nodeId = 0; nodeId < nodeList.Count(); ++nodeId )
		{
			INode *pCurrentNode = nodeList[nodeId];
			if ( pCurrentNode->GetObjectRef() == pObject )
			{
				pNode = pCurrentNode;
				break;
			}
		}

		return pNode;
	}

	INode *GetNodeBySubAnimatable( INode *root, Animatable *pAnimatable )
	{
		if ( !root || !pAnimatable )
		{
			return NULL;
		}

		INodeTab nodeList;
		GetAllNodes( GetCOREInterface()->GetRootNode(), nodeList );

		INode *pNode = NULL;
		for ( int nodeId = 0; nodeId < nodeList.Count(); ++nodeId )
		{
			INode *pCurrentNode = nodeList[0];
			Animatable *pParentAnimatable = NULL;
			int subAnimatableId;

			bool bSuccess = GetAnimatableParent( pCurrentNode, pAnimatable, pParentAnimatable, subAnimatableId );
			if ( bSuccess )
			{
				return pCurrentNode;
			}
		}

		return NULL;
	}

	// CAL-05/24/02: TODO: this should really go to core\decomp.cpp, and it should be optimized.
	//		For now, it's defined locally in individual files in the ctrl project.
	void ComposeMatrix( const AffineParts &ap, Matrix3 &mat )
	{
		Matrix3 tm;
		
		mat.IdentityMatrix();
		mat.SetTrans( ap.t );

		if ( ap.f != 1.0f ) {				// has f component
			tm.SetScale( Point3( ap.f, ap.f, ap.f ) );
			mat = tm * mat;
		}

		if ( !ap.q.IsIdentity() ) {			// has q rotation component
			ap.q.MakeMatrix( tm );
			mat = tm * mat;
		}
		
		if ( ap.k.x != 1.0f || ap.k.y != 1.0f || ap.k.z != 1.0f ) {		// has k scale component
			tm.SetScale( ap.k );
			if ( !ap.u.IsIdentity() ) {			// has u rotation component
				Matrix3 utm;
				ap.u.MakeMatrix( utm );
				mat = Inverse( utm ) * tm * utm * mat;
			} else {
				mat = tm * mat;
			}
		}
	}

	const TCHAR *GetTmpDir()
	{
		int maxTempDirId = 
#if MAX_VERSION_MAJOR >= 9
		APP_TEMP_DIR;
#else
		APP_PLUGCFG_DIR;
#endif

		return IPathConfigMgr::GetPathConfigMgr()->GetDir( maxTempDirId );
	}

	void DBG_PrintActionTables()
	{
		IActionManager *pActionManager = GetCOREInterface()->GetActionManager();

		int actionTableCount = pActionManager->NumActionTables();
		for ( int actionTableIndex = 0; actionTableIndex < actionTableCount; ++actionTableIndex )
		{
			ActionTable *pActionTable = pActionManager->GetTable( actionTableIndex );
			fm::string actionTableName = pActionTable->GetName();
			ActionTableId actionTableId = pActionTable->GetId();
			ActionContextId actionContextId = pActionTable->GetContextId();
			
			g_warningLog.append( "ActionTable : ( " );
			g_warningLog.append( " id = " );
			g_warningLog.append( actionTableId );
			g_warningLog.append( " ," );
			g_warningLog.append( " contextId = " );
			g_warningLog.append( actionContextId );
			g_warningLog.append( " ," );
			g_warningLog.append( " name = " );
			g_warningLog.append( actionTableName );
			g_warningLog.append( " )" );
			g_warningLog.append( "\n" );
			g_warningLog.append( "----------------------------------------------------------------------" );
			g_warningLog.append( "\n" );

			int actionCount = pActionTable->Count();
			for ( int actionIndex = 0; actionIndex < actionCount; ++actionIndex )
			{
				ActionItem *pAction = (*pActionTable)[actionIndex];
				//TRUE returnCode = pAction->ExecuteAction();
				CStr descriptionText;
				pAction->GetDescriptionText( descriptionText );
				int actionId = pAction->GetId();

				CStr menuText;
				pAction->GetMenuText( menuText );

				TCHAR *pShortcutString = pAction->GetShortcutString();

				g_warningLog.append( "Action : ( " );
				g_warningLog.append( " id = " );
				g_warningLog.append( actionId );
				g_warningLog.append( " ," );
				g_warningLog.append( " descriptionText = " );
				g_warningLog.append( descriptionText );
				g_warningLog.append( " ," );
				g_warningLog.append( " menuText = " );
				g_warningLog.append( menuText );
				g_warningLog.append( " ," );
				g_warningLog.append( " shortcutString = " );
				g_warningLog.append( pShortcutString );
				g_warningLog.append( " )" );
				g_warningLog.append( "\n" );
			}

			g_warningLog.append( "\n" );
		}
	}

	ActionItem *FindActionItemByDescriptionText( ActionTable *pActionTable, const fm::string &descriptionText )
	{
		if ( !pActionTable )
		{
			return NULL;
		}

		ActionItem *pRequestedActionItem = NULL;

		int actionCount = pActionTable->Count();
		for ( int actionIndex = 0; actionIndex < actionCount; ++actionIndex )
		{
			ActionItem *pAction = (*pActionTable)[actionIndex];

			CStr thisDescriptionText;
			pAction->GetDescriptionText( thisDescriptionText );

			if ( !_stricmp( thisDescriptionText.data(), descriptionText.c_str() ) )
			{
				return pAction;
			}
		}

		return NULL;
	}
}