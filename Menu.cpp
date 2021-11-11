#include "Menu.h"
#include "Gamemode.h"
#include "Client.h"
#include "FontLoader.h"


char IPbuffer[IPBUFFERSIZE];


Menu::Menu(FontLoader* font):Font(font) {

}

Menu::~Menu()
{

}

void Menu::drawMenu()
{
	switch (menuMode) {
	case 0:
		Font->SetScreen(WINSIZE, WINSIZE);
		Font->Select();

		//Write in terms of WINSIZE? No, be globals that are set by a switch on RESOLUTION set in menus
		Font->SetColor(0.9f, 0.1f, 0.9f);
		Font->Print((char*)"MONOTHRONE", 300, 560);

		Font->SetColor(0.9f, 0.9f, 0.9f);

		//Draw each button

		if (current_button == 0) {
			Font->SetColor(0.1f, 0.99f, 0.2f);
		}
		Font->Print((char*)"HOST", 200, 500);

		Font->SetColor(0.9f, 0.9f, 0.9f);

		if (current_button == 1) {
			Font->SetColor(0.1f, 0.99f, 0.2f);
		}

		Font->Print((char*)"JOIN", 200, 400);

		Font->SetColor(0.9f, 0.9f, 0.9f);

		if (current_button == 2) {
			Font->SetColor(0.1f, 0.99f, 0.2f);
		}

		Font->Print((char*)"OPTIONS", 200, 300);

		Font->SetColor(0.9f, 0.9f, 0.9f);

		if (current_button == 3) {
			Font->SetColor(0.1f, 0.99f, 0.2f);
		}

		Font->Print((char*)"EXIT", 200, 200);


		Font->SetColor(0.1f, 0.9f, 0.9f);

		if (IPbuffer) {
			Font->Print(IPbuffer, 300, 400);
		}
		break;
	case 1:
	{
		Font->SetScreen(WINSIZE, WINSIZE);
		Font->Select();

		//Write in terms of WINSIZE? No, be globals that are set by a switch on RESOLUTION set in menus
		Font->SetColor(0.8f, 0.8f, 0.8f);
		Font->Print((char*)"Wait Room", 50, 570);
		Font->Print((char*)"Waiting for Players", 50, 540);

		int i = 0;
		for (MenuPlayerRep player : players) {
			Font->Print(player.strRep, 50, 450 - i * 15);
			i++;
		}


		if (current_button == 0) {
			Font->SetColor(0.1f, 0.99f, 0.2f);
		}

		Font->Print((char*)"Start Game", 400, 200);
		Font->SetColor(0.7f, 0.7f, 0.7f);

		if (current_button == 1) {
			Font->SetColor(0.1f, 0.99f, 0.2f);
		}

		Font->Print((char*)"Exit", 400, 170);
		Font->SetColor(0.7f, 0.7f, 0.7f);

		break;
	}
	case 2:
		break;
	}

	
}

/*
void Menu::setIPbuffer(char key, int place)
{
	if (place < IPBUFFERSIZE && place >= 0) {
		IPbuffer[place] = key;
	}
}*/

int Menu::changeButton(int dir)
{
	switch(menuMode) {
	case 0:
		current_button = (BUTTONCOUNT_0 + current_button + dir) % BUTTONCOUNT_0;
		break;
	case 1:
		current_button = (BUTTONCOUNT_1 + current_button + dir) % BUTTONCOUNT_1;
		break;
	case 2:
		current_button = (BUTTONCOUNT_2 + current_button + dir) % BUTTONCOUNT_2;
		break;
	}
	
	return current_button;
}

void Menu::menu_key_call(int key)
{
	if (charCount > 0 && menuMode == 0) {
		in = key;
		selectMenuButton(1); //select JOIN and add to the input
		return;
	}

	
	switch (key) {
	case GLFW_KEY_UP:
		//select button above
		changeButton(-1);
		break;

	case GLFW_KEY_DOWN:
		//select button below
		changeButton(1);
		break;
	case GLFW_KEY_RIGHT:
		selectMenuButton(current_button);
		break;
	case GLFW_KEY_ENTER:
		selectMenuButton(current_button);
		break;
	}
}

void Menu::selectMenuButton(int index)
{
	switch (menuMode) {
	case 0:
	{
		Client* client;
		Gamemode* gm;

		if (index < 2) {
			client = Client::getInstance();
			gm = &Gamemode::getInstance();
		}


		switch (index) {
		case 0: //HOST
			//Startup the server, set the server address to local address and init the game, set mode to 1;

			

			client->setServerAddress("127.0.0.1");

			//thread creation happens in gamemode, esp the server happens later so it doesn't fail


			gm->setIsHost(true);

			menuMode = 1;

			//gm->setMode(1);
			gm->init2();

			break;
		case 1: //JOIN
			//Have user input IP address showing characters each tick show the buffer, add dots automatically? at second press enter reset client serv addi & set mode 1

			if (in == GLFW_KEY_ENTER && charCount >= 7) { //idk about len
				//Idk how to validate an IP

				//validate IP 
				for (int i = 0; i < charCount; i++) {
					if ((IPbuffer[i] < '0' || IPbuffer[i] > '9') && IPbuffer[i] != '.') {
						//Not a valid IP for sure
						std::fill_n(IPbuffer, 16, ' '); //refill buffer

						charCount = 0; //reset the selection

						return;

					}
				}

				client->setServerAddress(IPbuffer);


				//test connection?
				//and send to wait room? -> wait room need to get some info from server and game mode

				menuMode = 1;
				
				



				//Setup map?
				gm->setIsHost(false);
				gm->setMode(1);


				//Somehow make sure that we are connected


				gm->init2();
				std::fill_n(IPbuffer, 16, ' '); //refill buffer
				charCount = 0;
				break;

			}

			//check numlock codes
			if (in > 255) {
				in = in - 16;
			}

			if (IPbuffer[0] <= 0) { //first input was the enter key or other trash
				IPbuffer[0] = in;
				charCount = 1;
				break;
			}

			//otherwise we've already got at least 1 character and key is already got
			IPbuffer[charCount] = in;
			charCount++;



			break;
		case 2: //OPTIONS
			//Open other menu do later

			//set the menuMode

			break;
		case 3: //EXIT
			//End the game in gamemode, ensure ressource cleanup
			gm = &Gamemode::getInstance();

			break;
		default:
			break;
		}
		break;
	}
	case 1: //We are in waitroom
	{
		switch (index) {
		case 0:
		{
			Gamemode* gm = &Gamemode::getInstance();
			gm->setMode(1);
			gm->init3();
			break;
		}
		case 1:
			break;
		default :
			break;
		}


		break;
	}
	case 2:
	{
		break;
	}
	}

	
}

void Menu::addPlayer(int slot, char* address)
{
	MenuPlayerRep pl = MenuPlayerRep(slot, address);

	//make sure we haven't already added it
	for (MenuPlayerRep p : players) {
		if (p == pl) {
			return;
		}
	}

	players.push_back(pl);
}


