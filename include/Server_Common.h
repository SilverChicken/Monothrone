#pragma once
#include <stdio.h>
#include <WinSock2.h>



#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wsock32.lib")





//GLFW key definitions kept consistent through game, server & client


/* The unknown key */
#define GLFW_KEY_UNKNOWN            -1
#define GLFW_KEY_NOINPUT            GLFW_KEY_UNKNOWN

/* Printable keys */
#define GLFW_KEY_SPACE              32
#define GLFW_KEY_APOSTROPHE         39  /* ' */
#define GLFW_KEY_COMMA              44  /* , */
#define GLFW_KEY_MINUS              45  /* - */
#define GLFW_KEY_PERIOD             46  /* . */
#define GLFW_KEY_SLASH              47  /* / */
#define GLFW_KEY_0                  48
#define GLFW_KEY_1                  49
#define GLFW_KEY_2                  50
#define GLFW_KEY_3                  51
#define GLFW_KEY_4                  52
#define GLFW_KEY_5                  53
#define GLFW_KEY_6                  54
#define GLFW_KEY_7                  55
#define GLFW_KEY_8                  56
#define GLFW_KEY_9                  57
#define GLFW_KEY_SEMICOLON          59  /* ; */
#define GLFW_KEY_EQUAL              61  /* = */
#define GLFW_KEY_A                  65
#define GLFW_KEY_B                  66
#define GLFW_KEY_C                  67
#define GLFW_KEY_D                  68
#define GLFW_KEY_E                  69
#define GLFW_KEY_F                  70
#define GLFW_KEY_G                  71
#define GLFW_KEY_H                  72
#define GLFW_KEY_I                  73
#define GLFW_KEY_J                  74
#define GLFW_KEY_K                  75
#define GLFW_KEY_L                  76
#define GLFW_KEY_M                  77
#define GLFW_KEY_N                  78
#define GLFW_KEY_O                  79
#define GLFW_KEY_P                  80
#define GLFW_KEY_Q                  81
#define GLFW_KEY_R                  82
#define GLFW_KEY_S                  83
#define GLFW_KEY_T                  84
#define GLFW_KEY_U                  85
#define GLFW_KEY_V                  86
#define GLFW_KEY_W                  87
#define GLFW_KEY_X                  88
#define GLFW_KEY_Y                  89
#define GLFW_KEY_Z                  90
#define GLFW_KEY_LEFT_BRACKET       91  /* [ */
#define GLFW_KEY_BACKSLASH          92  /* \ */
#define GLFW_KEY_RIGHT_BRACKET      93  /* ] */
#define GLFW_KEY_GRAVE_ACCENT       96  /* ` */
#define GLFW_KEY_WORLD_1            161 /* non-US #1 */
#define GLFW_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GLFW_KEY_ESCAPE             256
#define GLFW_KEY_ENTER              257
#define GLFW_KEY_TAB                258
#define GLFW_KEY_BACKSPACE          259
#define GLFW_KEY_INSERT             260
#define GLFW_KEY_DELETE             261
#define GLFW_KEY_RIGHT              262
#define GLFW_KEY_LEFT               263
#define GLFW_KEY_DOWN               264
#define GLFW_KEY_UP                 265
#define GLFW_KEY_PAGE_UP            266
#define GLFW_KEY_PAGE_DOWN          267
#define GLFW_KEY_HOME               268
#define GLFW_KEY_END                269
#define GLFW_KEY_CAPS_LOCK          280
#define GLFW_KEY_SCROLL_LOCK        281
#define GLFW_KEY_NUM_LOCK           282
#define GLFW_KEY_PRINT_SCREEN       283
#define GLFW_KEY_PAUSE              284
#define GLFW_KEY_F1                 290
#define GLFW_KEY_F2                 291
#define GLFW_KEY_F3                 292
#define GLFW_KEY_F4                 293
#define GLFW_KEY_F5                 294
#define GLFW_KEY_F6                 295
#define GLFW_KEY_F7                 296
#define GLFW_KEY_F8                 297
#define GLFW_KEY_F9                 298
#define GLFW_KEY_F10                299
#define GLFW_KEY_F11                300
#define GLFW_KEY_F12                301
#define GLFW_KEY_F13                302
#define GLFW_KEY_F14                303
#define GLFW_KEY_F15                304
#define GLFW_KEY_F16                305
#define GLFW_KEY_F17                306
#define GLFW_KEY_F18                307
#define GLFW_KEY_F19                308
#define GLFW_KEY_F20                309
#define GLFW_KEY_F21                310
#define GLFW_KEY_F22                311
#define GLFW_KEY_F23                312
#define GLFW_KEY_F24                313
#define GLFW_KEY_F25                314
#define GLFW_KEY_KP_0               320
#define GLFW_KEY_KP_1               321
#define GLFW_KEY_KP_2               322
#define GLFW_KEY_KP_3               323
#define GLFW_KEY_KP_4               324
#define GLFW_KEY_KP_5               325
#define GLFW_KEY_KP_6               326
#define GLFW_KEY_KP_7               327
#define GLFW_KEY_KP_8               328
#define GLFW_KEY_KP_9               329
#define GLFW_KEY_KP_DECIMAL         330
#define GLFW_KEY_KP_DIVIDE          331
#define GLFW_KEY_KP_MULTIPLY        332
#define GLFW_KEY_KP_SUBTRACT        333
#define GLFW_KEY_KP_ADD             334
#define GLFW_KEY_KP_ENTER           335
#define GLFW_KEY_KP_EQUAL           336
#define GLFW_KEY_LEFT_SHIFT         340
#define GLFW_KEY_LEFT_CONTROL       341
#define GLFW_KEY_LEFT_ALT           342
#define GLFW_KEY_LEFT_SUPER         343
#define GLFW_KEY_RIGHT_SHIFT        344
#define GLFW_KEY_RIGHT_CONTROL      345
#define GLFW_KEY_RIGHT_ALT          346
#define GLFW_KEY_RIGHT_SUPER        347
#define GLFW_KEY_MENU               348
#define GLFW_KEY_LAST               GLFW_KEY_MENU



#define SOCKET_BUFFER_SIZE 1024			//Max amount we'll send
#define MAX_CLIENTS 5
#define CLIENT_TIMEOUT 5                //5 seconds of no messages = timeout

typedef char uint8;
typedef bool bool32;
typedef float float32;
typedef unsigned int uint32;
typedef unsigned short uint16;


const uint32	TICKS_PER_SECOND = 15;
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
	Collision   // tell server there is an upcoming collision: gonna run into a thing/movement is blocked
};

enum class Server_Message : uint8
{
	Join_Result,       // tell client they're accepted/rejected
	State,             // tell client game state = list of deltas about what's going on
	Collision_Result   // tell client what happened in regards to the collision
};


//Information we need to package and send to the Clients
struct Player_State
{
	int x, y;
	bool valid;

};



struct Delta_State
{
	uint8 x0, y0;     //initial position of object-> where we fetch the object from
	uint8 x1, y1;	  //final position if required for action, otherwise -1 -1 (for move will be destination)
	uint8 actionCode; //any other info about the change: selected, deselected, destroyed, spawned, collected/ing... careful that the type supports it
};

//Information we get from Client

//For now we take 1 input at a time, we can configure that in the future

struct Player_Input
{
	uint8 input; // What key was pressed?
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