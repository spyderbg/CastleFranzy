#include "CIrrLichtSceneGraph.h"
#include "CIrrLichtObjectPropertiesPanel.h"
#include "CIrrLichtCanvas.h"
#include "ICameraSceneNode.h"

BEGIN_EVENT_TABLE(CSceneGraph, wxTreeCtrl)
      //EVT_PAINT(CIrrLichtCanvas::OnPaint)
	  EVT_KEY_DOWN(CSceneGraph::OnKeyDown)
	  EVT_TREE_SEL_CHANGED(wxID_ANY, CSceneGraph::OnSelChanged)
	  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, CSceneGraph::OnItemActivated)
END_EVENT_TABLE()


void
CSceneGraph::OnItemActivated(wxTreeEvent& evnt)
{
	CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);
	if(GetSelection().IsOk())
	{
		irr::scene::ISceneNode* pNode = ((CSceneNode *)GetItemData(GetSelection()))->m_pNode;
		switch(pNode->getType())
		{
		case irr::scene::ESNT_CAMERA:
		case irr::scene::ESNT_COLLADA_CAMERA:
		case irr::scene::ESNT_CAMERA_MAYA:
			m_pSceneManager->setActiveCamera((irr::scene::ICameraSceneNode *)pNode);
			break;
		case irr::scene::ESNT_COLLADA_ROOT:
		case irr::scene::ESNT_SCENE_MANAGER:
			break;
		default:
			irr::core::aabbox3df box = pNode->getTransformedBoundingBox();
			
			irr::scene::ICameraSceneNode *camera = CIrrLichtCanvas::s_pInstance->m_pCamera;
			{
				camera->setTarget(box.getCenter());
				irr::core::vector3df size = (box.MaxEdge - box.MinEdge);
				float maxSize = size.X > size.Y ? (size.X > size.Z ? size.X : size.Z) : (size.Y > size.Z ? size.Y : size.Z);
				float distanceOfCamera = (maxSize / 2) * (1 + camera->getFOV() / (3.141596 / 2) );
				camera->setPosition(box.getCenter() + distanceOfCamera);
			}
		}
	}
}

void 
CSceneGraph::Select(irr::scene::ISceneNode *pNode)
{
	wxTreeItemId it = GetRootItem();
	
	Select(pNode, it);
}

bool 
CSceneGraph::Select(irr::scene::ISceneNode *pNode, wxTreeItemId &parent)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId it = GetFirstChild(parent, cookie);

	while(it.IsOk())
	{
		if(((CSceneNode *)GetItemData(it))->m_pNode == pNode)
		{
			SelectItem(it);
			return true;
		}

		if(Select(pNode, it))
		{
			return true;
		}

		it = GetNextChild(parent, cookie);
	}
	return false;
}

void 
CSceneGraph::OnSelChanged(wxTreeEvent& evnt)
{
	if(GetSelection().IsOk())
	{
		CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);
		m_pPropertyPanel->setObject(((CSceneNode *)GetItemData(GetSelection()))->m_pNode);
	}

}

void
CSceneGraph::RemoveSelection()
{

	if(GetSelection().IsOk())
	{
		CIrrLichtCanvas::SLockSceneForModifications lock(CIrrLichtCanvas::s_pInstance);
		irr::scene::ISceneNode *pNode = ((CSceneNode *)GetItemData(GetSelection()))->m_pNode;
		pNode->remove();
		RefreshData();
	}
}

void 
CSceneGraph::OnKeyDown(wxKeyEvent &e)
{
	switch(e.GetKeyCode())
	{
	case WXK_F5:
		RefreshData();
		break;
	case WXK_DELETE:
		RemoveSelection();
		break;
	default:
		e.Skip();
	}
	
}