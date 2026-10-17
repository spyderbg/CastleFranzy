#include "CIrrLichtObjectPropertiesPanel.h"
#include "CColladaSceneNodeAnimator.h"
#include "CIrrLichtCanvas.h"
#include <ISceneNode.h>
#include <wx/propgrid/advprops.h>
#include "IMeshSceneNode.h"
#include "CIrrLichtObjectMeshBufferPanel.h"
#include "CIrrLichtAnimationPanel.h"

CObjectNotebook::CObjectNotebook(wxWindow *pParent)
: wxAuiNotebook(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TAB_SPLIT | 
		wxAUI_NB_TAB_EXTERNAL_MOVE | 
		wxAUI_NB_SCROLL_BUTTONS | 
		wxAUI_NB_WINDOWLIST_BUTTON | 
		wxAUI_NB_BOTTOM | 
		wxBORDER_NONE) 
{
	m_pObjectPanel = new CObjectPropertiesPanel(this);
	AddPage(m_pObjectPanel, "Object");

	m_pMeshPanel = new CObjectMeshBufferPanel(this);
	AddPage(m_pMeshPanel, "Mesh Buffers");

	m_pAnimations = new CAnimationPanel(this);
	AddPage(m_pAnimations, "Animations");	
}

void 
CObjectNotebook::RefreshAnimation() 
{ 
	m_pAnimations->Refresh(); 
}

void 
CObjectNotebook::setObject(irr::scene::ISceneNode *pObject)
{
	m_pObjectPanel->setObject(pObject);

	try
	{
		irr::scene::IMeshSceneNode *pMesh = dynamic_cast<irr::scene::IMeshSceneNode *>(pObject);
		m_pMeshPanel->setMeshSceneNode(pMesh);
	}
	catch ( ... )
	{
		m_pMeshPanel->setMeshSceneNode(0);
	}

	try
	{
		irr::collada::CRootSceneNode *pRoot = dynamic_cast<irr::collada::CRootSceneNode *>(pObject);
		m_pAnimations->setRootSceneNode(pRoot);
	}
	catch ( ... )
	{
		m_pAnimations->setRootSceneNode(0);
	}
	
}


/*
class CObjectPropertiesGrid
	: public CAttributesGrid
{
	CObjectPropertiesGrid(wxWindow *pParent) 
		: CAttributesGridPage()
		, m_pObject(0)
	{
	}

	void setObject(irr::scene::ISceneNode *pObject);

	virtual void OnPostPropertyGridChanged();

	irr::scene::ISceneNode *m_pObject;

};*/



void 
CObjectPropertiesGrid::OnPostPropertyGridChanged(  )
{
	CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);
	irr::io::SAttributeReadWriteOptions option;
	option.Flags = irr::io::EARWF_FOR_EDITOR;
	m_pObject->deserializeAttributes(this, &option);
}

void 
CObjectPropertiesGrid::setObject(irr::scene::ISceneNode *pObject)
{
	CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);

	if(m_pObject)
	{
		m_pObject->setDebugDataVisible(0);
		m_pObject->drop();
	}

	m_pObject = pObject;

	if(m_pObject)
	{
		m_pObject->grab();
		irr::io::SAttributeReadWriteOptions option;
		option.Flags = irr::io::EARWF_FOR_EDITOR;
		m_pObject->serializeAttributes(this, &option);
		m_pObject->setDebugDataVisible(irr::scene::EDS_BBOX);
	}
}

 
void 
CObjectPropertiesGrid::OnShow()
{
	if(m_pObject)
	{
		m_pObject->setDebugDataVisible(irr::scene::EDS_BBOX);
	}
}

void 
CObjectPropertiesPanel::setObject(irr::scene::ISceneNode *pObject)
{
	if(m_pObject == pObject)
		return;
	
	m_pObject = pObject;
	irr::io::CAttributes::clear();
	Clear();
	
	
	if(pObject)
	{
		m_pGrid = new CObjectPropertiesGrid();
		m_pGrid->setObject(pObject);		
		AddPage(wxEmptyString, wxPG_NULL_BITMAP, m_pGrid);
	}

	//CollapseAll();
	Refresh();
}