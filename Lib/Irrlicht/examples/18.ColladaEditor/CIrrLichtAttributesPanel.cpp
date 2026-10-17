#include "CIrrLichtAttributesPanel.h"
#include "CColladaSceneNodeAnimator.h"
#include "CIrrLichtCanvas.h"
#include <ISceneNode.h>
#include <wx/propgrid/advprops.h>

BEGIN_EVENT_TABLE(CAttributesGrid, wxPropertyGridPage)
	EVT_PG_CHANGED(wxID_ANY, CAttributesGrid::OnPropertyGridChanged)
END_EVENT_TABLE()

void 
CAttributesGrid::OnPropertyGridChanged( wxPropertyGridEvent& event )
{
    wxPGProperty *property = event.GetProperty();

    // It may be NULL
    if ( !property )
        return;

    // Get name of changed property
    const wxString& fullname = property->GetName();
	wxString name = fullname;
	if(fullname.find('.'))
	{
		name = fullname.SubString(0, fullname.find('.')-1);
	}
    // Get resulting value
    wxVariant value = property->GetValue();
	
	irr::io::IAttribute *pAttr = getAttributeP(name);
	if(!pAttr)
		return;
	switch(pAttr->getType())
	{
	case irr::io::EAT_FLOAT:
		pAttr->setFloat(value.GetReal());
		break;
	case irr::io::EAT_BOOL:
		pAttr->setBool(value.GetBool());
		break;
	case irr::io::EAT_ENUM:
		pAttr->setInt(value.GetInteger());
		break;
	case irr::io::EAT_VECTOR3D:
		{
			irr::core::vector3df vector = pAttr->getVector();
			if(fullname.find(".x") != wxString::npos)
			{
				vector.X = value.GetReal();
			} 
			else if(fullname.find(".y") != wxString::npos)
			{
				vector.Y = value.GetReal();
			} 
			else if(fullname.find(".z") != wxString::npos)
			{
				vector.Z = value.GetReal();
			}
			pAttr->setVector(vector);
		}
		break;
	case irr::io::EAT_QUATERNION:
		{
			irr::core::quaternion quat = pAttr->getQuaternion();
			if(fullname.find(".x") != wxString::npos)
			{
				quat.X = value.GetReal();
			} 
			else if(fullname.find(".y") != wxString::npos)
			{
				quat.Y = value.GetReal();
			} 
			else if(fullname.find(".z") != wxString::npos)
			{
				quat.Z = value.GetReal();
			}
			else if(fullname.find(".w") != wxString::npos)
			{
				quat.W = value.GetReal();
			}
			quat.normalize();
			pAttr->setQuaternion(quat);
		}
		break;
	case irr::io::EAT_INT:
		pAttr->setInt(value.GetInteger());
		break;
	case irr::io::EAT_COLOR: // color attribute
		{
			irr::video::SColor color = pAttr->getColor();
			if(fullname.find(".Red") != wxString::npos)
			{
				color.setRed(value.GetInteger());
			} else if(fullname.find(".Green") != wxString::npos)
			{
				color.setGreen(value.GetInteger());
			} else if(fullname.find(".Blue") != wxString::npos)
			{
				color.setBlue(value.GetInteger());
			} else if(fullname.find(".Alpha") != wxString::npos)
			{
				color.setAlpha(value.GetInteger());
			} else if(fullname.find(".Color") != wxString::npos)
			{
				wxColourProperty newColor;
				newColor.SetValue(value);
				color.setRed(newColor.GetVal().m_colour.Red());
				color.setGreen(newColor.GetVal().m_colour.Green());
				color.setBlue(newColor.GetVal().m_colour.Blue());
			}
			pAttr->setColor(color);
		}
		break;
	case irr::io::EAT_COLORF: // color attribute
		{
			irr::video::SColorf color = pAttr->getColorf();
			if(fullname.find(".Red") != wxString::npos)
			{
				color.R = value.GetReal();
			} else if(fullname.find(".Green") != wxString::npos)
			{
				color.G = value.GetReal();
			} else if(fullname.find(".Blue") != wxString::npos)
			{
				color.B = value.GetReal();
			} else if(fullname.find(".Alpha") != wxString::npos)
			{
				color.A = value.GetReal();
			} else if(fullname.find(".Color") != wxString::npos)
			{
				wxColourProperty newColor;
				newColor.SetValue(value);
				color.R = newColor.GetVal().m_colour.Red() / 255.0f;
				color.G = newColor.GetVal().m_colour.Green() / 255.0f;
				color.B = newColor.GetVal().m_colour.Blue() / 255.0f;
			}
			pAttr->setColor(color);
		}
		break;
	}

	OnPostPropertyGridChanged();
}


