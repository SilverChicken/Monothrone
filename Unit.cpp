#include "Unit.h"
#include "Map.h"
#include "ImLoader.h"

#include<iostream>
#include<queue>
#include<unordered_map>

#include "Gamemode.h"

//Eventually remove
#include "Ressource.h"
//#include "Throne.h"


//Helper Functions for A* & movement
Location* target;  
Location* collectTarget;  //Location we are trying to collect from
bool const locComp(std::pair<Location*, float>, std::pair<Location*, float>);
float calcDist(Location*);   //calculates the distance to target


//Gamemode* game = &Gamemode::getInstance();


Unit::Unit(int own, Location* loc, Map * map)
{
	owner = own;
	animState = own;                //Initialize animState to owner as state 0
	setLoc(map->findClosest(loc));  //This will always be true;
	selected = false;

	game = &Gamemode::getInstance();

	//Set class identifier
	classType = UNIT_CLASS_T;

	//get rid, move to gamemode
	/*
	Unit * thro = game->getThrone(own);
	if (thro) {   
		if (thro->getClassType() == THRONE_CLASS_T) {  //Class type for throne
			throneRef = (Throne*)thro;
		}
		else {
			throneRef = nullptr;
		}
		
	}
	else {  //Happens for Throne class only as 1st to spawn
		throneRef = nullptr;
	}
	
	*/

}

Unit::~Unit()
{
	//delete[](actions);
}

int Unit::getTexLoc()
{
	return textLoc;
}

void Unit::draw(unsigned int texture, GLuint shaderprog)
{

	//send some info about where you are
	glUniform2f(glGetUniformLocation(shaderprog, "location"), loc->getPos().x, loc->getPos().y);

	if (selected && carrying) {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 3.0f);        //Particle effect and selection
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[particleLoc[partAnimState]]);
	} else if (selected) {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 1.0f);        //Just selection
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[Unit::selectLoc]);
	} else if (carrying) {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 2.0f);        //Just particle effect
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[particleLoc[partAnimState]]);
	}
	else {
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 0.0f);        //None
	}



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Unit::update(Map* map)
{

	
	Location* go;

	if (!path.empty()) {        //Update location set by move
		go = path.front();
		if (setLoc(go)) {		//If we can go, then we pop it. 
			path.pop_front();  
			moveTries = 0;
		}
		else {   //Oh no, this is no longer a valid path!
			//What if it's never = 3* not free?
			moveTries++;
			if (moveTries > 2) { //move() here to target again after n tries -> ends up looking really weird, since issues at end of mvt
				path.clear(); //give up
			}
		}
	}
	else if (collecting) {
		if (map->isAdjacent(loc, collectTarget)) {
			FinishCollect(map);
		}
	}

	if (carrying) { //Then it's Crystal or Energy
		partAnimState = (partAnimState + PARTANIMSTEP) % PARTANIMCT;
		if (map->isAdjacent(loc, game->getThrone(owner)->getLoc())) { //Then we made it back to Throne!
			game->incRessource(carrying, owner);
			collect(game->findClosestType(collectTarget, carrying), map);  // restarts loop
			carrying = 0; //no longer carrying

			
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

bool * Unit::getActions()
{
	return actions;
}

bool Unit::move(Location* targetLoc, Map* map)
{

	target = map->findClosestTo(targetLoc, loc);

	glm::vec2 dirs[4] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f) };

	std::priority_queue<std::pair<Location*, float>, std::vector<std::pair<Location*, float>>, decltype(&locComp)> stack(locComp);
	std::unordered_map<Location*, Location*> from;
	std::unordered_map<Location*, float> cost;

	float newCost = 0;
	glm::vec2 newPos;
	Location* newLoc;

	collecting = false;   //If we were collecting then are sent to another destination, we stop collecting;

	stack.push(std::pair<Location*,float>(loc, 0.0f)); //add the start location
	from.emplace(loc, nullptr);
	cost.emplace(loc, 0);

	while (!stack.empty()) {
		Location* current = stack.top().first;
		stack.pop();

		if (current == target) {  //what to do if we can't quite make it? What to do if we're in a group?
			
			//Delete previous path
			path.clear();

			//We have found a path. Store it in the path list
			path.emplace_front(target);

			while (from.at(current) != loc) {   //Loop through from until we get to where we currently are, and exclude the current location
				path.emplace_front(from.at(current));
				current = from.at(current);
			}
			return true;
		}

		newCost = cost.at(current) + 1;

		for (glm::vec2 dir: dirs) {
			newPos = current->getPos() + dir;    //Make sure we aren't running off the map
			newLoc = map->getLoc(newPos);

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

bool Unit::collect(Location * location, Map* map)
{
	Locateable* locOwner = location->getOwner();

	if (locOwner == nullptr) {
		return false;
	}

	if (locOwner->getClassType() <= MAXRESCLASST && locOwner->getClassType() >= 2) { //2&3 are collectible ressources, else mountain
		collectTarget = location;
		move(location, map);  //Set movement to the ressource
		collecting = true;    //Set internal flag and trigger collection once we are at location
		return true;
	}
	return false;
}


bool Unit::FinishCollect(Map* map)  //We know that we are adjacent to target 
{
	Locateable* locOwner = collectTarget->getOwner();
	//this can still be nullptr!!!
	if (!locOwner) return false;


	int typeObj = locOwner->getClassType();
	
	if ( typeObj <= MAXRESCLASST && typeObj > MOUNTAIN_CLASS_T && !carrying) { //Check it's still a ressource and that we don't already have a ressource  /////// OR IF IT"S THRONE?
		Ressource* res = (Ressource*)locOwner;   //Safe cast
		res->destroy();                          //Forces an include avoid by having virtual Locateable function? 

		collecting = false;    //Done collecting? Or don't and loop until condition on area near 


		//Add fact that we are carrying a ressource, changes anim
		if (typeObj == ENERGY_CLASS_T) { //Energy
			carrying = ENERGY_CLASS_T;
			partAnimState = 0;
		}
		else if (typeObj == CRYSTAL_CLASS_T) { //Crystal
			carrying = CRYSTAL_CLASS_T;
			partAnimState = 1;
		}

		//If we have a throneRef then we move back to the throne to cash it in
                    
		move(game->getThrone(owner)->getLoc(), map);    //This is why we need to pass map
		
		//Call another function once we are adjacent to Throne to return the ressource

		return true;
	}
	else {
		return false;
	}
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

bool const locComp(std::pair<Location*, float> a, std::pair<Location*, float> b) //compare priority, if the same use x values
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


float calcDist(Location* a) { //Rounded Euclidian distance
	float x = a->getPos().x - target->getPos().x;
	float y = a->getPos().y - target->getPos().y;
	return (sqrt(x*x + y*y));
}