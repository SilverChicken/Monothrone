#include "Worm.h"
 
Worm::Worm(int owner, Location* loc, Map* map):Unit(owner, loc, map)
{
	textLoc = textLocs[0]; //default blue for now, will be f(PID)
}

Worm::~Worm()
{
}

int Worm::getTexLoc()
{
	return textLoc;
}

bool Worm::move(int x, int y)
{

	return false;
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