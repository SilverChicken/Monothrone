#pragma once

#include "Server_Common.h"
#include <vector>
#include <mutex>



class Server
{
private:

	//Info/Function vars
	bool is_running = false;

	//REMOVE
	std::mutex event_mtx;

	//Time measurement
	UINT sleep_granularity_ms;
	bool32 sleep_granularity_was_set;
	LARGE_INTEGER clock_frequency;

	//Server data
	SOCKADDR_IN local_address;    //Where this server is
	SOCKET sock;

	Timing_Info timing_info;

	//Game info

	//REMOVE
	std::vector<Delta_State> deltas;
	Event_Info event_info;

	//Temp saves
	IP_Endpoint from_endpoint;    //used to store most recent message
	int flags = 0;
	SOCKADDR_IN from;
	int from_size;

	//Temp
	uint32 player_x = 0, player_y = 0;
	int msgCount = 0; //for populate message counting

	
	uint8 buffer[SOCKET_BUFFER_SIZE];
	//remove
	uint8 eventBuffer[SOCKET_BUFFER_SIZE];
	


	int Startup(); //Currently start and doesn't stop running
	bool SendWithRetry(uint8* buf);

	void parse();

	void handleInput();
	void handleEvent(Event_Info evt);

	//Remove all
	//Event emission and management
	void sendEvent();
	void addEventype(int type);
	void addEventArgs(int argNum, uint16 arg);
	void addEventLoc(int x, int y);

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

