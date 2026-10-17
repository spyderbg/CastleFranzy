////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////

#include "Core/Application.h"

#include "Game/Config/GameConfig.h"

////////////////////////////////////////////////////////

const bool CGameConfig::DisableRTT = false;
const bool CGameConfig::EnableCheats = false;

SColor CGameConfig::EmissiveColorForInteractiveObjects = 0xff505050;

bool CGameConfig::NoWeaponCD = false;
bool CGameConfig::AllWeaponsSold = false;
bool CGameConfig::AllowDowngrades = false;
bool CGameConfig::Invulnerable = false;
bool CGameConfig::DontSpawnUnits = false;
bool CGameConfig::RenderToTexture = false;
bool CGameConfig::DrawFPS = false;
bool CGameConfig::UnlockAllLevels = false;

////////////////////////////////////////////////////////

CGameConfig::CGameConfig()
{
}

CGameConfig::~CGameConfig()
{
}

bool CGameConfig::exportObjects(const c8 * fileName, TAttributesMap & map, const wchar_t * objectName)
{
	IFileSystem * fs = __IRR_FILE_SYSTEM();
	IWriteFile* file = fs->createAndWriteFile(fileName);
	if( file == NULL )
		return false;
	else
		file->drop();
	IXMLWriter * xml = fs->createXMLWriter(fileName);
	if (!xml)
		return false;

	xml->writeXMLHeader(); 
	xml->writeLineBreak();
	xml->writeComment((stringw(objectName) + L" config file").c_str());	
	xml->writeLineBreak();
	xml->writeLineBreak();
	
	for (TAttributesMap::Iterator it = map.getIterator(); !it.atEnd(); it++)
	{
		xml->writeElement(objectName, false, L"id", it->getKey().c_str());
		xml->writeLineBreak();
		it->getValue()->write(xml, false);
		xml->writeClosingTag(objectName);
		xml->writeLineBreak();
		xml->writeLineBreak();
	}
	
	xml->drop();
	return true;
}

