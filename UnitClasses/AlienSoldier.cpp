#include "AlienSoldier.h"
#include "../Game.h"

AlienSoldier::AlienSoldier(Game* gamePtr, int health, int power, int attackCapacity)
    : Unit(gamePtr, UnitType::AS, health, power, attackCapacity)
{}

void AlienSoldier::print() const
{
    std::cout << "AD " << this->getId() << " shots [";
    foughtUnits.printList();
    std::cout << "]\n";
}

void AlienSoldier::attack()
{
    LinkedQueue<Unit*> enemyList = gamePtr->getEnemyList(ArmyType::EARTH, UnitType::ES, attackCapacity);
    LinkedQueue<Unit*> tempList;
    Unit* enemyUnit = nullptr;

    while (!enemyList.isEmpty())
    {
        // Get the unit and remove it from the list
        enemyList.dequeue(enemyUnit);

        // Set the first attack time if it's the first time attacking
        if (enemyUnit->getFirstAttackTime() == -1)
        {
            enemyUnit->setFirstTimeAttack(gamePtr->getCurrentTimestep());
            enemyUnit->setFirstAttackDelay();
        }

        // Receive damage and check whether it's dead or not
        enemyUnit->recieveDamage(calcUAP(enemyUnit));
        if (enemyUnit->getHealth() == 0)
        {
            gamePtr->killUnit(enemyUnit);
            enemyUnit->setDestructionTime(gamePtr->getCurrentTimestep());
            enemyUnit->setDestructionDelay();
            enemyUnit->setBattleDelay();
        }
        else
            tempList.enqueue(enemyUnit);

        // Store the IDs of the fought units to be printed later
        foughtUnits.enqueue(enemyUnit->getId());
    }

    // Empty the tempList and re-add the alive units back to their lists
    while (!tempList.isEmpty())
    {
        Unit* tempUnitPtr = nullptr;
        tempList.dequeue(tempUnitPtr);
        gamePtr->addUnit(tempUnitPtr);
    }
}