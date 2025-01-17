#include "AlienSoldier.h"
#include "../Game.h"

AlienSoldier::AlienSoldier(Game* gamePtr, double health, int power, int attackCapacity)
    : Unit(gamePtr, UnitType::AS, health, power, attackCapacity)
{}

void AlienSoldier::printUnit()
{
    std::cout << "AS " << id;
}

bool AlienSoldier::attack()
{
    // Calculate the number of earth soldiers and saver units to attack
    int soldiersCapacity = attackCapacity / 2;
    int saversCapacity = attackCapacity - soldiersCapacity;

    // Get the lists of earth soldiers and saver units to attack
    LinkedQueue<Unit*> soldiersList = gamePtr->getEnemyList(ArmyType::EARTH, UnitType::ES, soldiersCapacity);
    LinkedQueue<Unit*> saversList = gamePtr->getEnemyList(ArmyType::EARTH_ALLIED, UnitType::SU, saversCapacity);

    std::string foughtUnits = "";

    // Check for a successful attack
    bool attackCheck = false;

    // Create a pointer to the enemy unit
    Unit* enemyUnit = nullptr;

    while (soldiersList.dequeue(enemyUnit) || saversList.dequeue(enemyUnit))
    {
        // Calculate the UAP and apply the damage
        enemyUnit->receiveDamage(calcUAP(enemyUnit));

        // Check if the unit is dead, needs healing or can join the battle
        if (enemyUnit->isDead())
            gamePtr->addToKilledList(enemyUnit);
        else if (enemyUnit->needsHeal())
            gamePtr->addUnitToMaintenanceList(dynamic_cast<HealableUnit*>(enemyUnit));
        else
            gamePtr->addUnit(enemyUnit);

        // Store the IDs of the fought units to be printed later
        if (foughtUnits != "")
            foughtUnits += ", ";
        foughtUnits += std::to_string(enemyUnit->getId());

        // Nullify the pointer
        enemyUnit = nullptr;

        // If this line is reached, at least one unit was attacked
        attackCheck = true;
    }

    if (foughtUnits != "")
        gamePtr->registerAttack(this, "shots", foughtUnits);

    return attackCheck;
}