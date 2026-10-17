#pragma once

#include "Game/Objects/UnitParts/UnitKind.h"

class CCombo
{
public:
	explicit CCombo();
	virtual ~CCombo();

	// returns bonus to add
	int checkCombo(int damageSource, UnitKind kind);
	int checkLevelEnd (bool maxHealth);
	int checkBoarRampage (int* unitsHitByKind);

private:
	int		m_RoadCastle;
};
