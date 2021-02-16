#include "Ressource.h"

Ressource::Ressource()
{

}

Ressource::~Ressource()
{

}

void Ressource::draw(GLuint shaderprog)
{
	
	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);

	glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 0.0f);

	//std::cout << topLeft->getPos().x << "   " << topLeft->getPos().y << std::endl;
	//std::cout<< loc->getPos().x << "   " << loc->getPos().y << std::endl;


	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
