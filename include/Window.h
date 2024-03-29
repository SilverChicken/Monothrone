#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
//#include "Cube.h"
#include "shader.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define WINSIZE 640

class Window
{
public:
	static int width;
	static int height;

	static glm::vec3 lastPoint;


	static void initialize_objects();
	static void clean_up();

	static GLFWwindow* create_window(int width, int height);

	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void character_callback(GLFWwindow * window, unsigned int codepoint);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);

	static glm::vec3 trackBallMapping(float x, float y);
};

#endif
