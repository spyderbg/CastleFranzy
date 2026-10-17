///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Game/Config/Upgrades.h"

///////////////////////////////////////////////////////////////////////

class CLevelStats
{
public:
	CLevelStats();
	~CLevelStats();

	void		reset();

public:
	int			Gold;
	int			Souls;
	int			Kills;

	int			WeaponLevel[WT_Count];
	bool		WeaponAvailable[WT_Count];
	int			SpellAvailable;
};

class CDayStats
{
public:
	CDayStats();
	~CDayStats();

	void		reset();

public:
	int			Gold;
	int			Souls;
	int			Kills;

	s32			killsGoblins;
	s32			killsWarlocks;
	s32			killsBoars;
	s32			killsTrolls;
	s32			killsElfs;
};