#ifndef DUMB_TANK_H
#define DUMB_TANK_H

#include "aitank.h"
#include <list>
#include <algorithm>
#include "obstacle.h"
#include "playerTank.h"

class DumbTank : public AITank
{
private:
	int iGeneratedX; // Generated X position
	int iGeneratedY; // Generated Y position

	Position m_currentGenPos; //Randomly generated position
	Position m_currentTargetPos; //Current target position
	Position m_lastPosition; //Our last position
	Position m_movingTo; //Moving to specified destination
	std::list<Position> m_generatedPos; //List of randomly generated positions
	std::list<Position> m_buildingList; //List of enemy buidling positions
	std::list<Position> m_enemyList; //List of enemy positions
	std::list<Position> m_baseList; // List of our bases
	std::vector<float> m_baseAngle; // Angle from our turret to bases
	std::vector<float> m_baseAITankDist; // Distance to our bases

	bool bRiskInvolved; // Is our angle is risky?
	bool bCloseToUs; // Is enemy close to our tank?

	int iPrevScore; // Our previous score
	int iHitBase; // We hit the building
	int iBuildings; // Number of buildings
	
	//Tank events
	enum TankEvent
	{
		Stop, // Stop our tank
		Move, // Move our tank
		Shoot // Shoot at the target
	};

	TankEvent e_currentTankEvent; // Our current event

	bool bFire; // Fire the shell
public:
    DumbTank(); // Constructor
    ~DumbTank(); // Destructor

	void move(); // Move the tank
	void startMoving(); // Start the movement of our tank
	void startShooting(); // Start shooting at the target
	void canWeShoot(Position goal, double dAngle); // Check if we can fire the shell at the target
	void reset(); // Resest when you die
	void collided(); // Collision resolution
	void markTarget(Position p); // Mark the enemy buldings
	void markEnemy(Position p); // Mark the enemy
	void markBase(Position p); // Mark our bases
	void markShell(Position p); // Mark the shell
	bool isFiring(); // Are we firing?
	void score(int thisScore,int enemyScore); // Our score

	float crossProduct(Position first, Position second); // Cross product calculation
	float magnitude(Position somePos); // Magnitude of a vector
	float magnitude(float fX, float fY); // Magnitude of a vector
	Position normalize(Position somePos); // Normalization
};
#endif