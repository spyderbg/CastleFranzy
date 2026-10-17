/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/*
	Based off the a sample by Neil Hazzard and Nikolai Sander.
	Copyright (C) Autodesk 2000, All Rights Reserved.
*/

#include "StdAfx.h"
#include "ColladaModifierManager.h"
#include "ColladaModifiers.h"

ColladaModifierManager::~ColladaModifierManager()
{
	CLEAR_POINTER_VECTOR( modifiers );
}

void ColladaModifierManager::ResolveModifiers(INode *pNode, Object* obj)
{
	CLEAR_POINTER_VECTOR( modifiers );
	ResolveModifiers( pNode, obj, modifiers );
}

void ColladaModifierManager::ResolveModifiers( INode *pNode, Object *obj, const ColladaModifierStack &targetModifiers, ColladaModifierStack &foundModifiers )
{
	if ( obj == NULL
		 || targetModifiers.size() == 0 )
	{
		return;
	}

	SClass_ID		sc;
	IDerivedObject* dobj;
	Object *currentObject = obj;

	int modIdx = 0;
	sc = obj->SuperClassID();
	if (sc == GEN_DERIVOB_CLASS_ID || sc == DERIVOB_CLASS_ID || sc == WSM_DERIVOB_CLASS_ID)
	{
		dobj = (IDerivedObject*)obj;
		while (sc == GEN_DERIVOB_CLASS_ID || sc == DERIVOB_CLASS_ID || sc == WSM_DERIVOB_CLASS_ID)
		{
			for (int j = 0; j < dobj->NumModifiers(); j++, modIdx++)
			{
				Modifier* mod = dobj->GetModifier(j);

				// matching test
				for ( ColladaModifierStack::const_iterator colladaModifierIter = targetModifiers.begin(); colladaModifierIter != targetModifiers.end(); ++colladaModifierIter )
				{
					if ( (*colladaModifierIter)->Matches( mod ) )
					{
						ColladaModifier *pColladaModifier = (*colladaModifierIter)->CreateInstance();

						pColladaModifier->SetDerivedObject( dobj );
						pColladaModifier->SetModifierIndex( j );
						pColladaModifier->SetModifierStackIndex( modIdx );
						pColladaModifier->SetResolved( true );

						foundModifiers.push_back( pColladaModifier );
						break;
					}
				}
			}
			dobj = (IDerivedObject*) dobj->GetObjRef();
			currentObject = (Object*) dobj;
			sc = dobj->SuperClassID();
		}
	}

	int bct = currentObject->NumPipeBranches(FALSE);
	for (int bi = 0; bi < bct; bi++)
	{
		Object* bobj = currentObject->GetPipeBranch( bi,FALSE );
		ResolveModifiers( pNode, bobj, targetModifiers, foundModifiers );
		//return;
	}
}