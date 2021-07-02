#pragma once
#include "Server_Common.h"
#include <vector>

class Client
{
private:
	int Startup(const char* Saddress = "127.0.0.1"); //default is local server
	void Cleanup();

	//Info vars
	bool is_running = false;
	int shouldTick = 0;

	//Server info
	SOCKADDR_IN server_address;    //Where this server is
	SOCKET sock;

	Timing_Info timing_info;

	uint8 buffer[SOCKET_BUFFER_SIZE];
	int bytes_written;
	int flags = 0;

	Player_State objects[MAX_CLIENTS];
	uint32 num_objects = 0;
	uint16 slot = 0xFFFF;

	Client_Message msgType = Client_Message::Join;

	void ConstructMessage();


	//gameplay vars
	uint8 input; 
	int x0, y0;

	//temp variables for populating 
	int popCount = 0;
	std::vector<Populate_Msg> pops;

	Client() { Startup(); };

public :	

	static Client* getInstance();

	int run(); //Basically checks if we have input sends either an idle/input/join/leave based on inputs then also unpacks the server msg
	void stop(); //stops the run loop

	//Timing info
	int tick();
	void resetTick();

	//Control function for gamemode to use
	void addInput(int key);
	void addLoc(int x, int y);
	void sendPopMsg();

	uint16 getSlot();

	void setServerAddress(char* addi);
	
	~Client() { Cleanup();  };
};

