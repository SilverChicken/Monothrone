#include "Unit.h"
#include "Map.h"
#include "ImLoader.h"

#include<iostream>
#include<queue>
#include<unordered_map>

#include "Gamemode.h"
#include "Utils.h"

//Eventually remove
#include "Ressource.h"
#include "Wall.h"
//#include "Throne.h"


//Helper Functions for A* & movement

//bool const locComp(std::pair<Location*, float>, std::pair<Location*, float>);
//float calcDist2(Location*, Location*);   //calculates the distance to target
//Gamemode* game = &Gamemode::getInstance();

const bool moveCond(int, Location *);


Unit::Unit(int own, Location* loc, Map * map)
{
	owner = own;
	animState = own;                //Initialize animState to owner as state 0
	partAnimState = own;            //For Spawn animation we also want it init to owner
	setLoc(map->findClosest(loc));  //This will always be true;
	selected = false;

	game = &Gamemode::getInstance();

	//Set class identifier
	classType = UNIT_CLASS_T;


}

Unit::~Unit()
{
	//delete[](actions);
}

int Unit::getTexLoc()
{
	return textLoc;
}

int Unit::getOwner()
{
	return owner;
}


bool Unit::swap(Location * swapLocation) //This is go
{
	//check that we are both adjacent
	if (game->getMap()->isAdjacent(loc, swapLocation)) {
		Locateable* other = swapLocation->getOwner();
		if (!other) {
			return false;
		}
		if (other->getClassType() >= UNIT_CLASS_T) {  //Then we are trying to swap with a unit
			Unit* otherU = (Unit*)other;              //Safe cast make dynamic? std::static_cast<Unit*>
			if (otherU->owner == owner && otherU->actions[0]) {             //Check that they have the same owner & otherU can move

				//Then we can swap!

				//if other not moving, just make them release , then you release then they take yours and you take theirs
				//update will take care of popping the location from path.
				if (otherU->path.empty()) {
					swapLoc(otherU);
					return true;
				}
				//what if they are also moving?
				else if (otherU->path.front() == loc) {//then check their path.front  if it's our location then we perform the swap and pop their front
					swapLoc(otherU);
					otherU->path.pop_front();
					return true;
				}


				//if it's not our location then they'll move out of our way anyway! so return false


			}
		}
	}

	return false;
}

//Not a member for sending reasons
const bool moveCond(int thisOwn, Location * target) //thisOwn is the owner of the unit probably moving
{
	if (target->state) {
		return true;
	}
	Locateable* other = target->getOwner();
	if (other) {
		if (other->getClassType() >= UNIT_CLASS_T) {
			Unit* otherU = (Unit*)other; //safe cast make dynamic?
			if (otherU->getOwner() == thisOwn && otherU->getActions()[0]) { //same owner and it can move
				//then we are swappable!
				return true;
			}
		}
	}
	return false;
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


	//Not super happy with this, out of bounds possible with particleLoc, spawnLoc and deathLoc since we use the same var to index


	else if (lifeState == 0) { //Run the Spawn animation pb if we select
		glUniform1f(glGetUniformLocation(shaderprog, "overlay"), 4.0f);        //Just particle but it's the spawn anim
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, ImLoader::textures[spawnLoc[partAnimState]]);
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

	
	if (lifeState == 0) {

		//putting this here ensures that no actions can happen since it returns from update.

		//Use the particle anim variable!
		partAnimState = (partAnimState + LIFEANIMSTEP) ;   //Spawn only plays once, no looping

		if (partAnimState >= SPAWNANIMCT) { //Go into normal mode
			lifeState = 1;
			partAnimState = 0;
		}

		return;
	}
	else if (lifeState == 2) { //On death make sure to reset particle anim state

		partAnimState = (partAnimState + LIFEANIMSTEP) % DEATHANIMCT;

		if (partAnimState >= DEATHANIMCT) { //Go into normal mode
			
			//Basically destroy and cleanup

		}

		return;
	}


	Location* go;

	if (building && !path.empty()) { //Check if we're buidling before we move possibly optimize and put into mvt routine
		go = path.front();
		if (go == target) {
			path.clear();
			FinishBuild();
		}
	}


	if (!path.empty()) {        //Update location set by move
		
		go = path.front();
		if (setLoc(go)) {		//If we can go, then we pop it. 
			path.pop_front();  
			moveTries = 0;
		}
		else {   //Oh no, this is no longer a valid path!
			//What if it's never = 3* not free?
			moveTries++;
			if (swap(go)) { //if taken by someone swappable
				path.pop_front(); //Then movement was success!
				moveTries = 0;
			} else if(moveTries > 2) {//move() here to target again after n tries -> ends up looking really weird, since issues at end of mvt
				path.clear(); //give up
			}
		}

		//if almost at path then check if we're building


	}
	else if (collecting) {
		if (map->isAdjacent(loc, collectTarget)) {
			FinishCollect(map);
		}
	}

	if (carrying) { //Then it's Crystal or Energy

		//maybe put in function?
		partAnimState = (partAnimState + PARTANIMSTEP) % PARTANIMCT;
		if (map->isAdjacent(loc, game->getThrone(owner)->getLoc())) { //Then we made it back to Throne!
			game->incRessource(carrying, owner);
			go = game->findClosestType(collectTarget, carrying);
			if (go) {
				collect(go, map);  // restarts loop if valid ressource exists
			}
			
			carrying = 0; //no longer carrying

		}
	}

	if (spawnTimer) { //If we have an active spawn timer we decrease it
		spawnTimer--;
	}




}

