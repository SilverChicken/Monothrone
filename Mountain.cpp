#include "Mountain.h"
#include "Map.h"
#include <stdlib.h>
#include <time.h>

#include<iostream>

Mountain::Mountain(Location * location, Map* map)
{
	time(NULL);

	glm::vec2 bias = location->getPos();
	locBias = bias;
	VerticalVar = 3;   
	HorizVar = 4;	    
	probSpawn = 1.0 / 35.0;  //E of 1/p to stop spawning 

	//get random texture from array:
	int texInd = rand() % 4;
	textLoc = textLocs[texInd];    // 4 is the size of textLocs

	//Set class identifier
	classType = MOUNTAIN_CLASS_T;
	
	location = spawnLoc(location, map);   //Finds a free nearby position by jiggling

	if (setLoc(location)) { //This *should* be true because of the previous check
		
		//all is innited, now add this to the Map's ressource count, and then decide if we spawn another in the cluster
		game->addMountain(this);

		//now decide if we are spawning another 
		if ( ((double)rand() / RAND_MAX) >= probSpawn) {
			spawn(map);
		}
		//and we're done, rendering is done by locateable

	}
	else {
		std::cout << "Mountain spawned at taken location: (" << location->getPos().x << ", " << location->getPos().y << ")" << std::endl;
		//stop the spawning
	}





}

Mountain::Mountain(Location * location, Map * map, bool single) //The bool doesn't mean anything, it just means we're using this version
{
	//get random texture from array:
	int texInd = rand() % 4;
	textLoc = textLocs[texInd];    // 4 is the size of textLocs

	//Set class identifier
	classType = MOUNTAIN_CLASS_T;


	if (setLoc(location)) { //This *should* be true because of this function should not be called except to fill
		game->addMountain(this);
	}
	else {
		std::cout << "Mountain spawned at taken location: (" << location->getPos().x << ", " << location->getPos().y << ")" << std::endl;
	}

}

Mountain::~Mountain()
{
//	delete [] textLocs;
}

int Mountain::getTextLoc()
{
	return textLoc;
}

void Mountain::spawn(Map * map)
{
	//Where to spawn this new mountain?
	//Near bias. How near? Variance near?
	glm::vec2 newBias = locBias;
	newBias.x += rand() % HorizVar - (HorizVar / 2);
	newBias.y += rand() % VerticalVar - (VerticalVar / 2);

	Location * newLoc = map->getLoc(newBias);
	Mountain * mtn = new Mountain(newLoc , map);
	
}

void Mountain::draw(unsigned int texture, GLuint shaderprog)
{

	//send material information to the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	Ressource::draw(shaderprog);

}





