#include "Window.h"
#include <iostream>

#include "Gamemode.h"


#include "Map.h"
#include "Player.h"
#include "ImLoader.h"


#include <stdlib.h>
#include <time.h>


const char* window_title = "GLFW Starter Project";

GLint shaderProgram;

Gamemode* game = &Gamemode::getInstance();


//Timer for tick function
Timer ticker = Timer::getInstance();

//Objects were rendering
//Map* map;
//Player * player;


// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"

int Window::width;
int Window::height;

//glm::vec3 Window::lastPoint = glm::vec3(0.0, 0.0, 0.0);



void Window::initialize_objects()
{
	//glm::vec3 lpos = glm::vec3(8.0f, 6.0f, 6.0f);


	game->init(); //initialize Gamemode
	

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
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
	game->update();
}

void Window::display_callback(GLFWwindow* window)
{

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), 0); //Set the texture Locations
	glUniform1i(glGetUniformLocation(shaderProgram, "overTex"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "particle"), 2);

	glUseProgram(shaderProgram);

	//update uniforms

	glUniform1f(glGetUniformLocation(shaderProgram, "renderRout"), 0);

	game->drawMap();


	glUniform1f(glGetUniformLocation(shaderProgram, "renderRout"), 1);
	
	
	game->draw(shaderProgram);


	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) //Eventually check for non-US layouts
{
	game->key_callback(window, key, scancode, action, mods);
}

void Window::character_callback(GLFWwindow* window, unsigned int codepoint)
{

}


void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0) {
		//player->changeZoom(1.0);
		//std::cout << player->getZoom() << std::endl;
	}
	else if (yoffset < 0) {
		//player->changeZoom(-1.0);
		//std::cout << player->getZoom() << std::endl;
	}

}

void Window::cursor_pos_callback(GLFWwindow * window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	/*
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
	*/

}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	/*
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
	}*/
}


glm::vec3 Window::trackBallMapping(float x, float y)
{
	glm::vec3 v = glm::vec3();
	/*
	double d;
	v.x = (2.0f*x - Window::width) / Window::width;
	v.y = (Window::height - 2.0f*y) / Window::height;
	v.z = 0.0;
	d = glm::length(v);
	d = (d < 1.0) ? d : 1.0;
	v.z = float(sqrtf(1.001 - d * d));
	v = v / glm::length(v); // Still need to normalize, since we only capped d, not v.
	*/
	return v;
}
