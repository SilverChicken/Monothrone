#include "Player.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include<iostream>
#include"Map.h"
#include"Unit.h"

#include "Gamemode.h"


#define BINDINGCOUNT 6  //Also defined in Unit.h 


//bool vecSearch(Unit*, std::list<Unit*>&); //Helper function to check if A is in the Vector. Iterates forwards
//bool vecRemove(Unit*, std::list<Unit*>&); //Helper function to check if A is in the Vector, if it is it removes A

Gamemode* playerGame = &Gamemode::getInstance();

Player::Player(int Npid, Location* location, Map * mapo)
{

	//setup

	//PID will be fed in by main, based on #of players currently on server
	PID = Npid;
	energy = 0;
	crystal = 0;

	//Possibly update the GUI here?

	hasThrone = true;
	zoom = 0;
	camBoxX = 5; 
	camBoxY = 5;

	
	map = mapo; //get reference to the map


	playerGame->addPlayer(PID, this);  //Adds self to map references

	setLoc(location);  //Set the location to the specified location

	setBotLeft(location->getPos() - glm::vec2(ZOOMDEF / 2.0, ZOOMDEF / 2.0) ); //Puts you in middle of screen

	texLoc = textLocs[PID];

	bindings = new int[BINDINGCOUNT];

	



}

Player::~Player()
{
	for (std::map<Unit*, bool>::iterator it = units.begin(); it != units.end(); ++it) {
		if (it->first) {
			delete(it->first);
		}
		
	}

	delete(bindings);
}


//gets and sets

int Player::getPID()
{
	return PID;
}

const int Player::getTexLoc()
{
	return texLoc;
}

Location * Player::getLoc()
{
	return loc;
}

float Player::getZoom()
{
	return zoom;
}

bool Player::setLoc(Location * newLoc)
{
	//can move to anywhere
	loc = newLoc;
	return true;
}

bool Player::setLoc(int x, int y)
{
	if (map != nullptr) {
		setLoc(map->getLoc(x, y));
		return true;
	}
	return false;
}

bool Player::setLoc(glm::vec2 locate)
{
	if (map != nullptr) {
		setLoc(map->getLoc(locate));
		return true;
	}
	return false;
}

int Player::getCrystal()
{
	return crystal;
}

int Player::getEnergy()
{
	return energy;
}

std::map<Unit*, bool> Player::getSelection()
{
	return selection;
}

void Player::UpdateSelectedUnits() //Called after we update bindings!
{
	std::vector<int> selectType;
	int classT;

	for (auto it = selection.begin(); it != selection.end(); it++) {
		classT = it->first->getClassType();
		selectType.push_back(classT);
	}
	playerGame->updateGuiUnit(selectType);

	
}

void Player::updateBindings()
{

	bool impl; //Tracks if function i is implemented by ALL members of selection
	int j = 0; //The index of bindings that we are trying to fill
	for (int i = 0; i < ACTIONCOUNT; i++) {
		impl = true && !selection.empty();
		for (auto it = selection.begin(); it != selection.end() && impl; it++) { //for loop stops as soon as impl is false
			impl = impl && it->first->getActions()[i]; //Makes sure that function i is implemented by every element of selection
		}
		if (impl && j < BINDINGCOUNT) { //If it was implemented, fill in the first available spot in bindings	
			bindings[j] = i;
			j++;
		}
	}
	while (j < BINDINGCOUNT) { //fill the rest of bindings with -1 for no functions
		bindings[j] = -1;
		j++;
	}

	//Now that Bindings is done, update the Gui bindings!
	playerGame->updateGuiBind(&bindings[0]);

	//Also update the gui selection! pass object by reference
	UpdateSelectedUnits();


}

void Player::addUnit(Unit * newUnit)
{
	units[newUnit] = newUnit->isSelected();
}

void Player::update()
{
	if (!pause) { 
		for (auto it = units.begin(); it != units.end(); it++) {
			it->first->update(map); //Works bc virtual -> We don't need explicit types, just matching function definitions!
		}
	}
}


//Gameplay fcts


