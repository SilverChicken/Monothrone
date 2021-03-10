#include "Map.h"
#include<iostream>
#include<queue>

//Helper function for A*
bool const locCompute(std::pair<Location*, float>, std::pair<Location*, float>);
float calcDist(Location*, Location*);   //calculates the distance to target

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
	/*
	for (Ressource* res : Mountains) {
		delete(res);
	}
	for (Ressource* res :Crystals) {
		delete(res);
	}
	for (Ressource* res : Energies) {
		delete(res);
	}
	*/

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



Location * Map::getLoc(glm::vec2 pos)
{
	return getLoc(int(pos.x),int(pos.y));
}

Location * Map::getLoc(int x, int y)
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

/*
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
*/

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
				newVert = getLoc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) { //Check if the vertex isn't going to be checked AND hasn't already been
					stack.push_back(newVert);        //If it wasn't then we add it to the visited list
				}
			}
			if (x < MAPSIZE - 1) { //vertex to the right
				x2 = x + 1;
				y2 = y;
				newVert = getLoc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
					stack.push_back(newVert);
				}
			}
			if (y > 0) {
				x2 = x;
				y2 = y - 1;
				newVert = getLoc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
					stack.push_back(newVert);
				}
			}
			if (y < MAPSIZE - 1) {
				x2 = x;
				y2 = y + 1;
				newVert = getLoc(x2, y2);
				if (!listSearch(newVert, stack) && visited.find(newVert) == visited.end()) {
					stack.push_back(newVert);
				}
			}
		}

		//Now the vertices are added so back to the top of the while loop!

	}
	//Getting here means the stack becomes empty :(
	return nullptr; //then the search has failed. we throw an exception, this should basically end the game. Maybe an easter egg?
	//Basically this is a bit dangerous

}



Location * Map::findClosestTo(Location * start, Location * target) //closest point to start toward target
{
	//Priority queue, look till free, go toward second location -> min distance

	glm::vec2 dirs[4] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f) };

	std::priority_queue<std::pair<Location*, float>, std::vector<std::pair<Location*, float>>, decltype(&locCompute)> stack(locCompute);
	std::unordered_map<Location*, float> cost;

	float newCost = 0;
	glm::vec2 newPos;
	Location* newLoc;

	stack.push(std::pair<Location*, float>(start, 0.0f)); //add the start location
	cost.emplace(start, 0);

	while (!stack.empty()) {
		Location* current = stack.top().first;
		stack.pop();

		if (current->state) {  //if we find a free spot, return
			return current;
		}

		newCost = cost.at(current) + 3;  //Everytime we move away from the original point, 3* as bad as getting closer to the target.

		for (glm::vec2 dir : dirs) {
			newPos = current->getPos() + dir;    //Make sure we aren't running off the map
			newLoc = getLoc(newPos);

			bool cnd = cost.find(newLoc) == cost.end();  //check if it's already there
			if (!cnd) {
				cnd = newCost < cost.at(newLoc);         //check if we found a better path to it, 
			}

			if (cnd ) { //then newLoc is not in cost so we add it to all of them, since it must be taken
				cost.emplace(newLoc, newCost);
				stack.push(std::pair<Location*, float>(newLoc, newCost + calcDist(newLoc, target)));
			}
		}
	}
	return nullptr;
}

void Map::draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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


bool const locCompute(std::pair<Location*, float> a, std::pair<Location*, float> b) //compare priority, if the same use x values
{
	//Need to be careful, if reflexive false, then keys are equivalent -> prioritize x over y

	if (a.second != b.second) {
		return a.second > b.second;
	}
	else { //If they are equal pick greater x
		float da = a.first->getPos().x - b.first->getPos().x;
		return (da > 0.0);
	}

}


float calcDist(Location* a, Location* b) { //Rounded Euclidian distance -> maybe make float
	float x = a->getPos().x - b->getPos().x;
	float y = a->getPos().y - b->getPos().y;
	return (sqrt(x*x + y*y));
}