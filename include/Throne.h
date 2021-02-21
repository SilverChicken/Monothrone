#pragma once
#include "Unit.h"
class Throne : public Unit
{
private:
	const int textLocs[3] = { 6, 7, 8 };    //locations for mountain are index 12-15 (ImLoader.cpp)
	int textLoc;
public:
	Throne(int, Location*, Map* map);
	~Throne();

	void draw(unsigned int, GLuint);

	int getTexLoc();

	bool move(Location*, Map*);   // 0- Move
	bool collect(int x, int y);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer
	bool build(int x, int y, std::string obj);		  // 2- Build obj *string* at x,y
	Unit* spawn(std::string baby, glm::vec2 place);   // 3- Spawn a unit *string* at a location
	bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
};

