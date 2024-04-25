#ifndef HEAL_UNIT_H
#define HEAL_UNIT_H

#include "Unit.h"
#include "../DEFS.h"

class HealUnit: public Unit
{
public:
	HealUnit(Game*, double, int, int);
	bool attack(); // Heal units from the unit maintenance list
};

#endif