bool CGameConfig::importObjects(const c8 * fileName, TAttributesMap& attrMap, const stringw& type)
{
	IFileSystem * fs = __IRR_FILE_SYSTEM();
	IXMLReader * xml = fs->createXMLReader(fileName);
	if (!xml)
		return false;

	while (xml->read())
	{
		switch(xml->getNodeType())
		{
			case EXN_ELEMENT:
			{
				stringw objectType = xml->getNodeName();
				if( type != objectType )
					continue;

				stringw objectId = xml->getAttributeValue(L"id");
				IAttributes* objectAttr = fs->createEmptyAttributes();
				IAttributes* parentAttr = getAttributes(objectType, xml->getAttributeValue(L"extends"));

				objectAttr->read(xml, true, xml->getNodeName());

				if( parentAttr )
					copyAttributes( objectAttr, parentAttr, false );

				/// add attributes
				if( attrMap.find(objectId) )
				{
					IAttributes* old = attrMap[objectId];
					old->drop();
				}

				attrMap[objectId] = objectAttr;

				break;
			}
		}
	}	

	xml->drop();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool CGameConfig::importFile(const c8 * fileName)
{
	IFileSystem * fs = __IRR_FILE_SYSTEM();
	IXMLReader * xml = fs->createXMLReader(fileName);
	if (!xml)
		return false;

	while (xml->read())
	{
		switch(xml->getNodeType())
		{
			case EXN_ELEMENT:
			{
				stringw objectType = xml->getNodeName();
				stringw objectId = xml->getAttributeValue(L"id");
				
				clear( objectType, objectId );

				IAttributes* objectAttr = fs->createEmptyAttributes();
				IAttributes* parentAttr = getAttributes(objectType, xml->getAttributeValue(L"extends"));

				objectAttr->read(xml, true, xml->getNodeName());

				if (parentAttr)
				{
					copyAttributes(objectAttr, parentAttr, false);
				}

				setAttributes(objectType, objectId, objectAttr);
				break;
			}
		}
	}	

	xml->drop();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void CGameConfig::copyAttributes(IAttributes * dest, IAttributes * src, bool overwrite)
{
	for (u32 i = 0; i < src->getAttributeCount(); i++)
	{
		const c8 * name = src->getAttributeName(i);

		if (!overwrite && dest->existsAttribute(name))
		{
			continue;
		}

		E_ATTRIBUTE_TYPE type = src->getAttributeType(i);
		
		switch (type)
		{
		case EAT_INT:			dest->setAttribute(name, src->getAttributeAsInt(i));		break;
		case EAT_FLOAT:			dest->setAttribute(name, src->getAttributeAsFloat(i));		break;
		case EAT_STRING:		dest->setAttribute(name, src->getAttributeAsString(i).c_str());	break;
		case EAT_BOOL:			dest->setAttribute(name, src->getAttributeAsBool(i));		break;
		case EAT_ENUM:			dest->setAttribute(name, src->getAttributeAsEnumeration(i));break;
		case EAT_COLOR:			dest->setAttribute(name, src->getAttributeAsColor(i));		break;
		case EAT_COLORF:		dest->setAttribute(name, src->getAttributeAsColorf(i));		break;
		case EAT_VECTOR3D:		dest->setAttribute(name, src->getAttributeAsVector3d(i));	break;
		case EAT_POSITION2D:	dest->setAttribute(name, src->getAttributeAsPosition2d(i));	break;
		//case EAT_VECTOR2D:
		case EAT_RECT:			dest->setAttribute(name, src->getAttributeAsRect(i));		break;
		case EAT_MATRIX:		dest->setAttribute(name, src->getAttributeAsMatrix(i));		break;
		case EAT_QUATERNION:	dest->setAttribute(name, src->getAttributeAsQuaternion(i));	break;
		case EAT_BBOX:			dest->setAttribute(name, src->getAttributeAsBox3d(i));		break;
		case EAT_PLANE:			dest->setAttribute(name, src->getAttributeAsPlane3d(i));	break;
		case EAT_TRIANGLE3D:	dest->setAttribute(name, src->getAttributeAsTriangle3d(i));	break;
		case EAT_LINE2D:		dest->setAttribute(name, src->getAttributeAsLine2d(i));		break;
		case EAT_LINE3D:		dest->setAttribute(name, src->getAttributeAsLine3d(i));		break;
		//case EAT_STRINGWARRAY:
		//case EAT_FLOATARRAY:
		//case EAT_INTARRAY:
		//case EAT_BINARY:
		//case EAT_TEXTURE:
		//case EAT_USER_POINTER:
		default: 
			GX_ASSERT(!"Attribute type not supported");
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void CGameConfig::clear()
{
	for (TObjectAttrMap::Iterator itType = m_objectAttributes.getIterator(); !itType.atEnd(); itType++)
	{
		stringw type = itType->getKey();
		TAttributesMap * map = itType->getValue();

		for (TAttributesMap::Iterator itId = map->getIterator(); !itId.atEnd(); itId++)
		{
			itId->getValue()->drop();
		}

		MM_DELETE( map );
	}

	m_objectAttributes.clear();
}

void CGameConfig::clear( const stringw& type )
{
	for( TObjectAttrMap::Iterator itType = m_objectAttributes.getIterator(); !itType.atEnd(); )
	{
		stringw keyType = itType->getKey();
		TAttributesMap* attrMap = itType->getValue();

		if( keyType == type )
		{
			for( TAttributesMap::Iterator itId = attrMap->getIterator(); !itId.atEnd(); itId++ )
			{
				itId->getValue()->drop();
			}

			itType++;

			SAFE_DEL( attrMap );

			m_objectAttributes.remove( keyType );
		}
		else
		{
			itType++;
		}
	}
}

void CGameConfig::clear( const stringw& type, const stringw & id )
{
	for( TObjectAttrMap::Iterator itType = m_objectAttributes.getIterator(); !itType.atEnd(); )
	{
		stringw keyType = itType->getKey();
		TAttributesMap* attrMap = itType->getValue();

		if( keyType == type )
		{
			for( TAttributesMap::Iterator itId = attrMap->getIterator(); !itId.atEnd();  )
			{
				stringw attrKey = itId->getKey();
				IAttributes* attr = itId->getValue();

				itId++;

				if( attrKey == id )
				{
					attr->drop();
					attrMap->remove( attrKey );
				}
			}
			
			itType++;

			if( attrMap->isEmpty() )
			{
				m_objectAttributes.remove( keyType );
				SAFE_DEL( attrMap );
			}
		}
		else
		{
			 itType++;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

IAttributes* CGameConfig::getAttributes(const stringw & type, const stringw & id)
{
	if (!m_objectAttributes.find(type))
		return NULL;

	TAttributesMap * map = m_objectAttributes[type];

	if (!map->find(id))
		return NULL;

	return (*map)[id];
}

TObjectAttrMap& CGameConfig::getObjectAttrMap ()
{
	return m_objectAttributes;
}

TAttributesMap& CGameConfig::getAttributesMap (const stringw& type)
{
	TAttributesMap* map = m_objectAttributes[type];
	return *map;
}

////////////////////////////////////////////////////////////////////////////////

void CGameConfig::setAttributes(const stringw & type, const stringw & id, IAttributes * attributes)
{
	if (!m_objectAttributes.find(type))
	{
		m_objectAttributes[type] = new TAttributesMap();
	}

	TAttributesMap * map = m_objectAttributes[type];

	if (map->find(id))
	{
		IAttributes * old = (*map)[id];
		old->drop();
	}

	(*map)[id] = attributes;
}

////////////////////////////////////////////////////////////////////////////////

void CGameConfig::printAttributes(const stringw & type, const stringw & id)
{
	IAttributes * attr = getAttributes(type, id);
	if (attr)
	{
		wprintf(L"<%s id=\"%s\">\n", type.c_str(), id.c_str());
		printAttributes(attr);
		wprintf(L"</%s>\n", type.c_str());
	}
}

void CGameConfig::printAttributes(IAttributes * attributes)
{
	for (u32 i = 0; i < attributes->getAttributeCount(); i++)
	{
		const wchar_t * type = attributes->getAttributeTypeString(i);
		const c8 * name = attributes->getAttributeName(i);

		wchar_t wideName[32];
		mbstowcs(wideName, name, 32);

		wprintf(L"\t%s %s\n", type, wideName);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
