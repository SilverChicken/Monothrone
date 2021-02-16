#include "Crystal.h"
#include "Map.h"
#include <stdlib.h>
#include <time.h>

#include<iostream>

Crystal::Crystal(Location * location, Map* map)
{
	time(NULL);

	glm::vec2 bias = location->getPos();
	locBias = bias;
	VerticalVar = 3;
	HorizVar = 3;
	probSpawn = 1.0 / 15.0;  //E of 1/p to stop spawning 

	//get random texture from array:
	int texInd = rand() % 3;
	textLoc = textLocs[texInd];    // 4 is the size of textLocs

	//Set class identifier
	classType = 3;

		//while the location this object was given was invalid
	int tries = 0; //so that we don't loop for too long

	while (!location->state) {
		int dir = rand() % 5;
		switch (dir) {
		case 0:
			bias += glm::vec2(1.0, 0.0);
			location = map->getloc(bias);
			break;
		case 1:
			bias += glm::vec2(1.0, 0.0);
			location = map->getloc(bias);
			break;
		case 2:
			bias += glm::vec2(1.0, 0.0);
			location = map->getloc(bias);
			break;
		case 3:
			bias += glm::vec2(1.0, 0.0);
			location = map->getloc(bias);
			break;
		default:
			bias += glm::vec2(1.0, 1.0);
			location = map->getloc(bias);
			break;
		}
		tries++;

		if (tries > 20) {
			location = map->getloc(rand() % MAPSIZE, rand() % MAPSIZE);
		}
	} //now we know that location is free!

	if (setLoc(location)) { //This *should* be true because of the previous check

		//all is innited, now add this to the Map's ressource count, and then decide if we spawn another in the cluster
		map->addCrystals(this);

		//now decide if we are spawning another 
		if (((double)rand() / RAND_MAX) >= probSpawn) {
			spawn(map);
		}
		//and we're done, rendering is done by locateable

	}
	else {
		std::cout << "Crystal spawned at taken location: (" << location->getPos().x << ", " << location->getPos().y << ")" << std::endl;
		//stop the spawning
	}

}


Crystal::~Crystal()
{

}

int Crystal::getTextLoc()
{
	return textLoc;
}

void Crystal::spawn(Map * map)
{
	//Where to spawn this new Crystal?
	//Near bias. How near? Variance near?
	glm::vec2 newBias = locBias;
	newBias.x += rand() % HorizVar - (HorizVar / 2);
	newBias.y += rand() % VerticalVar - (VerticalVar / 2);

	Location * newLoc = map->getloc(newBias);
	Crystal * mtn = new Crystal(newLoc, map);

}

void Crystal::draw(unsigned int texture, GLuint shaderprog)
{

	//send material information to the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Ressource::draw(shaderprog);

}

