#pragma once
#include "Ressource.h"

class Map;

class Energy :
	public Ressource
{
private:
	static constexpr int textLocs[3] = { 16,17,18 };    //locations for mountain are index 12-15 (ImLoader.cpp)
	int textLoc;

public:
	Energy(Location * location, Map* map);
	Energy(Location* location, Map* map, bool single);
	~Energy();
	int getTextLoc();
	void spawn(Map * map); // spawn another mountain near current location
	void draw(unsigned int, GLuint);
};

