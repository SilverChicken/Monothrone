#include "Wall.h"
#include "include\Wall.h"

#include "Map.h"

Wall::Wall(int own, Location * location, Map* map)
{
	owner = own;
	setLoc(map->findClosest(loc));
}

Wall::~Wall()
{
}

void Wall::destroy()
{
}

void Wall::draw(GLuint)
{

}

int Wall::getTextLoc()
{
	return textLoc;
}
