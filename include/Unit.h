#pragma once
#include"Locateable.h"
#include<string>
#include<vector>
#include<list>

#define BINDINGCOUNT 6  //Also defined in Player.cpp


class Map;


class Unit : public Locateable
{
private:
	int owner;
	std::vector<int> abilities; //Each function has an ID, this holds the functions that a given unit implements

	std::list<Location *> path; //could use a stack instead

	bool selected;

protected:
	int hp;
	int atk;
	int speed;

	int textLoc;

	bool actions[BINDINGCOUNT];

public:
	Unit(int, Location*, Map*);
	~Unit();

	int getTexLoc();

	static const int selectLoc = 31; //Location of overlay texture for selected units

	void draw(unsigned int,GLuint);

	void update();

	bool select(int);
	void deselect();
	bool isSelected();

	//ALL possible abilities
	bool* getActions();
	
	bool move(Location *, Map*);   // 0- Move
	
	virtual bool collect(int x, int y);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer

	virtual bool build(int x, int y, std::string obj);		  // 2- Build obj *string* at x,y
	
	virtual Unit* spawn(std::string baby, glm::vec2 place);   // 3- Spawn a unit *string* at a location, checks if free first
	
	virtual bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
	
	
	
};

