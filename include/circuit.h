#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "solver.h"

enum ElementType { NONE, RESISTOR = 1, INDUCTOR, DC_SOURCE, CAPACITOR, WIRE };

struct ElementModel_T
{
	ElementType type;
	olc::vi2d position;
	olc::vi2d size{ 32, 20 };
	std::array<int, 2> node = { 0, 0 };
	int ID = 0;
	int rotation;
	bool isTouched;

	ElementModel_T(ElementType ltype = NONE, olc::vi2d pos = { 0,0 }, bool Rotation = false)
	{
		type = ltype;
		position = pos;
		rotation = Rotation;
	}
};

struct WireModel_T : public ElementModel_T
{

	WireModel_T() : ElementModel_T() {};
};

class Circuit
{
private:
	std::vector<ElementModel_T*> elements;
	std::vector<ElementModel_T*> sources;
	std::vector<ElementModel_T*> wires;
	std::vector<ElementModel_T*>* screenMap[8][4] = { nullptr };
public:
	//getters
	std::vector<ElementModel_T* >* GetSources() { return &sources; };
	std::vector<ElementModel_T* >* GetElements() { return &elements; };
	std::vector<ElementModel_T* >* GetNodes() { return &wires; };


	void AddElement(ElementModel_T& element, olc::vi2d mousePos);
	void DeleteElement(ElementModel_T& element);
	void MakeNode(ElementModel_T& first, ElementModel_T& second);
	ElementModel_T* CircuitTouched(olc::vi2d mousePos);
};

#endif