#include "Gui.h"
#include "include\Gui.h"

#include "ImLoader.h"

#include <string.h>



static const float vertices[32] = {                          //For render the tile icons
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



void Gui::init()
{

	renderSetup();

	setCrystal(0);  //needs to set once to initialize the render vars
	setEnergy(0);

	blank = new char(' ');
	//Init the binding strings so we have something to print, is already done with setting blank
	Keybind[0] = blank;
	Keybind[1] = blank;
	Keybind[2] = blank;
	Keybind[3] = blank;
	Keybind[4] = blank;
	Keybind[5] = blank;


	//Set up the Font
	if (!Font.Load("fonts/Small_Fonts.bff")) {
		std::cout << "Font failed to Load" << std::endl;
	}
	Font.SetColor(0.9f, 0.9f, 0.9f); //default white

}

Gui::Gui()
{
	init();
}

Gui::~Gui()
{
	delete blank;
}

void Gui::setCrystal(int newCry)
{
	std::string c = std::to_string(newCry);
	if (crystal) {
		delete[] crystal;
	}
	crystal = new char[c.size() + 1];
	strcpy_s(crystal, c.size() + 1, c.c_str());
}

void Gui::setEnergy(int newEne)
{
	std::string e = std::to_string(newEne);
	if (energy) {
		delete[] energy;
	}
	energy = new char[e.size() + 1];
	strcpy_s(energy, e.size() + 1,e.c_str());
}

void Gui::setBinds(int * pBinds)
{
	//Check that pBinds has size of player bindings
	//if (sizeof(pBinds) / sizeof(int) != BINDINGCOUNT) {
	//	return;
	//}

	//So that check isn't great since it fails since we just pass one element


	//So pBinds is an array of size 6, and they are ordered QWERTY, with an in holding the index of Move, Count, etc
	for (int i=0; i < BINDINGCOUNT; i++) {
		if (pBinds[i] == -1) { //Then it's an empty value
			if (Keybind[i] != blank) {
				delete[] Keybind[i];
				Keybind[i] = blank;
			}
		}
		else {
			if (Keybind[i] != blank) {
				delete[] Keybind[i];
			}
			if (pBinds[i] >= 0 && pBinds[i] < BINDINGCOUNT) { //check it won't make us go out of bounds
				std::string str = binds[pBinds[i]]; //Gets the correct name from pBinds
				Keybind[i] = new char[str.size() + 1];
				strcpy_s(Keybind[i], str.size() + 1, str.c_str()); //Copy it into Keybind
			}
			

		}
	}


}

void Gui::setUnits(std::vector<int>& unitsInSel)
{
	UnitBind.clear();

	std::string name;
	char* Cname;

	for (int Ct : unitsInSel) { //Assume Unique
		name = units[Ct];
		Cname = new char[name.size() + 1];
		strcpy_s(Cname, name.size() + 1, name.c_str());
		UnitBind.push_back(Cname);
	}
	
}

FontLoader* Gui::getFontRef()
{
	return &Font;
}

void Gui::draw()
{
	//draw everything static first then add unit count,, type, bindings and all

	//Draw the text!
	
	Font.SetScreen(WINSIZE,WINSIZE);
	Font.Select();
	Font.SetColor(0.9f, 0.9f, 0.9f);
	//Write in terms of WINSIZE? No, be globals that are set by a switch on RESOLUTION set in menus

	Font.Print((char*)"Units: ", 15, 85);   

	//Draw the unit names
	Font.SetCursor(60, 85);
	int unitSize = UnitBind.size();
	for (int i = 0; i != unitSize; i++) {
		//Print each word, the cursor auto updates
		char* word = UnitBind[i];
		Font.Print(word);

		//add , and space if not last
		if (i != unitSize - 1) {
			Font.Print((char*)", ");
		}
	}





	Font.Print((char*)"Q- ", 10, 55);
	Font.Print((char*)"W- ", 160, 55);
	Font.Print((char*)"E- ", 310, 55);
	Font.Print((char*)"R- ", 10, 30);
	Font.Print((char*)"T- ", 160, 30);
	Font.Print((char*)"Y- ", 310, 30);


	//draw the names of the buttons
	Font.Print(Keybind[0], 30, 55);
	Font.Print(Keybind[1], 185, 55);
	Font.Print(Keybind[2], 330, 55);
	Font.Print(Keybind[3], 30, 30);
	Font.Print(Keybind[4], 180, 30);
	Font.Print(Keybind[5], 330, 30);



	//draw the ressource counts

	
	Font.Print( crystal, 500, 70);
	Font.Print( energy, 500, 26);

}

void Gui::drawIcons(GLuint shaderprog, float xpos, float ypos)
{
	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), xpos + 14, ypos - 1.5);  //Make const and zoom mod?

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ImLoader::textures[textLocs[0]]);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUniform2f(glGetUniformLocation(shaderprog, "location"), xpos + 14, ypos  - 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ImLoader::textures[textLocs[1]]);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Gui::renderSetup() {
	//rendering

// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_vert);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vert);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		5 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//here we set the material for each object
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));



	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);


	glGetError();
}