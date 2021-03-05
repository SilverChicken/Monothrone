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

	//Helper function to get a spawning location
	Location * spawnLoc(Location* location, Map* map);

	//static Map* mapRef;


public:
	Ressource();
	virtual ~Ressource();

	void destroy();
	
	void draw(GLuint);
	virtual void draw(unsigned int, GLuint);  //This one is for override only
	
	virtual int getTextLoc();
};

