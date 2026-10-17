#pragma once

#include <wx/treectrl.h>
#include <ISceneManager.h>
#include <ISceneNode.h>

class CObjectNotebook;



class CSceneGraph
	: public wxTreeCtrl
{
public:
	class CSceneNode : public wxTreeItemData
	{
	public:
		CSceneNode(irr::scene::ISceneNode *pNode) : m_pNode(pNode)
		{
			m_pNode->grab();
		};

		virtual ~CSceneNode()
		{
			m_pNode->drop();
		}
		irr::scene::ISceneNode *m_pNode;
	};

	CSceneGraph(wxWindow *parent) : wxTreeCtrl(parent), m_pSceneManager(0), m_pPropertyPanel(0), m_ListingFlags(0) {}

	void OnItemActivated(wxTreeEvent& evnt);
	void OnSelChanged(wxTreeEvent&);
	void Select(irr::scene::ISceneNode *pNode);
	bool Select(irr::scene::ISceneNode *pNode, wxTreeItemId &parent);

	enum EListingFlags
	{
		ELF_LIGHT = 0x01,
		ELF_GEOMETRY = 0x02,
		ELF_HIERARCHY = 0x04,
		ELF_CAMERA = 0x08,
	};

	int m_ListingFlags;

	void SetListingFlags(int flags)
	{
		m_ListingFlags = flags;
		RefreshData();
	}

	void SetListingFlags(EListingFlags flag, bool value)
	{
		m_ListingFlags &= ~flag;
		if(value)
		{
			m_ListingFlags |= flag;
		}
		RefreshData();
	}

	virtual void AddNode(irr::scene::ISceneNode *pNode, wxTreeItemId *parent = 0)
	{
		wxTreeItemId newItem;
		bool bAddToTree = false;

		switch(pNode->getType())
		{
		case irr::scene::ESNT_MESH:
		case irr::scene::ESNT_COLLADA_MESH:
			bAddToTree = (m_ListingFlags & ELF_GEOMETRY);
			break;
		case irr::scene::ESNT_CAMERA:
		case irr::scene::ESNT_CAMERA_MAYA:
		case irr::scene::ESNT_CAMERA_FPS:
		case irr::scene::ESNT_COLLADA_CAMERA:
			bAddToTree = (m_ListingFlags & ELF_CAMERA);
			break;
		case irr::scene::ESNT_COLLADA_NODE:
			bAddToTree = (m_ListingFlags & ELF_HIERARCHY);
			break;
		case irr::scene::ESNT_LIGHT:
			bAddToTree = (m_ListingFlags & ELF_LIGHT);
			break;
		case irr::scene::ESNT_COLLADA_ROOT:
		default:
			bAddToTree = true;
			break;
		}

		if(bAddToTree)
		{
			if(parent == 0)
			{
				if(!pNode->getUID() || *pNode->getUID() == 0)
				{
					newItem = AddRoot(pNode->getDebugName(),-1,-1, new CSceneNode(pNode));
				}
				else
				{
					newItem = AddRoot(pNode->getUID(),-1,-1, new CSceneNode(pNode));
				}
			}
			else
			{
				if(!pNode->getUID() || *pNode->getUID() == 0)
				{
					newItem = AppendItem(*parent, pNode->getDebugName(),-1,-1, new CSceneNode(pNode));
				}
				else
				{
					newItem = AppendItem(*parent, pNode->getUID(),-1,-1, new CSceneNode(pNode));
				}
			}

			const irr::core::list<irr::scene::ISceneNode*>& children = pNode->getChildren();
			irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = pNode->getChildren().begin();
			for(; it != children.end(); it++)
			{
				AddNode(*it, &newItem);
			}
		}
		else
		{
			const irr::core::list<irr::scene::ISceneNode*>& children = pNode->getChildren();
			irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = pNode->getChildren().begin();
			for(; it != children.end(); it++)
			{
				AddNode(*it, parent);
			}
		}
	}

	virtual void Update() 
	{
		
	}

	void OnKeyDown(wxKeyEvent &e);

	void RemoveSelection();

	void RefreshData()
	{
		DeleteAllItems();
		if(m_pSceneManager)
		{
			AddNode(m_pSceneManager->getRootSceneNode());
		}
		Expand(this->GetRootItem());
	}

	void setSceneManager(irr::scene::ISceneManager *pSceneManager) 
	{
		m_pSceneManager = pSceneManager;
		RefreshData();
	}

	void setPropertyPanel(CObjectNotebook *pPropertyPanel) { m_pPropertyPanel = pPropertyPanel; }

	irr::scene::ISceneManager *m_pSceneManager;

	protected:
	CObjectNotebook *m_pPropertyPanel;
	DECLARE_EVENT_TABLE()
};