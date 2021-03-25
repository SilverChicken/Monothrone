#pragma once
#include "Locateable.h"

class Map;

class Wall : public Locateable
{
private:
	int owner;
	static constexpr int textLocs[3] = {49, 50, 51 };

	int textLoc;

public:
	Wall(int own, Location * location, Map* map);
	~Wall();

	void destroy();

	void draw(GLuint);

	virtual int getTextLoc();

};

