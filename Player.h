#pragma once

#include"Locateable.h"  //This one is optional
#include"Unit.h"
#include<vector>
#include<list>


#define ZOOMDEF 20

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
	std::list<Unit*> selection;


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
	bool setLoc(Location*);
	bool setLoc(int, int);  //overloads for coordinates
	bool setLoc(glm::vec2);

	std::list<Unit*> getSelection();

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

