#include "Gui.h"
#include "include\Gui.h"

#include <string.h>

void Gui::init()
{

	setCrystal(0);  //needs to set once to initialize the render vars
	setEnergy(0);

	blank = new char(' ');
	//Init the binding strings so we have something to print, is already done with setting blank
	Keybind[0] = blank;
	Keybind[1] = blank;
	Keybind[2] = blank;
	Keybind[3] = blank;
	Keybind[4] = blank;
	Keybind[5] = blank;


	//Set up the Font
	if (!Font.Load("fonts/Small_Fonts.bff")) {
		std::cout << "Font failed to Load" << std::endl;
	}
	Font.SetColor(0.5f, 0.9f, 0.9f);

}

Gui::Gui()
{
	init();
}

Gui::~Gui()
{
	delete blank;
}

void Gui::setCrystal(int newCry)
{
	std::string c = std::to_string(newCry);
	if (crystal) {
		delete[] crystal;
	}
	crystal = new char[c.size() + 1];
	strcpy_s(crystal, c.size() + 1, c.c_str());
}

void Gui::setEnergy(int newEne)
{
	std::string e = std::to_string(newEne);
	if (energy) {
		delete[] energy;
	}
	energy = new char[e.size() + 1];
	strcpy_s(energy, e.size() + 1,e.c_str());
}

void Gui::setBinds(int * pBinds)
{
	//Check that pBinds has size of player bindings
	//if (sizeof(pBinds) / sizeof(int) != BINDINGCOUNT) {
	//	return;
	//}

	//So that check isn't great since it fails since we just pass one element


	//So pBinds is an array of size 6, and they are ordered QWERTY, with an in holding the index of Move, Count, etc
	for (int i=0; i < BINDINGCOUNT; i++) {
		if (pBinds[i] == -1) { //Then it's an empty value
			if (Keybind[i] != blank) {
				delete[] Keybind[i];
				Keybind[i] = blank;
			}
		}
		else {
			if (Keybind[i] != blank) {
				delete[] Keybind[i];
			}
			if (pBinds[i] >= 0 && pBinds[i] < BINDINGCOUNT) { //check it won't make us go out of bounds
				std::string str = binds[pBinds[i]]; //Gets the correct name from pBinds
				Keybind[i] = new char[str.size() + 1];
				strcpy_s(Keybind[i], str.size() + 1, str.c_str()); //Copy it into Keybind
			}
			

		}
	}


}

void Gui::setUnits()
{
}

void Gui::draw()
{
	//draw everything static first then add unit count,, type, bindings and all

	//Draw the text!
	
	Font.SetScreen(WINSIZE,WINSIZE);
	Font.Select();

	//Write in terms of WINSIZE? No, be globals that are set by a switch on RESOLUTION set in menus

	Font.Print((char*)"Units: ", 15, 85);   
	Font.Print((char*)"Q- ", 10, 55);
	Font.Print((char*)"W- ", 160, 55);
	Font.Print((char*)"E- ", 310, 55);
	Font.Print((char*)"R- ", 10, 30);
	Font.Print((char*)"T- ", 160, 30);
	Font.Print((char*)"Y- ", 310, 30);


	//draw the names of the buttons
	Font.Print(Keybind[0], 30, 55);
	Font.Print(Keybind[1], 185, 55);
	Font.Print(Keybind[2], 330, 55);
	Font.Print(Keybind[3], 30, 30);
	Font.Print(Keybind[4], 180, 30);
	Font.Print(Keybind[5], 330, 30);



	//draw the ressource counts

	
	Font.Print( crystal, 500, 70);
	Font.Print( energy, 500, 30);

}
