#include "ImLoader.h"

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<iostream>


bool ImLoader::Loadtextures()  //Load all the textures we need to play
{
	const int texCt = 73;
	const char* textLoc[texCt] = {  "art/Char_B.png",  "art/Char_P.png", "art/Char_R.png",			// 0,1,2            Player
						    "art/Manuf_B.png", "art/Manuf_P.png", "./art/Manuf_R.png",				// 3,4,5            Manuf
						    "art/Throne_B.png", "art/Throne_P.png", "art/Throne_R.png",				// 6,7,8            Throne
							"art/Ref_B.png", "art/Ref_P.png", "art/Ref_R.png",						// 9,10,11          Refrac
							"art/Mnt_1.png", "art/Mnt_2.png", "art/Mnt_3.png", "art/Mnt_4.png",		// 12,13,14,15      Mountain
							"art/nrg_1.png", "art/nrg_2.png", "art/nrg_3.png",  					// 16,17,18			Energy
							"art/cryst_1.png", "art/cryst_2.png", "art/cryst_3.png",				// 19,20,21         Crystals
							"art/Worm_B.png", "art/Worm_P.png", "art/Worm_R.png",					// 22,23,24         Worm1		
							"art/Worm_B1.png", "art/Worm_P1.png", "art/Worm_R1.png",				// 25,26,27         Worm2
							"art/Worm_B2.png", "art/Worm_P2.png", "art/Worm_R2.png",				// 28,29,30         Worm3
							"art/Select.png",                                                       // 31               Selection
							"art/NRGpart_0.png", "art/NRGpart_1.png", "art/NRGpart_2.png",			// 32, 33, 34		Energy Particles
							"art/CRYpart_0.png", "art/CRYpart_1.png", "art/CRYpart_2.png",          // 35, 36, 37       Crystal Particles
							"art/Worker_B0.png", "art/Worker_P0.png", "art/Worker_R0.png",			// 38, 39, 40         Worker		
							"art/Worker_B1.png", "art/Worker_P1.png", "art/Worker_R1.png",			// 41, 42, 43         Worker1
							"art/Worker_B2.png", "art/Worker_P2.png", "art/Worker_R2.png",			// 44, 45, 46         Worker2
							"art/CRYSTIcon.png", "art/NRGIcon.png",									// 47, 48		    Icons for Gui
							"art/Wall_B.png", "art/Wall_P.png", "art/Wall_R.png",                   // 49, 50, 51       Wall textures
							"art/Spawn_B0.png", "art/Spawn_P0.png", "art/Spawn_R0.png",             // 52, 53, 54       Spawn 0
							"art/Spawn_B1.png", "art/Spawn_P1.png", "art/Spawn_R1.png",             // 55, 56, 57       Spawn 1
							"art/Spawn_B2.png", "art/Spawn_P2.png", "art/Spawn_R2.png",             // 58, 59, 60       Spawn 2
							"art/Death_B0.png", "art/Death_P0.png", "art/Death_R0.png",             // 61, 62, 63       Death 0
							"art/Death_B1.png", "art/Death_P1.png", "art/Death_R1.png",             // 64, 65, 66       Death 1
							"art/Death_B2.png", "art/Death_P2.png", "art/Death_R2.png",             // 67, 68, 69       Death 2
							"art/Death_B3.png", "art/Death_P3.png", "art/Death_R3.png",             // 70, 71, 72       Death 3
	};

	textures = new unsigned int[texCt];

	glGenTextures(texCt, &textures[0]);

	for(int i = 0; i < texCt; i++) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(textLoc[i], &width, &height, &nrChannels, 0);
		int colorMode = nrChannels == 3 ? GL_RGB : GL_RGBA;
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	return true;
}
