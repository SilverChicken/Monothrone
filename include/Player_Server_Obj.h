#pragma once
#include "PlayerType.h"
#include "Unit.h"

#include <map>


class Player_Server_Obj : public PlayerType
{
private:
	//std::map<Unit*, bool> selection; //selection to know which units carry out the action

public:
	//Direct command access (for server)
	//int perform_action(int);
};