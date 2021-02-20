#pragma once
#include<ctime>

//This will be singleton class

class Timer //This class will be used as a trashy imprecise timer. It increments when Window::idle_callback() is called
{           //Eventually this will be managed by the Server manager, so everything is synced
private:
	int tick = 100;   //length of tick
	int count = 0;     //current point
	void ontick();

	Timer() {};


public:

	static Timer& getInstance();

	bool state = false;  //This variable will be checked by Window and any others who use the timer
	void inc();
};

