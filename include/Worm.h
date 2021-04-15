#pragma once
#include "Unit.h"

#define NUMTEX 9   //# textures available to this class
#define ANIMSTEP 3 //# of textures to skip for next anim state, usually 3

class Worm : public Unit
{
private:
	static constexpr int textLocs[9] = { 22, 23, 24, 25, 26, 27, 28, 29, 30 };    //locations for Worm anim are index 22-30 (ImLoader.cpp)

public:
	Worm(int, Location*, Map* map);
	~Worm();

	void draw(unsigned int, GLuint);
	void update(Map*);

	bool move(Location*, Map*);   // 0- Move
	bool collect(Location *, Map*);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer
	bool build(Location*, int obj);		  // 2- Build obj *string* at x,y
	Unit* spawn(Location*, int obj);   // 3- Spawn a unit *string* at a location
	bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
};

