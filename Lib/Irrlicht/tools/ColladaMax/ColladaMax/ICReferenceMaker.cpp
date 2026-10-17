/**********************************************************************
 *<
	FILE: ModularSkinCategory.cpp

	DESCRIPTION: A simple planar deflector

	CREATED BY: Rolf Berteig
			

	HISTORY: Peter Watje 3-15-00

 *>	Copyright (c) 200, All Rights Reserved.
 **********************************************************************/

#include "StdAfx.h"
#include "ICReferenceMaker.h"

int SimpleReferenceMaker::NumRefs()
{
	return m_references.size();
}

RefTargetHandle SimpleReferenceMaker::GetReference(int i)
{
	if ( i >= NumRefs() )
	{
		return NULL;
	}

	return (RefTargetHandle)m_references[i];
}

void SimpleReferenceMaker::SetReference(int i, RefTargetHandle rtarg)
{
	if ( i >= NumRefs() )
	{
		return;
	}

	m_references[i] = rtarg;
}

void SimpleReferenceMaker::RegisterReference( RefTargetHandle refTargetHandle )
{
	ReferenceTargetHandleList::iterator it = m_references.find( refTargetHandle );
	if ( it == m_references.end() )
	{
		int oldArraySize = m_references.size();
		m_references.resize( oldArraySize + 1 );
		SetReference( oldArraySize, NULL );
		ReplaceReference( oldArraySize, refTargetHandle );
	}
}

void SimpleReferenceMaker::UnregisterReference( RefTargetHandle refTargetHandle )
{
	int index = m_references.find_index( refTargetHandle );
	if ( index != -1 )
	{
		DeleteReference( index );
		m_references.erase( index );
	}
}

void SimpleReferenceMaker::UnregisterAllReferences()
{
	//for ( size_t index = 0; index < m_references.size(); ++index )
	//{
	//	DeleteReference( index );
	//}

	DeleteAllRefsFromMe();
	m_references.clear();
}

