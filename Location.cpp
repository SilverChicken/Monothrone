#include "Location.h"
//#include"Locateable.h"
#include <iostream>

Location::Location()
{
}

Location::Location(glm::vec2 position) {
	//std::cout << "Location init" << std::endl;
	lock = true;
	state = true;
	pos = position;
	//std::cout << pos.x << "    " << pos.y << std::endl;
}


Location::Location(int x, int y) : Location(glm::vec2((float)x, (float)y))  {}


Location::~Location(){

}

glm::vec2 Location::getPos() {
	return pos;
}

Locateable * Location::getOwner(){
	return owner;  
}

bool Location::take(Locateable* obj) { 
	if (lock) {
		//if (obj->setLoc(this)) { //assumes that obj->setLoc(this) is true
		lock = false;
		state = false;
		owner = obj;
		return true;
	}
	return false;
}

Location* Location::occupy(Locateable* player)
{
	return this;
}


bool Location::release(Locateable* obj) { //gets called by something after it takes the lock of where it's moving to
	if (owner == obj) { 
		lock = true; 
		state = true;
		owner = nullptr;
		return true;
	}
	return false;
}


