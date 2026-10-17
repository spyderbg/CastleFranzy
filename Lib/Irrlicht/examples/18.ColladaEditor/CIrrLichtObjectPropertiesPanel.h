#ifndef __CIRRLICHTOBJECTPROPERTIES_H__
#define __CIRRLICHTOBJECTPROPERTIES_H__

#include <CAttributes.h>
#include <IAttributeExchangingObject.h>
#include <wx/propgrid/manager.h>
#include <wx/timer.h>
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "CIrrLichtAttributesPanel.h"

namespace irr
{
namespace scene
{
class ISceneNode;
}
}

class CObjectPropertiesPanel;
class CObjectMeshBufferPanel;
class CAnimationPanel;

class CObjectNotebook
	: public wxAuiNotebook
{
public:
	CObjectNotebook(wxWindow *pParent);

	void setObject(irr::scene::ISceneNode *pObject);
	void RefreshAnimation();
protected:
	CObjectPropertiesPanel	*m_pObjectPanel;
	CObjectMeshBufferPanel	*m_pMeshPanel;
	CAnimationPanel			*m_pAnimations;
	irr::scene::ISceneNode	*m_pObject;
};

class CObjectPropertiesGrid
	: public CAttributesGrid
{
public:
	CObjectPropertiesGrid() 
		: CAttributesGrid()
		, m_pObject(0)
	{
	}

	~CObjectPropertiesGrid()
	{
		setObject(0);
	}

	void setObject(irr::scene::ISceneNode *pObject);

	virtual void OnPostPropertyGridChanged();

	virtual void OnShow();
protected:
	irr::scene::ISceneNode *m_pObject;

};

class CObjectPropertiesPanel
	: public CAttributesGridManager	
{
public:

	CObjectPropertiesPanel(wxWindow *pParent) 
		: CAttributesGridManager(pParent)
		, m_pGrid(0)
		, m_pObject(0)
	{
	}

	void setObject(irr::scene::ISceneNode *pObject);

protected:
	irr::scene::ISceneNode * m_pObject;
	CObjectPropertiesGrid *m_pGrid;
	DECLARE_NO_COPY_CLASS(CObjectPropertiesPanel);
};

#endif