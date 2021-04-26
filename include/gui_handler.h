#ifndef GUI_HANDLER_H
#define GUI_HANDLER_H

#include "solver.h"
#include "olcPixelGameEngine.h"
#include "circuit.h"

#define WINDOW_HEIGHT 240	
#define WINDOW_WIDTH (int)(1.78*WINDOW_HEIGHT)

struct DrawingMenu
{
	struct
	{
		ElementType type = RESISTOR;
		olc::vi2d position{ 10, 10 };
		olc::vi2d size{ 32, 20 };
		std::vector<olc::vi2d> points[2] = { { {-12,0}, {-9, 0}, {-7, 8}, {-2, -8}, {2, 8}, {7, -8}, {9, 0}, {12,0} },
											{ {0,-12}, {0, -9}, {8, -7}, {-8, -2}, {8, 2}, {-8, 7}, {0, 9}, {0,12}} };
	} resistor;
	struct
	{
		ElementType type = CAPACITOR;
		olc::vi2d position{ 47, 10 };
		olc::vi2d size{ 32, 20 };
		std::vector<olc::vi2d> points_l[2] = { { {-10, 0}, {-6,0},{-6, 8}, {-5, 8}, {-5, -8}, {-6, -8}, {-6,0} },
												{ {0, -10}, {0,-6},{8, -6}, {8, -5}, {-8, -5}, {-8, -6}, {0,-6} } };
		std::vector<olc::vi2d> points_r[2] = { { {10, 0}, {6, 0}, { 6, 8 }, {5, 8}, { 5, -8 }, { 6, -8 }, {6, 0} },
											{ {0, 10}, {0, 6}, { 8, 6 }, {8, 5}, { -8, 5 }, { -8, 6 }, {0, 6} } };
	} capacitor;
	struct
	{
		ElementType type = INDUCTOR;
		olc::vi2d position{ 84, 10 };
		olc::vi2d size{ 32, 20 };
		std::vector<olc::vi2d> points[2] = { { {-16, 0}, {-12, 0}, {-4, 0}, {4, 0}, {12, 0}, {16, 0} },
											{ {0, -16}, {0, -12}, {0, -4}, {0, 4}, {0, 12}, {0, 16} } };
	} inductor;
	struct
	{
		ElementType type = DC_SOURCE;
		olc::vi2d position{ 121, 10 };
		olc::vi2d size{ 32, 20 };
		std::vector<olc::vi2d> points_l[4] = { { {-10, 0}, {-6, 0 }, {-6, -8}, { -3, -8 }, { -3, 8 }, { -6, 8 }, {-6, 0} },
												{ {0, -10}, {0, -6 }, {-8, -6}, { -8, -3 }, { 8, -3 }, { 8, -6 }, {0, -6} },
												{ {10, 0}, {6, 0 }, {6, -8}, { 3, -8 }, { 3, 8 }, { 6, 8 }, {6, 0} },
												{ {0, 10}, {0, 6 }, {-8, 6}, { -8, 3 }, { 8, 3 }, { 8, 6 }, {0, 6} } };
		std::vector<olc::vi2d> points_r[4] = { { {10, 0}, {6,0}, {6, -5}, {3, -5}, {3, 5},{6, 5},{6,0} },
												{ {0, 10}, {0,6}, {-5, 6}, {-5, 3}, {5, 3},{5, 6},{0,6} },
												{ {-10, 0}, {-6,0}, {-6, -5}, {-3, -5}, {-3, 5},{-6, 5},{-6,0} },
												{ {0, -10}, {0,-6}, {-5, -6}, {-5, -3}, {5, -3},{5, -6},{0,-6} } };
	} dcSource;
	struct
	{
		ElementType type = WIRE;
		olc::vi2d position{ 158, 10 };
		olc::vi2d size{ 32, 20 };
		std::vector<olc::vi2d> points = { {-12, -5}, {-12, 5}, {-12, 0}, {12, 0}, {12, -5}, {12, 5} };
		olc::vi2d anchor;
		ElementModel_T* elementAnchorPtr;
		bool isPathLegit = true;
	} wire;
	struct
	{
		olc::vi2d position{ 2, 33 };
		olc::vi2d size{ WINDOW_WIDTH - 4 , WINDOW_HEIGHT - 2 };
	}drawingArea;

	ElementType drawThis = NONE;
};

class GUI_handler
{
private:
	DrawingMenu Menu;

public:
	enum GUI_State { IDLE, DRAWING_ELEMENT, DRAWING_NODE, SIMULATION_RUN };
	GUI_State GUI_state = IDLE;
	void DrawScheme(Circuit& circuit, Solver* app_ptr);
	void DrawMenu(Solver* app_ptr);
	bool IsAreaPermited(olc::vi2d& mousePos);
	void StateMachine(olc::vi2d& mousePos, olc::HWButton& buttonState, Circuit& myCircuit, Solver* appPtr);
	void switchState(GUI_State state, ElementModel_T** ElementPtr);
	ElementType CheckMenuClick(olc::vi2d& mousePos);
	bool isInRect(olc::vi2d& point, olc::vi2d& pos, olc::vi2d& size);
	void DrawOnline(olc::vi2d& mousePos, Solver* appPtr, ElementModel_T& elementToDraw, Circuit& myCircuit);
	void DrawElement(olc::vi2d offset, Solver* appPtr, ElementType element, int rotation = 0, bool contour = false, olc::Pixel color = olc::WHITE);
	void DrawWire(olc::vi2d& mousePos, Solver* appPtr, Circuit& myCircuit);
	void AnchorNode(ElementModel_T& element, olc::vi2d& mousePos);
	void AnchorNode(olc::vi2d& mousePos);
};

#endif