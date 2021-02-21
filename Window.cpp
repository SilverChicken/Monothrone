#include "Window.h"
#include "Map.h"
#include "Player.h"
#include "ImLoader.h"
#include <iostream>

#include <stdlib.h>
#include <time.h>

#include "Mountain.h"
#include "Crystal.h"
#include "Energy.h"
#include "Worm.h"
#include "Timer.h"
#include "Throne.h"

const char* window_title = "GLFW Starter Project";

GLint shaderProgram;

//Timer for tick function
Timer ticker = Timer::getInstance();

//Objects were rendering
Map* map;
Player * player;
Throne * throne;
std::vector<Worm*> worms;
unsigned int * ImLoader::textures;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"


//Avg # of Mountain, Crystal and Energy clusters -> will be passed in by menu
#define MTNRangeCT 15
#define Vmtn 6
#define NRGRangeCT 6
#define Vnrg 4
#define CRYRangeCT 8
#define Vcry 4

int Window::width;
int Window::height;

glm::vec3 Window::lastPoint = glm::vec3(0.0, 0.0, 0.0);


//helper functions
void SpawnStartRessource(Map * mapping, int MTN, int VarMtn, int NRG, int Varnrg, int CRY, int Varcry);



void Window::initialize_objects()
{
	glm::vec3 lpos = glm::vec3(8.0f, 6.0f, 6.0f);


	ImLoader::Loadtextures();

	
	
	map = new Map();
	player = new Player(0, map);

	player->setLoc(map->getloc(16,12));
	player->setBotLeft(map->getloc(0, 0));
	//can do that in player too, eventually should.

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]); 
	glActiveTexture(GL_TEXTURE0);

	//setup ressources around map
	SpawnStartRessource(map, MTNRangeCT, Vmtn, NRGRangeCT, Vnrg, CRYRangeCT, Vcry);


	//setup basic units
	//testing out worm spawning
	Worm* worm;
	for (int i = 0; i < 6; i++) {
		worm = new Worm(player->getPID(), map->getloc(2,2), map);
		worms.push_back(worm);
	}

	throne = new Throne(player->getPID(), map->getloc(5,5), map);

	worm->move(map->getloc(20, 15), map);

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(player);
	delete(map);

	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);
}

void Window::idle_callback()
{
	ticker.inc();
	if (ticker.state) { //Update all objects! 
		//Update unit positions who has their references?

		//for now just update worms
		for (Worm* worm : worms) {
			//std::cout << "worm is update" << std::endl;
			worm->update();
		}

	}
}

void Window::display_callback(GLFWwindow* window)
{

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), 0); // set it manually
	glUniform1i(glGetUniformLocation(shaderProgram, "overTex"), 1);

	glUseProgram(shaderProgram);

	//update uniforms

	glUniform1f(glGetUniformLocation(shaderProgram, "renderRout"), 0);

	map->draw();


	glUniform1f(glGetUniformLocation(shaderProgram, "renderRout"), 1);
	
	
	for (Ressource* res : map->getMountains()) {
		Mountain* mtn = (Mountain*)res;
		mtn->draw(ImLoader::textures[mtn->getTextLoc()], shaderProgram);
	}
	for (Ressource* res : map->getCrystals()) {
		Crystal* cry = (Crystal*)res;
		cry->draw(ImLoader::textures[cry->getTextLoc()], shaderProgram);
	}
	for (Ressource* res : map->getEnergies()) {
		Energy* nrg = (Energy*)res;
		nrg->draw(ImLoader::textures[nrg->getTextLoc()], shaderProgram);
	}

	for (std::vector<Worm*>::iterator it = worms.begin(); it != worms.end(); ++it) {
		Worm* worm = *it;
		worm->draw(ImLoader::textures[worm->getTexLoc()], shaderProgram);
	}

	player->draw(ImLoader::textures[player->getTexLoc()], shaderProgram);

	throne->draw(ImLoader::textures[throne->getTexLoc()], shaderProgram);

	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) //Eventually check for non-US layouts
{

	// Check for a key press
	if (action == GLFW_PRESS)
	{
		switch (key) {
		case GLFW_KEY_ESCAPE: // Check if escape was pressed
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_UP:
			//move UP
			player->move(0);
			break;

		case GLFW_KEY_DOWN:
			//move down
			player->move(-1);
			break;

		case GLFW_KEY_RIGHT:
			//move RIGHT
			player->move(1);
			break;

		case GLFW_KEY_LEFT:
			//move LEFT
			player->move(2);
			break;

		case GLFW_KEY_ENTER:
			player->select();
			break;

		case GLFW_KEY_LEFT_SHIFT:
			player->deselect();
			break;

		case GLFW_KEY_N:
			
			std::cout << "Breakpoint" << std::endl;
			player->incEnergy(5);
			break;

		case GLFW_KEY_Q:
			player->actionQ();
			break;

		case GLFW_KEY_W:
			player->actionW();
			break;

		case GLFW_KEY_E:
			player->actionE();
			break;

		case GLFW_KEY_R:
			player->actionR();
			break;

		case GLFW_KEY_T:
			player->actionT();
			break;

		case GLFW_KEY_Y:
			player->actionY();
			break;


		default:
			break;
		}
	}

	if (action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_UP:
			//move UP
			player->move(0);
			break;

		case GLFW_KEY_DOWN:
			//move down
			player->move(-1);
			break;

		case GLFW_KEY_RIGHT:
			//move RIGHT
			player->move(1);
			break;

		case GLFW_KEY_LEFT:
			//move LEFT
			player->move(2);
			break;
		default:
			break;
		}
	}
}

