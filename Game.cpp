#include <iostream>
#include <iomanip>
#include <fstream>

#include "Game.h"
#include "DEFS.h"
#include "UnitClasses/Unit.h"
#include "UnitClasses/AlienMonster.h"
#include "logger.h"

// Global helper functions
float calculateRatio(int numerator, int denominator) // Helper function to calculate the ratio of two numbers
{
	if (denominator == 0)
		return 0;

	return (float) numerator / denominator;
}

float calculatePercentage(int numerator, int denominator) // Helper function to calculate the percentage of two numbers
{
	return calculateRatio(numerator, denominator) * 100;
}


Game::Game(): gameMode(GameMode::INTERACTIVE), currentTimestep(0), earthArmy(this), alienArmy(this), earthAlliedArmy(this), randomGenerator(this), logger(this, "log.txt")
{}

void Game::run(GameMode gameMode, std::string inputFileName, std::string outputFileName)
{
	// Change the game mode 
	setGameMode(gameMode);
	logger.log("Game mode changed to " + gameModeToString(gameMode));

	// Load the parameters from the file and set the parameters in the random generator
	if (!loadParameters(inputFileName)) // If the file is not found, print an error message and return
	{
		std::cout << "Error: File not found!" << std::endl;
		return;
	}
	logger.log("Parameters loaded from file " + inputFileName);

	// Run the game
	bool didArmiesAttack = true;
	do
	{
		// Increment Timestep
		currentTimestep++;
		logBeginOfTimeStep();

		// Generate units for both armies
		randomGenerator.generateUnits();

		// Start fight
		didArmiesAttack = startAttack();

		// Spread infection in the Earth Army
		earthArmy.spreadInfection();

		// Print the output
		if (gameMode == GameMode::INTERACTIVE)
		{
			printAll();
			std::cout << "Press Enter to continue..." << std::endl;
			while (std::cin.get() != '\n');
		}
		logEndOfTimeStep();
	} while (!battleOver(didArmiesAttack));

	// Produce the output file
	generateOutputFile(outputFileName);

	// Print the final results
	printFinalResults();
}

std::string Game::gameModeToString(GameMode)
{
	switch (gameMode)
	{
		case GameMode::INTERACTIVE:
			return "Interactive";
		case GameMode::SILENT:
			return "Silent";
	}

	return "Unknown";
}

bool Game::startAttack()
{
	// Make both armies attack
	logger.log("Earth Army attacking...");
	bool didEarthArmyAttack = earthArmy.attack();

	logger.log("Alien Army attacking...");
	bool didAlienArmyAttack = alienArmy.attack();

	logger.log("Allied Earth Army attacking...");
	bool didEarthAlliedAttack = earthAlliedArmy.attack();

	// Return if any of the armies successfully attacked
	return didEarthArmyAttack || didAlienArmyAttack || didEarthAlliedAttack;
}

void Game::setGameMode(GameMode gameMode)
{
	this->gameMode = gameMode;
}

bool Game::battleOver(bool didArmiesAttack) const
{
	// Game ending cases
	bool anArmyDied = earthArmy.isDead() || alienArmy.isDead(); // If one army completely killed the other
	bool noAttackTie = !didArmiesAttack; // If both armies weren't able to attack - considered as a tie

	// Don't check for end battle condition unless it has run for at least 40 timesteps
	return currentTimestep >= 40 && (anArmyDied || noAttackTie);
}

bool Game::areUnitsFighting() const
{
	return earthArmy.getFightingUnitsCount() + alienArmy.getFightingUnitsCount();
}

bool Game::doesEarthNeedHelp() const
{
	return earthArmy.needAllyHelp();
}

void Game::killSaverUnits()
{
	Unit* saverToKill = earthAlliedArmy.removeUnit(UnitType::SU); // Remove a saver from its list
	
	while (saverToKill)
	{
		saverToKill->receiveDamage(saverToKill->getHealth()); // Prepare it to be killed
		addToKilledList(saverToKill); // Add it to the killed list
		saverToKill = nullptr;
		saverToKill = earthAlliedArmy.removeUnit(UnitType::SU); // Remove another saver
	}

}

void Game::logStartOfGame()
{
	logger.log("Game started.");
}

