#pragma once

#include<vector>
#include<string>

class FontLoader;


#define BUTTONCOUNT_0 4
#define BUTTONCOUNT_1 2
#define BUTTONCOUNT_2 5
#define IPBUFFERSIZE 16

struct MenuPlayerRep {
	char* strRep;
	MenuPlayerRep(int slot, char* IP) {
		if (slot >= 0 || slot <= 9) {
			char slotChar = slot + 48;
			std::string str = "Player number ";
			str.push_back(slotChar);
			str.append(" from address ");
			str.append(IP);

			strRep = (char*)malloc(str.size() + 1);
			memcpy(strRep, str.c_str(), str.size() + 1); //malloc probably won't fail
		}
		else {
			strRep = "Invalid slot";
		}
	}

	bool operator==(MenuPlayerRep cmp) {
		bool eq = true;
		char* i = strRep, * j = cmp.strRep;
		while (*i != '\0' && *j != '\0') { //should be enough
			if (*i != *j) {
				return false;
			}
			i++;
			j++;
		}
		return true;
	}
};


class Menu
{

	public:

		Menu(FontLoader* font);
		~Menu();

		void drawMenu();

		//Menu functionality
		//void setIPbuffer(char key, int place);
		int changeButton(int dir);

		void menu_key_call(int key);
		void selectMenuButton(int index); 

		void addPlayer(int slot, char* address);


	private:	
		FontLoader* Font;

		int current_button = 0;

		//Buffer handling vars
		int in;
		int charCount = 0;

		int menuMode = 0; //main menu: 0,  waitroom: 1,  options: 2

		std::vector<MenuPlayerRep> players;


};

