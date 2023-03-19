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

#include "Client.h"
#include "Server.h"
#include "Menu.h"

unsigned int * ImLoader::textures; //The static array from ImLoader

//Helper functions for starting the threads
void startClient();
void startServer();

//int vecSearch(Location * location, std::vector<Location*> & visited);  //Just loops from the back to the front, more likely to find
//void vecRemove(Ressource * res, std::vector<Ressource*> & vec);

Gamemode::~Gamemode()
{
	if (client) {
		client->stop();
	}
	if (isHost) {
		serv->stop();
		Servthread.join();
	}
	
	Clithread.join();
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
	menu = new Menu(gui->getFontRef());

	//Clithread = std::thread();
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Client::getInstance(), NULL, 0, NULL);
	Clithread = std::thread(startClient);
	

	
	
}

void Gamemode::init2()
{
	client = Client::getInstance(); //fine since the thread is initialized 
	
	map = new Map();

	if (isHost) {
		Servthread = std::thread(startServer);

		//only generate if we're host, otherwise we'll populate the map from a long message from the server
		//setup ressources around map
		SpawnStartRessource(map, MTNRangeCT, Vmtn, NRGRangeCT, Vnrg, CRYRangeCT, Vcry); //This functions serves to delay the requests from client to make sure we've joined?



		//First populate is only RESSOURCES. Next ones can have both

		


	}


	//Check connection first


	
	//now the server is definitely inited and so is the client so we'll ask to populate the maps
	//client->sendPopMsg();  //we will then populate our map. This may take a hot second, so add a delay
	
	

	//we need to wait some time to make sure that the client has had time to connect to the host:


	//WAITROOM -> spin here for a bit to make sure pop is done


	//create the dummy players when the connect if you are the server

	//else create your playerTypes


	if (isHost) {
		serv = Server::getInstance();
	}


	//send a populate message to all clients who are connected once everyone who has connected is connected
	client->sendPopMsg(); //request pop as soon as we connect
	
	

	//only create your own Player after waitroom



	//Then we send deltas for the things we just created?


	
	
	
	




}

void Gamemode::init3()
{

	//Set the extra textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]);
	glActiveTexture(GL_TEXTURE0);




	//Check that we have correctly connected to the HOST if not abort?
	//In init3 we should already necessarily be connected 
	
	//use slot to init PID! have utils for random faraway points
	player = new Player(client->getSlot(), map->getLoc(7, 7), map);

	localPlayer = client->getSlot();

	if (isHost) {
		serv = Server::getInstance();
	}

	//setup basic units
	//testing out worm spawning

	//Throne MUST be spawned first
	Unit* throne = player->spawnUnit<Throne>(map->getLoc(5, 5));

	for (int i = 0; i < 4; i++) {
		player->spawnUnit<Worm>(map->getLoc(5, 5));
	}


	//Dummy player2

	/// <summary> We can't support two players. Define main player to figure out displaying, disable the display on second player?  Make a dummy player class that has no controls


	//PlayerType* p2 = new PlayerType(0, map);

	//Dummy enemies
	//p2->spawnUnit<Worm>(map->getLoc(9, 5)); 
	//p2->spawnUnit<Worm>(map->getLoc(9, 7));


	//Make sure to reset the client's tick variable
	client->resetTick();

	client->beginInput();
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

	mapping->setIsPop(true);

}

