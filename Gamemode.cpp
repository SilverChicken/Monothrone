#include "Gamemode.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "Map.h"

#include "Player.h"
#include "Throne.h"
#include "Worm.h"


#include "Ressource.h"
#include "Mountain.h"
#include "Energy.h"
#include "Crystal.h"

#include "ImLoader.h"


unsigned int * ImLoader::textures; //The static array from ImLoader


bool vecSearch(Location * location, std::vector<Location*> & visited);  //Just loops from the back to the front, more likely to find


Gamemode::~Gamemode()
{
//delete all the things

	for (int i = 0; i < MAX_PLAYER; i++) {
		if (Thrones.count(i)) {  //Get Thrones & Players
			delete Thrones[i];
		}
		if (Players.count(i)) {
			delete Players[i];    //This destructor will propagate to Units of Player
		}
	}

	delete(map);


}

Gamemode & Gamemode::getInstance()
{
	static Gamemode instance;
	return instance;
}

void Gamemode::init()
{
	ImLoader::Loadtextures();

	map = new Map();
	player = new Player(2, map->getLoc(7, 7), map);

	//can do that in player too, eventually should.

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]);
	glActiveTexture(GL_TEXTURE0);

	//setup ressources around map
	SpawnStartRessource(map, MTNRangeCT, Vmtn, NRGRangeCT, Vnrg, CRYRangeCT, Vcry);


	//setup basic units
	//testing out worm spawning


	//Throne MUST be spawned first
	Unit* throne = player->spawnUnit<Throne>(map->getLoc(5, 5));

	for (int i = 0; i < 6; i++) {
		player->spawnUnit<Worm>(map->getLoc(5, 5));
	}


}

void Gamemode::categorizeAccess()
{
	//get vector of maps each map will rpz a connected component
	std::vector<std::unordered_map<Location*, bool>* > components; //vector of map pointers
	std::unordered_map<Location*, bool> freeLocs;
	std::vector<Location*> stack;



	Location* loc;
	glm::vec2 pos;

	//fill the free locations
	for (int i = 0; i < MAPSIZE; i++) {
		for (int j = 0; j < MAPSIZE; j++) {
			loc = map->getLoc(i,j);
			if (loc->state) {
				freeLocs[loc] = true;
			}
		}
	}

	while (!freeLocs.empty()) {
		std::unordered_map<Location*, bool> * comp = new std::unordered_map<Location*, bool>();

		glm::vec2 dirs[4] = { glm::vec2(1.0, 0.0) , glm::vec2(-1.0, 0.0) , glm::vec2(0.0, 1.0) , glm::vec2(0.0, -1.0) };

		loc = freeLocs.begin()->first;  //take the first free element
		stack.push_back(loc);			//Add it to the empty stack



		while (!stack.empty()) {  //Conduct a DFS search at a random location not logged location

			loc = stack.back();
			stack.pop_back();

			comp->emplace(loc, true);	//Add to connected component
			freeLocs.erase(loc);		//Remove from free locations



			pos = loc->getPos();

			for (glm::vec2 dir : dirs) { //Add free neighbors to stack 
				loc = map->getLoc(pos + dir);

				//check that loc is not on stack and that it is free
				if (freeLocs.count(loc) && !vecSearch(loc, stack)) {
					stack.push_back(loc);
				}
			}
		} //Continue until we have exhausted the component

		//So now we have a complete connected component!
		components.push_back(comp); //Add the component to the vector!

		//start again while freeLocs is still not empty!

	}

	//All free locations have been classified into components

	for (std::unordered_map<Location*, bool>* comp : components) {
		if (comp->size() < MINCOMP) { //Then we fill
			for (auto it : *comp) {
				//Call Gamemode spawn function to get a Mountain
				Ressource * mtn = new Mountain(it.first, map, true);
			}
		}
	}

	//count its total size in a hashmap? map.size()s

	//Go through list of maps, get the count for each. either get max and fill others or fill all that have size less than some CONSNTAT

	//For each map in list, if it is below criterion, for each element we creat MOUNTAIN or LOCATEABLE inside that takes the lock.



}

