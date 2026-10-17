#ifndef __CIRRLICHTDRAWCALLS_H__
#define __CIRRLICHTCOLLADADATABASE_H__

#include "CIrrLichtAttributesPanel.h"
#include <list>
#include "CIrrLichtBatch.h"

class CDrawCallElements
	: public CAttributesGrid
{
public:
	CDrawCallElements()
	{

	}

	void setList(std::vector<irr::CBatch *> *pList)
	{
		m_pList = pList;
		irr::io::CAttributes::clear();
		Clear();
		
		if(pList)
		{
			for(int i = 0; i < pList->size(); i++)
			{
				char id[16];
				sprintf(id, "Call %d", i);
				pushId(id);
				(*pList)[i]->serializeAttributes(this);
				popId();
			} 
			CollapseAll();
		}
	}

	virtual void OnPostPropertyGridChanged()
	{
		/*irr::io::SAttributeReadWriteOptions option;
		option.Flags = irr::io::EARWF_FOR_EDITOR;
		m_pMaterial->deserializeAttributes(this, &option);*/
	}

	void OnSelected(wxPropertyGridEvent &event);

	std::vector<irr::CBatch *> *m_pList;

	DECLARE_EVENT_TABLE();
};

class CDrawCallFilters
	: public CAttributesGrid
{
public:
	CDrawCallFilters();
	
	virtual void OnPostPropertyGridChanged();
};

class CDrawCallsList
	: public CAttributesGridManager
{
public:
	CDrawCallsList(wxWindow *parent) : CAttributesGridManager(parent)
	{
		assert(s_pInstance == 0);
		s_pInstance = this;
		AddPage("Filters", wxPG_NULL_BITMAP, &filter);
		AddPage("DrawCalls", wxPG_NULL_BITMAP, &element);
	}

	void setList(std::vector<irr::CBatch *> *pList)
	{
		element.setList(pList);
		wxPropertyGridManager::Refresh();
	}
	
	/*void OnItemActivated(wxListEvent& evnt)
	{

	}

	void OnSelChanged(wxListEvent&)
	{

	}*/

	static CDrawCallsList & getInstance() { return *s_pInstance; }

protected:
	CDrawCallFilters filter;
	CDrawCallElements element;
	static CDrawCallsList *s_pInstance;
};

#endif