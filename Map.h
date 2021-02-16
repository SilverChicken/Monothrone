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


class Ressource;


#define MAPSIZE 64

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
	std::vector<Ressource *> Mountains; //Locations of the mountains
	std::vector<Ressource *> Crystals;  //Locations of the crystals
	std::vector<Ressource *> Energies;  //Locations of the energies
public:
	Map();  //setup drawing of shapes too
	~Map();
	void init();

	//gets and sets

	Location* getloc(glm::vec2);               //2 ways of accessing into the array
	Location* getloc(int x, int y);

	std::vector<Ressource *> getMountains();
	std::vector<Ressource *> getCrystals();
	std::vector<Ressource *> getEnergies();
	bool addMountain(Ressource *);
	bool addCrystals(Ressource *);
	bool addEnergies(Ressource *);


	//render/gameplay fct

	//do we actually need getBox?
	bool ** getBox(glm::vec2, glm::vec2);          //returns the Array of booleans based on whether or not each location is occupied 
	bool ** getBox(int x, int y, int x2, int y2);  //-> the inputs are the size of the box
	Location* findClosest(Location*);              //-> returns the location of the very closest available point to specified location,
	Location * findClosestRecc(Location * base, std::list<Location*> & visited, std::list<Location*> & stack); //  priority is left, right down then up
	void draw();

	GLuint VBO_vert, VBO_norm, VAO, EBO;



};

