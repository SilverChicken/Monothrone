#pragma once
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include<unordered_map>
#include "Timer.h"


#define MAX_PLAYER 3


//Avg # of Mountain, Crystal and Energy clusters -> will be passed in by menu
#define MTNRangeCT 15
#define Vmtn 6
#define NRGRangeCT 6
#define Vnrg 4
#define CRYRangeCT 8
#define Vcry 4

#define MINCOMP 64   //Minimum Area for a component to not be filled


//This class will manage interactions between the following parties.
//Some of these will have a reference to GameMode from which they can call the interactions
//This class will be held by Window which will then only be in charge of managing GLFW and this class
class Player;
class Map;
class Unit;
class Throne;
class Ressource;

class Gamemode
{
private:

	//Timer for tick function
	Timer ticker = Timer::getInstance();

	//Objects were rendering
	Map* map;
	Player * player;
	

	std::unordered_map<int, Unit *> Thrones;
	std::unordered_map<int, Player *> Players;

	std::vector<Ressource *> Mountains; //Refs to the mountains
	std::vector<Ressource *> Crystals;  //Refs to the crystals
	std::vector<Ressource *> Energies;  //Refs to the energies

	Gamemode() {};

public:

	~Gamemode();
	static Gamemode& getInstance();

	void init();
	void categorizeAccess(); //gets connected components, fills the smaller ones with locs
	void SpawnStartRessource(Map * mapping, int MTN, int VarMtn, int NRG, int Varnrg, int CRY, int Varcry);


	std::vector<Ressource *> getMountains();
	std::vector<Ressource *> getCrystals();
	std::vector<Ressource *> getEnergies();
	bool addMountain(Ressource *);        //------------------ Mountain
	bool addCrystals(Ressource *);        //------------------ Crystal
	bool addEnergies(Ressource *);        //Called whenever an Energy is Spawned


	Unit* getThrone(int);
	Player* getPlayer(int);
	void addThrone(int, Unit*);
	void addPlayer(int, Player*);


	void update();
	void draw(GLuint);
	void drawMap();


	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

