#pragma once
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include<unordered_map>
#include <thread>




#define MAX_PLAYER 3


//Avg # of Mountain, Crystal and Energy clusters -> will be passed in by menu
#define MTNRangeCT 15
#define Vmtn 6
#define NRGRangeCT 6
#define Vnrg 4
#define CRYRangeCT 8
#define Vcry 4

#define MINCOMP 64   //Minimum Area for a component to not be filled


//This class will manage interactions between the following parties.
//Some of these will have a reference to GameMode from which they can call the interactions
//This class will be held by Window which will then only be in charge of managing GLFW and this class
class Player;
class PlayerType;
class Map;
class Location;
class Unit;
class Throne;
class Ressource;
class Wall;
class Gui;


class Client;
class Server;
class Menu;

struct Populate_Msg;

class Gamemode
{
private:

	//Timer for tick function
	//Timer ticker = Timer::getInstance();

	//Server/Client
	Server* serv = nullptr;
	Client* client = nullptr;
	bool isHost = false;

	//Threads
	std::thread Servthread;
	std::thread Clithread;



	//Objects we're rendering
	Map* map = nullptr;
	Player * player = nullptr;
	Gui * gui = nullptr;  //-> will need multiple for multiplayer?
	Menu* menu = nullptr;

	int mode = 0; //0 is menu, 1 is playing
	int localPlayer = 0; //set to whichever our local player is
	bool pause = false;
	

	//std::vector<Unit *> Enemies; //Refs to the temp baddies for drawing

	std::unordered_map<int, Unit *> Thrones;
	std::unordered_map<int, PlayerType *> Players;
	std::unordered_map<Wall*, int> Walls;

	std::vector<Ressource *> Mountains; //Refs to the mountains
	std::vector<Ressource *> Crystals;  //Refs to the crystals
	std::vector<Ressource *> Energies;  //Refs to the energies

	Gamemode() {};
	void cleanup();

	void categorizeAccess(); //gets connected components, fills the smaller ones with locs
	void SpawnStartRessource(Map * mapping, int MTN, int VarMtn, int NRG, int Varnrg, int CRY, int Varcry);

public:

	~Gamemode();
	static Gamemode& getInstance();


	void init();    //Load pre-stuff and start client
	void init2();   //start server and do the connection
	void init3();   //Create the map and place all the ressource it, communicate it to all parties
	


	//Interfacing Unit -> Player and relay to Gui
	void incRessource(int val, int player);
	void updateGuiBind(int* pBind);
	void updateGuiUnit(std::vector<int>&);
	Unit* spawnUnit(int player, int obj, Location* spawnLoc);
	void getSelection(int player, std::vector<Unit*>& out);
	
	//Interface Server/Rest of world
	void createPopulateInfo(std::vector<Populate_Msg>& out);
	void populateMap(std::vector<Populate_Msg>& in);

	//Interface with Menu
	void addMenuPlayer(int slot, char* address);

	Location* findClosestType(Location *, int);    //finds nearest locateable of the type passed in as int.
	Location* findClosestNotOwned(Location*, int, int); //finds nearest Unit that is owned by another player

	//Super specific definition, used to move thru your own units, or like get other units? make take unit and cnd unit
	Location* findClosestToCnd(Location*, Location*, Unit*, const bool cnd(Unit*, Location*));

	Location* findClosestType(Location *, int, int bound); //Same as findClosestType but bounded
	Location* findClosestToCnd(Location*, Location*, Unit*, const bool cnd(Unit*, Location*), int);

	std::vector<Ressource *> getMountains();
	std::vector<Ressource *> getCrystals();
	std::vector<Ressource *> getEnergies();
	bool addMountain(Ressource *);        //------------------ Mountain
	bool addCrystals(Ressource *);        //------------------ Crystal
	bool addEnergies(Ressource *);        //Called whenever an Energy is Spawned
	void removeRessource(Ressource *);    //Removes Crystal or Energy
	void addWall(Wall*);
	
	int getMode();
	int setMode(int m);
	void setIsHost(bool h);

	Unit* getThrone(int);
	PlayerType* getPlayer(int);
	void addThrone(int, Unit*);
	void addPlayer(int, PlayerType*);

	void destroyUnit(Unit*);
	void dealDamageUnit(Unit*, int amount, int cause);
	void unitSpawned(Unit*);

	Map * getMap();
	

	void update();
	void draw(GLuint);
	void drawMap();
	void drawGui();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); //this is local, will send info to server & the rest
	void key_action_remote(int key, int pl, int x, int y); //this is remote, will apply info sent from server
	void event_remote(int type, int player, int x, int y, int arg0, int arg1, int arg2, int arg3);
	void server_event_remote(int type, int player, int x, int y, int arg0, int arg1, int arg2, int arg3);
	
};

