#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include"Location.h"

#include<vector>
#include<list>
#include<unordered_map>

class Ressource;
class Unit;
class Player;


#define MAPSIZE 64   //Side of map
#define MINCOMP 64   //Minimum Area for a component to not be filled

class Map
{
private:
	int mapSize = MAPSIZE;
	float vertices[12] = {
		-0.98f, -0.98f, 0.0f,
		0.98f, -0.98f, 0.0f,
		0.98f,  0.98f, 0.0f,
		-0.98f, 0.98f, 0.0f

	};
	unsigned int indices[6] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	Location* map[MAPSIZE][MAPSIZE];    //2D array 
	//Location ** map;
	std::vector<Ressource *> Mountains; //Refs to the mountains
	std::vector<Ressource *> Crystals;  //Refs to the crystals
	std::vector<Ressource *> Energies;  //Refs to the energies

	std::unordered_map<int, Unit *> Thrones;
	std::unordered_map<int, Player *> Players;

	


public:
	Map();  //setup drawing of shapes too
	~Map();

	//Initializtion functions
	void init();
	void categorizeAccess(); //gets connected components, fills the smaller ones with locs -> Move it to GameMode


	//gets and sets

	Location* getLoc(glm::vec2);               //2 ways of accessing into the array
	Location* getLoc(int x, int y);

	std::vector<Ressource *> getMountains();
	std::vector<Ressource *> getCrystals();
	std::vector<Ressource *> getEnergies();
	bool addMountain(Ressource *);        //------------------ Mountain
	bool addCrystals(Ressource *);        //------------------ Crystal
	bool addEnergies(Ressource *);        //Called whenever an Energy is Spawned

	Unit* getThrone(int);
	Player* getPlayer(int);
	void addThrone(int, Unit*);
	void addPlayer(int, Player*);


	//render/gameplay fct

	//do we actually need getBox?
	bool ** getBox(glm::vec2, glm::vec2);          //returns the Array of booleans based on whether or not each location is occupied 
	bool ** getBox(int x, int y, int x2, int y2);  //-> the inputs are the size of the box
	//Currently unusused


	bool isAdjacent(Location*, Location*);   //Tells you if A and B are adjacent tiles

	Location* findClosest(Location*);              //-> returns the location of the very closest available point to specified location,
	Location * findClosestRecc(std::unordered_map<Location*, bool> & visited, std::list<Location*> & stack); //  priority is left, right down then up
	
	Location* findClosestTo(Location*, Location*); //finds closest free spot that is closest to the second argument
	Location* findClosestRess(Location *);         //finds nearest collectible ressource to selected location

	void draw();

	GLuint VBO_vert, VBO_norm, VAO, EBO;


};

