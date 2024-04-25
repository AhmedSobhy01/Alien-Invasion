#ifndef ALIEN_MONSTER_H
#define ALIEN_MONSTER_H

#include "Unit.h"
#include "../DEFS.h"

class AlienMonster: public Unit
{
public:
	AlienMonster(Game*, int, int, int);
	void printFought();
	bool attack();
};

#endif