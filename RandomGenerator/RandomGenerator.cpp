#include "RandomGenerator.h"

#include "../UnitClasses/Unit.h"
#include "../UnitClasses/EarthSoldier.h"
#include "../UnitClasses/EarthGunnery.h"
#include "../UnitClasses/EarthTank.h"
#include "../UnitClasses/HealUnit.h"
#include "../UnitClasses/AlienSoldier.h"
#include "../UnitClasses/AlienMonster.h"
#include "../UnitClasses/AlienDrone.h"
#include "../Game.h"

RandomGenerator::RandomGenerator(Game* gamePtr): gamePtr(gamePtr)
{
	// Initialize the parameters with default values
	setN(0);
	setProb(0);
	setEarthParameters(0, 0, 0, 0, { 0, 0 }, { 0, 0 }, { 0, 0 });
	setAlienParameters(0, 0, 0, { 0, 0 }, { 0, 0 }, { 0, 0 });
}

void RandomGenerator::generateUnits() const
{
	for (int i = 0; i < 2; i++)
	{
		ArmyType armyType = (i == 0) ? ArmyType::EARTH : ArmyType::ALIEN;

		int A = getRandomNumber(1, 100);

		if (A <= prob) // If the probability is satisfied, generate the units
		{
			Unit* newUnit = nullptr;

			// Generate N units
			for (int i = 0; i < N; i++)
			{
				newUnit = generateUnit(armyType);

				if (newUnit) // Unit may be nullptr if the max number of units is reached
					gamePtr->addUnit(newUnit); // Add the unit to the suitable army & list based on the unit type
			}
		}
	}
}

Unit* RandomGenerator::generateUnit(ArmyType armyType) const
{
	Unit* newUnit = nullptr;

	int B = getRandomNumber(1, 100);

	if (armyType == ArmyType::EARTH)
	{
		// Check if the max number of earth units is reached
		if (Unit::cantCreateEarthUnit())
			return nullptr;

		int power = getRandomNumber(earthPowerRange.min, earthPowerRange.max);
		int health = getRandomNumber(earthHealthRange.min, earthHealthRange.max);
		int attackCapacity = getRandomNumber(earthAttackCapacityRange.min, earthAttackCapacityRange.max);

		if (B <= ESPercentage)
			newUnit = new EarthSoldier(gamePtr, health, power, attackCapacity);
		else if (B <= (ESPercentage + ETPercentage))
			newUnit = new EarthTank(gamePtr, health, power, attackCapacity);
		else if (B <= (ESPercentage + ETPercentage + EGPercentage))
			newUnit = new EarthGunnery(gamePtr, health, power, attackCapacity);
		else
			newUnit = new HealUnit(gamePtr, health, power, attackCapacity);
	}
	else
	{
		// Check if the max number of alien units is reached
		if (Unit::cantCreateAlienUnit())
			return nullptr;

		int power = getRandomNumber(alienPowerRange.min, alienPowerRange.max);
		int health = getRandomNumber(alienHealthRange.min, alienHealthRange.max);
		int attackCapacity = getRandomNumber(alienAttackCapacityRange.min, alienAttackCapacityRange.max);

		if (B <= ASPercentage)
			newUnit = new AlienSoldier(gamePtr, health, power, attackCapacity);
		else if (B <= (ASPercentage + AMPercentage))
			newUnit = new AlienMonster(gamePtr, health, power, attackCapacity);
		else
			newUnit = new AlienDrone(gamePtr, health, power, attackCapacity);
	}

	return newUnit;
}

int RandomGenerator::getRandomNumber(int min, int max) const
{
	return min + rand() % (max - min + 1);
}

void RandomGenerator::setN(int N)
{
	this->N = N;
}

void RandomGenerator::setProb(int prob)
{
	this->prob = prob;
}

void RandomGenerator::setEarthParameters(int ESPercentage, int ETPercentage, int EGPercentage, int EHPercentage, Range earthPowerRange, Range earthHealthRange, Range earthAttackCapacityRange)
{
	this->ESPercentage = ESPercentage;
	this->ETPercentage = ETPercentage;
	this->EGPercentage = EGPercentage;
	this->EHPercentage = EHPercentage;

	this->earthPowerRange = earthPowerRange;
	this->earthHealthRange = earthHealthRange;
	this->earthAttackCapacityRange = earthAttackCapacityRange;
}

void RandomGenerator::setAlienParameters(int ASPercentage, int AMPercentage, int ADPercentage, Range alienPowerRange, Range alienHealthRange, Range alienAttackCapacityRange)
{
	this->ASPercentage = ASPercentage;
	this->AMPercentage = AMPercentage;
	this->ADPercentage = ADPercentage;

	this->alienPowerRange = alienPowerRange;
	this->alienHealthRange = alienHealthRange;
	this->alienAttackCapacityRange = alienAttackCapacityRange;
}

