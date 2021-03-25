#pragma once
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "FontLoader.h"
#include <string>

#define BINDINGCOUNT 6

struct unitList { //1 var for each unit type
	bool worm = 0;
	bool worker = 0;
	bool throne = 0;
	bool refrac = 0;
	bool manufac = 0;
};

class Gui {

private:
	//float toolHeight = 10;
	const std::string binds[BINDINGCOUNT] = { "Move", "Collect", "Build","Spawn","Consume", "Action" }; //Action names

	const int textLocs[2] = { 47, 48 }; //first is nrg second is crystal

	char* blank; //Use this to print blank spaces
	char* Keybind[BINDINGCOUNT] = {NULL, NULL, NULL, NULL, NULL, NULL };
	
	

	char* crystal;
	char* energy; //saved so we don't have to convert each draw
	unitList units;

	float vertices[32] = {  //Just for last part of the Gui, the crystal & evergy count                        
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

	FontLoader Font;


	void init();

public:
	Gui();
	~Gui();

	void setCrystal(int);
	void setEnergy(int);
	void setBinds(int*); //Sets the curent bindings takes the bindings array from Player
	void setUnits();    


	//Render

	void draw(); //Draws the text (must have shader disabled)
	void drawIcons(); //Draws the rest (this needs shader!)


};