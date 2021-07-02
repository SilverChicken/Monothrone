#include "Server.h"
#include "GmToServerClient.h"

#include <timeapi.h>

//Find ways of setting
//Vars that Server Populates from Input
//uint32 playerID = 0;
//char playerInput = 0;


//Vars Populated for Output

//unit position delta
//Ressource delta/value
//selection delta





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
	is_running = true;
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

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////smth here is not right
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
				//printf("Player coord %d, %d \n", client_objects[i].x, client_objects[i].y);


			}
		}


		

		wait_for_tick_end(tick_start_time, &timing_info);

	}

	return 0;

}

void Server::stop()
{
	is_running = false;
}

void Server::sendPopMsg()
{
	// 0      1 - 4   5 - .... whenever
	// type   #msgs   type, x, y, owner (if type > UNITCLASST )
	//Careful, not all messages are same size, check for parsing in client

	std::vector<Populate_Msg> pops;
	GmToServerClient::createPopInfo(pops);

	//first send count of the populate messages. 
	if (msgCount == 0) {
		msgCount = pops.size();
	}
	
	uint32 bytes = 0;

	buffer[0] = (uint8)Server_Message::Populate_Result;
	bytes++;

	//might be bigger than 1 byte so take 4 slots in case : 16 bits
	memcpy(&buffer[1], &msgCount, 4);
	bytes += 4;

	for (Populate_Msg pop : pops) {
		buffer[bytes] = pop.type;
		bytes++;
		buffer[bytes] = pop.x;
		bytes++;
		buffer[bytes] = pop.y;
		bytes++;

		if (pop.type > UNIT_TYPE) {
			buffer[bytes] = pop.owner;
			bytes++;
		}

		//we have one fewer msg to send
		msgCount--;

		//Check if we've filled buffer up
		if (bytes >= BUFSIZ - 4 || msgCount == 0) {  //This msg might be broken up into 5/6 chunks or more


			//send a chunk to all the clients
			//Doesn't matter who sent the request, update everyone if performance issue, change rate at which clients send request to pop


			for (IP_Endpoint IP : client_endpoints) {

				if (IP.address) {
					SOCKADDR_IN to;
					to.sin_family = AF_INET;
					to.sin_addr.S_un.S_addr = IP.address;
					to.sin_port = IP.port;
		
					

					if (sendto(sock, buffer, bytes, flags, (SOCKADDR*)&to, sizeof(to)) != SOCKET_ERROR)
					{
						//then the message sent correctly
					}
					else {
						printf("Failed to send Populate Message to Client at address %d \n", IP.address);
						printf("WSA error: %d \n", WSAGetLastError());
					}
				}
				

				//regardless we keep going atm

			}
			//Rebuild the start of the message and update the new count of messages

			bytes = 0;

			memcpy(&buffer[1], &msgCount, 4);
			bytes += 5;
			//then we'll start filling it up again!
			//on the last iteration this is overhead

		}


	}
}


Server* Server::getInstance()
{
	static Server* serv;
	if (serv) {
		return serv;
	}
	serv = new Server();
	serv->run();
	return serv;
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

			uint8 cmd = buffer[3];
			client_inputs[slot].input = cmd;
			client_inputs[slot].x = buffer[4];
			client_inputs[slot].y = buffer[5];
			time_since_heard_from_clients[slot] = 0.0f;
		}


		break;
	}
	case Client_Message::Collision:
	{
		

		//send Server_Message::Collision_Result
		break;
	}
	case Client_Message::Populate:
	{	
		sendPopMsg();
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
		int x, y;

		if (client_endpoints[i].address && key) //Make sure we actually got an input
		{

			x = client_inputs[i].x;
			y = client_inputs[i].y;

			//Basically do the code from gamemode key callback
			switch (key)
			{
			case CMD_MOVE:
				//for player i we tell all units in selection to move to location, like in player! actually exactly in player
				
				//Server NOT will hold references to all the players/or get them from gamemode
				GmToServerClient::sendCommand(CMD_MOVE, i, x, y);
				//then we make it happen


				//Then we construct a message:


				//Loop through all of the units of the player that are selected


				//for each one we add a delta of their location, and their destination with CMD_MOVE as the command



				break;
			case CMD_COLLECT:
				break;
			case CMD_BUILD:		   
				break;
			case CMD_SPAWN:		   
				break;
			case CMD_CONSUME:	   
				break;
			case CMD_SELECT:	  
				break;
			case CMD_DESELECT:	   
				break;
			case CMD_DESELECT_ALL: 
				break;
			case CMD_PAUSE:		   
				break;
			case CMD_LEAVE:		   
				break;
			case (uint8)GLFW_KEY_NOINPUT:
				//do nothing
				break;
			default:
				printf("unhandled input %c  from player %d \n", key, i);
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

