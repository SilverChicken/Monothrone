#include "Throne.h"
#include "Map.h"
#include "Player.h"

#include "Gamemode.h"


Throne::Throne(int owner, Location * loc, Map * map):Unit(owner, loc, map)
{
	hp = 25;
	atk = 0;
	speed = 0;
	textLoc = textLocs[owner]; 

	classType = THRONE_CLASS_T;

	game->addThrone(owner, this);
	//playerRef = game->getPlayer(owner);            //Forces include Map.h, and relies on Player being created before.

	//Define which actions are defined
	actions[0] = false;
	actions[1] = false;
	actions[2] = true;
	actions[3] = true;
	actions[4] = true;
	actions[5] = false;
}

Throne::~Throne()
{

}

void Throne::draw(unsigned int texture, GLuint shaderprog)
{
	//send material information to the shader 
	//We don't need to do anything special

	Unit::draw(texture, shaderprog);
}

bool Throne::move(Location *, Map *)
{
	return false;
}

bool Throne::collect(Location *, Map*)
{
	return false;
}

bool Throne::build(int x, int y, std::string obj)
{
	return false;
}

void Throne::spawn(Location * location, int obj)
{
	if (!spawnTimer) {

		//Some method of choosing which Class we're spawning, local variable that gets set with upgrades, use as obj
		
		//use obj as a selector? or just don't

		
		//for now just spawn Worms
		Unit::spawn(location, WORM_CLASS_T);



	}
}

bool Throne::consume(Unit * food)
{
	return false;
}
