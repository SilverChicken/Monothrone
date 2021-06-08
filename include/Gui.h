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
#include <vector>

#define BINDINGCOUNT 6
#define UNITCOUNT 15     //Mirror MAX_CLASS_T

#define BUTTONCOUNT 4

class Gui {

private:

	//float toolHeight = 10;
	const std::string binds[BINDINGCOUNT] = { "Move", "Collect", "Build","Spawn","Consume", "Action" }; //Action names

	const std::string units[UNITCOUNT] = {" ", "Mountain", "Energy", "Crystal", "Unit",
									"Throne", "Refractory", "Manufactory", "Worm", "Worker" }; // index is Class_T

	const int textLocs[2] = { 47, 48 }; //first is nrg second is crystal

	char* blank = NULL; //Use this to print blank spaces
	char* Keybind[BINDINGCOUNT] = {NULL, NULL, NULL, NULL, NULL, NULL };
	
	std::vector<char*> UnitBind;

	char* crystal = nullptr;
	char* energy = nullptr; //saved so we don't have to convert each draw
	//unitList units;

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

	void renderSetup(); //for init
	GLuint VBO_vert, VAO, EBO;

	FontLoader Font;


	void init();

public:
	Gui();
	~Gui();

	void setCrystal(int);
	void setEnergy(int);
	void setBinds(int*); //Sets the curent bindings takes the bindings array from Player
	void setUnits(std::vector<int>&);    

	FontLoader* getFontRef();

	//Render

	void draw(); //Draws the text (must have shader disabled)
	//void drawMenu();
	void drawIcons(GLuint, float xpos, float ypos); //Draws the rest (this needs shader!)

};