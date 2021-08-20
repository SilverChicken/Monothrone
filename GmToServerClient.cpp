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

void GmToServerClient::sendCommand(int cmd, int pl, int x, int y)
{
	Gamemode* game = &Gamemode::getInstance();
	game->key_action_remote(cmd, pl, x, y);
}
