#pragma once
#include "Ressource.h"

class Map;

class Mountain : public Ressource
{
private:
	const int textLocs[4] = { 12, 13, 14, 15 };    //locations for mountain are index 12-15 (ImLoader.cpp)
	int textLoc;

public:
	Mountain(Location * location, Map* map);
	~Mountain();
	int getTextLoc();
	void spawn(Map * map); // spawn another mountain near current location
	void draw(unsigned int, GLuint);
};

