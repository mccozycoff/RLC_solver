#ifndef SOLVER_H
#define SOLVER_H

#include "olcPixelGameEngine.h"


class Solver : public olc::PixelGameEngine
{

public:
	Solver()
	{
		sAppName = "RLCsolver v1.0";
	}
public:
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

};



#endif SOLVER_H
