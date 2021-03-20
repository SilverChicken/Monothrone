#pragma once
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "FontLoader.h"


class Gui {

private:
	//float toolHeight = 10;

	float vertices[32] = {                          
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

	void draw();


};