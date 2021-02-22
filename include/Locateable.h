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


class Locateable
{
protected:
	Location* loc;

	int classType = 0; //type flag that all subs will inherit -> typeOf; Make into CONSTANTS eventually
	// 0 - Just locateable
	// 1 - Mountain
	// 2 - Energy
	// 3 - Crystal
	// 4 - Unit
	// 5 - Throne
	// 6 - Refactory
	// 7 - Manufactory
	// 8 - Worm
	// 
	//
	//


	float vertices[32] = {                          //needs to change with zoom? or changed in shader
		// positions          // texture coords
		 0.1f,  0.1f, 0.0f,   1.0f, 1.0f,   // top right
		 0.1f,  0.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		 0.0f,  0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
		 0.0f,  0.1f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[6] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	GLuint VBO_vert, VAO, EBO;

	

public:
	Locateable();
	~Locateable();
	int getClassType();
	Location* getLoc();                 //Only works for non-overlap, for overlap define own function		
	bool setLoc(Location* newLoc);      //logic here makes sure it does not have 2 locations at once
};

