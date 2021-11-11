#include "GmToServerClient.h"
#include "Gamemode.h"
#include "Client.h"
#include "Server.h"




void GmToServerClient::createPopInfo(std::vector<Populate_Msg>& out)
{
	Gamemode* game = &Gamemode::getInstance();
	game->createPopulateInfo(out);
}


void GmToServerClient::populateMap(std::vector<Populate_Msg>& in) {
	Gamemode* game = &Gamemode::getInstance();
	game->populateMap(in);
}

void GmToServerClient::addPlayerToMenu(int slot, int address)
{
	int addiLen = 16;
	Gamemode* game = &Gamemode::getInstance();

	//Aesthetically convert the int to a char*
	char* addi = new char[addiLen + 1];
	memset(addi, ' ', addiLen);
	addi[addiLen] = '\0';
	int pos = addiLen-1;
	while (address > 0) {
		addi[pos] = '0' + address % 10;
		pos--;
		address = address / 10;
	}

	game->addMenuPlayer(slot, addi);

	delete[] addi;

}

void GmToServerClient::sendCommand(int cmd, int pl, int x, int y)
{
	Gamemode* game = &Gamemode::getInstance();
	game->key_action_remote(cmd, pl, x, y);
}
