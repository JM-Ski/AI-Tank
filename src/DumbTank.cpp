#include "DumbTank.h"
#include <cmath>

DumbTank::DumbTank() // Construtor
{
	//Initialize all variables
	e_currentTankEvent = TankEvent::Move;
	bFire = false;
	iPrevScore = 0;
	iHitBase = 0;
	iBuildings = 10;
	bRiskInvolved = false;
	bCloseToUs = false;
}

DumbTank::~DumbTank(){} // Destructor

void DumbTank::reset()
{
	bFire = false;
	bRiskInvolved = false;
	bCloseToUs = false;
	e_currentTankEvent = TankEvent::Move;
}

void DumbTank::move()
{	
	if (e_currentTankEvent == TankEvent::Move) { startMoving(); } // Move the tank
	if (e_currentTankEvent == TankEvent::Stop) { stop(); } // Stop the tank
	
	if (e_currentTankEvent == TankEvent::Shoot && !m_buildingList.empty() ||
		e_currentTankEvent == TankEvent::Shoot && !m_enemyList.empty()) { startShooting(); } // Start shooting if we see something
	else //Else don't fire and keep moving
	{
		bFire = false;
		e_currentTankEvent = TankEvent::Move;
	}
}

void DumbTank::collided()
{
	m_lastPosition = pos; // This resolves collision by generating new destination in our startMoving() function
}

void DumbTank::markTarget(Position p)
{
	e_currentTankEvent = TankEvent::Stop; // Make sure to stop again

	bool bMatchesBuilding = false; // Any matching buildings?

	for (std::list<Position>::iterator itBuilding = m_buildingList.begin(); itBuilding != m_buildingList.end(); ++itBuilding) // Check the list for repeated entries
	{
		if (p.getX() == itBuilding->getX() && p.getY() == itBuilding->getY()) //If u find one
		{ 
			bMatchesBuilding = true;
			break; // Mark it down
		} 
	}

	if (m_buildingList.empty() && !bMatchesBuilding) { m_buildingList.push_back(p); } // As long as list is empty and we don't pass copies to it
	else { e_currentTankEvent = TankEvent::Shoot; } // If our list isn't empty then we might as well shoot things
}

void DumbTank::markEnemy(Position p)
{
	if (numberOfShells > iBuildings - iHitBase) // Check if we have enough shells to fire at the player
	{	
		m_enemyList.push_back(p); // Add it to our list
		e_currentTankEvent = TankEvent::Shoot;
	}
}

void DumbTank::markBase(Position p)
{
	//e_currentTankEvent = TankEvent::Stop; // Stop the tank
	bool bMatchesBase = false; // Any matching buildings?

	for (std::list<Position>::iterator itBase = m_baseList.begin(); itBase != m_baseList.end(); ++itBase) // Check the list for repeated entries
	{
		if (p.getX() == itBase->getX() && p.getY() == itBase->getY()) //If u find one
		{
			bMatchesBase = true;
			break; // Mark it down
		}
	}

	if (m_buildingList.empty() && !bMatchesBase) { m_baseList.push_back(p); } // As long as list is empty and we don't pass copies to it
}

void DumbTank::markShell(Position p)
{
	//e_currentDirection = Direction::None;
	//std::cout << "Shell spotted at (" <<p.getX() << ", " << p.getY() << ")\n"; 
}

