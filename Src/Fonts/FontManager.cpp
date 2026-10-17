////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Fonts/FontManager.h"
#include "Fonts/Font.h"

#include "Game/Config/GameConfig.h"

#include "Common/StringTable.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

CFontManager::CFontManager() :
	m_Initialized( false )
{
}

CFontManager::~CFontManager()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CFontManager::ctor()
{
	m_Initialized = CGameConfig::GetInstance()->importFile("config/fonts.xml");
}

void CFontManager::dtor()
{
	for (TFontMap::Iterator iter = m_fontMap.getIterator(); !iter.atEnd(); iter++)
	{
		CFont* font = iter->getValue();
		SAFE_DEL( font );
	}

	m_fontMap.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CFont* CFontManager::GetFont(const stringw& name)
{
	if( !m_Initialized )
	{
		ctor();
	}
	
	stringw fontName = name;

	IAttributes * attr = CGameConfig::GetInstance()->getAttributes(L"font", name);

	if(__MGR_STRINGS()->GetLanguage() == Language_Japanese)
	{
		if (attr->existsAttribute("JapaneseEquivalent"))
		{
			fontName = stringw( attr->getAttributeAsString("JapaneseEquivalent").c_str() );
		}
	}

/*	if(__MGR_STRINGS()->GetLanguage() == Language_SimplifiedChinese)
	{
		if (attr->existsAttribute("ChineseEquivalent"))
		{
			fontName = stringw( attr->getAttributeAsString("ChineseEquivalent").c_str() );
		}
	}
*/
	/// Get node
	TFontMap::Node * node = m_fontMap.find(fontName);

	if (node)
	{
		return node->getValue();
	}

	/// Create new font
	CFont * font = NEW CFont();
	font->Load(fontName);
	m_fontMap[fontName] = font;

	return font;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
