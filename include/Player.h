#pragma once

#include"Locateable.h"  
#include<vector>
#include<list>
#include<map>


#define ZOOMDEF 20
#define GUISPACE 4
#define MAXZOOM 4
#define MINZOOM -4


#define KEY_UP     0
#define KEY_DOWN   -1
#define KEY_RIGHT  1
#define KEY_LEFT   2

#define MOVE_LOC     0
#define COLLECT_LOC  1
#define BUILD_LOC    2
#define SPAWN_LOC    3
#define CONSUME_LOC  4


class Map;
class Unit;


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
	std::map<Unit*, bool> selection;   //  map of selected units, subset of units map

	//Vector of all units you own? Or set Or Hashmap?
	std::map<Unit*, bool> units;

	//array of functions that are bound to AZERTY how it it set? Selection. When Selection is updated we iterate through it
	int* bindings;


	//rendering Vars
	const int textLocs[3] = { 0, 1, 2 };

	int texLoc;                    //indicates where the tile's texture is within the texture array
	float zoom;
	int camBoxX;                       //indicates when we move the camera as distance from center
	int camBoxY;
	Location* botLeft;                 //location of bottom left corner of the screen -> 0,0 for the rendering
	std::vector<Location*> vision;     //locations that player can see
	std::vector<Location*> clouds;     //Where the fog of war is


public:
	Player(int, Location*, Map *);
	~Player();

	//gets & sets
	int getPID();
	const int getTexLoc();
	Location* getLoc();
	float getZoom();
	bool setLoc(Location*);
	bool setLoc(int, int);  //overloads for coordinates
	bool setLoc(glm::vec2);

	int getCrystal();
	int getEnergy();

	//Unit management
	std::map<Unit*, bool> getSelection();
	void update();

	//Pauses the game(for now)
	bool pause = false;
	

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

	template <class T> 
	Unit* spawnUnit(Location * location); 



	//Action button
	void actionKey(int);  //bindings: Q-0; W-1; E-2; R-3; T-4; Y-5


	
	//render fcts
	bool addVision(Location*);
	bool removeVision(Location*);
	bool removeCloud(Location*);
	float changeZoom(float);
	bool cull(Location *);        //Says if the object at location is on screen
	void draw(unsigned int*, GLuint);

private:

	//Private gameplay fct
	void updateBindings();
	void addUnit(Unit*);

	//private render fcts
	void setBotLeft(Location*);
	void setBotLeft(int, int);
	void setBotLeft(glm::vec2);

	bool checkCameraChange();     //will also change the camera if necessary
	

};




//Needs to be defined here because it's a template function
template<class T>
inline Unit * Player::spawnUnit(Location * location)
{
	Unit * unit = new T(PID, location, map);
	units[unit] = false;                       //Adds the unit to the list of current units
	return unit;

}