void Game::logBeginOfTimeStep()
{
	logger.log("Begin of timestep.");
}

void Game::addToKilledList(Unit* unit)
{
	logger.log("Adding unit to killed list: " + unit->toString());
	killedList.enqueue(unit);
}

void Game::log(std::string message)
{
	logger.log(message);
}

void Game::logEndOfTimeStep()
{
	logger.log("End of timestep.");
}

void Game::logEndOfGame()
{
	logger.log("Game ended.");
}

void Game::printFinalResults() const
{
	std::cout << std::endl;

	// Print an overflow error message
	if (Unit::cantCreateEarthUnit() || Unit::cantCreateAlienUnit())
		std::cout << "Warning: The maximum number of units has been reached!" << std::endl << std::endl;

	// Print battle results
	std::cout << "What a battle!" << std::endl;
	std::cout << "Battle Result: " << battleResult() << std::endl;
	std::cout << "Check the output file for a detailed conclusion" << std::endl;
}

std::string Game::battleResult() const
{
	int totalEarthUnits = earthArmy.getUnitsCount(UnitType::ES) + earthArmy.getUnitsCount(UnitType::ET) + earthArmy.getUnitsCount(UnitType::EG);
	int totalAlienUnits = alienArmy.getUnitsCount(UnitType::AS) + alienArmy.getUnitsCount(UnitType::AM) + alienArmy.getUnitsCount(UnitType::AD);

	if (earthArmy.isDead() && !alienArmy.isDead()) // If the Earth army is dead and the Alien army is not dead, the Alien army wins
		return "Alien Army wins!";
	else if (!earthArmy.isDead() && alienArmy.isDead()) // If the Earth army is dead and the Alien army is not dead, the Earth army wins
		return "Earth Army wins!";
	else // If both armies are dead or no army was able to attack, the result is a draw
		return "Draw!";
}

void Game::addUnit(Unit* unit)
{
	if (unit)
		logger.log("Adding unit: " + unit->toString());
	else
	{
		logger.log("Adding unit: nullptr");
		return;
	}

	ArmyType armyType = unit->getArmyType();

	switch (armyType)
	{
		case ArmyType::EARTH:
			earthArmy.addUnit(unit);
			break;

		case ArmyType::ALIEN:
			alienArmy.addUnit(unit);
			break;

		case ArmyType::EARTH_ALLIED:
			earthAlliedArmy.addUnit(unit);
			break;
	}
}

Unit* Game::removeUnit(ArmyType armyType, UnitType unitType)
{
	switch (armyType)
	{
		case ArmyType::EARTH:
			return earthArmy.removeUnit(unitType);

		case ArmyType::ALIEN:
			return alienArmy.removeUnit(unitType);

		case ArmyType::EARTH_ALLIED:
			return earthAlliedArmy.removeUnit(unitType);
	}

	return nullptr;
}

LinkedQueue<Unit*> Game::getEnemyList(ArmyType armyType, UnitType unitType, int attackCapacity)
{
	LinkedQueue<Unit*> enemyUnits;
	Unit* enemyUnitPtr = nullptr;

	// Loop on the armies' lists depending on the attacker's attack capacity
	// if the required enemy unit is found, enqueue it to be sent
	switch (armyType)
	{
		case ArmyType::EARTH:
			for (int i = 0; i < attackCapacity; i++)
			{
				enemyUnitPtr = earthArmy.removeUnit(unitType);
				if (enemyUnitPtr)
					enemyUnits.enqueue(enemyUnitPtr);
				else
				{
					logger.log(Unit::unitTypeString(unitType) + " to attack: []");
					break;
				}
			}
			break;

		case ArmyType::ALIEN:
			for (int i = 0; i < attackCapacity; i++)
			{
				enemyUnitPtr = alienArmy.removeUnit(unitType);
				if (enemyUnitPtr)
					enemyUnits.enqueue(enemyUnitPtr);
				else
				{
					logger.log(Unit::unitTypeString(unitType) + " to attack: []");
					break;
				}
			}
			break;

		case ArmyType::EARTH_ALLIED:
			for (int i = 0; i < attackCapacity; i++)
			{
				enemyUnitPtr = earthAlliedArmy.removeUnit(unitType);
				if (enemyUnitPtr)
					enemyUnits.enqueue(enemyUnitPtr);
				else
				{
					logger.log(Unit::unitTypeString(unitType) + " to attack: []");
					break;
				}
			}
	}

	logger.log(Unit::unitTypeString(unitType) + " to attack: [");
	for (int i = 0; i < enemyUnits.getCount(); i++)
	{
		Unit* unit = nullptr;
		enemyUnits.dequeue(unit);
		logger.log(unit->toString());
		enemyUnits.enqueue(unit);
	}
	logger.log("]");

	return enemyUnits;
}

