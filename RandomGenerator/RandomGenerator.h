#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <string>

#include "../DEFS.h"
#include "../UnitClasses/Unit.h"

class Game;

class RandomGenerator
{
private:
	Game* gamePtr;

	int N; // Number of units to generate
	int ESPercentage; // Earth Soldier percentage
	int ETPercentage; // Earth Tank percentage
	int EGPercentage; // Earth Gunner percentage
	int ASPercentage; // Alien Soldier percentage
	int AMPercentage; // Alien Monster percentage
	int ADPercentage; // Alien Drone percentage
	int prob; // Probability of generating the units
	Range earthPowerRange; // Earth unit power range
	Range earthHealthRange; // Earth unit health range
	Range earthAttackCapacityRange; // Earth unit attack capacity range
	Range alienPowerRange; // Alien unit power range
	Range alienHealthRange; // Alien unit health range
	Range alienAttackCapacityRange; // Alien unit attack capacity range

private:
	Unit* generateUnit(ArmyType) const; // Generate a unit of the given army type

public:
	RandomGenerator(Game*);
	void generateUnits() const; // Generate units for the both armies
	int getRandomNumber(int, int) const; // Generate a random number between the given range

	// Setters
	void setN(int);
	void setProb(int);

	void setEarthParameters(int, int, int, Range, Range, Range); // Sets earth parameters loaded by game class
	void setAlienParameters(int, int, int, Range, Range, Range); // Sets alien parameters loaded by game class
};

#endif