//RefResult ModifierReferenceMaker::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message )
//{           
//	switch (message)
//	{      
//		case REFMSG_REF_DELETED:
//		{
//			for ( int i = 0 ; i < m_references.size(); i++ )
//			{
//				if ( hTarget == m_references[i] ) 
//				{ 
//					DeleteReference( i );
//					m_references.erase( i );
//				}
//			}
//			break;
//		}
//		//case REFMSG_TARGET_DELETED:   
//		//{ 
//		//	for ( int i = 0 ; i < m_references.size(); i++ )
//		//	{
//		//		if ( hTarget == m_references[i] ) 
//		//		{ 
//		//			DeleteReference( i );
//		//			m_references.erase( i );
//		//		}
//		//	}
//		//	break;
//		//}
//		//case REFMSG_NODE_NAMECHANGE:
//		//{ 
//		//	//if (hTarget==custnode) 
//		//	//{ 
//		//	//	custname = TSTR(custnode->GetName());
//		//	//	ShowName(0);cancelled=FALSE;
//		//	//}
//		//	//if (hTarget==distnode) 
//		//	//{ 
//		//	//	distname = TSTR(distnode->GetName());
//		//	//	ShowName(1);cancelled=FALSE;
//		//	//}
//		//	//if (hTarget==mbase) 
//		//	//{ 
//		//	//	normname = TSTR(mbase->GetName());
//		//	//	ShowName(shownormname);cancelled=FALSE;
//		//	//}
//		//	//BOOL notfound=TRUE;
//		//	//for (int i=0;(i<nlist.Count())&&(notfound);i++)
//		//	//{
//		//	//	if (hTarget==nlist[i]) 
//		//	//	{
//		//	//		notfound=FALSE;SetUpList();cancelled=FALSE;
//		//	//	}
//		//	//}
//		//	//break;
//		//}
//		//case REFMSG_CHECK_FOR_INVALID_BIND:
//		//case REFMSG_NODE_LINK:     
//		//case REFMSG_CHANGE:
//		//{ 
//		//	//int pblst=0;
//		//	//if (pblock && (pblock==hTarget))
//		//	//{ 
//		//	//	pblst=pblock->LastNotifyParamNum();
//		//	//	if (pblst==FSParticlePCloud::PB_METACOURSE)
//		//	//		return REF_STOP;
//		//	//	if (pblst==FSParticlePCloud::PB_SPAWNLIFEVLUE)
//		//	//		return REF_STOP;
//		//	//}
//		//	//MeshInvalid();
//		//	//ParticleInvalid();
//		//	//if (editOb==this) InvalidateUI();
//		//	//cancelled=FALSE;
//		//	//break;
//		//}
//		//case REFMSG_SUBANIM_STRUCTURE_CHANGED:
//		//{
//		//	//EnableWindow(GetDlgItem(hptype,IDC_SP_MAPCUSTOMEMIT),TRUE);
//		//	//if (editOb==this) InvalidateUI();
//		//	//       NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
//		//	//break;
//		//}
//		//default: 
//		//{
//		//	break;
//		//}
//	}
//
//	return REF_SUCCEED;
//}
//
//RefResult SkinnedMeshReferenceMaker::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message )
//{           
//	switch (message)
//	{      
//		case REFMSG_REF_DELETED:
//		{
//			for ( int i = 0 ; i < m_references.size(); i++ )
//			{
//				if ( hTarget == m_references[i] ) 
//				{ 
//					DeleteReference( i );
//					m_references.erase( i );
//				}
//			}
//			break;
//		}
//		//case REFMSG_TARGET_DELETED:   
//		//{ 
//		//	for ( int i = 0 ; i < m_references.size(); i++ )
//		//	{
//		//		if ( hTarget == m_references[i] ) 
//		//		{ 
//		//			DeleteReference( i );
//		//			m_references.erase( i );
//		//		}
//		//	}
//		//	break;
//		//}
//		//case REFMSG_NODE_NAMECHANGE:
//		//{ 
//		//	//if (hTarget==custnode) 
//		//	//{ 
//		//	//	custname = TSTR(custnode->GetName());
//		//	//	ShowName(0);cancelled=FALSE;
//		//	//}
//		//	//if (hTarget==distnode) 
//		//	//{ 
//		//	//	distname = TSTR(distnode->GetName());
//		//	//	ShowName(1);cancelled=FALSE;
//		//	//}
//		//	//if (hTarget==mbase) 
//		//	//{ 
//		//	//	normname = TSTR(mbase->GetName());
//		//	//	ShowName(shownormname);cancelled=FALSE;
//		//	//}
//		//	//BOOL notfound=TRUE;
//		//	//for (int i=0;(i<nlist.Count())&&(notfound);i++)
//		//	//{
//		//	//	if (hTarget==nlist[i]) 
//		//	//	{
//		//	//		notfound=FALSE;SetUpList();cancelled=FALSE;
//		//	//	}
//		//	//}
//		//	//break;
//		//}
//		//case REFMSG_CHECK_FOR_INVALID_BIND:
//		//case REFMSG_NODE_LINK:     
//		//case REFMSG_CHANGE:
//		//{ 
//		//	//int pblst=0;
//		//	//if (pblock && (pblock==hTarget))
//		//	//{ 
//		//	//	pblst=pblock->LastNotifyParamNum();
//		//	//	if (pblst==FSParticlePCloud::PB_METACOURSE)
//		//	//		return REF_STOP;
//		//	//	if (pblst==FSParticlePCloud::PB_SPAWNLIFEVLUE)
//		//	//		return REF_STOP;
//		//	//}
//		//	//MeshInvalid();
//		//	//ParticleInvalid();
//		//	//if (editOb==this) InvalidateUI();
//		//	//cancelled=FALSE;
//		//	//break;
//		//}
//		//case REFMSG_SUBANIM_STRUCTURE_CHANGED:
//		//{
//		//	//EnableWindow(GetDlgItem(hptype,IDC_SP_MAPCUSTOMEMIT),TRUE);
//		//	//if (editOb==this) InvalidateUI();
//		//	//       NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
//		//	//break;
//		//}
//		//default: 
//		//{
//		//	break;
//		//}
//	}
//
//	return REF_SUCCEED;
//}