void Game::addUnitToMaintenanceList(HealableUnit* unit)
{
	if (unit)
		logger.log("Adding unit to maintenance list: " + unit->toString());
	else
	{
		logger.log("Adding unit to maintenance list: nullptr");
		return;
	}

	// Enqueue the unit with its priority to the maintenance list
	unitMaintenanceList.enqueue(unit, unit->getHealPriority());

	// Set the time when the unit joined the UML
	unit->setUMLjoinTime(currentTimestep);
}

LinkedQueue<HealableUnit*> Game::getUnitsToMaintainList(int attackCapacity)
{
	LinkedQueue<HealableUnit*> unitsToMaintain;

	HealableUnit* unit = nullptr;
	int dummyPri = 0;
	for (int i = 0; i < attackCapacity; i++)
	{
		unit = nullptr;
		if (unitMaintenanceList.dequeue(unit, dummyPri))
			unitsToMaintain.enqueue(unit);
		else
			break;
	}

	logger.log("Units to maintain: " + std::to_string(unitsToMaintain.getCount()));
	logger.log("Units to maintain: [");
	for (int i = 0; i < unitsToMaintain.getCount(); i++)
	{
		HealableUnit* unit = nullptr;
		unitsToMaintain.dequeue(unit);
		logger.log(unit->toString());
		unitsToMaintain.enqueue(unit);
	}
	logger.log("]");

	return unitsToMaintain;
	return unitsToMaintain;
}

void Game::incrementInfectedESCount()
{
	earthArmy.incrementInfectedSoldiersCount();
}

void Game::decrementInfectedESCount()
{
	earthArmy.decrementInfectedSoldiersCount();
}

void Game::printKilledList() const
{
	std::cout << killedList.getCount() << " units [";
	killedList.printList();
	std::cout << "]" << std::endl;
}

void Game::printUnitMaintenanceList() const
{
	std::cout << unitMaintenanceList.getCount() << " units [";
	unitMaintenanceList.printList();
	std::cout << "]" << std::endl;
}

void Game::printAll()
{
	std::cout << std::endl;
	std::cout << "Current Timestep " << currentTimestep << std::endl;

	std::cout << "============== Earth Army Alive Units =========================" << std::endl;
	earthArmy.printArmy();

	std::cout << std::endl << "============== Alien Army Alive Units =========================" << std::endl;
	alienArmy.printArmy();

	std::cout << std::endl << "============== Earth Allied Army Alive Units ===================" << std::endl;
	earthAlliedArmy.printArmy();

	if (areUnitsFighting())
	{
		std::cout << std::endl << "============== Units fighting at current step =================" << std::endl;
		earthArmy.printFightingUnits();
		alienArmy.printFightingUnits();
		earthAlliedArmy.printFightingUnits();
	}
	else
		std::cout << std::endl << "============== No units fighting at current step ==============" << std::endl;

	std::cout << std::endl << "============== Maintenance List Units =========================" << std::endl;
	printUnitMaintenanceList();

	std::cout << std::endl << "============== Killed/Destructed Units ========================" << std::endl;
	printKilledList();
}

