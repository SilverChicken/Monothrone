#include "Timer.h"


void Timer::ontick() //calls all of the tick functions for the 
{
	state = true;
}

Timer & Timer::getInstance()
{
	static Timer instance;
	return instance;
}

void Timer::inc()
{
	count++;
	if (count > tick){
		count = 0;
		ontick();
		return;
	}
	state = false;
	return;
}
