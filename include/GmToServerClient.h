#pragma once

#include<vector>

#define UNIT_TYPE 4 //keep consistent with UNIT_CLASS_T

struct Populate_Msg;


//just serves as interface between two classes
class GmToServerClient {
public:

	//for populate message requests
	static void createPopInfo(std::vector<Populate_Msg>& out);
	static void populateMap(std::vector<Populate_Msg>& in);

	//For Menu updates
	static void addPlayerToMenu(int slot, int address);


	//Pass player command infos onto the Gamdemode
	static void sendCommand(int cmd, int pl, int x, int y);

};


