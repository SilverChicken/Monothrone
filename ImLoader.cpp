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
	int texCt = 32;
	const char* textLoc[32] = {  "../art/Player.jpg",  "../art/Char_P.png", "../art/Char_R.png",				// 0,1,2            Player
						    "../art/Manuf_B.png", "../art/Manuf_P.png", "../art/Manuf_R.png",					// 3,4,5            Manuf
						    "../art/Throne_B.png", "../art/Throne_P.png", "../art/Throne_R.png",				// 6,7,8            Throne
							"../art/Ref_B.png", "../art/Ref_P.png", "../art/Ref_R.png",							// 9,10,11          Refrac
							"../art/Mnt_1.png", "../art/Mnt_2.png", "../art/Mnt_3.png", "../art/Mnt_4.png",		// 12,13,14,15      Mountain
							"../art/nrg_1.png", "../art/nrg_2.png", "../art/nrg_3.png",  						// 16,17,18			Energy
							"../art/cryst_1.png", "../art/cryst_2.png", "../art/cryst_3.png",					// 19,20,21         Crystals
							"../art/Worm_B.png", "../art/Worm_P.png", "../art/Worm_R.png",						// 22,23,24         Worm1		
							"../art/Worm_B1.png", "../art/Worm_P1.png", "../art/Worm_R1.png",					// 25,26,27         Worm2
							"../art/Worm_B2.png", "../art/Worm_P2.png", "../art/Worm_R2.png",					// 28,29,30         Worm3
							"../art/Select.png"                                                                 // 31
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
