#include "Unit.h"
#include "Map.h"
#include "ImLoader.h"


Unit::Unit(int own, Location* loc, Map * map)
{
	owner = own;			
	setLoc(map->findClosest(loc));  //this will always be true;
	selected = false;

	//Set class identifier
	classType = 4;

}

Unit::~Unit()
{
}

void Unit::draw(GLuint shaderprog)
{
	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);


	if (selected) {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 1.0f);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]);
	}
	else {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 0.0f);
	}


	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Unit::select(int PID)
{
	if (PID == owner) {
		selected = true;
		return true;
	}
	return false;
}

void Unit::deselect()
{
	selected = false;
}

bool Unit::move(int x, int y)
{
	return false;
}

bool Unit::collect(int x, int y)
{
	return false;
}

bool Unit::build(int x, int y, std::string obj)
{
	return false;
}

Unit * Unit::spawn(std::string baby, glm::vec2 place)
{
	return nullptr;
}

bool Unit::consume(Unit * food)
{
	return false;
}