GameStatistics Game::countStatistics()
{
	GameStatistics gameStatistics = { 0 };

	// Earth Army Statistics
	const int earthUnitTypesCount = 4;
	UnitType earthUnitTypes[] = { UnitType::ES, UnitType::ET, UnitType::EG, UnitType::EH };

	countArmyStatistics(gameStatistics, ArmyType::EARTH, earthUnitTypes, earthUnitTypesCount);

	// Alien Army Statistics
	const int alienUnitTypesCount = 3;
	UnitType alienUnitTypes[] = { UnitType::AS, UnitType::AM, UnitType::AD };

	countArmyStatistics(gameStatistics, ArmyType::ALIEN, alienUnitTypes, alienUnitTypesCount);

	// Allied Army Statistics
	const int alliedUnitTypesCount = 1;
	UnitType alliedUnitTypes[] = { UnitType::SU };
	countArmyStatistics(gameStatistics, ArmyType::EARTH_ALLIED, alliedUnitTypes, alliedUnitTypesCount);

	// Killed Units Statistics
	countKilledUnitsStatistics(gameStatistics);

	// Unit Maintenance List
	countUnitMaintenanceStatistics(gameStatistics);

	return gameStatistics;
}

void Game::countArmyStatistics(GameStatistics& gameStatistics, ArmyType armyType, UnitType unitTypes[], int unitTypesCount)
{
	// Count the statistics of the given army
	Unit* unit = nullptr;

	for (int i = 0; i < unitTypesCount; i++)
	{
		int count = getUnitsCount(armyType, unitTypes[i]);

		for (int j = 0; j < count; j++)
		{
			// Remove the unit from the army
			unit = removeUnit(armyType, unitTypes[i]);

			// Unit Counts
			gameStatistics.unitCounts[unitTypes[i]]++;
			gameStatistics.totalUnitsCount++;

			// Total Army Units Count
			gameStatistics.armyStatistics[armyType].totalUnitsCount++;

			// Count the infected Earth Soldiers
			if (unitTypes[i] == UnitType::ES)
			{
				EarthSoldier* earthSoldier = dynamic_cast<EarthSoldier*>(unit);
				if (earthSoldier->isInfected() || earthSoldier->isImmune()) // Check if the unit is infected or immune (has been infected before)
					gameStatistics.totalInfectedESCount++;
			}

			// Add the unit back to the army
			addUnit(unit);

			// Nullify the pointer
			unit = nullptr;
		}
	}
}

void Game::countKilledUnitsStatistics(GameStatistics& gameStatistics)
{
	Unit* unit = nullptr;
	int count = killedList.getCount();

	for (int i = 0; i < count; i++)
	{
		// Remove the unit from the killed list
		killedList.dequeue(unit);

		// Get the unit type and army type
		ArmyType armyType = unit->getArmyType();
		UnitType unitType = unit->getUnitType();

		// Unit Counts
		gameStatistics.unitCounts[unitType]++;
		gameStatistics.totalUnitsCount++;

		// Destructed Unit Counts
		gameStatistics.destructedUnitCounts[unitType]++;
		gameStatistics.totalDestructedUnitsCount++;

		// Total Army Units Count
		gameStatistics.armyStatistics[armyType].totalUnitsCount++;
		gameStatistics.armyStatistics[armyType].totalDestructedUnitsCount++;

		// Count the infected Earth Soldiers
		if (unitType == UnitType::ES)
		{
			EarthSoldier* earthSoldier = dynamic_cast<EarthSoldier*>(unit);
			if (earthSoldier->isInfected() || earthSoldier->isImmune()) // Check if the unit is infected or immune (has been infected before)
				gameStatistics.totalInfectedESCount++;
		}

		// Delays
		gameStatistics.armyStatistics[armyType].totalFirstAttackDelays += unit->getFirstAttackDelay();
		gameStatistics.armyStatistics[armyType].totalBattleDelays += unit->getBattleDelay();
		gameStatistics.armyStatistics[armyType].totalDestructionDelays += unit->getDestructionDelay();

		// Add the unit back to the killed list
		killedList.enqueue(unit);

		// Nullify the pointer
		unit = nullptr;
	}
}

