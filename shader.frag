#version 330 core
// This is a sample fragment shader.

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

#define SCREENSIZE 20.0  //# of tiles on screen by default, modified by zoom

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float alpha;
in vec2 texCoord;
in vec4 FragPos;


uniform Material material;


uniform sampler2D myTexture;
uniform sampler2D overTex;


uniform vec2 BL;
uniform float renderRout;
uniform float zoom;
uniform float overlay;  //if 0 no overlay, else overlay 2 textures

//0 is map
//1 is character/any 1 tile thing that is rendered from a texture that its draw uploads








// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color; 


// Function prototypes
float mod(float a, float b);
vec4 renderMap();
vec4 renderTile(sampler2D tex, vec2 coord);


void main()
{    
	if(renderRout == 1){
		color = renderTile(myTexture, texCoord);
	} else {
		color = renderMap();
	}


	//color = FragPos;
    
}



float mod(float a, float b){  //takes positive vals by adding 1
	a = a + 1.0;
	while(a > b){
	a = a - b;
	}
	return a;

}



vec4 renderMap(){
	//constants
	vec4 linecolor = vec4(0.01f, 0.01f, 0.01f, 1.0);
	vec4 backcolor = vec4(0.95f, 0.95f, 0.95f, 1.0);

	//Has constants set for zoom = 0, then modded if zoom non 0
	float gap = 0.1; // - zoom / SCREENSIZE;
	float posSide = 0.0975f; // - zoom / SCREENSIZE;
	float negSide = gap - posSide; // - zoom / SCREENSIZE;

	if( (mod(FragPos.x , gap) > posSide) || (mod(FragPos.x, 0.1) < negSide) ){
		color = linecolor;
	} else if( (mod(FragPos.y, gap) > posSide) || (mod(FragPos.y, gap) < negSide) ){
		return linecolor;
	}
	else{
		return backcolor;
	}

}



vec4 renderTile(sampler2D tex, vec2 coord){
	vec4 color = texture(tex, coord);
	if(overlay == 0.0){                 //No overlay -> just texture
		return color;
	} else {
		if(color.a == 0.0f){                  //If there's an overlay we just replace the parts where the texture is clear
			return texture(overTex, coord);
		} else {
			return color;
		}
	}
	
}