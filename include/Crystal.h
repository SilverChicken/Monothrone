#pragma once
#include "Ressource.h"

class Map;

class Crystal :	public Ressource
{
private:
	static constexpr int textLocs[3] = { 19, 20, 21 };    //locations for crystal are index 19-21 (ImLoader.cpp)
	int textLoc;

public:
	Crystal(Location * location, Map* map);
	~Crystal();
	int getTextLoc();
	void spawn(Map * map); // spawn another mountain near current location
	void draw(unsigned int, GLuint);
};

