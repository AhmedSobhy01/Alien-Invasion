#include <iostream>
#include <fstream>
#include <sstream>

#include "Game.h"
#include "DEFS.h"
#include "UnitClasses/Unit.h"

std::string Game::loadFile(std::string fileName)
{
	std::fstream fin(fileName);
	std::string wholeFile;

	if (fin.is_open())
	{
		std::string newLine;
		while (std::getline(fin, newLine))
			wholeFile += newLine + " ";
	}
	return wholeFile;
}

Game::Game(): gameMode(GameMode::INTERACTIVE), currentTimestep(0), randomGenerator(nullptr)
{}

void Game::run(GameMode gameMode, std::string inputFileName)
{
	// Change the game mode
	changeGameMode(gameMode);

	// Load parameters for random generator
	std::string inputParameters = loadFile(inputFileName);
	randomGenerator = new RandomGenerator(this, inputParameters);

	// Run the game
	while (!battleOver())
	{
		incrementTimestep();

		std::cout << "\nCurrent Timestep " << currentTimestep << std::endl;
		earthArmy.print();
		alienArmy.print();
		printKilledList();
	}
}

void Game::incrementTimestep()
{
	currentTimestep++;

	// Generate units for both armies
	randomGenerator->generateArmy(ArmyType::EARTH);
	randomGenerator->generateArmy(ArmyType::ALIEN);
}

void Game::changeGameMode(GameMode gameMode)
{
	this->gameMode = gameMode;
}

bool Game::battleOver() const
{
	return currentTimestep > 10;
}

void Game::addUnit(Unit* unit)
{
	if (unit->getArmyType() == ArmyType::EARTH)
		earthArmy.addUnit(unit);
	else if (unit->getArmyType() == ArmyType::ALIEN)
		alienArmy.addUnit(unit);
}

void Game::killUnit(Unit* unit)
{
	killedList.enqueue(unit);
}

void Game::printKilledList() const
{
	std::cout << "============== Killed/Destructed Units ==============" << std::endl;
	std::cout << killedList.getCount() << " units [";
	killedList.printList();
	std::cout << "]" << std::endl;
}

int Game::getCurrentTimestep() const
{
	return currentTimestep;
}

Game::~Game()
{}