#pragma once
#include"Locateable.h"
#include<string>
#include<vector>

class Map;

class Unit : Locateable
{
private:
	int owner;
	int hp;
	int atk;
	int speed;
	std::vector<int> abilities; //Each function has an ID, this holds the functions that a given unit implements

	bool selected;

public:
	Unit(int, Location*, Map*);
	~Unit();

	static const int selectLoc = 31; //Location of overlay texture for selected units

	void draw(GLuint);

	bool select(int);
	void deselect();

	//ALL possible abilities

	
	virtual bool move(int x, int y);   // 0- Move
	
	virtual bool collect(int x, int y);  // 1- Collect something at (x,y)  maybe actually takes a ressource pointer

	virtual bool build(int x, int y, std::string obj);		  // 2- Build obj *string* at x,y
	
	virtual Unit* spawn(std::string baby, glm::vec2 place);   // 3- Spawn a unit *string* at a location, checks if free first
	
	virtual bool consume(Unit* food);						  // 4- Consume a unit to upgrade internally
	
	
	
};

