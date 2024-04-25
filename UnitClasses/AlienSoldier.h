#ifndef ALIEN_SOLDIER_H
#define ALIEN_SOLDIER_H

#include "Unit.h"
#include "../DEFS.h"

class AlienSoldier: public Unit
{
public:
	AlienSoldier(Game*, int, int, int);
	void printFought();
	bool attack();
};

#endif