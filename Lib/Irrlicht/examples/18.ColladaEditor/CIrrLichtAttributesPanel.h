#ifndef __CIRRLICHTATTRIBUTESGRID_H__
#define __CIRRLICHTATTRIBUTESGRID_H__

#include <CAttributes.h>
#include <IAttributeExchangingObject.h>
#include <wx/propgrid/manager.h>
#include <wx/timer.h>
#include <vector>

namespace irr
{
namespace scene
{
class ISceneNode;
}
}

class CAttributesGridManager
	: public wxPropertyGridManager
	, public irr::io::CAttributes
{
public:

	CAttributesGridManager(wxWindow *pParent)
		: wxPropertyGridManager(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_TOOLBAR | wxPG_SPLITTER_AUTO_CENTER)
	{
		SetExtraStyle(wxPG_EX_MODE_BUTTONS
                  | wxPG_EX_UNFOCUS_ON_ENTER);
	}
};

class CAttributesGrid
	: public wxPropertyGridPage
	, public irr::io::CAttributes
{
public:

	CAttributesGrid() 
		: wxPropertyGridPage() //pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_TOOLBAR | wxPG_SPLITTER_AUTO_CENTER)
	{
		/*SetExtraStyle(wxPG_EX_MODE_BUTTONS
                  | wxPG_EX_UNFOCUS_ON_ENTER);*/
	}

	//wxPGId position2dId = Append( new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>")) );
	std::vector<wxPGId> idsStack;
	wxPGId pushId(const char *id)
	{
		wxPGId pgid = Append( new wxStringProperty(id,wxPG_LABEL,wxT("<composed>")) );
		idsStack.push_back(pgid);
		return pgid; 
	}

	void popId()
	{
		idsStack.pop_back();
	}

	wxPGProperty* Append( wxPGProperty* property )
	{
		if(idsStack.size() > 0)
		{
			return AppendIn(idsStack.back(), property);
		}
		else
		{
			return wxPropertyGridPage::Append(property);
		}
	}

	void OnPropertyGridChanged( wxPropertyGridEvent& event );

	virtual void OnPostPropertyGridChanged() {};

	//! Adds an attribute as integer
	virtual void addInt(const char* attributeName, int value, bool readOnly);

	//! Adds an attribute as float
	virtual void addFloat(const char* attributeName, float value, bool readOnly);

	//! Adds an attribute as string
	virtual void addString(const char* attributeName, const char* value, bool readOnly);

	//! Adds an attribute as 3d vector
	virtual void addVector3d(const char* attributeName, irr::core::vector3df value, bool readOnly);

	virtual void addPosition2d(const char* attributeName, irr::core::position2di value, bool readOnly);

	//! Adds an attribute as axis aligned bounding box
	virtual void addBox3d(const char* attributeName, irr::core::aabbox3df value, bool readOnly);

	virtual void addQuaternion(const char* attributeName, irr::core::quaternion value, bool readOnly);

	//! Adds an attribute as color
	virtual void addColor(const char* attributeName, irr::video::SColor value, bool readOnly);

	//! Adds an attribute as floating point color
	virtual void addColorf(const char* attributeName, irr::video::SColorf value, bool readOnly);

	//! Adds an attribute as bool
	virtual void addBool(const char* attributeName, bool value, bool readOnly);

	//! Adds an attribute as enum
	virtual void addEnum(const char* attributeName, const char* enumValue, 
		const char* const* enumerationLiterals, bool readOnly);

	//! Adds an attribute as enum
	virtual void addEnum(const char* attributeName, int enumValue, 
		const char* const* enumerationLiterals, bool readOnly);

protected:

	DECLARE_EVENT_TABLE()
	DECLARE_NO_COPY_CLASS(CAttributesGrid);
};

#endif