void Game::countUnitMaintenanceStatistics(GameStatistics& gameStatistics)
{
	HealableUnit* healableUnit = nullptr;
	int count = unitMaintenanceList.getCount();
	int priority = 0;

	for (int i = 0; i < count; i++)
	{
		// Remove the unit from the maintenance list
		unitMaintenanceList.dequeue(healableUnit, priority);

		// Get the unit type & army type
		ArmyType armyType = healableUnit->getArmyType();
		UnitType unitType = healableUnit->getUnitType();

		// Unit Counts
		gameStatistics.unitCounts[unitType]++;
		gameStatistics.totalUnitsCount++;

		// Total Army Units Count
		gameStatistics.armyStatistics[armyType].totalUnitsCount++;

		// Count the infected Earth Soldiers
		if (unitType == UnitType::ES)
		{
			EarthSoldier* earthSoldier = dynamic_cast<EarthSoldier*>(healableUnit);
			if (earthSoldier->isInfected() || earthSoldier->isImmune()) // Check if the unit is infected or immune (has been infected before)
				gameStatistics.totalInfectedESCount++;
		}

		// Add the unit back to the maintenance list
		unitMaintenanceList.enqueue(healableUnit, priority);

		// Nullify the pointer
		healableUnit = nullptr;
	}
}

