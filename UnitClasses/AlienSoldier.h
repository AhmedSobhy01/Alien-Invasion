#ifndef ALIEN_SOLDIER_H
#define ALIEN_SOLDIER_H

#include "Unit.h"
#include "../DEFS.h"

class AlienSoldier : public Unit
{
public:
	AlienSoldier(UnitType, int, int, int);
	void print();
	void attack(Unit*);
};
#endif