void DumbTank::startShooting()
{
	// If risk is involved, but we can't reach the enemy or target
	if (bRiskInvolved && !bCloseToUs) { startMoving(); } // Move the tank and don't waste the time
	else { stop(); } //Stop the tank
	
	// Setup some variables to record distance
	float fXpos; 
	float fYpos;

	if (!m_enemyList.empty()) // If we spot the enemy
	{
		m_currentTargetPos = m_enemyList.back(); // Our first target

		//Get distance
		fXpos = m_currentTargetPos.getX() - turret.getPosition().x;
		fYpos = m_currentTargetPos.getY() - turret.getPosition().y;
	}
	else
	{
		m_currentTargetPos = m_buildingList.front(); // Our first target
		m_buildingList.pop_front(); // Pop from the list

		//Get distance
		fXpos = m_currentTargetPos.getX() - turret.getPosition().x;
		fYpos = m_currentTargetPos.getY() - turret.getPosition().y;
	}

	//Save our target
	Position goal;
	goal.set(fXpos, fYpos, 0.f);

	//Calculate angle of the target
	double dAngleGoal = atan2(fYpos, fXpos);
	// limit to the range -pi ... pi
	if (dAngleGoal < -PI) { dAngleGoal += 2 * PI; }
	if (dAngleGoal > PI) { dAngleGoal -= 2 * PI; }
	//Convert to degrees
	dAngleGoal = RAD2DEG(dAngleGoal);

	//Iterate through the list of bases
	for (std::list<Position>::iterator it = m_baseList.begin(); it != m_baseList.end(); ++it)
	{
		//Calculate the distance
		float fAngX = it->getX() - turret.getPosition().x;
		float fAngY = it->getY() - turret.getPosition().y;

		//Get an angle
		double dBuildingAngle = atan2(fAngY, fAngX);
		// limit to the range -pi ... pi
		if (dBuildingAngle < -PI) { dBuildingAngle += 2 * PI; }
		if (dBuildingAngle > PI) { dBuildingAngle -= 2 * PI; }
		//Convert to degrees
		dBuildingAngle = RAD2DEG(dBuildingAngle);
		
		//Add it to our list
		m_baseAngle.push_back(dBuildingAngle);
	}

	//Get ritation of our turret
	float fRadTurretRotation = DEG2RAD(turretTh);

	//Start from 0 to 1
	float fAngleX = cos(fRadTurretRotation);
	float fAngleY = sin(fRadTurretRotation);

	//Conver angle to vector
	Position currentTurretPos; 
	currentTurretPos.set(fAngleX, fAngleY, 0.f);

	//Normalize all vectors
	Position normTurret = normalize(currentTurretPos);
	Position normGoal = normalize(goal);

	//Calculate angles
	double dAngleOfTurret = atan2(normTurret.getY(), normTurret.getX());
	double dAngleOfTarget = atan2(normGoal.getY(), normGoal.getX());
	
	//Calculate the difference
	double dAngleToRotate = dAngleOfTarget - dAngleOfTurret;

	// limit to the range -pi ... pi
	if (dAngleToRotate < -PI) { dAngleToRotate += 2 * PI; }
	if (dAngleToRotate > PI) { dAngleToRotate -= 2 * PI; }

	//The tolerance to 0
	const double dAngleTolerance = 0.015f;

	//If we can tolerate it
	if (abs(dAngleToRotate) < dAngleTolerance) { canWeShoot(goal, dAngleGoal); } // Check if we can shoot it, if yes then fire else don't!
	else  if (dAngleToRotate < 0.f) // If angle is negative
	{
		turretGoLeft(); //  Turn left
		bFire = false; // And don't shoot
	}
	else //If positive
	{
		turretGoRight(); //Turn right
		bFire = false; // and don't shoot
	}
	
	//Make sure to clear our lists to get up-to-date searches
	m_enemyList.clear();
	m_baseAngle.clear();
	m_baseAITankDist.clear();
}

void DumbTank::canWeShoot(Position goal, double dAngle)
{
	//stop the turret
	stopTurret();
	//No risk at the moment
	bRiskInvolved = false;
	//We assume our tagret is close
	bCloseToUs = true; 
	float fDistance = magnitude(goal); // Calculate distance to our target

	 // Calculate distances from our buildings
	for (std::list<Position>::iterator it = m_baseList.begin(); it != m_baseList.end(); ++it)
	{
		//Get distance
		float fX = it->getX() - turret.getPosition().x;
		float fY = it->getY() - turret.getPosition().y;

		//Set this variable
		Position dist;
		dist.set(fX, fY, 0.f);

		//Calculate the length and add it to our list
		float fMag = magnitude(dist);
		m_baseAITankDist.push_back(fMag);
	}

	//Check how close the enemy is to our buildings
	for (int i = 0; i < m_baseAITankDist.size(); i++) 
	{
		//If enemy's distance is greater than ours
		if (fDistance > m_baseAITankDist.at(i))
		{
			bCloseToUs = false;
			break;
		}
	}

	if (bCloseToUs) { bFire = true; } // Fire if he is
	else 
	{ 
		// Take angle from the list
		for (int i = 0; i < m_baseAngle.size(); i++) 
		{
			float fCompareAngles = abs(dAngle) - abs(m_baseAngle.at(i)); // Compare them
			fCompareAngles = abs(fCompareAngles); // Get rid of negative numbers
			// How close our turret is to our buildings? Setup some threshold...
			float fTooRisky = 35.f; 

			//If lower then it means it's too risky to shoot
			if (fCompareAngles < fTooRisky)
			{
				bRiskInvolved = true; // Risk is involved
				break;
			}
		}

		if (bRiskInvolved) { bFire = false; } // Do not shoot if too risky
		else { bFire = true; } // Shoot the target
	} 
}