bool Player::move(int dir) 
{
	// 0 is up, -1 is down, 1 is right, 2 is left


	glm::vec2 locate;
	switch (dir) {
		case KEY_UP:
			locate = loc->getPos();
			if (locate.y < MAPSIZE - 1) {        //Make sure we're not on an edge
				locate += glm::vec2(0.0, 1.0);
				setLoc(map->getLoc(locate));     //Set new location
				checkCameraChange();             //Adjust Camera if Necessary
				return true;  
			}
			break;
		case KEY_DOWN:
			locate = loc->getPos();
			if (locate.y > 0.0) {
				locate -= glm::vec2(0.0, 1.0);
				setLoc(map->getLoc(locate));
				checkCameraChange();
				return true;
			}
			break;
		case KEY_RIGHT:
			locate = loc->getPos();
			if (locate.x < MAPSIZE - 1) {
				locate += glm::vec2(1.0, 0.0);
				setLoc(map->getLoc(locate));
				checkCameraChange();
				return true;
			}
			break;
		case KEY_LEFT:
			locate = loc->getPos();
			if (locate.x > 0.0) {
				locate -= glm::vec2(1.0, 0.0);
				setLoc(map->getLoc(locate));
				checkCameraChange();
				return true;
			}
			break;
		default:
			return false;
			break;
	}
	return false;
}

bool Player::select(Location* location)
{
	Locateable* unit = location->getOwner();
	if ( unit == nullptr) {
		return false;
	}
	else if(unit->getClassType() >= MINUNITCLASST) {  //4 is the code for units, all ints above are subclasses of unit
		Unit * target = (Unit*)unit;      //Then this is a safe cast

		if (selection.find(target) == selection.end()) {
			if (target->select(PID)) { //make sure it wasn't already selected just double checks
				selection[target] = true;
				updateBindings();									//Update the bindings
				return true;
			}
		}
		else { //it was already selected

			const bool selectCond(Unit*, Location *);
			Location* newSel = playerGame->findClosestToCnd(location, loc, target, selectCond);
			if (newSel) { //If we;re out of the unit then don't
				select(newSel); //select the closest one which is this one.
			//but what if we run out? Bound the search
			}
			
		}
	}
	return false;
}

const bool selectCond(Unit* Utype, Location * location) { //needs another parameter, or a unit pointer instead
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



Unit * Player::deselect()
{
	Locateable* unit = loc->getOwner();
	if (unit == nullptr) {
		return nullptr;
	}
	else if (unit->getClassType() >= MINUNITCLASST) {						//This is the code for units
		Unit * target = (Unit*)unit;							//Then this is a safe cast					
		if(selection.find(target) != selection.end()){			//Make sure it's currently selected 
			selection.erase(target);							//Erase
			target->deselect();									//Unit logic for being deselected
			updateBindings();									//Update the bindings
			return target;
		}
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

bool Player::checkHasThrone()
{
	return hasThrone;
}

bool Player::incEnergy(int)
{
	energy++;
	
	return true;
}

bool Player::incCrystal(int)
{
	crystal++;
	return true;
}

bool Player::decEnergy(int val)
{
	if (val <= energy) {
		energy -= val;
		return true;
	} 
	return false;
}

bool Player::decCrystal(int val)
{
	if (val <= crystal) {
		crystal -= val;
		return true;
	}
	return false;
}


void Player::actionKey(int key)
{
	switch (bindings[key]) { //Eventually define these as CONSTANTS for readability
		case MOVE_LOC://Move
			for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
				it->first->move(loc, map); //Virtual so it will call the appropriate derived
			}
			break;
		case COLLECT_LOC://Collect
			for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
				it->first->collect(loc, map); //Virtual so it will call the appropriate derived
			}
			break;
		case BUILD_LOC://Build
			for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
				it->first->build(loc, 0); //Placeholder obj for now
			}
			break;
		case SPAWN_LOC://Spawn
			for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it) {
				it->first->spawn(loc, 0); //Placeholder obj for now
			}
			break;
		case CONSUME_LOC://Consume
			break;
		case 5://Currently not accessible
			break;
		default:
			break;

	}


	

}






