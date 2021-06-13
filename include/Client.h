#pragma once
#include "Server_Common.h"


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

	uint8 input; 

	Client() { Startup(); };

public :	

	static Client* getInstance();

	int run(); //Basically checks if we have input sends either an idle/input/join/leave based on inputs then also unpacks the server msg
	void stop(); //stops the run loop

	//Timing info
	int tick();
	void resetTick();

	void addInput(int key);

	void setServerAddress(char* addi);
	
	~Client() { Cleanup();  };
};

