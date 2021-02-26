#include "Worker.h"

Worker::Worker(int owner, Location * loc, Map * map):Unit(owner, loc, map)
{
	hp = 3;
	atk = 1;
	speed = 1;
	textLoc = textLocs[owner];

	classType = WORKER_CLASS_T;

	//Define which actions are defined
	actions[0] = true;
	actions[1] = true;
	actions[2] = false;
	actions[3] = false;
	actions[4] = true;
	actions[5] = false;
}

Worker::~Worker()
{
	delete[] actions;
}

void Worker::draw(unsigned int texture, GLuint shaderprog)
{
	//send material information to the shader
	//nothing special here either

	Unit::draw(texture, shaderprog);
}

void Worker::update(Map * map)
{
	if (!path.empty()) { //Only update if moving
		animState = (animState + ANIMSTEP) % NUMTEX;
		textLoc = textLocs[animState];
	}

	Unit::update(map);
}

bool Worker::move(Location * location, Map * map)
{
	return Unit::move(location, map);
}

bool Worker::collect(Location * location, Map * map)
{
	return Unit::collect(location, map);
}

bool Worker::build(int x, int y, std::string obj)
{
	return false;
}

Unit * Worker::spawn(std::string baby, glm::vec2 place)
{
	return nullptr;
}

bool Worker::consume(Unit * food)
{
	return false;
}
