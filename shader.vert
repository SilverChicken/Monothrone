#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.


//#define MAPSIZEHF 16.0   //map is from -16 to 16 on relative scale, we'll be using half of it
#define SCREENSIZE 20.0  //# of tiles on screen by default, modified by zoom


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTexCoord;

// Uniform variables can be updated by fetching their location and passing values to that location
//uniform mat4 projection;
//uniform mat4 modelview;


uniform vec2 location;
uniform vec2 BL;
uniform float zoom;
uniform float renderRout;


// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float alpha;
out vec2 texCoord;
out vec4 FragPos;

void main()
{

	float screenhf = SCREENSIZE / 2.0;

	if(renderRout == 1){   //Tile
		//we need to normalize the coordinates in terms of the size of the size of the map but also keep track of where the 'camera' is 
		//this moves the top left to TL and subtracts the screen half. But we need to account for the shape of the tile! use position.

		float xpos = position.x + (location.x - BL.x - screenhf) / screenhf;
		float ypos = position.y + (location.y - BL.y - screenhf) / screenhf;

		gl_Position = vec4( xpos, ypos, position.z , 1.0);
		FragPos = vec4(xpos, ypos, position.z , 1.0);
		texCoord = aTexCoord;

	} else {               //Map

		gl_Position = vec4(position.x, position.y, position.z, 1.0);
		FragPos = vec4(position.x, position.y, position.z, 1.0);      //position of a point same as position but accessible
		texCoord = aTexCoord;
	}	
    

	//normal = mat3(transpose(inverse(modelview))) * norms;
    alpha = 1.0f;
}