void Game::generateOutputFile(std::string outputFileName)
{
	// Open the output file
	std::ofstream fout(outputFileName);

	// Count the statistics
	GameStatistics gameStatistics = countStatistics();

	// Print decorated the battle results
	fout << "======================================================================" << std::endl;
	fout << std::right << std::setw(40);
	fout << "Battle Results" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Battle Result: " << battleResult() << std::endl;
	fout << "Total Timesteps: " << currentTimestep << std::endl;

	// Print the killed units
	fout << std::endl;
	fout << "======================================================================" << std::endl;
	fout << std::right << std::setw(40);
	fout << "Killed Units" << std::endl;
	fout << "======================================================================" << std::endl;

	// Print the header
	fout << std::left;
	fout << std::setw(12) << "Td";
	fout << std::setw(12) << "ID";
	fout << std::setw(12) << "Tj";
	fout << std::setw(12) << "Df";
	fout << std::setw(12) << "Dd";
	fout << std::setw(12) << "Db" << std::endl;

	// Print the units
	Unit* killedUnit = nullptr;
	for (int i = 0; i < gameStatistics.totalDestructedUnitsCount; i++)
	{
		killedList.dequeue(killedUnit);

		fout << std::setw(12) << killedUnit->getDestructionTime();
		fout << std::setw(12) << killedUnit->getId();
		fout << std::setw(12) << killedUnit->getJoinTime();
		fout << std::setw(12) << killedUnit->getFirstAttackDelay();
		fout << std::setw(12) << killedUnit->getDestructionDelay();
		fout << std::setw(12) << killedUnit->getBattleDelay() << std::endl;

		killedList.enqueue(killedUnit);
	}

	// Earth Army Statistics
	fout << std::endl;
	fout << "======================================================================" << std::endl;
	fout << std::right << std::setw(45);
	fout << "Earth Army Statistics" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Total ES Count: " << gameStatistics.unitCounts[UnitType::ES] << std::endl;
	fout << "Total ET Count: " << gameStatistics.unitCounts[UnitType::ET] << std::endl;
	fout << "Total EG Count: " << gameStatistics.unitCounts[UnitType::EG] << std::endl;
	fout << "Total EH Count: " << gameStatistics.unitCounts[UnitType::EH] << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Destructed ESs/Total ESs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::ES], gameStatistics.unitCounts[UnitType::ES]) << "%" << std::endl;
	fout << "Destructed ETs/Total ETs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::ET], gameStatistics.unitCounts[UnitType::ET]) << "%" << std::endl;
	fout << "Destructed EGs/Total EGs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::EG], gameStatistics.unitCounts[UnitType::EG]) << "%" << std::endl;
	fout << "Destructed EHs/Total EHs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::EH], gameStatistics.unitCounts[UnitType::EH]) << "%" << std::endl;
	fout << "Total Destructed Earth Units/Total Earth Units = " << calculatePercentage(gameStatistics.armyStatistics[ArmyType::EARTH].totalDestructedUnitsCount, gameStatistics.armyStatistics[ArmyType::EARTH].totalUnitsCount) << "%" << std::endl;
	fout << "Total Infected ESs/Total Earth Units = " << calculatePercentage(gameStatistics.totalInfectedESCount, gameStatistics.armyStatistics[ArmyType::EARTH].totalUnitsCount) << "%" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Average of First Attack Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::EARTH].totalFirstAttackDelays, gameStatistics.armyStatistics[ArmyType::EARTH].totalUnitsCount) << std::endl;
	fout << "Average of Destruction Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::EARTH].totalDestructionDelays, gameStatistics.armyStatistics[ArmyType::EARTH].totalUnitsCount) << std::endl;
	fout << "Average of Battle Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::EARTH].totalBattleDelays, gameStatistics.armyStatistics[ArmyType::EARTH].totalUnitsCount) << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Df/Db = " << calculatePercentage(gameStatistics.armyStatistics[ArmyType::EARTH].totalFirstAttackDelays, gameStatistics.armyStatistics[ArmyType::EARTH].totalBattleDelays) << "%" << std::endl;
	fout << "Dd/Db = " << calculatePercentage(gameStatistics.armyStatistics[ArmyType::EARTH].totalDestructionDelays, gameStatistics.armyStatistics[ArmyType::EARTH].totalBattleDelays) << "%" << std::endl;

	// Alien Army Statistics
	fout << std::endl;
	fout << "======================================================================" << std::endl;
	fout << std::right << std::setw(45);
	fout << "Alien Army Statistics" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Total AS Count: " << gameStatistics.unitCounts[UnitType::AS] << std::endl;
	fout << "Total AM Count: " << gameStatistics.unitCounts[UnitType::AM] << std::endl;
	fout << "Total AD Count: " << gameStatistics.unitCounts[UnitType::AD] << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Destructed ASs/Total ASs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::AS], gameStatistics.unitCounts[UnitType::AS]) << "%" << std::endl;
	fout << "Destructed AMs/Total AMs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::AM], gameStatistics.unitCounts[UnitType::AM]) << "%" << std::endl;
	fout << "Destructed ATs/Total ATs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::AD], gameStatistics.unitCounts[UnitType::AD]) << "%" << std::endl;
	fout << "Total Destructed Alien Units/Total Alien Units = " << calculatePercentage(gameStatistics.armyStatistics[ArmyType::ALIEN].totalDestructedUnitsCount, gameStatistics.armyStatistics[ArmyType::ALIEN].totalUnitsCount) << "%" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Average of First Attack Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::ALIEN].totalFirstAttackDelays, gameStatistics.armyStatistics[ArmyType::ALIEN].totalUnitsCount) << std::endl;
	fout << "Average of Destruction Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::ALIEN].totalDestructionDelays, gameStatistics.armyStatistics[ArmyType::ALIEN].totalUnitsCount) << std::endl;
	fout << "Average of Battle Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::ALIEN].totalBattleDelays, gameStatistics.armyStatistics[ArmyType::ALIEN].totalUnitsCount) << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Df/Db = " << calculatePercentage(gameStatistics.armyStatistics[ArmyType::ALIEN].totalFirstAttackDelays, gameStatistics.armyStatistics[ArmyType::ALIEN].totalBattleDelays) << "%" << std::endl;
	fout << "Dd/Db = " << calculatePercentage(gameStatistics.armyStatistics[ArmyType::ALIEN].totalDestructionDelays, gameStatistics.armyStatistics[ArmyType::ALIEN].totalBattleDelays) << "%" << std::endl;

	// Allied Army Statistics
	fout << std::endl;
	fout << "======================================================================" << std::endl;
	fout << std::right << std::setw(45);
	fout << "Earth Allied Army Statistics" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Total SU Count: " << gameStatistics.unitCounts[UnitType::SU] << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Destructed SUs/Total SUs = " << calculatePercentage(gameStatistics.destructedUnitCounts[UnitType::SU], gameStatistics.unitCounts[UnitType::SU]) << "%" << std::endl;
	fout << "======================================================================" << std::endl;
	fout << "Average of Battle Delay = " << calculateRatio(gameStatistics.armyStatistics[ArmyType::EARTH_ALLIED].totalBattleDelays, gameStatistics.armyStatistics[ArmyType::EARTH_ALLIED].totalUnitsCount) << std::endl;

	// Close the output file
	fout.close();
}

