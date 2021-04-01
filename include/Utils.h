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
	template <typename T>
	static void vecRemove(T* res, std::vector<T*>& list);
	static int vecSearch(Location * location, std::vector<Location*>& list);
	static bool vecSearchInt(int i, std::vector<int>& vec);
};

//Make all template?

template<typename T>
inline void Utils::vecRemove(T * res, std::vector<T*>& list)
{
	typename std::vector<T *>::iterator it;
	for (it = list.end(); it != list.begin();) {
		if (*--it == res) {
			list.erase(it);
			break;
		}
	}
}
