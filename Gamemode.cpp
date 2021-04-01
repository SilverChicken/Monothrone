#include "Gamemode.h"
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <time.h>

#include "Map.h"
#include "Gui.h"

#include "Player.h"
#include "Throne.h"
#include "Worm.h"
#include "Worker.h"

#include "Wall.h"

#include "Ressource.h"
#include "Mountain.h"
#include "Energy.h"
#include "Crystal.h"

#include "ImLoader.h"

#include "Utils.h"


unsigned int * ImLoader::textures; //The static array from ImLoader


//int vecSearch(Location * location, std::vector<Location*> & visited);  //Just loops from the back to the front, more likely to find
//void vecRemove(Ressource * res, std::vector<Ressource*> & vec);

Gamemode::~Gamemode()
{



}

Gamemode & Gamemode::getInstance()
{
	static Gamemode instance;
	return instance;
}

void Gamemode::init()
{
	ImLoader::Loadtextures();

	gui = new Gui();
	map = new Map();
	player = new Player(1, map->getLoc(7, 7), map);
	

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

	for (int i = 0; i < 4; i++) {
		player->spawnUnit<Worm>(map->getLoc(5, 5));
	}

	
}

void Gamemode::cleanup()
{
	//delete all the things

	for (int i = 0; i < MAX_PLAYER; i++) {
		if (Thrones.count(i)) {  // Thrones are already deleted as they are units
			//delete Thrones[i];
		}
		if (Players.count(i)) {
			delete Players[i];    //This destructor will propagate to Units of Player
		}
	}

	Locateable::cleanup();

	delete(map);
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
				if (freeLocs.count(loc) && !Utils::vecSearch(loc, stack)) {
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
	std::cout << "Crystal cluster count: " << count << std::endl;

	for (int i = 0; i < count; i++) {
		x = rand() % MAPSIZE;
		y = rand() % MAPSIZE;
		res = new Energy(mapping->getLoc(x, y), mapping);
	}


}

void Gamemode::incRessource(int type, int player)
{
	Player * pl = Players.at(player); //should never be null
	if (type == ENERGY_CLASS_T) {
		pl->incEnergy(1);
		gui->setEnergy(pl->getEnergy());  //Updates the Gui 
	}
	else if (type == CRYSTAL_CLASS_T) {
		pl->incCrystal(1);
		gui->setCrystal(pl->getCrystal()); 
	}
}

void Gamemode::updateGuiBind(int * pBind)
{
	gui->setBinds(&pBind[0]);
}

void Gamemode::spawnUnit(int playe, int obj, Location* spawnLoc)
{
	Player * pl = Players.at(playe);

	switch (obj) { //NOTE only spawn Unit subclasses
		case UNIT_CLASS_T:
			break;
		case THRONE_CLASS_T:
			pl->spawnUnit<Throne>(spawnLoc);
			break;
		case REFACTORY_CLASS_T:
			break;
		case MANUFACTORY_CLASS_T:
			break;
		case WORM_CLASS_T:
			pl->spawnUnit<Worm>(spawnLoc);
			break;
		case WORKER_CLASS_T:
			pl->spawnUnit<Worker>(spawnLoc);
			break;
		default:
			std::cout << "Invalid obj type send to Gamemode::SpawnUnit" << std::endl;
			break;


	}

}

Location * Gamemode::findClosestType(Location * base, int type)
{
	//Dfs early termination, looking for classT of owner

	std::unordered_map<Location*, bool> visited;
	std::list<Location*> stack;   //the current stack to look through, should be a deque

	stack.push_back(base);

	while (!stack.empty()) {

		Location* base = stack.front();
		stack.pop_front();
		Location * newVert;

		if (base->getOwner() != nullptr) {   
			//race condition on base if someone else takes it between these 2 lines? never happens bc exception
			if (base->getOwner()->getClassType() == type) {
				return base;
			}
		}


		int x = (int)base->getPos().x;
		int y = (int)base->getPos().y;
		int x2 = 0;
		int y2 = 0;


		visited[base] = base->state; //we have now visited this vertex

		if (x > 0) { //there's a vertex on the left
			x2 = x - 1;
			y2 = y;
			newVert = map->getLoc(x2, y2);
			if (!Utils::listSearch(newVert, stack) && visited.find(newVert) == visited.end()) { //Check if the vertex isn't going to be checked AND hasn't already been
				stack.push_back(newVert);        //If it wasn't then we add it to the visited list
			}
		}
		if (x < MAPSIZE - 1) { //vertex to the right
			x2 = x + 1;
			y2 = y;
			newVert = map->getLoc(x2, y2);
			if (!Utils::listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
				stack.push_back(newVert);
			}
		}
		if (y > 0) {
			x2 = x;
			y2 = y - 1;
			newVert = map->getLoc(x2, y2);
			if (!Utils::listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
				stack.push_back(newVert);
			}
		}
		if (y < MAPSIZE - 1) {
			x2 = x;
			y2 = y + 1;
			newVert = map->getLoc(x2, y2);
			if (!Utils::listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
				stack.push_back(newVert);
			}
		}

		//Now the vertices are added so back to the top of the while loop!

	}
	//Getting here means the stack becomes empty :(
	return nullptr; //then the search has failed. we throw an exception, this should basically end the game. Maybe an easter egg?
	//Basically this is a bit dangerous and it should be handled otherly.

}



Location * Gamemode::findClosestToCnd(Location * start, Location * target, int ownerP, const bool cond(int, Location*))
{
	//Priority queue, look till free, go toward second location -> min distance

	glm::vec2 dirs[4] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f) };

	std::priority_queue<std::pair<Location*, float>, std::vector<std::pair<Location*, float>>, decltype(&Utils::locComp)> stack(Utils::locComp);
	std::unordered_map<Location*, float> cost;

	float newCost = 0;
	glm::vec2 newPos;
	Location* newLoc;

	stack.push(std::pair<Location*, float>(start, 0.0f)); //add the start location
	cost.emplace(start, 0.0f);


	//How to we get a unit to do this search? ALL WE NEED IS A OWNER INT
	/*
	Unit* startOwn = (Unit*)start->getOwner(); //oh fuck, this search goes around a random point.
	if (!startOwn) { //Make sure we have a valid owner in order to use the condition, should only happen if improperly called
		std::cout << "Gamemode::findClosestToCnd failed to find the owner of start" << std::endl;
		return nullptr;
	}*/


	while (!stack.empty()) {
		Location* current = stack.top().first;
		stack.pop();

		if (cond(ownerP,current)) {  //if we find a free spot, return
			return current;
		}

		newCost = cost.at(current) + 3;  //Everytime we move away from the original point, 3* as bad as getting closer to the target.

		for (glm::vec2 dir : dirs) {
			newPos = current->getPos() + dir;    //Make sure we aren't running off the map
			newLoc = map->getLoc(newPos);

			bool cnd = cost.find(newLoc) == cost.end();  //check if it's already there
			if (!cnd) {
				cnd = newCost < cost.at(newLoc);         //check if we found a better path to it, 
			}

			if (cnd) { //then newLoc is not in cost so we add it to all of them, since it must be taken
				cost.emplace(newLoc, newCost);
				stack.push(std::pair<Location*, float>(newLoc, newCost + Utils::calcDist(newLoc, target)));
			}
		}
	}
	return nullptr;
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

