#pragma once
#include"Locateable.h"
#include<vector>
#include<list>

#define ACTIONCOUNT 6  //Also defined in Player.cpp
#define PARTANIMCT 6
#define PARTANIMSTEP 2


class Map;
class Gamemode;


class Unit : public Locateable
{
private:
	int owner;
	
	//Helper Vars for movement and collection
	int moveTries = 0;        //Keeps track of number of times we get stuck in a row
	Location* target;         //Target for movement
	Location* collectTarget;  //Location we are trying to collect from

	const int particleLoc[PARTANIMCT] = {32, 35, 33, 36, 34, 37}; //Location of overlay texture for selected units, will be 6 evens are nrg odd is cryst

	bool selected;



protected:
	int hp;
	int atk;
	int speed;

	Gamemode * game;

	//Anim vars
	int textLoc;  
	int animState = 0;
	int partAnimState = 0;
	
	std::list<Location *> path; //could use a stack instead? or a deque is the one
	 
	//Turn flags to state machine struct that is either idle, moving, collecting, carrying, or building?


	//Collect variables and functions
	bool collecting = false;
	int carrying = 0;           //0- nothing, 3- crystal, 2- energy  
	bool FinishCollect(Map* map);       //Called once the unit has arrived and is adjacent to a collectible

	//Build variables and functions
	bool building = false;
	bool FinishBuild();


	//Spawn variables and functions
	int spawnTimer = 0;


	bool actions[ACTIONCOUNT];

public:
	Unit(int, Location*, Map*);
	virtual ~Unit(); //needs to be virtual but that makes an exception?

	int getTexLoc();

	static const int selectLoc = 31; //Location of overlay texture for selected units

	virtual void draw(unsigned int,GLuint); //if virtual then derived can call without being recast!

	virtual void update(Map*);

	bool select(int);
	void deselect();
	bool isSelected();

	//ALL possible abilities
	bool* getActions();
	
	virtual bool move(Location *, Map*);   // 0- Move
	
	virtual bool collect(Location *, Map*);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer

	virtual bool build(Location *, int obj);		  // 2- Build locateable of classID obj at x,y
	
	virtual void spawn(Location *, int obj);   // 3- Spawn a unit *string* at a location, must be fully imp by child
	
	virtual bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
	
	
	
};

