#include "Utils.h"

bool Utils::listSearch(Location * location, std::list<Location*>& list)
{
	std::list<Location *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == location) {
			return true;
		}
	}
	return false;
}

float Utils::calcDist(Location * a, Location * b)
{
	float x = a->getPos().x - b->getPos().x;
	float y = a->getPos().y - b->getPos().y;
	return (sqrt(x*x + y * y));
}

bool const Utils::locComp(std::pair<Location*, float> a , std::pair<Location*, float> b)
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

int Utils::vecSearch(Location * location, std::vector<Location*>& list) //linear search from the back
{
	std::vector<Location *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == location) {
			return true;
		}
	}
	return false;
}
