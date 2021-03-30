#include "Wall.h"
#include "include\Wall.h"

#include "Map.h"
#include "ImLoader.h"

Wall::Wall(int own, Location * location, Map* map)
{
	owner = own;
	setLoc(map->findClosest(loc));

	//classType = 0;
}

Wall::~Wall()
{
}

void Wall::destroy()
{
	loc->release(this);
}

void Wall::draw(GLuint shaderprog)
{
	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ImLoader::textures[textLocs[owner]]);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}


