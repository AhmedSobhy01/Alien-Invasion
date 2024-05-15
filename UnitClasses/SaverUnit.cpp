#include "SaverUnit.h"
#include "../Game.h"
#include <iostream>

SaverUnit::SaverUnit(Game* gamePtr, double health, int power, int attackCapacity)
    : Unit(gamePtr, UnitType::SU, health, power, attackCapacity)
{}

void SaverUnit::printFought()
{
    if (!foughtUnits.isEmpty())
    {
        std::cout << "SU " << getId() << " shots [";
        foughtUnits.printList();
        std::cout << "]" << std::endl;

        clearFoughtUnits(); // Clear the list after printing
    }
}

bool SaverUnit::attack()
{
    // Get the lists of alien soldiers to attack
    LinkedQueue<Unit*> enemyList = gamePtr->getEnemyList(ArmyType::ALIEN, UnitType::AS, attackCapacity);

    // Check for a successful attack
    bool attackCheck = false;

    // Create a pointer to the enemy unit
    Unit* enemyUnit = nullptr;

    while (enemyList.dequeue(enemyUnit))
    {
        gamePtr->log("Earth Allied " + getUnitTypeString() + " " + toString() + " is attacking Alien " + enemyUnit->getUnitTypeString() + " " + enemyUnit->toString() + " with UAP " + std::to_string(calcUAP(enemyUnit)));

        // Calculate the UAP and apply the damage
        enemyUnit->receiveDamage(calcUAP(enemyUnit));

        // Check if the unit is dead or can join the battle
        if (enemyUnit->isDead())
            gamePtr->addToKilledList(enemyUnit);
        else
            gamePtr->addUnit(enemyUnit);

        // Store the IDs of the fought units to be printed later
        foughtUnits.enqueue(enemyUnit->getId());

        gamePtr->log("Earth Allied " + getUnitTypeString() + " " + toString() + " attacked Alien " + enemyUnit->getUnitTypeString() + " " + enemyUnit->toString());

        // Nullify the pointer to avoid duplication
        enemyUnit = nullptr;

        // If this line is reached, at least one unit was attacked
        attackCheck = true;
    }
    return attackCheck;
}