void Gamemode::incRessource(int type, int player)
{
	PlayerType * pl = Players.at(player); //should never be null
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

void Gamemode::updateGuiUnit(std::vector<int>& selection)
{
	std::vector<int> uniqUnits;
	bool unique;
	int sel;
	for (int i = 0; i < (int)selection.size(); i++) {
		sel = selection[i];
		unique = true;
		for (int j = i + 1; j < (int)selection.size() && unique; j++) {
			unique = sel != selection[j];
		}
		if (unique) {
			uniqUnits.push_back(sel);
		}
	}
}

Unit* Gamemode::spawnUnit(int playe, int obj, Location* spawnLoc)
{
	PlayerType * pl = Players.at(playe);

	switch (obj) { //NOTE only spawn Unit subclasses
		case UNIT_CLASS_T:
			break;
		case THRONE_CLASS_T:
			return pl->spawnUnit<Throne>(spawnLoc);
			break;
		case REFACTORY_CLASS_T:
			break;
		case MANUFACTORY_CLASS_T:
			break;
		case WORM_CLASS_T:
			return pl->spawnUnit<Worm>(spawnLoc);
			break;
		case WORKER_CLASS_T:
			return pl->spawnUnit<Worker>(spawnLoc);
			break;
		default:
			std::cout << "Invalid obj type send to Gamemode::SpawnUnit" << std::endl;
			break;
	}
	return nullptr;
}

void Gamemode::getSelection(int player, std::vector<Unit*>& out) 
{
	PlayerType* pl = Players.at(player);
	out = pl->getSelection();
}

void Gamemode::createPopulateInfo(std::vector<Populate_Msg>& out)
{

	if (!map->getIsPop()) {
		
		//don't populate it and the server will notice

		return;
	}


	//just in case we clear the vector
	out.clear();

	Location* loc;
	Locateable* obj;
	Unit* unit;

	//now we iterate through map populating pop and pushing it
	//May be smarter to just iterate through Mountains, Energies, Crystals, Player->units ?
	for (int i = 0; i < MAPSIZE; i++) {
		for (int j = 0; j < MAPSIZE; j++) {

			loc = map->getLoc(i, j);
			obj = loc->getOwner();

			if (obj) {
				Populate_Msg pop;            ///////////////////////CHECK THAT THIS WORKS TO FILL THE VECTOR
				pop.type = obj->getClassType();
				pop.x = i;
				pop.y = j;
				
				if (pop.type >= UNIT_CLASS_T) {
					unit = (Unit*)obj;
					pop.owner = unit->getOwner();
				}
				else {
					pop.owner = -1;
				}

				//add entry to vector since there's an object there
				out.push_back(pop);
			}
		}
	}
}

void Gamemode::populateMap(std::vector<Populate_Msg>& in)
{

	Location* loc;


	for (Populate_Msg pop : in) {

		loc = map->getLoc(pop.x, pop.y);

		if (!loc->state) {
			continue;
		}

		switch (pop.type) {
		case LOCATEABLE_CLASS_T:
		{
			//this should never happen
		}
			break;
		case MOUNTAIN_CLASS_T:
		{
			Mountain* mount = new Mountain(loc, map, true);
			break;
		}
		case ENERGY_CLASS_T:
		{
			Energy* energ = new Energy(loc, map, true);
			break;
		}
		case CRYSTAL_CLASS_T:
		{
			Crystal* cry = new Crystal(loc, map, true);
			break;
		}
		case UNIT_CLASS_T:
		{
			//should never happen either
			break; 
		}
		case THRONE_CLASS_T:
		{
			if (pop.owner <= MAX_CLIENTS) {
				PlayerType* pl = getPlayer(pop.owner);
				pl->spawnUnit<Throne>(loc);
			}
			break;
		}
		case REFACTORY_CLASS_T:
		{
			if (pop.owner <= MAX_CLIENTS) {
				PlayerType* pl = getPlayer(pop.owner);
				//pl->spawnUnit<Refactory>(loc);
			}
			break;
		}
		case MANUFACTORY_CLASS_T:
		{
			break;
		}
		case WORM_CLASS_T:
		{
			if (pop.owner <= MAX_CLIENTS) {
				PlayerType* pl = getPlayer(pop.owner);
				pl->spawnUnit<Worm>(loc);
			}
			break;
		}
		case WORKER_CLASS_T:
		{
			break;
		}
		default:
		{
			//Ignore faulty entry? will be caught on the verification
			break;
		}
		}
	}












}

void Gamemode::addMenuPlayer(int slot, char* address)
{
	menu->addPlayer(slot, address);
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

Location* Gamemode::findClosestNotOwned(Location* base, int owner, int bound)
{
	//Dfs early termination, looking for classT of unit or higher, spins within bound block radius

	std::unordered_map<Location*, bool> visited;
	std::list<Location*> stack;   //the current stack to look through, should be a deque

	stack.push_back(base);

	int rounds = 2 * bound * (bound + 1); //gets all spots within bound distance

	while (!stack.empty() && rounds >= 0) {

		rounds--;
		Location* base = stack.front();
		stack.pop_front();
		Location* newVert;

		if (base->getOwner() != nullptr) {
			Locateable* unit = base->getOwner();
			if (unit->getClassType() >= UNIT_CLASS_T) {
				Unit* uni = (Unit*)unit; //safe cast now
				if (uni->getOwner() != owner) {
					return base;
				}
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



Location * Gamemode::findClosestToCnd(Location * start, Location * target, Unit* ownerP, const bool cond(Unit*, Location*))
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

Location* Gamemode::findClosestType(Location* base, int type, int bound)
{
	//Dfs early termination, looking for classT of owner

	std::unordered_map<Location*, bool> visited;
	std::list<Location*> stack;   //the current stack to look through, should be a deque

	stack.push_back(base);

	while (!stack.empty() && (int)visited.size() < bound) {

		Location* base = stack.front();
		stack.pop_front();
		Location* newVert;

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

Location* Gamemode::findClosestToCnd(Location* start, Location* target, Unit* ownerP, const bool cond(Unit*, Location*), int bound)
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


	while (!stack.empty() && (int)cost.size() < bound) {
		Location* current = stack.top().first;
		stack.pop();

		if (cond(ownerP, current)) {  //if we find a free spot, return
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

int Gamemode::getMode()
{
	return mode;
}

int Gamemode::setMode(int m)
{
	mode = m;
	return mode;
}

void Gamemode::setIsHost(bool h)
{
	isHost = h;
}

Unit * Gamemode::getThrone(int own)
{
	return Thrones[own];
}

PlayerType * Gamemode::getPlayer(int own) //makes sure that this player exists
{
	if (Players.count(own)) {
		return Players[own];
	}
	else {
		return nullptr;
	}
	
}

void Gamemode::addThrone(int own, Unit * throne)
{
	if (throne->getClassType() == THRONE_CLASS_T) { //Check that it's actually a throne
		Thrones[own] = throne;
	}
}

void Gamemode::addPlayer(int own, PlayerType * player)
{
	Players[own] = player;
}

void Gamemode::destroyUnit(Unit* unit)
{
	PlayerType* pl = Players.at(unit->getOwner());
	//do gamemode level cleanup?


	pl->destroyUnit(unit);
}

void Gamemode::dealDamageUnit(Unit* unit, int amount, int cause) //This gets called on a client side
{
	//Our unit is damaged, we need to tell the others
	client->addEventype(EVT_DAMAGE);
	client->addEventArgs(0, unit->getID());
	client->addEventArgs(1, amount);
	client->addEventArgs(2, cause);
	client->sendEvent();
	//send our unit's ID, the amount of dmg and the cause. Our PID will be sent as well

}

void Gamemode::unitSpawned(Unit*)
{
	//Send unit ID, owner, and location? and type

}

Map * Gamemode::getMap()
{
	return map;
}

void Gamemode::update()
{
	if (mode == 1 && !pause) {
		if (client->tick()) { //Update all objects! 
		//Update unit positions mostly and anim?

		//Player will cascade the update to all its units
			for (std::pair<int, PlayerType*> pl : Players) { //Or do iterator and at it
				pl.second->update();
			}

			//Damage loop or secondary things that needed info from the first loop
			for (std::pair<int, PlayerType*> pl : Players) { //Or do iterator and at it
				pl.second->update2();
			}

		}
	}
	else if (mode == 0 && client) {
		client->requestJoin(); //keep trying to join by default while in menu
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

	//for (Unit* ene : Enemies) {
	//	if (!player->cull(ene->getLoc())) {
	//		ene->draw(ImLoader::textures[ene->getTexLoc()], shaderProgram);
	//	}
	//}

	for (auto wall : Walls) {
		if (!player->cull(wall.first->getLoc())) {
			wall.first->draw(shaderProgram);
		}
	}

	for (auto pl : Players) {
		if (pl.second == (PlayerType*)player) {
			pl.second->draw(ImLoader::textures, shaderProgram);
		}
		else {
			pl.second->draw(ImLoader::textures, player, shaderProgram);
		}
		
	}

	glm::vec2 camLoc = player->getBotLeft()->getPos();
	gui->drawIcons(shaderProgram, camLoc.x, camLoc.y);

}

void Gamemode::drawMap()
{
	map->draw();
}

void Gamemode::drawGui()
{
	if (mode == 0) {
		menu->drawMenu();
	}
	else {
		gui->draw();
	}
	
}


void Gamemode::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{

	///IF we are late and have sync issues between local actions and remote actions, we'll stop doing significant things locally and will send every visible action as a request to the server which it will then give us a result for.


	// Check for a key press
	if (action == GLFW_PRESS)
	{

		if (mode == 0) {
			menu->menu_key_call(key);
			return;
		}


		//For now just send every input
		//client->addInput(key);

		int input;
		Location* tempLoc;
		Unit* tempU;

		switch (key) {
		case GLFW_KEY_ESCAPE: // Check if escape was pressed

			//Tell the server we're leaving
			client->addInput(CMD_LEAVE);

			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);

			//delete appropriate things in appropriate order
			cleanup();

			break;

		//strictly local operations happen here, write an inputAction function for remote operations

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

		case GLFW_KEY_N:

			std::cout << "Breakpoint" << std::endl;
			//player->incEnergy(5);
			break;

		case GLFW_KEY_ENTER:

			//THIS MUST BE THE FINAL LOCATION WE ARE SELECTING. Make a function that returns jsut that //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//client->addLoc((int)player->getLoc()->getPos().x, (int)player->getLoc()->getPos().y);

			//maybe don't do this part and let server handle it? etc in the rest probably the best thing to do, only do things that the sever tells us, everything else is just a request
			//so select just returns which location to select from

			tempLoc = player->selectLocal(player->getLoc());
			if (tempLoc) {

				//tell the server
				client->addInput(CMD_SELECT);
				client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			}
			

			break;

		case GLFW_KEY_LEFT_SHIFT:
			client->addInput(CMD_DESELECT);

			//addloc statement needed here
			tempU = player->deselectLocal(player->getLoc());
			if (tempU) {
				tempLoc = tempU->getLoc();
				client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			}
			

			break;

		case GLFW_KEY_LEFT_CONTROL:
			client->addInput(CMD_DESELECT_ALL);
			//player->deselectAll();
			break;

		case GLFW_KEY_Q:
			input = player->actionKey(0);
			client->addInput(input);
			tempLoc = player->getLoc();
			client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);



			//add loc for player's location?

			break;

		case GLFW_KEY_W:

			//maybe don't call just translate? -> depends on lateness 

			input = player->actionKey(1);
			tempLoc = player->getLoc();
			client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			client->addInput(input);
			break;

		case GLFW_KEY_E:
			input = player->actionKey(2);
			tempLoc = player->getLoc();
			client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			client->addInput(input);
			break;

		case GLFW_KEY_R:
			input = player->actionKey(3);
			tempLoc = player->getLoc();
			client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			client->addInput(input);
			break;

		case GLFW_KEY_T:
			input = player->actionKey(4);
			tempLoc = player->getLoc();
			client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			client->addInput(input);
			break;

		case GLFW_KEY_Y:
			input = player->actionKey(5);
			tempLoc = player->getLoc();
			client->addLoc((int)tempLoc->getPos().x, (int)tempLoc->getPos().y);
			client->addInput(input);
			break;

		case GLFW_KEY_P:
			client->addInput(CMD_PAUSE);
			pause = !pause;
			break;

		default:
			//some redundancy of the checks here can clean up between callback and action
			//key_action(key, action, localPlayer);
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

void Gamemode::key_action_remote(int key, int pl, int x, int y)
{

	// This is called for remote actions that we received from the server or from client if we are the server


	PlayerType* play = getPlayer(pl);

	if (!play) { //if invalid player code we stop
		printf("Player %i does not exist in key_action_remote \n", pl);
		return;
	}

	//int input;

	switch (key) {

	case CMD_SELECT:
		play->select(map->getLoc(x, y));
		break;

	case CMD_DESELECT:
		play->deselect(map->getLoc(x, y));
		break;

	case CMD_DESELECT_ALL:
		play->deselectAll();
		break;

	case CMD_PAUSE:
		//client->addInput(CMD_PAUSE);
		pause = !pause;
		break;

	default:
		//default catches all the perform actions
		play->perform_action(key, map->getLoc(x,y));
		break;
	}

}

void Gamemode::server_event_remote(int type, int player, int x, int y, int arg0, int arg1, int arg2, int arg3)
{
	//This should ONLY be called by the server which applies the events locally and sends notices to other clients
	switch (type) {
	case EVT_DAMAGE:
	{
		Unit* target = Players[player]->getUnitById(arg0);
		int amount = arg1;
		int cause = arg2;
		//Validate that this is possible


		
		target->takeDamage(amount, cause);
		//Notify clients that this has happened


		break;
	}
	case EVT_DEATH:
	{
		break;
	}
	case EVT_SPAWN:
	{
		break;
	} 
	default:
		printf("Invalid event type, was %d", type);
		break;
	}
}

void Gamemode::event_remote(int type, int player, int x, int y, int arg0, int arg1, int arg2, int arg3)
{

}


void startClient()
{
	Client::getInstance();
}

void startServer()
{
	 Server::getInstance();
}
