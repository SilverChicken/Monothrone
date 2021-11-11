#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include "ServerKeyCodes.h"



#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wsock32.lib")





#define SOCKET_BUFFER_SIZE 1024			//Max amount we'll send
#define MAX_CLIENTS 3
#define CLIENT_TIMEOUT 5                //5 seconds of no messages = timeout

typedef char uint8;
typedef bool bool32;
typedef float float32;
typedef unsigned int uint32;
typedef unsigned short uint16;


const uint32	TICKS_PER_SECOND = 7;
const float32	DELTA_TICK = 1.0f / float32(TICKS_PER_SECOND);

const uint16 PORT = 9999;

static bool WSAIsInit = false;
static bool socketIsInit = false;


//Tracks Clients
struct IP_Endpoint
{
	uint32 address;
	uint16 port;

	bool operator==(IP_Endpoint cmp) {
		return this->address == cmp.address && this->port == cmp.port;
	}

};


struct Timing_Info
{
	LARGE_INTEGER clock_frequency;
	bool32 sleep_granularity_was_set;
};


enum class Client_Message : uint8
{
	Join,       // tell server we're new here
	Leave,      // tell server we're leaving
	Input,      // tell server our user input
	Event,      // tell the server an event has occured
	Collision,  // tell server there is an upcoming collision: gonna run into a thing/movement is blocked
	Populate    // tell server we need to populate the map
};

enum class Server_Message : uint8
{
	Join_Result,       // tell client they're accepted/rejected
	State,             // tell client game state = list of deltas about what's going on
	Collision_Result,  // tell client what happened in regards to the collision
	Populate_Result,   // tell client where everything is
};


//Information we need to package and send to the Clients so far there isn't actually anything
struct Player_State
{
	int x, y;
	bool valid;

	//keep track of all selected units?

};

struct Delta_State
{

	//maybe only one location is necessary
	uint8 playerCode; //We must know which player is being selected

	uint8 x0, y0;     //initial position of object-> where we fetch the object from ?
	uint8 x1, y1;	  //final position if required for action, otherwise -1 -1 (for move will be destination)
	uint8 actionCode; //any other info about the change: selected, deselected, destroyed, spawned, collected/ing... careful that the type supports it
};

struct Populate_Msg 
{
	uint8 type;    //what object is match CLASSTYPE
	uint8 owner;   //does it have an owner? if not -1
	uint8 x, y;    //location
};


//Information we get from Client

//For now we take 1 input at a time, we can configure that in the future

struct Player_Input
{
	uint8 input; // What key was pressed?
	//keypress is super not enough to determine what's happening
	//So this will be the command: move, select deselect....

	uint8 x, y; //and this is the location where the action is made


	//from this the server figures out what action was mean by the keypress and sends back the delta

	


};



//WSA functions

static int StartWSA() {
	WORD winsock_version = 0x202;
	WSADATA winsock_data;
	if (WSAStartup(winsock_version, &winsock_data))
	{
		printf("WSAStartup failed: %d", WSAGetLastError());
		return 1;
	}
	WSAIsInit = true;
	return 0;
}


static int CreateSocket(SOCKET &sock) { //also binds it and fills the parameters
	int address_family = AF_INET;
	int type = SOCK_DGRAM;
	int protocol = IPPROTO_UDP;
	sock = socket(address_family, type, protocol);

	if (sock == INVALID_SOCKET)
	{
		printf("socket failed: %d", WSAGetLastError());
		return 1;
	}
	socketIsInit = true;
	return 0;
}




//Timing functions and utils

static Timing_Info timing_info_create() {

	static Timing_Info timing_info;

	if (timing_info.sleep_granularity_was_set) {
		return timing_info;
	}
	else {
		timing_info = {};

		UINT sleep_granularity_ms = 1;
		timing_info.sleep_granularity_was_set = timeBeginPeriod(sleep_granularity_ms) == TIMERR_NOERROR;

		QueryPerformanceFrequency(&timing_info.clock_frequency);

		return timing_info;
	}
	
}

static float32 time_since(LARGE_INTEGER t, LARGE_INTEGER frequency)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return float32(now.QuadPart - t.QuadPart) / float32(frequency.QuadPart);
}


static void wait_for_tick_end(LARGE_INTEGER tick_start_time, Timing_Info* timing_info)
{
	float32 time_taken_s = time_since(tick_start_time, timing_info->clock_frequency);

	while (time_taken_s < DELTA_TICK)
	{
		if (timing_info->sleep_granularity_was_set)
		{
			DWORD time_to_wait_ms = (DWORD)((DELTA_TICK - time_taken_s) * 1000);
			if (time_to_wait_ms > 0)
			{
				Sleep(time_to_wait_ms);
			}
		}

		time_taken_s = time_since(tick_start_time, timing_info->clock_frequency);
	}
}


static bool socket_receive(SOCKET* sock, uint8* buffer, uint32 buffer_size, IP_Endpoint* out_from, uint32* out_bytes_received)
{
	int flags = 0;
	SOCKADDR_IN from;
	int from_size = sizeof(from);
	int bytes_received = recvfrom(*sock, (char*)buffer, buffer_size, flags, (SOCKADDR*)&from, &from_size);

	if (bytes_received == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK)
		{
			printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", error);
		}

		return false;
	}

	*out_from = {};
	out_from->address = ntohl(from.sin_addr.S_un.S_addr);
	out_from->port = ntohs(from.sin_port);

	*out_bytes_received = bytes_received;

	return true;
}