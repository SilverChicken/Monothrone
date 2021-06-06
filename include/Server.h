#pragma once

#include "Server_Common.h"



class Server
{
private:

	//Time measurement
	UINT sleep_granularity_ms;
	bool32 sleep_granularity_was_set;
	LARGE_INTEGER clock_frequency;

	//Server data
	SOCKADDR_IN local_address;    //Where this server is
	SOCKET sock;

	Timing_Info timing_info;


	//Temp saves
	IP_Endpoint from_endpoint;    //used to store most recent message
	int flags = 0;
	SOCKADDR_IN from;
	int from_size;

	//Temp
	uint32 player_x = 0, player_y = 0;

	
	uint8 buffer[SOCKET_BUFFER_SIZE];

	


	int Startup(); //Currently start and doesn't stop running

	void parse();

	void handleInput();



public:

	Server() { Startup(); };
	~Server() {	Cleanup();};

	void Cleanup();

	int run();

	bool IsRunning();

	
};

