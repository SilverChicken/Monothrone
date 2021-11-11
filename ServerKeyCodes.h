#pragma once


/* The unknown key */
#define GLFW_KEY_UNKNOWN            -1
#define GLFW_KEY_NOINPUT            GLFW_KEY_UNKNOWN


//These are consistent with the ACTION_LOC constants defined in Player
#define CMD_MOVE			6
#define CMD_COLLECT			1
#define CMD_BUILD			2
#define CMD_SPAWN			3
#define CMD_CONSUME			4

#define CMD_SELECT			5
#define CMD_DESELECT		10
#define CMD_DESELECT_ALL	7
#define CMD_PAUSE			8
#define CMD_LEAVE			9



#define CMD_MAXCODE			10