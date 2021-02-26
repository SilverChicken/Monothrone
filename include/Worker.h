#pragma once
#include "Unit.h"

#define NUMTEX 9   //# textures available to this class
#define ANIMSTEP 3 //# of textures to skip for next anim state, usually 3

class Worker :	public Unit
{
private:
	const int textLocs[9] = { 38, 39, 40, 41, 42, 43, 44, 45, 46 };    //locations for worker are index 38-46 (ImLoader.cpp)

public:
	Worker(int, Location*, Map* map);
	~Worker();

	void draw(unsigned int, GLuint);
	void update(Map*);

	bool move(Location*, Map*);   // 0- Move
	bool collect(Location *, Map*);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer
	bool build(int x, int y, std::string obj);		  // 2- Build obj *string* at x,y
	Unit* spawn(std::string baby, glm::vec2 place);   // 3- Spawn a unit *string* at a location
	bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally

};