void Window::character_callback(GLFWwindow* window, unsigned int codepoint)
{

}


void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0) {
		player->changeZoom(1.0);
		std::cout << player->getZoom() << std::endl;
	}
	else if (yoffset < 0) {
		player->changeZoom(-1.0);
		std::cout << player->getZoom() << std::endl;
	}

}

void Window::cursor_pos_callback(GLFWwindow * window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	glm::vec3 direction, curPoint;
//	float pixel_diff;
	float rot_angle, velocity;
	curPoint = trackBallMapping(xpos, ypos); // Map the mouse position to a logical sphere location.
	direction = curPoint - Window::lastPoint;
	velocity = glm::length(direction);
	if (velocity > 0.0001) // If little movement - do nothing.
	{
		glm::vec3 rotAxis = cross(Window::lastPoint, curPoint);
		rot_angle = velocity * 0.1f; //cte rotscale?

		Window::lastPoint = curPoint;
	}


}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	if (action == GLFW_PRESS) {
		glfwGetCursorPos(window, &xpos, &ypos);
		Window::lastPoint = trackBallMapping(xpos, ypos);
		glMatrixMode(GL_MODELVIEW); //We're editing the model view

	}

	if (action == GLFW_RELEASE) {
		glfwGetCursorPos(window, &xpos, &ypos);
		lastPoint = trackBallMapping(xpos, ypos);
		//glMatrixMode(0);
	}
}


glm::vec3 Window::trackBallMapping(float x, float y)
{
	glm::vec3 v;
	float d;
	v.x = (2.0f*x - Window::width) / Window::width;
	v.y = (Window::height - 2.0f*y) / Window::height;
	v.z = 0.0;
	d = glm::length(v);
	d = (d < 1.0) ? d : 1.0;
	v.z = sqrtf(1.001 - d * d);
	v = v / glm::length(v); // Still need to normalize, since we only capped d, not v.
	return v;
}






//Initial Spawning Helper functions

void SpawnStartRessource(Map * mapping, int MTN, int VarMtn, int NRG, int Varnrg, int CRY, int Varcry) {

	Ressource * res;
	int count = 0;
	int x, y;
	bool flag = true;

	srand(time(NULL));  // init rand

	count = (rand() % VarMtn) - round(VarMtn / 2) + MTN;             //# of clusters to spawn around MTN
	std::cout << "Mountain cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;                                      //Make more elegant random
		y = rand() % MAPSIZE;
		res = new Mountain(mapping->getloc(x,y), mapping);
	}


	count = (rand() % Varnrg) - round(Varnrg / 2) + NRG;
	std::cout << "Energy cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;
		y = rand() % MAPSIZE;
		res = new Crystal(mapping->getloc(x, y), mapping);
	}

	count = (rand() % Varcry) - round(Varcry / 2) + CRY;
	std::cout << "Energy cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;
		y = rand() % MAPSIZE;
		res = new Energy(mapping->getloc(x, y), mapping);
	}


}