//! Adds an attribute as integer

void 
CAttributesGrid::addInt(const char* attributeName, int value, bool readOnly)
{
	wxIntProperty *pInt = new wxIntProperty(attributeName, wxPG_LABEL, value);
	wxPGId Id = Append( pInt );
	if(readOnly)
	{
		pInt->SetFlag(wxPG_PROP_DISABLED);
	}
	irr::io::CAttributes::addInt(attributeName, value, readOnly);
}

//! Adds an attribute as float
void 
CAttributesGrid::addFloat(const char* attributeName, float value, bool readOnly)
{
	wxFloatProperty *pFloat = new wxFloatProperty(attributeName, wxPG_LABEL, value);
	wxPGId Id = Append( pFloat );
	if(readOnly)
	{
		pFloat->SetFlag(wxPG_PROP_DISABLED);
	}
	irr::io::CAttributes::addFloat(attributeName, value, readOnly);
}

//! Adds an attribute as string
void 
CAttributesGrid::addString(const char* attributeName, const char* value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName, wxPG_LABEL, value);
	wxPGId Id = Append( pString );
	if(readOnly)
	{
		pString->SetFlag(wxPG_PROP_DISABLED);
	}
	irr::io::CAttributes::addString(attributeName, value, readOnly);
}

//! Adds an attribute as 3d vector
void 
CAttributesGrid::addVector3d(const char* attributeName, irr::core::vector3df value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>"));
	wxPGId vector3dId = Append( pString );
    AppendIn( vector3dId, new wxFloatProperty(wxT("x"), wxPG_LABEL, value.X) );
    AppendIn( vector3dId, new wxFloatProperty(wxT("y"), wxPG_LABEL, value.Y) );
    AppendIn( vector3dId, new wxFloatProperty(wxT("z"), wxPG_LABEL, value.Z) );

	if(readOnly)
	{
		pString->SetFlagRecursively(wxPG_PROP_DISABLED, true);
	}

	irr::io::CAttributes::addVector3d(attributeName, value, readOnly);
}

