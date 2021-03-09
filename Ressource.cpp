#include "Ressource.h"
#include "Map.h"

Location * Ressource::spawnLoc(Location* location, Map* map)
{

	glm::vec2 bias = locBias; //or location->getPos() should be same

	//while the location this object was given was invalid
	int tries = 0; //so that we don't loop for too long

	while (!location->state) {
		int dir = rand() % 5;
		switch (dir) {
		case 0:
			bias += glm::vec2(1.0, 0.0);
			location = map->getLoc(bias);
			break;
		case 1:
			bias += glm::vec2(-1.0, 0.0);
			location = map->getLoc(bias);
			break;
		case 2:
			bias += glm::vec2(0.0, 1.0);
			location = map->getLoc(bias);
			break;
		case 3:
			bias += glm::vec2(0.0, -1.0);
			location = map->getLoc(bias);
			break;
		default:
			bias += glm::vec2(1.0, 1.0);
			location = map->getLoc(bias);
			break;
		}
		tries++;

		if (tries > 20) {
			location = map->getLoc(rand() % MAPSIZE, rand() % MAPSIZE);
		}
	} //now we know that location is free!

	return location;
}

Ressource::Ressource()
{

}

Ressource::~Ressource()
{

}

void Ressource::destroy()
{ 
	//particle effect?

	//remove from gameMode, where is reference?

	loc->release(this);     //release lock

	game->removeRessource(this);

	this->~Ressource();     //currently destroys but doesn't remove the map reference. May free twice? But not big deal
}

void Ressource::draw(GLuint shaderprog)
{
	
	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);

	glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 0.0f);

	//std::cout << topLeft->getPos().x << "   " << topLeft->getPos().y << std::endl;
	//std::cout<< loc->getPos().x << "   " << loc->getPos().y << std::endl;


	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Ressource::draw(unsigned int, GLuint)
{
}

int Ressource::getTextLoc()
{
	return 0;
}
