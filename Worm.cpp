#include "Worm.h"
 
Worm::Worm(int owner, Location* loc, Map* map) :Unit(owner, loc, map)
{
	hp = 1;
	atk = 1;
	speed = 1;
	textLoc = textLocs[owner]; 

	classType = WORM_CLASS_T;

	//Define which actions are defined
	actions[0] = true;
	actions[1] = true;
	actions[2] = true;
	actions[3] = false;
	actions[4] = true;
	actions[5] = false;

}

Worm::~Worm()
{
	//delete[] actions;
}

bool Worm::move(Location* location, Map* map)
{
	return Unit::move(location, map);
}

bool Worm::collect(Location * location, Map* map)
{
	return Unit::collect(location, map);
}

bool Worm::build(Location* location, int obj)
{

	return Unit::build(location, obj);
}

Unit * Worm::spawn(Location* location, int obj)
{

	return nullptr;
}

bool Worm::consume(Unit * food)
{
	return false;
}


void Worm::draw(unsigned int texture, GLuint shaderprog) {
	//send material information to the shader
	//nothing special here either

	Unit::draw(texture, shaderprog);
}

void Worm::update(Map * map)
{
	if (!path.empty()) { //Only update if moving
		animState = (animState + ANIMSTEP) % NUMTEX; 
		textLoc = textLocs[animState];
	}
	
	Unit::update(map);
}
