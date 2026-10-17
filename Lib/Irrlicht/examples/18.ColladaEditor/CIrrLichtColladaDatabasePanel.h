#ifndef __CIRRLICHTCOLLADADATABASE_H__
#define __CIRRLICHTCOLLADADATABASE_H__

#include <wx/treectrl.h>
#include "CColladaDatabase.h"

class CColladaDatabaseGraph
	: public wxTreeCtrl
{
public:
	class CSceneNode
		: public wxTreeItemData
	{
	public:
		enum E_DATA_TYPE
		{
			EDT_DATABASE,
		};

		CSceneNode(irr::collada::CColladaDatabase *pDatabase)
			: m_iType(EDT_DATABASE)
			, m_pData(pDatabase)
		{
			
		}

	protected:
		int m_iType;
		void *m_pData;
	};

	CColladaDatabaseGraph(wxWindow *parent) : wxTreeCtrl(parent)
	{
		assert(s_pInstance == 0);
		s_pInstance = this;
	}

	void OnItemActivated(wxTreeEvent& evnt)
	{

	}

	void OnSelChanged(wxTreeEvent&)
	{

	}

	virtual void AddDatabase(irr::collada::CColladaDatabase *pDatabase)
	{
		wxTreeItemId newItem = AddRoot(pDatabase->getAbsoluteFilename(),-1,-1, new CSceneNode(pDatabase));
	}

	virtual void RemoveDatabase(irr::collada::CColladaDatabase *pDatabase)
	{
		
	}

	static CColladaDatabaseGraph & getInstance() { return *s_pInstance; }

protected:
	static CColladaDatabaseGraph *s_pInstance;
};

#endif