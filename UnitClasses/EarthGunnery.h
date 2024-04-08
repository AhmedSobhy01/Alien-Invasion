#ifndef EARTH_GUNNERY_H
#define EARTH_GUNNERY_H

#include "Unit.h"
#include "../DEFS.h"

class EarthGunnery: public Unit
{
public:
	EarthGunnery(Game*, int, int, int);
	void print() const;
	void attack(Unit*);

	int getPriority() const;
};
#endif
