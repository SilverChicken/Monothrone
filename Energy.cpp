#include "Energy.h"
#include "Map.h"
#include <stdlib.h>
#include <time.h>

#include<iostream>

Energy::Energy(Location * location, Map* map)
{
	time(NULL);

	glm::vec2 bias = location->getPos();
	locBias = bias;
	VerticalVar = 4;
	HorizVar = 4;
	probSpawn = 1.0 / 10.0;  //E of 1/p to stop spawning 

	//get random texture from array:
	int texInd = rand() % 3;
	textLoc = textLocs[texInd];    // 3 is the size of textLocs

	//Set class identifier
	classType = ENERGY_CLASS_T;

	location = spawnLoc(location, map);   //Finds a free nearby position by jiggling

	if (setLoc(location)) { //This *should* be true because of the previous check

		//all is innited, now add this to the Map's ressource count, and then decide if we spawn another in the cluster
		game->addEnergies(this);

		//now decide if we are spawning another 
		if (((double)rand() / RAND_MAX) >= probSpawn) {
			spawn(map);
		}
		//and we're done, rendering is done by locateable

	}
	else {
		std::cout << "Energy spawned at taken location: (" << location->getPos().x << ", " << location->getPos().y << ")" << std::endl;
		//stop the spawning
	}


}

Energy::~Energy()
{

}

int Energy::getTextLoc()
{
	return textLoc;
}

void Energy::spawn(Map * map)
{
	//Where to spawn this new Energy?
	//Near bias. How near? Variance near?
	glm::vec2 newBias = locBias;
	newBias.x += rand() % HorizVar - (HorizVar / 2);
	newBias.y += rand() % VerticalVar - (VerticalVar / 2);

	Location * newLoc = map->getLoc(newBias);
	Energy * mtn = new Energy(newLoc, map);

}

void Energy::draw(unsigned int texture, GLuint shaderprog)
{

	//send material information to the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Ressource::draw(shaderprog);

}

