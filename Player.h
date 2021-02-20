#pragma once

#include"Locateable.h"  //This one is optional
#include"Unit.h"
#include<vector>
#include<list>
#include<map>


#define ZOOMDEF 20
#define MAXZOOM 4
#define MINZOOM -4

class Map;


class Player : Locateable
{
private:
	int PID;
	Map* map;

	//Gameplay Vars
	int energy;
	int crystal;
	bool hasThrone;
	//std::list<Unit*> selection;
	std::map<Unit*, bool> selection;   //  map of all units you own, bool true if selected

	//array of functions that are bound to AZERTY how it it set? Selection. When Selection is updated we iterate through it
	int* bindings;



	//rendering Vars

	const int texLoc = 1;                    //indicates where the tile's texture is within the texture array
	float zoom;
	int camBoxX;                       //indicates when we move the camera as distance from center
	int camBoxY;
	Location* botLeft;                 //location of bottom left corner of the screen -> 0,0 for the rendering
	std::vector<Location*> vision;     //locations that player can see
	std::vector<Location*> clouds;     //Where the fog of war is


public:
	Player(int , Map *);
	~Player();

	//gets & sets
	int getPID();
	const int getTexLoc();
	Location* getLoc();
	float getZoom();
	bool setLoc(Location*);
	bool setLoc(int, int);  //overloads for coordinates
	bool setLoc(glm::vec2);

	std::map<Unit*, bool> getSelection();
	//std::list<Unit*> getSelection();

	//Gameplay fcts
	bool move(int);
	bool select();
	Unit* deselect();
	void deselectAll();
	bool checkHasThrone();
	bool incEnergy(int);
	bool incCrystal(int);
	bool decEnergy(int);
	bool decCrystal(int);

	//Action buttons
	void actionQ();
	void actionW();
	void actionE();
	void actionR();
	void actionT();
	void actionY();

	//render fcts
	void setBotLeft(Location*);
	void setBotLeft(int, int);
	void setBotLeft(glm::vec2);

	bool checkCameraChange();     //will also change the camera if necessary

	bool addVision(Location*);
	bool removeVision(Location*);
	bool removeCloud(Location*);
	float changeZoom(float);
	void draw(unsigned int, GLuint);


};

