#pragma once
#include"Location.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>


#define MINUNITCLASST 4  //All locateables with class type >= to this are units
#define MAXRESCLASST 3   //All locateables with class type <= to this are ressources

#define LOCATEABLE_CLASS_T  0
#define MOUNTAIN_CLASS_T    1
#define ENERGY_CLASS_T      2
#define CRYSTAL_CLASS_T     3
#define UNIT_CLASS_T        4
#define THRONE_CLASS_T      5
#define REFACTORY_CLASS_T   6
#define MANUFACTORY_CLASS_T 7
#define WORM_CLASS_T        8
#define WORKER_CLASS_T      9


static const float vertices[32] = {                          //needs to change with zoom? or changed in shader
		// positions          // texture coords
		 0.1f,  0.1f, 0.0f,   1.0f, 1.0f,   // top right
		 0.1f,  0.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		 0.0f,  0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
		 0.0f,  0.1f, 0.0f,   0.0f, 1.0f    // top left 
};
static unsigned int indices[6] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

static bool inited = false; //Only send data to OpenGl once for all locateables?



class Locateable
{
protected:
	Location* loc;

	int classType = 0; //type flag that all subs will inherit -> typeOf; Make into CONSTANTS eventually reorganize order
	// 0 - Just locateable
	// 1 - Mountain
	// 2 - Energy
	// 3 - Crystal
	// 4 - Unit
	// 5 - Throne
	// 6 - Refactory
	// 7 - Manufactory
	// 8 - Worm
	// 9 - Worker
	//
	//


	

	static GLuint VBO_vert, VAO, EBO;

	

public:
	Locateable();
	virtual ~Locateable();

	static void cleanup(); //only call once

	int getClassType();
	Location* getLoc();                 //Only works for non-overlap, for overlap define own function		
	bool setLoc(Location* newLoc);      //logic here makes sure it does not have 2 locations at once

	bool swapLoc(Locateable*);               //Swaps 2 Locateabes' locs should be threadsafe

};

