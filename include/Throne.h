#pragma once
#include "Unit.h"



class Throne : public Unit
{
private:
	const int textLocs[3] = { 6, 7, 8 };    //locations for mountain are index 12-15 (ImLoader.cpp)
	
	//Player* playerRef;

public:
	Throne(int, Location*, Map* map);
	~Throne();

	//Interfacing with Player
	//void incRessource(int);



	//Unit overrides

	void draw(unsigned int, GLuint);

	bool move(Location*, Map*);						  // 0- Move to Location
	bool collect(Location *, Map*);					  // 1- Collect something at Location
	bool build(Location*, int obj);		  // 2- Build obj *string* at x,y
	Unit* spawn(Location *, int obj);   // 3- Spawn a unit *string* at a location
	bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
};

