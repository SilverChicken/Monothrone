#pragma once
#include<unordered_map>


//This class will manage interactions between the following parties.
//Some of these will have a reference to GameMode from which they can call the interactions
//This class will be held by Window which will then only be in charge of managing GLFW and this class
class Player;
class Map;
class Unit;
class Throne;
class Timer; // maybe also manage the timer?

class Gamemode
{
private:
	Map* map;
	std::unordered_map<int, Unit *> Thrones;
	std::unordered_map<int, Player *> Players;

	Gamemode() {};

public:

	~Gamemode();
	static Gamemode& getInstance();

	Unit* getThrone(int);
	Player* getPlayer(int);
	void addThrone(int, Unit*);
	void addPlayer(int, Player*);

};

