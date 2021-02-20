#include "Worm.h"
 
Worm::Worm(int owner, Location* loc, Map* map) :Unit(owner, loc, map)
{
	hp = 1;
	atk = 1;
	speed = 1;
	textLoc = textLocs[0]; //default blue for now, will be f(PID)

	//Define which actions are defined
	actions[0] = 0;
	actions[1] = 1;
	actions[2] = 2;
	actions[3] = 3;
	actions[4] = 4;
	actions[5] = -1;

}

Worm::~Worm()
{
}

int Worm::getTexLoc()
{
	return textLoc;
}

bool Worm::move(Location* location, Map* map)
{
	return Unit::move(location, map);
}

bool Worm::collect(int x, int y)
{

	return false;
}

bool Worm::build(int x, int y, std::string obj)
{

	return false;
}

Unit * Worm::spawn(std::string baby, glm::vec2 place)
{

	return nullptr;
}

bool Worm::consume(Unit * food)
{
	return false;
}


void Worm::draw(unsigned int texture, GLuint shaderprog) {
	//send material information to the shader
	glBindTexture(GL_TEXTURE_2D, texture);

	Unit::draw(shaderprog);
}