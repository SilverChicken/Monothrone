#include "Gui.h"
#include "include\Gui.h"

#include <string.h>

void Gui::init()
{

	//Set up the Font

	if (!Font.Load("fonts/Small_Fonts.bff")) {
		std::cout << "Font failed to Load" << std::endl;
	}
	Font.SetColor(0.8f, 0.9f, 0.9f);

}

Gui::Gui()
{
	init();
}

Gui::~Gui()
{
}

void Gui::draw()
{
	//draw everything static first then add unit count,, type, bindings and all

	//Draw the text!
	
	Font.SetScreen(WINSIZE,WINSIZE);
	Font.Select();
	Font.Print((char*)"Units: ", 15, 85);   
	//Write in terms of WINSIZE? No, be globals that are set by a switch on RESOLUTION set in menus
	Font.Print((char*)"Q- ", 10, 60);
	Font.Print((char*)"W- ", 160, 60);
	Font.Print((char*)"E- ", 310, 60);
	Font.Print((char*)"R- ", 10, 40);
	Font.Print((char*)"T- ", 160, 40);
	Font.Print((char*)"Y- ", 310, 40);

}
