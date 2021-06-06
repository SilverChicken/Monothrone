#pragma once
#include "Server_Common.h"


class Client
{
private:
	int Startup(const char* Saddress = "127.0.0.1"); //default is local server
	void Cleanup();

	//Server info
	SOCKADDR_IN server_address;    //Where this server is
	SOCKET sock;

	uint8 buffer[SOCKET_BUFFER_SIZE];
	int bytes_written;
	int flags = 0;

	Player_State objects[MAX_CLIENTS];
	uint32 num_objects = 0;
	uint16 slot = 0xFFFF;

	Client_Message msgType = Client_Message::Join;

	void ConstructMessage();

	uint8 input; 

public :	

	int run(); //Basically checks if we have input sends either an idle/input/join/leave based on inputs then also unpacks the server msg

	void addInput(int key);

	
	Client() { Startup(); };
	~Client() { Cleanup();  };
};

