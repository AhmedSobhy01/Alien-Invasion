#ifndef EARTH_ARMY_H
#define EARTH_ARMY_H

#include "Army.h"
#include "..\Containers\ArrayStack.h"
#include "..\Containers\LinkedQueue.h"
#include "..\Containers\PriorityQueue.h"
#include "..\UnitClasses\Unit.h"
#include "..\UnitClasses\EarthSoldier.h"
#include "..\UnitClasses\EarthTank.h"
#include "..\UnitClasses\EarthGunnery.h"

class EarthArmy: public Army
{
private:
    LinkedQueue<Unit*> soldiers;
    ArrayStack<Unit*> tanks;
    PriorityQueue<Unit*> gunneries;

public:
    void addUnit(Unit*);
    Unit* removeUnit(UnitType);
    Unit* pickAttacker(UnitType);
    void printArmy() const;
    void attack();
    bool isDead();

    // Getters
    int getUnitsCount(UnitType) const;

    ~EarthArmy();
};

#endif
