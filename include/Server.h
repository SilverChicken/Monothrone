#pragma once

#include "Server_Common.h"



class Server
{
private:

	//Info/Function vars
	bool is_running = false;


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
	int msgCount = 0; //for populate message counting

	
	uint8 buffer[SOCKET_BUFFER_SIZE];

	


	int Startup(); //Currently start and doesn't stop running

	void parse();

	void handleInput();

	Server() { Startup(); };

public:

	static Server* getInstance();


	~Server() {	Cleanup();};

	void Cleanup();

	int run();
	void stop();
	void sendPopMsg();

	bool IsRunning();

	
};

