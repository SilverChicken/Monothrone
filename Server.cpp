#include "Server.h"
#include <timeapi.h>

//Find ways of setting
//Vars that Server Populates from Input
uint32 playerID = 0;
char playerInput = 0;


//Vars Populated for Output

//unit position delta
//Ressource delta/value
//selection delta


//Info/Function vars
bool is_running = 1;



//Timing data
//Timing_Info timing_info;

IP_Endpoint client_endpoints[MAX_CLIENTS];
float32 time_since_heard_from_clients[MAX_CLIENTS];
Player_State client_objects[MAX_CLIENTS];
Player_Input client_inputs[MAX_CLIENTS];

/*
int main() {
	Server* serv = new Server();
	serv->run();
	return 0;
}
*/


int Server::run()
{

	while (is_running) //This is one tick
	{
		//Start measuring time
		LARGE_INTEGER tick_start_time;
		QueryPerformanceCounter(&tick_start_time);


		//Here we could created a new thread with std::thread, make it run recvfrom then continue.
		//For now we just make recvfrom not block

		while (true)
		{
			
			from_size = sizeof(from);
			int bytes_received = recvfrom(sock, buffer, SOCKET_BUFFER_SIZE, flags, (SOCKADDR*)&from, &from_size);

			if (bytes_received == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
					printf("recvfrom returned SOCKET_ERROR, WSAGetLastError() %d\n", error);
				}

				break;
			}

			
			from_endpoint.address = from.sin_addr.S_un.S_addr;
			from_endpoint.port = from.sin_port;

			parse();

		}


		handleInput();

		//Create the state packet
		buffer[0] = (uint8)Server_Message::State;
		uint32 bytes_written = 1;
		for (uint16 i = 0; i < MAX_CLIENTS; ++i)
		{
			if (client_endpoints[i].address)
			{
				//basically fill with Player_State

				memcpy(&buffer[bytes_written], &i, sizeof(i));
				bytes_written += sizeof(i);

				memcpy(&buffer[bytes_written], &client_objects[i].x, sizeof(client_objects[i].x));
				bytes_written += sizeof(client_objects[i].x);

				memcpy(&buffer[bytes_written], &client_objects[i].y, sizeof(client_objects[i].y));
				bytes_written += sizeof(client_objects[i].y);


			}

			//And fill with non-player dependent state

			//
			//
			//


		}


		// send back to client ALL CLIENTS
		int flags = 0;
		SOCKADDR_IN to;
		to.sin_family = AF_INET;
		int to_length = sizeof(to);

		for (uint16 i = 0; i < MAX_CLIENTS; ++i)
		{
			if (client_endpoints[i].address)
			{

				
				//Build message here for specific client




				to.sin_addr.S_un.S_addr = client_endpoints[i].address;
				to.sin_port = client_endpoints[i].port;

				if (sendto(sock, buffer, bytes_written, flags, (SOCKADDR*)&to, to_length) == SOCKET_ERROR) {
					printf("sendto failed: %d", WSAGetLastError());
					return 1;
				}


				//print state
				printf("Player coord %d, %d \n", client_objects[i].x, client_objects[i].y);


			}
		}


		

		wait_for_tick_end(tick_start_time, &timing_info);

	}

	return 0;

}


void Server::Cleanup()
{ //Cleanup ressources
	WSACleanup();
}

bool Server::IsRunning()
{
	return is_running;
}

int Server::Startup()
{
	StartWSA();

	CreateSocket(sock);

	//Bind the socket
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(PORT);
	local_address.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*)&local_address, sizeof(local_address)) == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		return 1;
	}



	//Set the Timer Granularity (remember to reset when paused/minimized/server ends)
	timing_info = timing_info_create();



	//set recvfrom to not block
	u_long enabled = 1;
	ioctlsocket(sock, FIONBIO, &enabled);

	printf("Server has Startup");
	return 0;
}