bool Unit::select(int PID)
{
	if (PID == owner && lifeState == 1) {
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

	//target = map->findClosestTo(targetLoc, loc); //Dont't call this
	target = game->findClosestToCnd(targetLoc, loc, owner, moveCond);

	if (!target) {
		return false; //Shouldn't ever happen
	}
	if (target == loc) { //already there
		return false;
	}


	glm::vec2 dirs[4] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, 0.0f) };

	std::priority_queue<std::pair<Location*, float>, std::vector<std::pair<Location*, float>>, decltype(&Utils::locComp)> stack(Utils::locComp);
	std::unordered_map<Location*, Location*> from;
	std::unordered_map<Location*, float> cost;

	float newCost = 0;
	glm::vec2 newPos;
	Location* newLoc;

	collecting = false;   //If we were collecting then are sent to another destination, we stop collecting;

	stack.push(std::pair<Location*,float>(loc, 0.0f)); //add the start location
	from.emplace(loc, nullptr);
	cost.emplace(loc, 0.0f);

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
			//then newLoc is not in cost so we add it to all of them, if it's free
			if (cnd && moveCond(owner, newLoc)) {//Used to be newLoc->state    //Change state to moveCon
				cost.emplace(newLoc, newCost);
				from.emplace(newLoc, current);
				stack.push(std::pair<Location*,float>(newLoc, newCost + Utils::calcDist(newLoc, target)));    //this is loss of data converts
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
	if (!locOwner) {
		//find closest between energy and crystal?
		Location* newTE = (game->findClosestType(collectTarget, ENERGY_CLASS_T));
		Location* newTC = (game->findClosestType(collectTarget, CRYSTAL_CLASS_T));
		//But eventually these will return null!!!

		if (!newTE && !newTC) {
		}
		else if (!newTE) {
			collect(newTC, map);
		}
		else if (!newTC) {
			collect(newTE, map);
		}
		else { //both valid
			bool whichT = Utils::calcDist(loc, newTC) < Utils::calcDist(loc, newTE);
			collect( whichT ? newTC : newTE, map); //which one are we going for?
		}

		return false;
	}


	int typeObj = locOwner->getClassType();
	
	if ( (typeObj == ENERGY_CLASS_T || typeObj == CRYSTAL_CLASS_T) && !carrying) { //Check it's still a ressource and that we don't already have a ressource  /////// OR IF IT"S THRONE?
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

bool Unit::FinishBuild()
{
	//We are 1 away from the destination, check tho
	if (game->getMap()->isAdjacent(loc, target)) {

		//For now we just make a wall * and then in future use obj
		Wall * wall = new Wall(owner, target, game->getMap());

		building = false;

		game->addWall(wall);
		//Add to gamemode map to draw


		return true;

	}

	return false;
}

bool Unit::build(Location * location, int obj)
{
		
	
	building = true;				//Flag that it's building
	move(location, game->getMap()); //Move to location

	//move until 1 away from location!!! when path has size1 we call the finishBuild function


	return false;
}

void Unit::spawn(Location * location, int obj)
{
	//This one doesn't allow movement

	//Find a free spot adjacent-> getlocation!!!
	Location* spawnLoc = game->getMap()->findClosest(location);
	//Don't check distance so that we can wallhack
	//if no locations return then there are no free locations
	


	//Spawn timer?
	spawnTimer = 10;  //Spawn cooldown constexpr?


	//Call gamemode function to spawn unit to player -> pass PID, obj
	game->spawnUnit(owner, obj, spawnLoc);

	//no free spots: no spawn!

}

bool Unit::consume(Unit * food)
{

	//Units have priority-> Unit class # lower = stronger.

	//consume called at player level, finds lowest value in selection, 

	//that one consumes the others if multiple one of them consumes all? or distribute

	//Then the destroy is passed and based on locateable # the caller of consume (eater) updates internally


	return false;
}

