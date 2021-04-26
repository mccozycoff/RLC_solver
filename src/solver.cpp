#define OLC_PGE_APPLICATION
#include "solver.h"
#include "gui_handler.h"
#include "circuit.h"


bool Solver::OnUserCreate() 
{

	return true; 
}

bool Solver::OnUserUpdate(float fElapsedTime)
{
	olc::vi2d mousePos;
	static GUI_handler gui;
	static Circuit my_circuit;

	olc::HWButton buttonState;
	mousePos = GetMousePos();
	buttonState = GetMouse(0);

	gui.DrawMenu(this);
	gui.DrawScheme(my_circuit, this);
	gui.StateMachine(mousePos, buttonState, my_circuit,this);

	Sleep(5);
	return true;
}


int main()
{
	Solver demo;
	if (demo.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 2, 2))
		demo.Start();
	

	return 0;
}

