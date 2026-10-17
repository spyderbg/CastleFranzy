#pragma once

#include <wx/panel.h>
#include "CIrrLichtAttributesPanel.h"
#include "SMaterial.h"
#include <wx/choice.h>
#include "IMeshSceneNode.h"
#include <wx/sizer.h>

class CObjectMeshBufferPropertiesPanel
	: public CAttributesGrid
{
public:
	CObjectMeshBufferPropertiesPanel()
		:  CAttributesGrid()
	{

	}

	~CObjectMeshBufferPropertiesPanel()
	{
	
	}

	void setMaterial(irr::video::SMaterial *pMaterial)
	{
		m_pMaterial = pMaterial;
		irr::io::CAttributes::clear();
		Clear();
		
		//AddPage("");

		if(pMaterial)
		{
			irr::io::SAttributeReadWriteOptions option;
			option.Flags = irr::io::EARWF_FOR_EDITOR;
			m_pMaterial->serializeAttributes(this, &option);
		}

		//Refresh();
	}

	virtual void OnPostPropertyGridChanged()
	{
		irr::io::SAttributeReadWriteOptions option;
		option.Flags = irr::io::EARWF_FOR_EDITOR;
		m_pMaterial->deserializeAttributes(this, &option);
	}

	irr::video::SMaterial *m_pMaterial;
};

class CMaterialChoise
	: public wxChoice
{
public:
	CMaterialChoise(wxWindow *parent) : wxChoice(parent, wxID_ANY, wxDefaultPosition, wxSize(1, 20)) {}
	void OnSelect(wxCommandEvent&);

	DECLARE_EVENT_TABLE();
};

class CObjectMeshBufferPanel
	: public wxPanel
{
public:
	CObjectMeshBufferPanel(wxWindow *pParent) 
		: wxPanel(pParent) 
		, m_pPropertiesPanel(0)
	{
		s_pInstance = this;
		wxBoxSizer *pParameterPanelSizer = new wxBoxSizer( wxVERTICAL );

		m_MaterialChoiceBox = new CMaterialChoise( this );
		m_MaterialChoiceBox->SetSelection( 0 );
		pParameterPanelSizer->Add( m_MaterialChoiceBox, 0, wxEXPAND, 0 );
		
		wxGridSizer *pParameterSize = new wxGridSizer( 1, 1, 0, 0 );
		m_pGridManager = new CAttributesGridManager(this);

		//m_pPropertiesPanel = new CObjectMeshBufferPropertiesPanel(this);
		//pParameterPanelSizer->Add(m_pPropertiesPanel, 1, wxEXPAND);
		pParameterSize->Add(m_pGridManager, 0, wxEXPAND);
		pParameterPanelSizer->Add( pParameterSize, 1, wxEXPAND, 0 );

		SetSizer(pParameterPanelSizer);
		Layout();
	}

	void setMeshSceneNode(irr::scene::IMeshSceneNode *pMeshSceneNode)
	{
		if(m_pMeshSceneNode == pMeshSceneNode)
		{
			return;
		}

		m_pGridManager->Clear();
		m_pGridManager->Refresh();

		/*for(int i = 0; i < m_pPropertiesPanel.size(); i++)
		{
			delete m_pPropertiesPanel[i];
		}*/

		m_pMeshSceneNode = pMeshSceneNode;
		m_MaterialChoiceBox->Clear();
		m_MaterialChoices.Clear();
		if(m_pMeshSceneNode)
		{
			m_pPropertiesPanel.resize(m_pMeshSceneNode->getMaterialCount());
			for(int i = 0; i < m_pMeshSceneNode->getMaterialCount(); i++)
			{
				CObjectMeshBufferPropertiesPanel *newPage = new CObjectMeshBufferPropertiesPanel();
				newPage->setMaterial(&m_pMeshSceneNode->getMaterial(i));
				wxString materialName;
				materialName.sprintf("%d", m_pMeshSceneNode->getMaterial(i).getHashValue());
				m_MaterialChoices.Add(materialName);
				m_pGridManager->AddPage(wxEmptyString, wxPG_NULL_BITMAP, newPage);
				m_pPropertiesPanel[i] = newPage;
			}
			m_MaterialChoiceBox->Append(m_MaterialChoices);
			m_MaterialChoiceBox->Select(0);
		}

		m_pGridManager->Refresh();
	}

//protected:
	std::vector<CObjectMeshBufferPropertiesPanel *> m_pPropertiesPanel;
	CAttributesGridManager	*m_pGridManager;
	wxArrayString m_MaterialChoices;
	wxChoice* m_MaterialChoiceBox;
	irr::scene::IMeshSceneNode *m_pMeshSceneNode;
public:
	static CObjectMeshBufferPanel *s_pInstance;
};