#pragma once

#include <wx/panel.h>
#include <wx/filepicker.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/propdev.h>
#include "CColladaSceneNodeAnimator.h"
#include "CColladaRootSceneNode.h"
#include "CIrrLichtAttributesPanel.h"
#include "CColladaSceneNodeAnimatorSet.h"

class CAnimationGrid
	: public wxPropertyGridPage
{
	void OnKeyEvent( wxKeyEvent& event );
protected:
	DECLARE_EVENT_TABLE();
};

class CAnimationPanel
	: public CAttributesGridManager
{
public:

	CAnimationPanel(wxWindow *pParent) : CAttributesGridManager(pParent), m_pGrid(0)
	{	
	}

	virtual ~CAnimationPanel()
	{
		if(m_pGrid)
			delete m_pGrid;
	}

	void setRootSceneNode(irr::collada::CRootSceneNode *pRootSceneNode)
	{
		m_pRootSceneNode = pRootSceneNode;
		Refresh();
		DragAcceptFiles(m_pRootSceneNode != 0);
	}

	void LoadAnimation(const char *pAnimationFile);

	void OnDropFile(wxDropFilesEvent&);

	virtual void OnPostPropertyGridChanged()
	{
		int iCurrentClip = GetProperty("Animation 0.CurrentClip")->GetValue().GetInteger();
		(*m_pRootSceneNode->getAnimators().begin())->getTimelineCtrl()->setClip(iCurrentClip);
	}

	void Refresh()
	{
		Clear();
		m_pGrid = new CAnimationGrid();
		AddPage(wxEmptyString, wxPG_NULL_BITMAP, m_pGrid);
		if(m_pRootSceneNode == 0)
			return;

		const irr::core::list<irr::scene::ISceneNodeAnimator*>& animatorList = m_pRootSceneNode->getAnimators();

		irr::core::list<irr::scene::ISceneNodeAnimator*>::ConstIterator it = animatorList.begin();
		for(int i = 0; it != animatorList.end(); i++, it++)
		{
			{
				irr::collada::CSceneNodeAnimator *pAnimator = dynamic_cast<irr::collada::CSceneNodeAnimator *> (*it);
				if(pAnimator)
				{
					wxString label;
					label.Printf("Animator %i", i);

					wxPGId animationId = m_pGrid->Append( new wxStringProperty(label,wxPG_LABEL,wxT("<composed>")) );

					
					{ // Add clips enum
						wxArrayString clips;
						for(int i = 0; i < pAnimator->getTimelineCtrl()->getClipCount(); i++)
						{
							clips.push_back(pAnimator->getTimelineCtrl()->getClipName(i));
						}
						m_pGrid->AppendIn(animationId, new wxEnumProperty(wxT("Current Clip"), wxPG_LABEL, clips, wxArrayInt(), 0) );

					}
					
					#if wxUSE_SPINBTN
						m_pGrid->AppendIn( animationId, new wxFloatProperty(wxT("BlendOutWeight"), wxPG_LABEL, pAnimator->BlendOutWeight) );
						//pg->Append( new wxIntProperty ( wxT("SpinCtrl"), wxPG_LABEL, 0 ) );
					
						SetPropertyEditor( label + "." + wxT("BlendOutWeight"), wxPG_EDITOR(SpinCtrl) );
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxPG_ATTR_MIN, (float)0 );  // Use constants instead of string
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxPG_ATTR_MAX, (float)1.0 );   // for reduced binary size.
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxT("Step"), (float)0.01 );
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxT("MotionSpin"), true );
						//pg->SetPropertyAttribute( wxT("SpinCtrl"), wxT("Wrap"), true );


					#endif
				}
			}

			{
				irr::collada::CSceneNodeAnimatorSet *pAnimator2 = dynamic_cast<irr::collada::CSceneNodeAnimatorSet *> (*it);
				if(pAnimator2)
				{
					wxString label;
					label.Printf("Animator %i", i);

					wxPGId animationId = m_pGrid->Append( new wxStringProperty(label,wxPG_LABEL,wxT("<composed>")) );

					
					{ // Add clips enum
						wxArrayString clips;
						for(int i = 0; i < pAnimator2->getTimelineCtrl()->getClipCount(); i++)
						{
							clips.push_back(pAnimator2->getTimelineCtrl()->getClipName(i));
						}
						m_pGrid->AppendIn(animationId, new wxEnumProperty(wxT("Current Clip"), wxPG_LABEL, clips, wxArrayInt(), 0) );

					}
					
					#if wxUSE_SPINBTN
						m_pGrid->AppendIn( animationId, new wxIntProperty(wxT("AnimationCount"), wxPG_LABEL, pAnimator2->getAnimationCount()) );
						m_pGrid->AppendIn( animationId, new wxIntProperty(wxT("CurrentAnimation"), wxPG_LABEL, pAnimator2->getCurrentAnimation()) );
						
					
						/*SetPropertyEditor( label + "." + wxT("BlendOutWeight"), wxPG_EDITOR(SpinCtrl) );
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxPG_ATTR_MIN, (float)0 );  // Use constants instead of string
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxPG_ATTR_MAX, (float)1.0 );   // for reduced binary size.
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxT("Step"), (float)0.01 );
						SetPropertyAttribute( label + "." + wxT("BlendOutWeight"), wxT("MotionSpin"), true );*/
						//pg->SetPropertyAttribute( wxT("SpinCtrl"), wxT("Wrap"), true );


					#endif
				}
			}
		}
		wxPropertyGridManager::Refresh();
	}


	void OnKeyEvent( wxKeyEvent& event );

protected:
	CAnimationGrid	*m_pGrid;
	irr::collada::CRootSceneNode *m_pRootSceneNode;
	DECLARE_EVENT_TABLE();
};

