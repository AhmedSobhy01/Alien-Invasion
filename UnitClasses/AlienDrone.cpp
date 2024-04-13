#include "AlienDrone.h"
#include "../Game.h"

AlienDrone::AlienDrone(Game* gamePtr, int health, int power, int attackCapacity)
    : Unit(gamePtr, UnitType::AD, health, power, attackCapacity)
{}

void AlienDrone::print() const
{
    std::cout << "AD " << this->getId() << " shots [";
    foughtUnits.printList();
    std::cout << "]\n";
}

void AlienDrone::attack()
{
    int ETnumber = (attackCapacity / 2) + 1;
    int EGnumber = attackCapacity - ETnumber;

    LinkedQueue<Unit*> ETlist = gamePtr->getEnemyList(ArmyType::EARTH, UnitType::ET, ETnumber);
    LinkedQueue<Unit*> EGlist = gamePtr->getEnemyList(ArmyType::EARTH, UnitType::EG, EGnumber);

    ArrayStack<Unit*> ETtempList;
    LinkedQueue<Unit*> EGtempList;

    for (int i = 0; i < attackCapacity; i++)
    {
        Unit* attackedUnit = nullptr;

        if (i % 2 == 0)
            ETlist.dequeue(attackedUnit);
        else
            EGlist.dequeue(attackedUnit);

        if (!attackedUnit)
            continue;

        // Check if it was attacked before or not
        if (attackedUnit->getFirstAttackTime() == -1)
        {
            attackedUnit->setFirstTimeAttack(gamePtr->getCurrentTimestep());
            attackedUnit->setFirstAttackDelay();
        }
            
        // Decrement health 
        attackedUnit->recieveDamage(calcUAP(attackedUnit));

        // Check if it was killed or not
        if (attackedUnit->getHealth())
        {
            UnitType unitType = attackedUnit->getUnitType();

            switch (unitType)
            {
                case UnitType::ET:
                    ETtempList.push(attackedUnit);
                    break;

                case UnitType::EG:
                    EGtempList.enqueue(attackedUnit);
                    break;
            }
        }
        else
        {
            gamePtr->killUnit(attackedUnit);
            attackedUnit->setDestructionTime(gamePtr->getCurrentTimestep());
            attackedUnit->setDestructionDelay();
            attackedUnit->setBattleDelay();
        }

        foughtUnits.enqueue(attackedUnit->getId());
    }

    // Re-adding attacked units to their original lists

    Unit* unit = nullptr;
    while (ETtempList.pop(unit))
        gamePtr->addUnit(unit);

    while (EGtempList.dequeue(unit))
        gamePtr->addUnit(unit);
}