void Server::parse()
{
	switch ((Client_Message)buffer[0])
	{
	case Client_Message::Join:
	{
		//not many players so loop through the endpoints to make sure we don't already have this player


		uint16 slot = uint16(-1);
		for (uint16 i = 0; i < MAX_CLIENTS; ++i) //Find free slot in client array
		{
			if (client_endpoints[i].address == 0)
			{
				slot = i;
			} else if(client_endpoints[i].address == from_endpoint.address) 
			{ //Then this player already has a slot
				slot = i;
				break;
			}
		}

		buffer[0] = (uint8)Server_Message::Join_Result;
		if (slot != uint16(-1))
		{
			buffer[1] = 1;
			memcpy(&buffer[2], &slot, 2);  //if found send index

			flags = 0;
			if (sendto(sock, buffer, 4, flags, (SOCKADDR*)&from, from_size) != SOCKET_ERROR)
			{
				client_endpoints[slot] = from_endpoint;
				time_since_heard_from_clients[slot] = 0.0f;
				client_objects[slot] = {};
				client_inputs[slot] = {};
			}
		}
		else  //else tell that the request was rejected.
		{
			buffer[1] = 0;

			flags = 0;
			sendto(sock, buffer, 2, flags, (SOCKADDR*)&from, from_size);
		}
	}
	break;

	case Client_Message::Leave:
	{

		uint16 ID;
		memcpy(&ID, &buffer[1], 2);
		IP_Endpoint playerIP;
		if (ID < MAX_CLIENTS) {
			playerIP = client_endpoints[ID];
		}
		else {
			printf("Invalid leave message from %d:%d, gave ID:%d", from_endpoint.address, from_endpoint.port, ID);
			break;
		}
		if (playerIP == from_endpoint) { //So this is valid message
			client_endpoints[ID] = {};
		}
		else {
			printf("Invalid leave message from %d:%d, gave ID:%d, did not match", from_endpoint.address, from_endpoint.port, ID);
			break;
		}

	}
	break;

	case Client_Message::Input:
	{

		//First take the player ID (int) from the buffer so we know which one it is

		uint16 slot;
		memcpy(&slot, &buffer[1], 2);

		if (client_endpoints[slot] == from_endpoint)
		{
			//Handle the input later, rn create the input struct for the user in question

			//printf("input received");

			//validate here somehow check for illegal chars?

			uint8 key_in = buffer[3];
			client_inputs[slot].input = key_in;
			time_since_heard_from_clients[slot] = 0.0f;
		}


		break;
	}
	case Client_Message::Collision:
	{
		

		//send Server_Message::Collision_Result
		break;
	}
	}
}

void Server::handleInput()
{
	//Handle the inputs
	for (uint16 i = 0; i < MAX_CLIENTS; ++i)
	{
		uint8 key = client_inputs[i].input;
		if (client_endpoints[i].address && key) //Make sure we actually got an input
		{
			//Basically do the code from gamemode key callback
			switch (key)
			{
			case (uint8)GLFW_KEY_UP:
				client_objects[i].y++;
				break;
			case (uint8)GLFW_KEY_DOWN:
				client_objects[i].y--;
				break;
			case (uint8)GLFW_KEY_RIGHT:
				client_objects[i].x++;
				break;
			case (uint8)GLFW_KEY_LEFT:
				client_objects[i].x--;
				break;
			case (uint8)GLFW_KEY_ESCAPE:
				is_running = false;
				break;
			case (uint8)GLFW_KEY_NOINPUT:
				//do nothing
				break;
			default:
				printf("unhandled input %c\n from player %d", key, i);
				break;
			}

			time_since_heard_from_clients[i] += DELTA_TICK;
			if (time_since_heard_from_clients[i] > CLIENT_TIMEOUT)
			{
				client_endpoints[i] = {};
			}

			//'clean the input'
			client_inputs[i] = {};

		}

	}
}