//Render fcts


void Player::setBotLeft(Location* newBL) {
	botLeft = newBL;
}

void Player::setBotLeft(int x, int y)
{
	if (map != nullptr) {
		setBotLeft(map->getLoc(x, y));
	}
}

void Player::setBotLeft(glm::vec2 locate)
{
	if (map != nullptr) {
		setBotLeft(map->getLoc(locate));
	}
}

bool Player::checkCameraChange()
{
	glm::vec2 BL = botLeft->getPos();     //Location of bottom left of the screen
	glm::vec2 Tloc = loc->getPos();       //Location of Character


	// zoomdef * some value of zoom?
	// ZOOMDEF is the default size of the grid on screen ie there are 20 squares in x and y by default

	if ( (Tloc.x - BL.x > (ZOOMDEF)/2 + camBoxX) && BL.x + ZOOMDEF < MAPSIZE) {   //Check to see if the on screen position passes the camera movement box
		BL += glm::vec2(1.0, 0.0);                                                // and if the camera is currently hitting one edge of the map
		setBotLeft(BL);
		return true;
	}
	else if ( (Tloc.x - BL.x < (ZOOMDEF) / 2 - camBoxX - 1) && BL.x > 0) {
		BL -= glm::vec2(1.0, 0.0);
		setBotLeft(BL);
		return true;
	}
	else if ( (Tloc.y - BL.y + GUISPACE> (ZOOMDEF) / 2 + camBoxY) && BL.y + ZOOMDEF - GUISPACE< MAPSIZE) {
		BL += glm::vec2(0.0, 1.0);
		setBotLeft(BL);
		return true;
	}
	else if ( (Tloc.y - BL.y < (ZOOMDEF) / 2 - camBoxY - 1) && BL.y > 0) {
		BL -= glm::vec2(0.0, 1.0);
		setBotLeft(BL);
		return true;
	}
	else {
		return false;
	}
}

bool Player::cull(Location * obj) //true then cull, false then don't
{
	//For now just find out if we're off screen then return false
	glm::vec2 p = obj->getPos();
	glm::vec2 bl = botLeft->getPos();
	if (p.x < bl.x || p.y < bl.y || p.x > bl.x + ZOOMDEF || p.y > bl.y + ZOOMDEF - GUISPACE) { //off the grid
		return true;
	}


	//Later we can incorporate vision

	return false;

}


bool Player::addVision(Location *)
{
	return false;
}

bool Player::removeVision(Location *)
{
	return false;
}

bool Player::removeCloud(Location *)
{
	return false;
}

Location * Player::getBotLeft()
{
	return botLeft;
}

float Player::changeZoom(float delta)
{

	//Make sure out zoom is within bounds

	zoom += delta; // so now we will see 2*delta fewer squares on screen. This means:
	if (zoom > MAXZOOM) {
		zoom = MAXZOOM;
	}
	else if (zoom < MINZOOM) {
		zoom = MINZOOM;
	}
	//camBoxX -= delta; // The camera will move tighter
	//camBoxY -= delta; //Probably change so it's not exactly linear


	return zoom;
	
}

void Player::draw(unsigned int* texture, GLuint shaderprog)
{

	//All unit draws are the same!

	for (auto it = units.begin(); it != units.end(); it++) {
		if (!cull(it->first->getLoc())) {
			it->first->draw(texture[it->first->getTexLoc()], shaderprog);
		}
	}


	//send material information to the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[texLoc]);

	//set overlay false, in case
	glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 0.0f);

	//send zoom info
	glUniform1f(glGetUniformLocation(shaderprog, "zoom"), zoom);

	//send camera info -> top left corner !
	glUniform2f(glGetUniformLocation(shaderprog, "BL"), botLeft->getPos().x, botLeft->getPos().y);

	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);


	//std::cout << topLeft->getPos().x << "   " << topLeft->getPos().y << std::endl;
	//std::cout<< loc->getPos().x << "   " << loc->getPos().y << std::endl;


	glBindVertexArray(Locateable::VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Locateable::EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




}
