/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ICOBJECT_PROPERTY_MANAGER_H_
#define _ICOBJECT_PROPERTY_MANAGER_H_

#include "ICUIManager.h"
#include "ICObjectProperty.h"

class IrrCollada;
class ICObjectPropertyPanel;

class ICObjectPropertyManager : public ICUIManager
{
private:
	fm::pvector<ICObjectProperty> m_objectProperties;
	fm::pvector<ICObjectPropertyPanel> m_panels;

public:
	ICObjectPropertyManager( IrrCollada *pIrrCollada ) 
		: ICUIManager( ICUIManager::MT_OBJECT_PROPERTY_MANAGER, pIrrCollada )
	{
		Init();
	}

	~ICObjectPropertyManager();
	
	void Release();
	void Init();
	void RefreshDialog();
	void DisplayDialog( BOOL show );
	fm::string ConstructDialogCaption();
	//void InitDialogPos();
	void PostCloneNotification( INodeTab *pOrigNodes, INodeTab *pClonedNodes, CloneType cloneType );

	DLG_PROC_DECL( ICObjectPropertyDialog )
	DLG_PROC_DECL( ICObjectPropertiesRollout )

	inline size_t GetNumObjectProperties() const
	{
		return m_objectProperties.size();
	}

	inline const ICObjectProperty &GetObjectPropertyRef( size_t index ) const
	{
		return *m_objectProperties[index];
	}

	const ICObjectProperty *GetObjectPropertyPtrByCID( const int cid ) const;
};

#endif // _ICOBJECT_PROPERTY_MANAGER_H_


