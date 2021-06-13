#pragma 


#include "Locateable.h"

#include <map>

class Unit;
class Map;
class Player;

//fully   class, unites Opponent and Player under 1 interface
//Later we will refactor this general functionality into this class so Player is less busy



class PlayerType : public Locateable
{
protected:

	int PID;
	Map* map = nullptr;

	//Gameplay Vars
	int energy;
	int crystal;
	bool hasThrone;

	//Vector of all units you own? Or set Or Hashmap?
	std::map<Unit*, bool> units;

	//rendering Vars
	//We don't render this object!

public:

	PlayerType();
	PlayerType(int, Map*);
	virtual ~PlayerType();

	//gets & sets
	int getPID();


	int getCrystal();
	int getEnergy();

	//Unit management
	virtual void update();
	virtual void update2();
	
	//Gameplay fcts
	bool checkHasThrone();
	bool incEnergy(int);
	bool incCrystal(int);
	bool decEnergy(int);
	bool decCrystal(int);
 

	//template can't be  
    template <class T>
	Unit* spawnUnit(Location* location);
	
	
	void destroyUnit(Unit* unit);


	//render fcts
	virtual void draw(unsigned int*, GLuint);  //this does nothing, only to be overriden by Player
	void draw(unsigned int*, PlayerType*, GLuint); //This one draws using the Player render settings

private:

	//unit management
	void addUnit(Unit*);
	void removeUnit(Unit*);

};



//Needs to be defined here because it's a template function
template<class T>
inline Unit* PlayerType::spawnUnit(Location* location)
{
	Unit* unit = new T(PID, location, map);
	units[unit] = false;                       //Adds the unit to the list of current units
	return unit;

}