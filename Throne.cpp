#include "Throne.h"

Throne::Throne(int owner, Location * loc, Map * map):Unit(owner, loc, map)
{
	hp = 25;
	atk = 0;
	speed = 0;
	textLoc = textLocs[owner]; //default blue for now, will be f(PID)

	//Define which actions are defined
	actions[0] = -1;
	actions[1] = 1;
	actions[2] = 2;
	actions[3] = 3;
	actions[4] = 4;
	actions[5] = -1;
}

Throne::~Throne()
{

}

void Throne::draw(unsigned int texture, GLuint shaderprog)
{
	//send material information to the shader
	glBindTexture(GL_TEXTURE_2D, texture);

	Unit::draw(shaderprog);
}

int Throne::getTexLoc()
{
	return textLoc;
}

bool Throne::move(Location *, Map *)
{
	return false;
}

bool Throne::collect(int x, int y)
{
	return false;
}

bool Throne::build(int x, int y, std::string obj)
{
	return false;
}

Unit * Throne::spawn(std::string baby, glm::vec2 place)
{
	return nullptr;
}

bool Throne::consume(Unit * food)
{
	return false;
}
