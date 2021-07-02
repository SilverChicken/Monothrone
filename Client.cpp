#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Client.h"
#include"Server_Common.h"
#include "GmToServerClient.h"

#include <time.h>


//Timing_Info timing_info;

/*
int main() {
	Client* cl = new Client();
	cl->run();
	return 0;
}
*/

void Client::ConstructMessage()
{

	//What about data left in the buffer: Either player data or server data, should not be sensitive. Otherwise overrite it when constructing

	switch (msgType) {
	case Client_Message::Join:
		buffer[0] = (uint8)Client_Message::Join;
		bytes_written = 1;
		break;
	case Client_Message::Input:

		//printf("Inputting \n");

		if (slot != 0xFFFF)
		{
			buffer[0] = (uint8)Client_Message::Input;
			bytes_written = 1;

			memcpy(&buffer[bytes_written], &slot, sizeof(slot));
			bytes_written += sizeof(slot);

			buffer[bytes_written] = input;
			bytes_written++;

			buffer[bytes_written] = x0;
			bytes_written++;

			buffer[bytes_written] = y0;
			bytes_written++;

		}
		else { //set msg state to input or leave and send join message

		}

		break;
	case Client_Message::Leave:
		buffer[0] = (uint8)Client_Message::Leave;
		bytes_written = 1;
		memcpy(&buffer[bytes_written], &slot, sizeof(slot));
		bytes_written += sizeof(slot);
		break;
	case Client_Message::Populate:
		buffer[0] = (uint8)Client_Message::Populate;
		bytes_written = 1;
		msgType = Client_Message::Input; //we only want to send 1 populate message
		break;
	case Client_Message::Collision:
		break;

	default:
		break;
	}
}

Client* Client::getInstance()
{
	static Client* client;
	if (client) {
		return client;
	}
	client = new Client();
	client->run();
	return client;
}

