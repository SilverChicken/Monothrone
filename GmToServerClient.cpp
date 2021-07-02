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
	
	int GLFW_key = 0;
	//here translate from cmd in server messages to cmds in gamemode i.e GLFW keys

	
	//could do translation in Server but that's harder to change later down the line if necessary. Keep here and maybe migrate to server in final version?
	//oh no we can't bc of player bindings changing around so we need to get the player bindings first




	switch (cmd) {
	case CMD_MOVE:
		GLFW_key = ;
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
		printf("unhandled input %c  from player %d \n", cmd, pl);
		break;
	}


	//WE NEED TO SELECT THE PLAYER SOMEHOW IN GAMEMODE

	game->key_action_remote(cmd, pl, x, y);

}
