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

bool vecSearch(Unit*, std::list<Unit*>&); //Helper function to check if A is in the Vector. Iterates forwards
bool vecRemove(Unit*, std::list<Unit*>&); //Helper function to check if A is in the Vector, if it is it removes A

Player::Player(int Npid, Map * mapo)
{

	//setup

	//PID will be fed in by main, based on #of players currently on server
	PID = Npid;
	energy = 0;
	crystal = 0;
	hasThrone = true;
	zoom = 0;
	camBoxX = 5; 
	camBoxY = 5;

	texLoc = textLocs[PID];

	bindings = new int[6];

	//get reference to the map
	map = mapo;



}

Player::~Player()
{
	for (std::map<Unit*, bool>::iterator it = units.begin(); it != units.end(); ++it) {
		delete(it->first);
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
		setLoc(map->getloc(x, y));
		return true;
	}
	return false;
}

bool Player::setLoc(glm::vec2 locate)
{
	if (map != nullptr) {
		setLoc(map->getloc(locate));
		return true;
	}
	return false;
}

std::map<Unit*, bool> Player::getSelection()
{
	return selection;
}

void Player::addUnit(Unit * newUnit)
{
	units[newUnit] = newUnit->isSelected();
}


//Gameplay fcts


bool Player::move(int dir) 
{
	// 0 is up, -1 is down, 1 is right, 2 is left


	glm::vec2 locate;
	//Location* locer;    -> to check is Map returns nullptr?
	switch (dir) {
		case 0:
			locate = loc->getPos();
			if (locate.y < MAPSIZE - 1) {        //Make sure we're not on an edge
				locate += glm::vec2(0.0, 1.0);
				setLoc(map->getloc(locate));     //Set new location
				checkCameraChange();             //Adjust Camera if Necessary
				return true;  
			}
			break;
		case -1:
			locate = loc->getPos();
			if (locate.y > 0.0) {
				locate -= glm::vec2(0.0, 1.0);
				setLoc(map->getloc(locate));
				checkCameraChange();
				return true;
			}
			break;
		case 1:
			locate = loc->getPos();
			if (locate.x < MAPSIZE - 1) {
				locate += glm::vec2(1.0, 0.0);
				setLoc(map->getloc(locate));
				checkCameraChange();
				return true;
			}
			break;
		case 2:
			locate = loc->getPos();
			if (locate.x > 0.0) {
				locate -= glm::vec2(1.0, 0.0);
				setLoc(map->getloc(locate));
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

bool Player::select()
{
	Locateable* unit = loc->getOwner();
	if ( unit == nullptr) {
		return false;
	}
	else if(unit->getClassType() == 4) {  //This is the code for units
		Unit * target = (Unit*)unit;      //Then this is a safe cast

		if (selection.find(target) == selection.end()) {
		//if (!vecSearch(target, selection)) {		  //Internal logic for the unit being selected
			if (target->select(PID)) { //make sure it wasn't already selected
				//selection.push_back(target);  //If we actually own the unit, add it to the selection
				selection[target] = true;
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
	else if (unit->getClassType() == 4) {						//This is the code for units
		Unit * target = (Unit*)unit;							//Then this is a safe cast
		//if(selection.erase(target) != selection.end());
		//std::list<Unit*>::iterator it = selection.begin();
		//if (vecRemove(target, selection)) {						//Make sure it's currently selected and erase it
		if(selection.find(target) != selection.end()){
			selection.erase(target);							//Erase
			target->deselect();									//Unit logic for being deselected
			return target;
		}
	}
	return nullptr;
}

void Player::deselectAll()
{
	selection.clear();
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

void Player::actionQ()
{
	//switch (abilities[0]) {

	//}



//for now it's just move
	for (std::map<Unit*, bool>::iterator it = selection.begin(); it != selection.end(); ++it ) {
		if (it->second) {
			it->first->move(loc, map);
		}
	}

}

void Player::actionW()
{
}

void Player::actionE()
{
}

void Player::actionR()
{
}

void Player::actionT()
{
}

void Player::actionY()
{
}


//Render fcts


void Player::setBotLeft(Location* newBL) {
	botLeft = newBL;
}

void Player::setBotLeft(int x, int y)
{
	if (map != nullptr) {
		setBotLeft(map->getloc(x, y));
	}
}

void Player::setBotLeft(glm::vec2 locate)
{
	if (map != nullptr) {
		setBotLeft(map->getloc(locate));
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
	else if ( (Tloc.y - BL.y > (ZOOMDEF) / 2 + camBoxY) && BL.y + ZOOMDEF < MAPSIZE) {
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

void Player::draw(unsigned int texture, GLuint shaderprog)
{

	//send material information to the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

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


	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

bool vecSearch(Unit * target, std::list<Unit*>& list) //Just tells you if target is in list
{
	for (std::list<Unit*>::iterator it = list.begin(); it != list.end(); ++it) {
		if (*it == target) {
			return true;
		}
	}
	return false;
}

bool vecRemove(Unit* target, std::list<Unit*>& list) { //this one tells you if it's in the list and it erases it if it finds it
	for (std::list<Unit*>::iterator it = list.begin(); it != list.end(); ++it) {
		if (*it == target) {
			list.erase(it);
			return true;
		}
	}
	return false;
}