void DumbTank::startMoving()
{
	//Stop the turret
	stopTurret();

	//Generate random numbers
	iGeneratedX = rand() % 781;
	iGeneratedY = rand() & 571;
	m_currentGenPos.set(iGeneratedX, iGeneratedY, 0.f); // Set this random destination

	// If list is empty
	if (m_generatedPos.empty())
	{
		m_lastPosition = pos; // Set last position to our current position
		m_generatedPos.push_back(pos); // Push it back on the stack
		m_generatedPos.push_back(m_currentGenPos);  //Push newly generated pos to the stack
	}
	else
	{
		bool bMatches = false;

		//Iterate through the list
		for (std::list<Position>::iterator it = m_generatedPos.begin(); it != m_generatedPos.end(); ++it)
		{
			if (m_currentGenPos.getX() == it->getX() &&
				m_currentGenPos.getY() == it->getY())
			{
				bMatches = true; // If it matches then set boolean to true
				break; //And stop looping
			}
		}

		//If it doesn't match anything in the list add it
		if (!bMatches) { m_generatedPos.push_back(m_currentGenPos); }
	}

	float fThresholdPos = 50.f; // setup some threshold

	if (pos.getX() >= m_lastPosition.getX() - fThresholdPos && pos.getX() <= m_lastPosition.getX() + fThresholdPos &&
		pos.getY() >= m_lastPosition.getY() - fThresholdPos && pos.getY() <= m_lastPosition.getY() + fThresholdPos)
	{
		stop(); // Stop the tank
		m_generatedPos.pop_front(); // Get rid of the previous destination
		m_movingTo = m_generatedPos.front(); //Get the first destination we are moving towards to from the list
		m_lastPosition = m_movingTo; // Set our last position to our current destination

	} //Change destination
	else
	{
		//Calculate distance
		float fXpos = m_movingTo.getX() - pos.getX();
		float fYpos = m_movingTo.getY() - pos.getY();

		Position goal;
		goal.set(fXpos, fYpos, 0.f);

		float fRadTankRotation = DEG2RAD(pos.getTh()); // Our tank's rotation

		// Convert our angle to vector
		//Start from 1 to 0
		float fAngleX = cos(fRadTankRotation);
		float fAngleY = sin(fRadTankRotation);
		Position fAngleVector; 
		fAngleVector.set(fAngleX, fAngleY, 0.f);

		//Normalize all vectors
		Position normTank = normalize(fAngleVector);
		Position normDest = normalize(goal);

		//Cross product calculations
		float fCrossProduct = crossProduct(normDest, normTank);
		float fThreshold = 0.05f; // Threshold
		if (fCrossProduct < -fThreshold) { goRight(); } // If negative go right
		else if (fCrossProduct > fThreshold) { goLeft(); } // if position go left
		else { goForward(); } // Else we will go forward
	}
}

bool DumbTank::isFiring()
{
	return bFire; // Fire condition
} 

void DumbTank::score(int thisScore,int enemyScore)
{
	//Calculate if he have hit the base
	int iBaseHit = iPrevScore + 10;
	if (iBaseHit == thisScore) { iHitBase++; } // If yes then tell us
	iPrevScore = thisScore; // Set previous score to this score
}

float DumbTank::crossProduct(Position first, Position second)
{
	float Z = first.getX() * second.getY() - first.getY() * second.getX(); // We are interested in Z axis <-------
	return Z;
}

Position DumbTank::normalize(Position somePos)
{
	float fX = somePos.getX() / (magnitude(somePos));
	float fY = somePos.getY() / (magnitude(somePos));
	Position normPos;
	normPos.set(fX, fY, 0.f);
	return normPos;
}

float DumbTank::magnitude(Position somePos)
{
	float mag = sqrt(somePos.getX() * somePos.getX() + somePos.getY() * somePos.getY());
	return mag;
}

float DumbTank::magnitude(float fX, float fY)
{
	float mag = sqrt(fX * fX + fY * fY);
	return mag;
}