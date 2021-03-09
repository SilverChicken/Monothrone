#pragma once
#include <list>
#include <vector>

#include "Location.h"

class Utils
{
public:
	static bool listSearch(Location * location, std::list<Location*> & visited);  //Just loops from the back to the front, more likely to find
	static bool const locComp(std::pair<Location*, float>, std::pair<Location*, float>); //For priority Queue Ordering
	static float calcDist(Location*, Location*); //Gives float distance
};

