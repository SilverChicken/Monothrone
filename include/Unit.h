#pragma once
#include"Locateable.h"
#include<string>
#include<vector>
#include<list>

#define ACTIONCOUNT 6  //Also defined in Player.cpp
#define PARTANIMCT 3
#define PARTANIMSTEP 1


class Map;


class Unit : public Locateable
{
private:
	int owner;

	int moveTries = 0;          //Keeps track of number of times we get stuck in a row

	const int particleLoc[3] = {32, 33, 34}; //Location of overlay texture for selected units, will be 6 evens are nrg odd is cryst

	bool selected;

protected:
	int hp;
	int atk;
	int speed;


	//Anim vars
	int textLoc;  
	int animState = 0;
	int partAnimState = 0;
	
	std::list<Location *> path; //could use a stack instead
	 

	bool collecting = false;
	int carrying = 0;           //0- nothing, 1- crystal, 2- energy  maybe all we need is bool?
	bool FinishCollect();       //Called once the unit has arrived and is adjacent to a collectible

	bool actions[ACTIONCOUNT];

public:
	Unit(int, Location*, Map*);
	~Unit();

	int getTexLoc();

	static const int selectLoc = 31; //Location of overlay texture for selected units

	void draw(unsigned int,GLuint); //if virtual then derived can call without being recast!

	virtual void update(Map*);

	bool select(int);
	void deselect();
	bool isSelected();

	//ALL possible abilities
	bool* getActions();
	
	virtual bool move(Location *, Map*);   // 0- Move
	
	virtual bool collect(Location *, Map*);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer

	virtual bool build(int x, int y, std::string obj);		  // 2- Build obj *string* at x,y
	
	virtual Unit* spawn(std::string baby, glm::vec2 place);   // 3- Spawn a unit *string* at a location, checks if free first
	
	virtual bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
	
	
	
};

