#include "Locateable.h"


GLuint Locateable::VBO_vert, Locateable::VAO, Locateable::EBO;

Locateable::Locateable()
{

	if (!inited) { //only needs to happen once
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
	

}

Locateable::~Locateable() //this deletes all objects
{
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO_vert);
	//glDeleteBuffers(1, &EBO);

	if (loc) {
		loc->release(this);
	}
}

void Locateable::cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_vert);
	glDeleteBuffers(1, &EBO);
}

int Locateable::getClassType()
{
	return classType;
}

Location* Locateable::getLoc()
{
	return loc;
}

bool Locateable::setLoc(Location* newLoc)
{
	if (loc != NULL) {
		if (newLoc->state) {              //Like this we never lose ownership of every lock
			if (loc->release(this)) {     //This one failed bc it is not null
				if (newLoc->take(this)) {
					loc = newLoc;
					return true;
				}
			}
		}
		return false;
	}
	else {
		if (newLoc->state) {
			if (newLoc->take(this)) {
				loc = newLoc;
				return true;
			}
		}
		return false;
	}
	
	
}

bool Locateable::swapLoc(Locateable * other)
{
	//disable interrupts?
	//Locateable* tmp = owner;   //No need to take/release
	//owner = swap->getOwner();  //we take theirs
	//swap->owner = tmp;         //they take ours

	//How to tell if smth went wrong?
	//We still need to update the locateable

	bool success = true;

	Location* tmp = other->getLoc();
	Location* tmp2 = loc;
	success = success && other->loc->release(other); 
	other->loc = NULL; //Also give away their location

	success = success && setLoc(tmp);					//Also releases loc;
	success = success && other->setLoc(tmp2);

	return success;  //if false then big bad
}