void Gamemode::removeRessource(Ressource * res)
{

	if (res->getClassType() == CRYSTAL_CLASS_T) {
		Utils::vecRemove<Ressource>(res, Crystals);
	}
	else if (res->getClassType() == ENERGY_CLASS_T) {
		Utils::vecRemove<Ressource>(res, Energies);
	}
}

void Gamemode::addWall(Wall * newWall)
{
	Walls[newWall] = newWall->getOwner();
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

Map * Gamemode::getMap()
{
	return map;
}

void Gamemode::update()
{
	ticker.inc();
	if (ticker.state) { //Update all objects! 
		//Update unit positions mostly and anim?

		//Player will cascade the update to all its units
		for (std::pair<int, Player*>  pl : Players) { //Or do iterator and at it
			pl.second->update();
		}
		

	}
}

void Gamemode::draw(GLuint shaderProgram)
{

	//Currently as virtual call the derived function which then calls the ressource draw with 1 param 
	//Write for loop for each player but use localPlayer for culling


	for (Ressource* res : Mountains) {
		if (!player->cull(res->getLoc())) {
			res->draw(ImLoader::textures[res->getTextLoc()], shaderProgram);
		} 
	}
	for (Ressource* res : Crystals) {
		if (!player->cull(res->getLoc())) {
			res->draw(ImLoader::textures[res->getTextLoc()], shaderProgram);
		}
	}
	for (Ressource* res : Energies) {
		if (!player->cull(res->getLoc())) {
			res->draw(ImLoader::textures[res->getTextLoc()], shaderProgram);
		}
	}

	for (auto wall : Walls) {
		if (!player->cull(wall.first->getLoc())) {
			wall.first->draw(shaderProgram);
		}
	}

	player->draw(ImLoader::textures, shaderProgram);

	glm::vec2 camLoc = player->getBotLeft()->getPos();
	gui->drawIcons(shaderProgram, camLoc.x, camLoc.y);

}

void Gamemode::drawMap()
{
	map->draw();
}

void Gamemode::drawGui()
{
	gui->draw();
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

			//delete appropriate things in appropriate order
			cleanup();

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
			//player->incEnergy(5);
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

		case GLFW_KEY_P:
			player->pause = !player->pause;
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


/*

int vecSearch(Location * location, std::vector<Location*>& list) //linear search from the back
{
	std::vector<Location *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == location) {
			return true;
		}
	}
	return false;
}

void vecRemove(Ressource * res, std::vector<Ressource*>& list) //linear search from the back, erases if found
{
	std::vector<Ressource *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == res) {
			list.erase(it);
			break;
		}
	}
}
*/