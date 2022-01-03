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

			from_endpoint.address = from.sin_addr.S_un.S_addr;
			from_endpoint.port = from.sin_port;

			parse();

		}


		handleInput();

		//Create the state packet
		buffer[0] = (uint8)Server_Message::State;
		uint32 bytes_written = 1;


		//basically fill with Player_State
		for (uint16 i = 0; i < MAX_CLIENTS; ++i)
		{

			/*
			if (client_endpoints[i].address)
			{
				

				memcpy(&buffer[bytes_written], &i, sizeof(i));
				bytes_written += sizeof(i);

				memcpy(&buffer[bytes_written], &client_objects[i].x, sizeof(client_objects[i].x));
				bytes_written += sizeof(client_objects[i].x);

				memcpy(&buffer[bytes_written], &client_objects[i].y, sizeof(client_objects[i].y));
				bytes_written += sizeof(client_objects[i].y);


			}
			*/

			

		}

		//And fill with non-player dependent state
		for (Delta_State delt : deltas) {

			memcpy(&buffer[bytes_written], &delt.playerCode, sizeof(delt.playerCode));
			bytes_written += sizeof(delt.playerCode);

			memcpy(&buffer[bytes_written], &delt.x0, sizeof(delt.x0));
			bytes_written += sizeof(delt.x0);

			memcpy(&buffer[bytes_written], &delt.y0, sizeof(delt.y0));
			bytes_written += sizeof(delt.y0);

			memcpy(&buffer[bytes_written], &delt.x1, sizeof(delt.x1));
			bytes_written += sizeof(delt.x1);

			memcpy(&buffer[bytes_written], &delt.y1, sizeof(delt.y1));
			bytes_written += sizeof(delt.y1);

			memcpy(&buffer[bytes_written], &delt.actionCode, sizeof(delt.actionCode));
			bytes_written += sizeof(delt.actionCode);

		}

		//clear it so that we don't do things multiple times
		deltas.clear();


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

				if (client_endpoints[i].address == local_address.sin_addr.S_un.S_addr) {

					//basically we don't need to tell the host twice since the server already updated? Change if this causes sync issues.
					continue;


				}



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

	printf("Server has Startup \n");
	return 0;
}

void Server::parse() //here we validate the input as much as possible
{
	switch ((Client_Message)buffer[0])
	{
	case Client_Message::Join:
	{
		//not many players so loop through the endpoints to make sure we don't already have this player
		//there is nothing more to validate on this message for now

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

			int bytes_written = 4;

			buffer[1] = 1;
			memcpy(&buffer[2], &slot, 2);  //if found send index


			//now for every player that we currently have we send their IP, and player number

			for (uint16 plSlot = 0; plSlot < MAX_CLIENTS; plSlot++) {
				int numPlayers = 0;
				//
				if (client_endpoints[plSlot].address) {
					memcpy(&buffer[4 + numPlayers * 6], &plSlot, 2);
					memcpy(&buffer[6 + numPlayers * 6], &client_endpoints[plSlot].address, 4);
					numPlayers++;

					//we don't send the port information for now, since we don't need it?

					bytes_written += 6;
				}
			}

			buffer[bytes_written++] = 'z'; //this will tell us to stop reading?
			


			flags = 0;
			if (sendto(sock, buffer, bytes_written, flags, (SOCKADDR*)&from, from_size) != SOCKET_ERROR)  //send this to all current players for menu and playerType setup?
			{
				client_endpoints[slot] = from_endpoint;
				time_since_heard_from_clients[slot] = 0.0f;
				client_objects[slot] = {};
				client_inputs[slot] = {};

				//and update our waitroom?

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

			//validate here somehow check for illegal chars? this will happen later in parse/handleinput

			uint8 cmd = buffer[3];

			client_inputs[slot].input = cmd;
			client_inputs[slot].x = buffer[4];
			client_inputs[slot].y = buffer[5];
			time_since_heard_from_clients[slot] = 0.0f;
		}


		break;
	}
	case Client_Message::Event:
	{
		Event_Info evt;
		memcpy(&evt.player, &buffer[1], 2);
		evt.type = buffer[3];
		evt.x = buffer[4];
		evt.y = buffer[5];
		memcpy(&evt.arg0, &buffer[6], 2);
		memcpy(&evt.arg1, &buffer[8], 2);
		memcpy(&evt.arg2, &buffer[10], 2);
		memcpy(&evt.arg3, &buffer[12], 2);

		handleEvent(evt);

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
	default:
	{
		printf("Invalid first bit in parse(), was %d", buffer[0]);
		break;
	}

	}
}

void Server::handleInput()
{
	//Handle the inputs
	for (uint8 i = 0; i < MAX_CLIENTS; ++i)
	{
		uint8 key = client_inputs[i].input;
		int x, y;

		if (client_endpoints[i].address && key ) //Make sure we actually got an input
		{

			if (key == GLFW_KEY_NOINPUT) {

				//do nothing

			}
			else if (key == CMD_LEAVE) {

				//disconnect from server and cleanup




			}
			else if (key <= CMD_MAXCODE) {
				x = client_inputs[i].x;
				y = client_inputs[i].y;

				//Basically do the code from gamemode key callback - but with different codes and remotely

				//for player i we tell all units in selection to move to location, like in player! actually exactly in player

				GmToServerClient::sendCommand(key, i, x, y);


				//Then we construct a piece of the update message: we add a delta to the list of deltas!


				// Have classcode for player and they will move all their selection!, actually some actions will just be player based


				Delta_State delt = { i, 0, 0, x, y, key };

				deltas.push_back(delt); //saves the player action delta

				//send to all clients except the client who sent the message? OR send to all clients except Host? probs later for sync
				//NO send to all clients at the end of the tick
			}
			else {
				printf("unhandled input %c  from player %d \n", key, i);
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

void Server::handleEvent(Event_Info evt)
{
	GmToServerClient::sendEvent(evt.type, evt.player, evt.x, evt.y, evt.arg0, evt.arg1, evt.arg2, evt.arg3);
}

