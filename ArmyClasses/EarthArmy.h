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

    // Counts needed for the outputfile statistics
    
    // Initial units count
    int EScount;
    int ETcount;
    int EGcount;

    int deadEScount;
    int deadETcount;
    int deadEGcount;

public:
    EarthArmy();
    void addUnit(Unit*);
    Unit* removeUnit(UnitType);
    void printArmy() const;
    void attack();

    // Getters
    int getUnitsCount(UnitType) const;
    int getDeadUnitsCount(UnitType) const;

    ~EarthArmy();
};

#endif