void Gamemode::SpawnStartRessource(Map * mapping, int MTN, int VarMtn, int NRG, int Varnrg, int CRY, int Varcry)
{
	Ressource * res;
	int count = 0;
	int x, y;
	bool flag = true;

	srand(time(NULL));  // init rand

	count = int((rand() % VarMtn) - round(VarMtn / 2) + MTN);             //# of clusters to spawn around MTN
	std::cout << "Mountain cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;                                      //Make more elegant random
		y = rand() % MAPSIZE;
		res = new Mountain(mapping->getLoc(x, y), mapping);
	}


	//Now we categorize access!
	categorizeAccess();



	count = int((rand() % Varnrg) - round(Varnrg / 2) + NRG);
	std::cout << "Energy cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;
		y = rand() % MAPSIZE;
		res = new Crystal(mapping->getLoc(x, y), mapping);
	}

	count = int((rand() % Varcry) - round(Varcry / 2) + CRY);
	std::cout << "Energy cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;
		y = rand() % MAPSIZE;
		res = new Energy(mapping->getLoc(x, y), mapping);
	}


}

std::vector<Ressource*> Gamemode::getMountains()
{
	return Mountains;
}

std::vector<Ressource*> Gamemode::getCrystals()
{
	return Crystals;
}

std::vector<Ressource*> Gamemode::getEnergies()
{
	return Energies;
}

bool Gamemode::addMountain(Ressource * res)
{
	if (res->getClassType() == MOUNTAIN_CLASS_T) {
		Mountains.push_back(res);
		return true;
	}
	return false;
}

bool Gamemode::addCrystals(Ressource * res)
{
	if (res->getClassType() == CRYSTAL_CLASS_T) {
		Crystals.push_back(res);
		return true;
	}
	return false;
}

bool Gamemode::addEnergies(Ressource * res)
{
	if (res->getClassType() == ENERGY_CLASS_T) {
		Energies.push_back(res);
		return true;
	}
	return false;
}

Unit * Gamemode::getThrone(int own)
{
	return Thrones[own];
}

Player * Gamemode::getPlayer(int own)
{
	return Players[own];
}

void Gamemode::addThrone(int own, Unit * throne)
{
	if (throne->getClassType() == THRONE_CLASS_T) { //Check that it's actually a throne
		Thrones[own] = throne;
	}
}

void Gamemode::addPlayer(int own, Player * player)
{
	Players[own] = player;
}

void Gamemode::update()
{
	ticker.inc();
	if (ticker.state) { //Update all objects! 
		//Update unit positions mostly and anim?

		//Player will cascade the update to all its units
		player->update();

	}
}

void Gamemode::draw(GLuint shaderProgram)
{

	//Currently as virtual call the derived function which then calls the ressource draw with 1 param 
	for (Ressource* res : Mountains) {
		res->draw(ImLoader::textures[res->getTextLoc()], shaderProgram);  
	}
	for (Ressource* res : Crystals) {
		res->draw(ImLoader::textures[res->getTextLoc()], shaderProgram);
	}
	for (Ressource* res : Energies) {
		res->draw(ImLoader::textures[res->getTextLoc()], shaderProgram);
	}

	player->draw(ImLoader::textures, shaderProgram);

}

void Gamemode::drawMap()
{
	map->draw();
}

void Gamemode::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
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

		case GLFW_KEY_LEFT_CONTROL:
			player->deselectAll();
			break;

		case GLFW_KEY_N:

			std::cout << "Breakpoint" << std::endl;
			player->incEnergy(5);
			break;

		case GLFW_KEY_Q:
			player->actionKey(0);
			break;

		case GLFW_KEY_W:
			player->actionKey(1);
			break;

		case GLFW_KEY_E:
			player->actionKey(2);
			break;

		case GLFW_KEY_R:
			player->actionKey(3);
			break;

		case GLFW_KEY_T:
			player->actionKey(4);
			break;

		case GLFW_KEY_Y:
			player->actionKey(5);
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




bool vecSearch(Location * location, std::vector<Location*>& list) //linear search from the back
{
	std::vector<Location *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == location) {
			return true;
		}
	}
	return false;
}