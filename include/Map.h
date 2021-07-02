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

//class Ressource;
//class Unit;
//class Player;


#define MAPSIZE 64   //Side of map

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

	bool isPop = false;

public:
	Map();  //setup drawing of shapes too
	~Map();

	//Initializtion functions
	void init();


	//gets and sets
	bool getIsPop();
	void setIsPop(bool val);
	Location* getLoc(glm::vec2);               //2 ways of accessing into the array
	Location* getLoc(int x, int y);

	bool isAdjacent(Location*, Location*);   //Tells you if A and B are adjacent tiles

	Location* findClosest(Location*);              //-> returns the location of the very closest available point to specified location,
	//  priority is left, right down then up
	Location* findClosestTo(Location*, Location*); //finds closest free spot stating at first that is closest to the second argument

	//Write bounded versions for bounded search
	Location* findClosest(Location*, int bound);              //Same as findClosest but search bounded to 'bound' steps
	Location* findClosestTo(Location*, Location*, unsigned int bound); 

	void draw();

	GLuint VBO_vert, VBO_norm, VAO, EBO;


};

