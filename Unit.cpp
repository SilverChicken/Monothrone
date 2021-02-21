#include "Unit.h"
#include "Map.h"
#include "ImLoader.h"

#include<iostream>
#include<queue>
#include<map>


//Helper Functions for A*
Location* target;  
bool const locComp(std::pair<Location*, int>, std::pair<Location*, int>);
int calcDist(Location*);   //calculates the distance to target


Unit::Unit(int own, Location* loc, Map * map)
{
	owner = own;			
	setLoc(map->findClosest(loc));  //this will always be true;
	selected = false;

	map->addUnit(this);

	//Set class identifier
	classType = 4;

}

Unit::~Unit()
{
	
}

void Unit::draw(GLuint shaderprog)
{
	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);


	if (selected) {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 1.0f);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]);
	}
	else {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 0.0f);
	}


	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Unit::update()
{
	//Udpate animation
	//send textloc elsewhere?



	//Update location set by move?
	Location* go;

	//std::cout << "Unit is updating" << std::endl;

	if (!path.empty()) {
		go = path.front();
		path.pop_front();
		if (setLoc(go)) {

		}
		else {   //Oh no, this is no longer a valid path for right now we give up, ALSO RN WE GIVE UP OUR LOCATION FOREVER AND BECOME UNTARGETABLE
			//move() here to target again
			//reset path variable
		}
	}

}

bool Unit::select(int PID)
{
	if (PID == owner) {
		selected = true;
		return true;
	}
	return false;
}

void Unit::deselect()
{
	selected = false;
}

bool Unit::isSelected()
{
	return selected;
}

bool Unit::move(Location* targetLoc, Map* map)
{

	target = map->findClosest(targetLoc);

	glm::vec2 dirs[4] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f) };

	std::priority_queue<std::pair<Location*, int>, std::vector<std::pair<Location*, int>>, decltype(&locComp)> stack(locComp);
	std::map<Location*, Location*> from;
	std::map<Location*, int> cost;

	int newCost = 0;
	glm::vec2 newPos;
	Location* newLoc;

	stack.push(std::pair<Location*,int>(loc, 0)); //add the start location
	from.emplace(loc, nullptr);
	cost.emplace(loc, 0);

	while (!stack.empty()) {
		Location* current = stack.top().first;
		stack.pop();

		if (current == target) {  //what to do if we can't quite make it? What to do if we're in a group?
			
			//We have found a path. Store it in the path list
			path.emplace_front(target);

			while (current != loc) {   //Loop through from until we get to where we currently are
				path.emplace_front(from.at(current));
				current = from.at(current);
			}
			return true;
		}

		newCost = cost.at(current) + 1;

		for (glm::vec2 dir: dirs) {
			newPos = current->getPos() + dir;    //Make sure we aren't running off the map
			newLoc = map->getloc(newPos);

			bool cnd = cost.find(newLoc) == cost.end();  //check if it's already there
			if (!cnd) {
				cnd = newCost < cost.at(newLoc);         //check if we found a better path to it, 
			}

			if (cnd && newLoc->state) { //then newLoc is not in cost so we add it to all of them, if it's free
				cost.emplace(newLoc, newCost);
				from.emplace(newLoc, current);
				stack.push(std::pair<Location*, int>(newLoc, newCost + calcDist(newLoc)));
			}
		} 
	}
	return false;
}

bool Unit::collect(int x, int y)
{
	return false;
}

bool Unit::build(int x, int y, std::string obj)
{
	return false;
}

Unit * Unit::spawn(std::string baby, glm::vec2 place)
{
	return nullptr;
}

bool Unit::consume(Unit * food)
{
	return false;
}

bool const locComp(std::pair<Location*, int> a, std::pair<Location*, int> b) //compare priority, if the same use x values
{
	//Need to be careful, if reflexive false, then keys are equivalent -> prioritize x over y

	if (a.second != b.second) {
		return a.second > b.second;
	}
	else { //If they are equal check if they have the same x
		float da = abs(a.first->getPos().x - target->getPos().x);
		float db = abs(b.first->getPos().x - target->getPos().x);
		return (da < db);
	}
	
}


int calcDist(Location* a) { //Rounded Euclidian distance
	float x = a->getPos().x - target->getPos().x;
	float y = a->getPos().y - target->getPos().y;
	return round(sqrt(x*x + y*y));
}