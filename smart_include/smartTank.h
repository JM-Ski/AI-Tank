#pragma once

#include "aiTank.h"

class SmartTank : public AITank
{
private:
	//Movement states
	enum Direction;
	Direction e_currentDirection; // Current direction
};