#pragma once

#include"Locateable.h"  
#include<vector>
#include<list>
#include<map>


#define ZOOMDEF 20
#define MAXZOOM 4
#define MINZOOM -4

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


	//Unit management
	std::map<Unit*, bool> getSelection();
	void addUnit(Unit*);
	void update();

	

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
	void draw(unsigned int*, GLuint);


};

template<class T>
inline Unit * Player::spawnUnit(Location * location)
{
	Unit * unit = new T(PID, location, map);
	units[unit] = false;                       //Adds the unit to the list of current units
	return unit;

}
