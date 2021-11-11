//#include "PlayerType.h"
#include "Gamemode.h"
#include "Unit.h"
#include "Player.h"

#include "ServerKeyCodes.h" //just for the command codes not a fan, will change dep


Gamemode* gameRef = &Gamemode::getInstance();


void PlayerType::checkFreeIds()
{
	int i = 0;
	while (i < currentId) {
		auto found = units.find(i);
		if (found == units.end()) {
			freeIds.push_back(i);
		}
	}
}

int PlayerType::getNextId()
{
	int result;
	if (currentId % 10 == 9 && freeIds.empty()) {
		checkFreeIds();
	}

	if(!freeIds.empty()) {
		result = freeIds.back();
		freeIds.pop_back();
		return result;
	}
	else {
		result = currentId;
		currentId++;
		return result;
	}
	
}


PlayerType::PlayerType()
{
	//default constructor for Player to take over should never be called
}

PlayerType::PlayerType(int Npid, Map* mapo)
{
	//setup

	//PID will be fed in by main, based on #of players currently on server
	PID = Npid;
	energy = 0;
	crystal = 0;

	//Possibly update the GUI here?

	hasThrone = true;


	map = mapo; //get reference to the map


	gameRef->addPlayer(PID, this);  //Adds self to map references

}

PlayerType::~PlayerType()
{
	for (std::map<int, Unit*>::iterator it = units.begin(); it != units.end(); ++it) {
		if (it->second) {
			delete(it->second);
		}

	}
}

int PlayerType::getPID()
{
	return PID;
}

int PlayerType::getCrystal()
{
	return crystal;
}

int PlayerType::getEnergy()
{
	return energy;
}

std::vector<Unit*> PlayerType::getSelection()
{

	std::vector<Unit*> out;
	for (auto tuple : selection) {
		out.push_back(tuple.first);
	}
	return out;
}


void PlayerType::update()
{
	for (auto it = units.begin(); it != units.end(); it++) {
		it->second->update(map); //Works bc virtual -> We don't need explicit types, just matching function definitions!
	}
}

void PlayerType::update2()
{
	for (auto it = units.begin(); it != units.end(); ) {
		it->second->update2(); //Apply damage and other stuff

		//check that the iterator is still valid, or if it was erase we need to not increment it
		if (it->second->getLifeState() == 3) {
			units.erase(it++);
		}
		else {
			it++;
		}

	}
}

bool PlayerType::checkHasThrone()
{
	return hasThrone;
}

bool PlayerType::incEnergy(int val)
{
	energy += val;
	return true;
}

bool PlayerType::incCrystal(int val)
{
	crystal += val;
	return true;
}

bool PlayerType::decEnergy(int val)
{
	if (val <= energy) {
		energy -= val;
		return true;
	}
	return false;
}

bool PlayerType::decCrystal(int val)
{
	if (val <= crystal) {
		crystal -= val;
		return true;
	}
	return false;
}

Location* PlayerType::select(Location* location)
{

	//ideally this logic should be much simpler, and the location we get here comes from the server and has pre-validated that the unit isn't already taken?




	////////have player send final location to server not initial location then simplify this piece.

	Locateable* unit = location->getOwner();
	if (unit == nullptr) {
		return location;
	}
	else if (unit->getClassType() >= MINUNITCLASST) {  //4 is the code for units, all ints above are subclasses of unit
		Unit* target = (Unit*)unit;      //Then this is a safe cast

		if (selection.find(target) == selection.end()) {
			if (target->select(PID)) { //make sure it wasn't already selected just double checks
				selection[target] = true;
				return location;
			}
		}
	}
	return location;
}


Unit* PlayerType::deselect(Location* location)
{
	Locateable* unit = location->getOwner();
	if (unit == nullptr) {
		return nullptr;
	}
	else if (unit->getClassType() >= MINUNITCLASST) {						//This is the code for units
		Unit* target = (Unit*)unit;							//Then this is a safe cast					
		if (selection.find(target) != selection.end()) {			//Make sure it's currently selected 
			selection.erase(target);							//Erase
			target->deselect();									//Unit logic for being deselected
			return target;
		}
	}
	return nullptr;
}

Unit* PlayerType::deselect(Unit* unit)
{
	if (selection.find(unit) != selection.end()) {			//Make sure it's currently selected 
		selection.erase(unit);							//Erase
		unit->deselect();									//Unit logic for being deselected
		return unit;
	}

	return nullptr;
}

void PlayerType::deselectAll()
{
	for (auto it = selection.begin(); it != selection.end(); it++) {
		it->first->deselect();
	}
	selection.clear();

}

int PlayerType::perform_action(int cmd, Location* aloc)
{
	/// <summary>
	/// Currently MOVE_LOC == CMD_MOVE if ever that changes there will need to be changing of the case statements 
	/// </summary>
	/// <param name="cmd">cmd in the CMD_MOVE format</param>
	/// <returns>Location of taken action</returns>

	switch (cmd) { 
	case CMD_MOVE://Move
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->move(aloc); //Virtual so it will call the appropriate derived
		}
		return MOVE_LOC;
	case CMD_COLLECT://Collect
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->collect(aloc); //Virtual so it will call the appropriate derived
		}
		return COLLECT_LOC;
	case CMD_BUILD://Build
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->build(aloc, 0); //Placeholder obj for now
		}
		return BUILD_LOC;
	case CMD_SPAWN://Spawn
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->spawn(aloc, 0); //Placeholder obj for now
		}
		return CONSUME_LOC;
	case CMD_CONSUME://Consume
		return CONSUME_LOC;
	case 5://Currently not accessible
		return 5;
	default:
		return -1;

	}
}

void PlayerType::destroyUnit(Unit* unit)
{
	//destroys and cleans up a unit you own
	if (unit->getOwner() == PID) {
		//Die animation already finished
		

		//Maybe this function has no use right.


		//remove from player units map will be done in the current update2

		//After this the unit should have exited the scope and be cleaned up!
		//unit->~Unit(); 

	}
}

void PlayerType::draw(unsigned int* texture, GLuint shaderprog)
{

}

void PlayerType::draw(unsigned int* texture, PlayerType* pl, GLuint shaderprog) //ONLY but Player as argument
{
	//All unit draws are the same!
	//Only draw your units

	Player* player = (Player*)pl;

	for (auto it = units.begin(); it != units.end(); it++) {
		if (!player->cull(it->second->getLoc())) {
			it->second->draw(texture[it->second->getTexLoc()], shaderprog);
		}
	}
}

void PlayerType::addUnit(Unit* newUnit)
{
	units[newUnit->getID()] = newUnit;
}

void PlayerType::removeUnit(Unit* del)
{
	units.erase(del->getID());
}
