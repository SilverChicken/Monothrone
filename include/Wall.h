#pragma once
#include "Locateable.h"

class Map;


//May become template for all non-unit player belongings. Shouldn't be many
class Wall : public Locateable
{
private:
	int owner;
	static constexpr int textLocs[3] = {49, 50, 51 };

	//int textLoc; same as owner!

public:
	Wall(int own, Location * location, Map* map);
	~Wall();

	int getOwner();

	void destroy();

	void draw(GLuint);


};

