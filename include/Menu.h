#pragma once

class FontLoader;


#define BUTTONCOUNT 4
#define IPBUFFERSIZE 16


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


	private:	
		FontLoader* Font;

		int current_button = 0;

		//Buffer handling vars
		int in;
		int charCount = 0;

};

