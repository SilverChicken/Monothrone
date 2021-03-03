#include "Map.h"
#include<iostream>


#include "Ressource.h"


//helper functions for vector search
bool listSearch(Location * location, std::list<Location*> & visited);  //Just loops from the back to the front, more likely to find


Map::Map()
{

	init();


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
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.




	// We've sent the vertex data over to OpenGL, but there's still something missing.
	// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);


	//here we set the material for each object


	glGetError();
}

Map::~Map()
{
	for (Ressource* res : Mountains) {
		delete(res);
	}
	for (Ressource* res :Crystals) {
		delete(res);
	}
	for (Ressource* res : Energies) {
		delete(res);
	}

	for (int i = 0; i < MAPSIZE; i++) {
		for (int j = 0; j < MAPSIZE; j++) {
			delete(map[i][j]);
		}
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_vert);
	glDeleteBuffers(1, &EBO);
}

void Map::init()
{

	std::cout << "Maps is innit" << std::endl;
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			//std::cout << i << "  " << j << std::endl;
			map[i][j] = new Location(i, j);

			// std::cout << map[i][j]->getPos().x << "  " << map[i][j]->getPos().y << std::endl;
		}
	}
}


void Map::categorizeAccess()
{
	//get vector of maps each map will rpz a connected component
	std::vector<std::unordered_map<Location*, bool>> * components;

	std::unordered_map<Location*, bool> freeLocs;

	Location* loc;

	//fill the free locations
	for (int i = 0; i < mapSize; i++) {  
		for (int j = 0; j < mapSize; j++) {
			loc = map[i][j];
			if (loc->state) {
				freeLocs[loc] = true;
			}
		}
	}

	while (!freeLocs.empty()) {
		std::unordered_map<Location*, bool> * comp = new std::unordered_map<Location*, bool>();


	}
	

	//Conduct a DFS search at a random location not logged location -> start off with a map of ALL locations and delete as we go along
	//count its total size in a hashmap?





	//Once we have exhausted the search we pick a point that is not in the hashmap already, and start again

	//When all FREE locations have been classified

	//Go through list of maps, get the count for each. either get max and fill others or fill all that have size less than some CONSNTAT

	//For each map in list, if it is below criterion, for each element we creat MOUNTAIN or LOCATEABLE inside that takes the lock.



}




Location * Map::getloc(glm::vec2 pos)
{
	return getloc(int(pos.x),int(pos.y));
}

Location * Map::getloc(int x, int y)
{
	//needs to check if x and y are in bounds
	if (x > MAPSIZE - 1) {
		//std::cout << "invalid location: " << x << " " << y << std::endl;
		x = MAPSIZE - 1;
	}
	else if (x < 0) {
		//std::cout << "invalid location: " << x << " " << y << std::endl;
		x = 0;
	}
	if (y > MAPSIZE - 1) {
		//std::cout << "invalid location: " << x << " " << y << std::endl;
		y = MAPSIZE - 1;
	}
	else if (y < 0) {
		//std::cout << "invalid location: " << x << " " << y << std::endl;
		y = 0;
	}

	return map[x][y];    //This will always be valid! not always free
}

std::vector<Ressource*> Map::getMountains()
{
	return Mountains;
}

std::vector<Ressource*> Map::getCrystals()
{
	return Crystals;
}

std::vector<Ressource*> Map::getEnergies()
{
	return Energies;
}

bool Map::addMountain(Ressource * mtn)
{
	Mountains.push_back(mtn);
	return true;              //bool if we ever change type
}

bool Map::addCrystals(Ressource * cry)
{
	Crystals.push_back(cry);
	return true;              //bool if we ever change type
}

bool Map::addEnergies(Ressource * nrg)
{
	Energies.push_back(nrg);
	return true;              //bool if we ever change type
}

Unit * Map::getThrone(int ID)
{
	return Thrones[ID];
}

Player * Map::getPlayer(int ID)
{
	return Players[ID];
}

void Map::addPlayer(int ID, Player * player)
{
	Players[ID] = player;
}

void Map::addThrone(int ID, Unit* throne)
{
	Thrones[ID] = throne;
}

bool ** Map::getBox(glm::vec2 a, glm::vec2 b)
{
	return getBox(a.x, a.y, b.x, b.y);
}

bool ** Map::getBox(int x, int y, int x2, int y2)   //This function may be useless? We went with traversing the Locatio* graph. 
{
	int xsize = abs(x2 - x + 1);
	int ysize = abs(y2 - y + 1);
	bool ** lockMap = new  bool * [xsize];   //allocate first then for loop it

	return nullptr;
}

bool Map::isAdjacent(Location * A, Location * B) 
{
	glm::vec2 Apos = A->getPos();
	glm::vec2 Bpos = B->getPos();
	glm::vec2 dirs[4] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f) };

	for (glm::vec2 dir : dirs) {
		if (Apos + dir == Bpos) {
			return true;
		}
	}
	return false;

}


//May write another one to find closest between target and a locateabl?
//And another that finds the closest space occupied by an owner of a certain class type?

Location * Map::findClosest(Location * base) //Maybe a bit slow so don't use for huge bunches at once, cache for group spawn?
{
	//Visited is a map which we may want to keep around for caching, eh probs not?
	std::unordered_map<Location*, bool> visited;
	std::list<Location*> stack;   //the current stack to look through, should be a deque

	stack.push_back(base);

	return findClosestRecc(visited, stack);

}

Location *  Map::findClosestRecc(std::unordered_map<Location*, bool>& visited, std::list<Location*>& stack)
{

	while (!stack.empty()) {

		Location* base = stack.front();
		stack.pop_front();
		Location * newVert;

		if (base->state) {
			return base;
		}
		else { //dfs search
			int x = (int)base->getPos().x;
			int y = (int)base->getPos().y;
			int x2 = 0;
			int y2 = 0;
			

			visited[base] = base->state; //we have now visited this vertex

			if (x > 0) { //there's a vertex on the left
				x2 = x - 1;
				y2 = y;
				newVert = getloc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) { //Check if the vertex isn't going to be checked AND hasn't already been
					stack.push_back(newVert);        //If it wasn't then we add it to the visited list
				}
			}
			if (x < MAPSIZE - 1) { //vertex to the right
				x2 = x + 1;
				y2 = y;
				newVert = getloc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
					stack.push_back(newVert);
				}
			}
			if (y > 0) {
				x2 = x;
				y2 = y - 1;
				newVert = getloc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
					stack.push_back(newVert);
				}
			}
			if (y < MAPSIZE - 1) {
				x2 = x;
				y2 = y + 1;
				newVert = getloc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
					stack.push_back(newVert);
				}
			}
		}

		//Now the vertices are added so bad to the top of the while loop!

	}
	//Getting here means the stack becomes empty :(
	return nullptr; //then the search has failed. we throw an exception, this should basically end the game. Maybe an easter egg?
	//Basically this is a bit dangerous
}

bool listSearch(Location * location, std::list<Location*>& list) //linear search from the back
{
	std::list<Location *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == location) {
			return true;
		}
	}
	return false;
}


void Map::draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Map::fillInaccessible()
{
	//Once we have spawned all Ressources, we need to find a way to categorize accessibility to player base -> try a move from Throne to every location?
}


