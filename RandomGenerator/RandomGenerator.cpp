#include "RandomGenerator.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include "../UnitClasses/Unit.h"
#include "../UnitClasses/EarthSoldier.h"
#include "../UnitClasses/EarthGunnery.h"
#include "../UnitClasses/EarthTank.h"
#include "../UnitClasses/AlienSoldier.h"
#include "../UnitClasses/AlienMonster.h"
#include "../UnitClasses/AlienDrone.h"

RandomGenerator::RandomGenerator(std::string wholeFile)
{
	loadParameters(wholeFile);
}

void RandomGenerator::loadParameters(std::string wholeFile)
{
	std::stringstream ss(wholeFile);
	ss >> N >> ESPercentage >> ETPercentage >> EGPercentage >> ASPercentage >> AMPercentage >> ADPercentage >> prob;

	char dummyHyphen;

	ss >> earthPowerRange.min >> dummyHyphen >> earthPowerRange.max; 
	ss >> earthHealthRange.min >> dummyHyphen >> earthHealthRange.max;
	ss >> earthAttackCapacityRange.min >> dummyHyphen >> earthAttackCapacityRange.max;

	ss >> alienPowerRange.min >> dummyHyphen >> alienPowerRange.max;
	ss >> alienHealthRange.min >> dummyHyphen >> alienHealthRange.max;
	ss >> alienAttackCapacityRange.min >> dummyHyphen >> alienAttackCapacityRange.max;
}

Unit* RandomGenerator::generateUnit(ArmyType armyType) const
{
	Unit* newUnit = nullptr;
	int A = getRandomNumber(1, 100);

	if (A <= prob)
	{
		int B = getRandomNumber(1, 100);
			
		if (armyType == ArmyType::EARTH) 
		{
			int power = getRandomNumber(earthPowerRange.min, earthPowerRange.max);
			int health = getRandomNumber(earthHealthRange.min, earthHealthRange.max);
			int attackCapacity = getRandomNumber(earthAttackCapacityRange.min, earthAttackCapacityRange.max);
			
			if (B <= ESPercentage)
				newUnit = new EarthSoldier(UnitType::ES, health, power, attackCapacity);
			else if(N <= (ESPercentage + ETPercentage))
				newUnit = new EarthTank(UnitType::ET, health, power, attackCapacity);
			else
				newUnit = new EarthGunnery(UnitType::EG, health, power, attackCapacity);
		}
		else 
		{
			int power = getRandomNumber(alienPowerRange.min, alienPowerRange.max);
			int health = getRandomNumber(alienHealthRange.min, alienHealthRange.max);
			int attackCapacity = getRandomNumber(alienAttackCapacityRange.min, alienAttackCapacityRange.max);

			if (B <= ASPercentage)
				newUnit = new AlienSoldier(UnitType::AS, health, power, attackCapacity);
			else if (B <= (ASPercentage + AMPercentage))
				newUnit = new AlienMonster(UnitType::AM, health, power, attackCapacity);
			else
				newUnit = new AlienDrone(UnitType::AD, health, power, attackCapacity);
		}
		
	}

	return newUnit;
}

int RandomGenerator::getRandomNumber(int min, int max) const
{
	return min + rand() % (max - min + 1);
}