int Client::run()
{
	

	printf("type w, a, s, or d to move, q to quit\n");
	is_running = true;

	while (is_running)
	{
		//Get tick
		LARGE_INTEGER tick_start_time;
		QueryPerformanceCounter(&tick_start_time);


		// get input
		//scanf_s("\n%c", &buffer[0], 1);

		//Construct the message: ID, input

		
		ConstructMessage();


		// send to server
		
		
		SOCKADDR* to = (SOCKADDR*)&server_address;
		int to_length = sizeof(server_address);
		if (sendto(sock, buffer, bytes_written, flags, to, to_length) == SOCKET_ERROR)     //sendto succeeds but the connextion closes ans server never receives anything?
		{
			printf("Message %d failed to send \n", msgType);
			printf("sendto failed: %d", WSAGetLastError());
			return 1;
		}

		//once we send the input clear it
		input = GLFW_KEY_NOINPUT;
		x0 = GLFW_KEY_NOINPUT;
		y0 = GLFW_KEY_NOINPUT;

		IP_Endpoint from;
		uint32 bytes_received;



		while (socket_receive(&sock, buffer, SOCKET_BUFFER_SIZE, &from, &bytes_received))
		{
			switch ((Server_Message)buffer[0])
			{
			case Server_Message::Join_Result:
			{
				if (buffer[1])
				{
					memcpy(&slot, &buffer[2], sizeof(slot));

					//Check validity of slot, move into input mode

					if (slot < MAX_CLIENTS) {
						msgType = Client_Message::Input;
					}

					

				}
				else
				{
					printf("server didn't let us in\n"); // Don't change msg configuration, keep asking to join
				}
			}
			break;
			case Server_Message::State:
			{
				num_objects = 0;
				uint32 bytes_read = 1;
				while (bytes_read < bytes_received)
				{
					uint16 id; // unused
					memcpy(&id, &buffer[bytes_read], sizeof(id));
					bytes_read += sizeof(id);

					memcpy(&objects[num_objects].x,
						&buffer[bytes_read],
						sizeof(objects[num_objects].x));
					bytes_read += sizeof(objects[num_objects].x);

					memcpy(&objects[num_objects].y,
						&buffer[bytes_read],
						sizeof(objects[num_objects].y));
					bytes_read += sizeof(objects[num_objects].y);

					++num_objects;
				}
			}
			break;

			case Server_Message::Collision_Result:
			{
				break;
			}
			case Server_Message::Populate_Result:
			{

				//parse the message into a vector of Populate_Msg

				// 0      1 - 4   5 - .... whenever
				// type   #msgs   type, owner, x, y

				uint32 readbytes = 1;
				int msgCountCheck;

				//check that our msgCounts are consistent with what's being sent, if not, we might have 2 sets of pop messages coming in
				if (popCount == 0) {
					memcpy(&popCount, &buffer[1], 4); //get the count if it's the first message
				}
				else {
					memcpy(&msgCountCheck, &buffer[1], 4); //get the count if it's the first message
					if (popCount == msgCountCheck) {
						//all is well
					}
					else {
						printf("Message Counts did not line up in Populate Result, we got %d expected %d", msgCountCheck, popCount);
						//probs handle differently if this comes up
					}

				}
				readbytes += 4;
				Populate_Msg pop;

				while (readbytes < bytes_received) {
					pop.type = buffer[readbytes];
					readbytes++;
					pop.x = buffer[readbytes];
					readbytes++;
					pop.y = buffer[readbytes];
					readbytes++;
					if (pop.type > UNIT_TYPE) {
						pop.owner = buffer[readbytes];
						readbytes++;
					}
					popCount--;
					pops.push_back(pop);
				}

				if (popCount == 0) { //we got em all!
					//Once we have the last message call the GmToClientServer function to populate the map

					GmToServerClient::populateMap(pops);

					//make sure to clear the vector so we reset it before next populate message burst
					pops.clear();

					//set the msgType back to input
					msgType = Client_Message::Input;

				}
				
				//else continue waiting for pop messages before we populate

				break;
			}

			default:
				printf("invalid Message format \n");
				break;
			}
		}



		//The received message should look like:

		/* Somethings that we will plan
		
		
		
		*/

		/*
		// grab data from packet
		uint32 read_index = 0;

		memcpy(&player_x, &buffer[read_index], sizeof(player_x));
		read_index += sizeof(player_x);

		memcpy(&player_y, &buffer[read_index], sizeof(player_y));
		read_index += sizeof(player_y);

		memcpy(&is_running, &buffer[read_index], sizeof(is_running));

		printf("x:%d, y:%d, is_running:%d\n", player_x, player_y, is_running);
		*/


		//tick now to tell the game to do its thing
		
		//increment a variable internally
		//Gamemode will decrement it and check at update?
		//=== trashy semaphore!!
		
		shouldTick++;

		wait_for_tick_end(tick_start_time, &timing_info);

	}



	printf("done");
	return 0;
	
	
}

void Client::stop()
{
	is_running = false;
}

int Client::tick()
{
	if (shouldTick) {
		shouldTick--;
		return 1;
	}
	return 0;
}

void Client::resetTick()
{
	shouldTick = 0;
}

void Client::addInput(int key)
{
	input = key;
}

void Client::addLoc(int x, int y)
{
	x0 = x;
	y0 = y;
}

void Client::sendPopMsg()
{
	msgType = Client_Message::Populate;
}

uint16 Client::getSlot()
{
	return slot;
}

void Client::setServerAddress(char* addi)
{
	server_address.sin_addr.S_un.S_addr = inet_addr(addi);
}

int Client::Startup(const char* Saddress)
{
	//Wait for the Server to be configured?
	//system("pause");

	//Give way to set
	//"71.142.244.46" will be Saddress for other computer

	//Don't init twice if we are both client and server?
	//if (!WSAIsInit) {
		if (StartWSA()) {
			printf("Startup Failed in Client Startup WSA \n");
			return 1;
		}
	//}
	//if (!socketIsInit) {
		if (CreateSocket(sock)) {
			printf("Startup Failed in Client Startup 3 Socket \n");
			return 1;
		}
	//}
	

	


	//Validate Saddress somehow: length? Regex match! // or make sure it's securely generated and passed


	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	//Assume that Saddress has already been set
	//server_address.sin_addr.S_un.S_addr = inet_addr(Saddress);
	//server_address.sin_addr.S_un.S_addr = inet_pton(Saddress);


	//Init the timing info currently breaks because this function can only be called once by a process, will work when everything is put together.
	timing_info = timing_info_create();


	//set recvfrom to not block
	u_long enabled = 1;
	ioctlsocket(sock, FIONBIO, &enabled);


	return 0;

}

void Client::Cleanup()
{
	WSACleanup();
}

