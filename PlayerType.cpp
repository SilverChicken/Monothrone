//#include "PlayerType.h"
#include "Gamemode.h"
#include "Unit.h"
#include "Player.h"


Gamemode* gameRef = &Gamemode::getInstance();

PlayerType::PlayerType()
{
	//default constructor for Player to take over
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
	for (std::map<Unit*, bool>::iterator it = units.begin(); it != units.end(); ++it) {
		if (it->first) {
			delete(it->first);
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


void PlayerType::update()
{
	for (auto it = units.begin(); it != units.end(); it++) {
		it->first->update(map); //Works bc virtual -> We don't need explicit types, just matching function definitions!
	}
}

void PlayerType::update2()
{
	for (auto it = units.begin(); it != units.end(); ) {
		it->first->update2(); //Apply damage and other stuff

		//check that the iterator is still valid, or if it was erase we need to not increment it
		if (it->first->getLifeState() == 3) {
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

bool Player::select(Location* location)
{

	//ideally this logic should be much simpler, and the location we get here comes from the server and has pre-validated that the unit isn't already taken?




	////////have player send final location to server not initial location then simplify this piece.





	Locateable* unit = location->getOwner();
	if (unit == nullptr) {
		return false;
	}
	else if (unit->getClassType() >= MINUNITCLASST) {  //4 is the code for units, all ints above are subclasses of unit
		Unit* target = (Unit*)unit;      //Then this is a safe cast

		if (selection.find(target) == selection.end()) {
			if (target->select(PID)) { //make sure it wasn't already selected just double checks
				selection[target] = true;
				updateBindings();									//Update the bindings
				return true;
			}
		}
		else { //it was already selected

			const bool selectCond(Unit*, Location*);
			Location* newSel = playerGame->findClosestToCnd(location, loc, target, selectCond);
			if (newSel) { //If we;re out of the unit then don't
				select(newSel); //select the closest one which is this one.
			//but what if we run out? Bound the search
			}

		}
	}
	return false;
}

const bool selectCond(Unit* Utype, Location* location) { //needs another parameter, or a unit pointer instead
	Locateable* locate = location->getOwner();
	if (locate != nullptr) {
		if (location->getOwner()->getClassType() == Utype->getClassType()) {
			Unit* otherU = (Unit*)locate;
			if (otherU->getOwner() == Utype->getOwner() && !otherU->isSelected()) {
				return true;
			}
		}
	}
	return false;
}



Unit* Player::deselect()
{
	Locateable* unit = loc->getOwner();
	if (unit == nullptr) {
		return nullptr;
	}
	else if (unit->getClassType() >= MINUNITCLASST) {						//This is the code for units
		Unit* target = (Unit*)unit;							//Then this is a safe cast					
		if (selection.find(target) != selection.end()) {			//Make sure it's currently selected 
			selection.erase(target);							//Erase
			target->deselect();									//Unit logic for being deselected
			updateBindings();									//Update the bindings
			return target;
		}
	}
	return nullptr;
}

Unit* Player::deselect(Unit* unit)
{
	if (selection.find(unit) != selection.end()) {			//Make sure it's currently selected 
		selection.erase(unit);							//Erase
		unit->deselect();									//Unit logic for being deselected
		updateBindings();									//Update the bindings
		return unit;
	}

	return nullptr;
}

void Player::deselectAll()
{
	for (auto it = selection.begin(); it != selection.end(); it++) {
		it->first->deselect();
	}
	selection.clear();
	updateBindings();									//Update the bindings

}

int PlayerType::perform_action(int cmd)
{
	/// <summary>
	/// Currently MOVE_LOC == CMD_MOVE if ever that changes there will need to be changing of the case statements 
	/// </summary>
	/// <param name="cmd">cmd in the CMD_MOVE format</param>
	/// <returns>Location of taken action</returns>

	switch (cmd) { 
	case MOVE_LOC://Move
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->move(loc, map); //Virtual so it will call the appropriate derived
		}
		return MOVE_LOC;
	case COLLECT_LOC://Collect
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->collect(loc, map); //Virtual so it will call the appropriate derived
		}
		return COLLECT_LOC;
	case BUILD_LOC://Build
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->build(loc, 0); //Placeholder obj for now
		}
		return BUILD_LOC;
	case SPAWN_LOC://Spawn
		for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
			it->first->spawn(loc, 0); //Placeholder obj for now
		}
		return CONSUME_LOC;
	case CONSUME_LOC://Consume
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
		if (!player->cull(it->first->getLoc())) {
			it->first->draw(texture[it->first->getTexLoc()], shaderprog);
		}
	}
}

void PlayerType::addUnit(Unit* newUnit)
{
	units[newUnit] = newUnit->isSelected();
}

void PlayerType::removeUnit(Unit* del)
{
	units.erase(del);
}