void 
CAttributesGrid::addPosition2d(const char* attributeName, irr::core::position2di value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>"));
	wxPGId position2dId = Append( pString );
    AppendIn( position2dId, new wxIntProperty(wxT("x"), wxPG_LABEL, value.X) );
    AppendIn( position2dId, new wxIntProperty(wxT("y"), wxPG_LABEL, value.Y) );
	if(readOnly)
	{
		pString->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addPosition2d(attributeName, value, readOnly);
}

//! Adds an attribute as axis aligned bounding box
void 
CAttributesGrid::addBox3d(const char* attributeName, irr::core::aabbox3df value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>"));
	wxPGId boundingboxId = Append( pString );
	wxPGId minEdge = AppendIn(boundingboxId, new wxStringProperty(wxT("Min Edge"),wxPG_LABEL,wxT("<composed>")) );
	AppendIn( minEdge, new wxFloatProperty(wxT("x"), wxPG_LABEL, value.MinEdge.X) );
    AppendIn( minEdge, new wxFloatProperty(wxT("y"), wxPG_LABEL, value.MinEdge.Y) );
    AppendIn( minEdge, new wxFloatProperty(wxT("z"), wxPG_LABEL, value.MinEdge.Z) );
	wxPGId maxEdge = AppendIn(boundingboxId, new wxStringProperty(wxT("Max Edge"),wxPG_LABEL,wxT("<composed>")) );
	AppendIn( maxEdge, new wxFloatProperty(wxT("x"), wxPG_LABEL, value.MaxEdge.X) );
    AppendIn( maxEdge, new wxFloatProperty(wxT("y"), wxPG_LABEL, value.MaxEdge.Y) );
    AppendIn( maxEdge, new wxFloatProperty(wxT("z"), wxPG_LABEL, value.MaxEdge.Z) );
	if(readOnly)
	{
		pString->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addBox3d(attributeName, value, readOnly);
}

void 
CAttributesGrid::addQuaternion(const char* attributeName, irr::core::quaternion value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>"));
	wxPGId quaternionId = Append( pString );
    AppendIn( quaternionId, new wxFloatProperty(wxT("x"), wxPG_LABEL, value.X) );
    AppendIn( quaternionId, new wxFloatProperty(wxT("y"), wxPG_LABEL, value.Y) );
    AppendIn( quaternionId, new wxFloatProperty(wxT("z"), wxPG_LABEL, value.Z) );
	AppendIn( quaternionId, new wxFloatProperty(wxT("w"), wxPG_LABEL, value.W) );
	if(readOnly)
	{
		pString->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addQuaternion(attributeName, value, readOnly);
}

//! Adds an attribute as color
void 
CAttributesGrid::addColor(const char* attributeName, irr::video::SColor value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>"));
	wxPGId colorId = Append( pString );
	wxColor color;
	color.Set(value.R, value.G, value.B);
	AppendIn( colorId, new wxColourProperty(wxT("Color"), wxPG_LABEL, color));
	AppendIn( colorId, new wxIntProperty(wxT("Alpha"), wxPG_LABEL, value.getAlpha()) );
	if(readOnly)
	{
		pString->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addColor(attributeName, value, readOnly);
}

//! Adds an attribute as floating point color
void 
CAttributesGrid::addColorf(const char* attributeName, irr::video::SColorf value, bool readOnly)
{
	wxStringProperty *pString = new wxStringProperty(attributeName,wxPG_LABEL,wxT("<composed>"));
	wxPGId colorId = Append( pString );
	wxColor color;
	color.Set(value.R * 255.0f, value.G * 255.0f, value.B * 255.0f);
	AppendIn( colorId, new wxColourProperty(wxT("Color"), wxPG_LABEL, color));
	AppendIn( colorId, new wxIntProperty(wxT("Alpha"), wxPG_LABEL, value.getAlpha() * 255) );
	if(readOnly)
	{
		pString->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addColorf(attributeName, value, readOnly);
}

//! Adds an attribute as bool
void 
CAttributesGrid::addBool(const char* attributeName, bool value, bool readOnly)
{
	wxBoolProperty *pBoolProp = new wxBoolProperty(attributeName,wxPG_LABEL,value);
	pBoolProp->SetAttribute(wxPG_BOOL_USE_CHECKBOX, 1);
	wxPGId Id = Append( pBoolProp );
	if(readOnly)
	{
		pBoolProp->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addBool(attributeName, value, readOnly);
}

//! Adds an attribute as enum
void 
CAttributesGrid::addEnum(const char* attributeName, const char* enumValue, const char* const* enumerationLiterals
						 , bool readOnly)
{
	//wxPGId colorId = Append( new wxEnumProperty(attributeName,wxPG_LABEL,enumerationLiterals) );
	//Attributes.push_back(irrnew CEnumAttribute(attributeName, enumValue, enumerationLiterals));
	irr::io::CAttributes::addEnum(attributeName, enumValue, enumerationLiterals, readOnly);
}

//! Adds an attribute as enum
void 
CAttributesGrid::addEnum(const char* attributeName, int enumValue, const char* const* enumerationLiterals
						 , bool readOnly)
{
	wxEnumProperty *pEnum = new wxEnumProperty(attributeName,wxPG_LABEL,(const wxChar **)enumerationLiterals, 0, enumValue);
	wxPGId Id = Append(pEnum);
	if(readOnly)
	{
		pEnum->SetFlagRecursively(wxPG_PROP_DISABLED, readOnly);
	}
	irr::io::CAttributes::addEnum(attributeName, enumValue, enumerationLiterals, readOnly);
}