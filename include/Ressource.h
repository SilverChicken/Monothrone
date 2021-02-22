#pragma once
#include"Locateable.h"

class Map;

class Ressource : public Locateable
{	
protected:                //Mostly variables set for spawning
	glm::vec2 locBias;  //Location bias
	int VerticalVar;    //Vertical Variance
	int HorizVar;		//Horizontal Variance
	double probSpawn;     //How strongly to group with current -> Probability of grouwing -> Geo distro success is stop

	//rendervars
	//const int texLoc = 2;                    //indicates where the tile's texture is within the texture array

	//Helper function to get a spawning location
	Location * spawnLoc(Location* location, Map* map);


public:
	Ressource();
	~Ressource();
	//virtual Location* spawn(); //spawns a new ressource of this type, follows the parameters
	void destroy();
	void draw(GLuint);
};