bool Game::loadParameters(std::string fileName)
{
	std::fstream fin(fileName);

	if (fin.is_open())
	{
		int N = 0;
		int prob = 0;

		int ESPercentage = 0;
		int ETPercentage = 0;
		int EGPercentage = 0;
		int EHPercentage = 0;

		int ASPercentage = 0;
		int AMPercentage = 0;
		int ADPercentage = 0;

		Range earthPowerRange = { 0, 0 };
		Range earthHealthRange = { 0, 0 };
		Range earthAttackCapacityRange = { 0, 0 };

		Range alienPowerRange = { 0, 0 };
		Range alienHealthRange = { 0, 0 };
		Range alienAttackCapacityRange = { 0, 0 };

		Range earthAlliedPowerRange = { 0, 0 };
		Range earthAlliedHealthRange = { 0, 0 };
		Range earthAlliedAttackCapacityRange = { 0, 0 };
		
		int infectingProbability = 0;
		int infectionThreshold = 0;

		fin >> N >> ESPercentage >> ETPercentage >> EGPercentage >> EHPercentage >> ASPercentage >> AMPercentage >> ADPercentage >> prob;

		char dummyHyphen = '\0'; // Dummy variable to read the hyphen

		fin >> earthPowerRange.min >> dummyHyphen >> earthPowerRange.max;
		fin >> earthHealthRange.min >> dummyHyphen >> earthHealthRange.max;
		fin >> earthAttackCapacityRange.min >> dummyHyphen >> earthAttackCapacityRange.max;

		fin >> alienPowerRange.min >> dummyHyphen >> alienPowerRange.max;
		fin >> alienHealthRange.min >> dummyHyphen >> alienHealthRange.max;
		fin >> alienAttackCapacityRange.min >> dummyHyphen >> alienAttackCapacityRange.max;
		fin >> infectingProbability;

		fin >> earthAlliedPowerRange.min >> dummyHyphen >> earthAlliedPowerRange.max;
		fin >> earthAlliedHealthRange.min >> dummyHyphen >> earthAlliedHealthRange.max;
		fin >> earthAlliedAttackCapacityRange.min >> earthAlliedAttackCapacityRange.max;
		fin >> infectionThreshold;

		randomGenerator.setN(N); // Set the number of units to generate
		randomGenerator.setProb(prob); // Set the probability of generating a unit

		randomGenerator.setEarthParameters(ESPercentage, EGPercentage, ETPercentage, EHPercentage, earthPowerRange, earthHealthRange, earthAttackCapacityRange); // Set the parameters for the Earth army
		randomGenerator.setAlienParameters(ASPercentage, AMPercentage, ADPercentage, alienPowerRange, alienHealthRange, alienAttackCapacityRange); // Set the parameters for the Alien army
		randomGenerator.setEarthAlliedParameters(earthAlliedPowerRange, earthAlliedHealthRange, earthAlliedAttackCapacityRange);

		AlienMonster::setInfectingProbability(infectingProbability); // Set the infecting probability for the Alien Monster
		EarthArmy::setInfectionThreshold(infectionThreshold); // Set the infection threshold percantage for the Earth Army

		fin.close(); // Close the file

		return true; // File loaded successfully
	}

	return false; // File failed to load
}

int Game::getCurrentTimestep() const
{
	return currentTimestep;
}

int Game::getUnitsCount(ArmyType armyType, UnitType unitType) const
{
	// Get the count of current alive units in their armies lists
	switch (armyType)
	{
		case ArmyType::EARTH:
			return earthArmy.getUnitsCount(unitType);

		case ArmyType::ALIEN:
			return alienArmy.getUnitsCount(unitType);
		case ArmyType:: EARTH_ALLIED:
			return earthAlliedArmy.getUnitsCount(unitType);
	}

	return 0;
}

int Game::getInfectedUnitsCount() const
{
	return earthArmy.getInfectedSoldiersCount();
}

Game::~Game()
{
	Unit* unit = nullptr;
	int dummyPri = 0;

	// Delete the units in the killed list
	while (killedList.dequeue(unit))
	{
		delete unit;
		unit = nullptr;
	}

	// Delete the units in the maintenance list
	HealableUnit* healableUnit = nullptr;
	while (unitMaintenanceList.dequeue(healableUnit, dummyPri))
	{
		delete healableUnit;
		healableUnit = nullptr;
	}
}