#pragma once
#include"glm\mat2x2.hpp"

class Locateable;


class Location
{
private:
	bool lock; //1 is free, 0 is taken
	glm::vec2 pos;
	Locateable* owner;
public:
	bool state;
	Location();
	Location(glm::vec2 position);
	Location(int x, int y);
	~Location();
	
	bool take(Locateable* obj);                         //takes if free and sets DO NOT CALL DIRECTLY. Use locateable.setLoc
	Location* occupy(Locateable* player);               //gets location but does not take lock, for player, & vision
	bool release(Locateable* obj);
	
	glm::vec2 getPos();
	Locateable* getOwner();

};

