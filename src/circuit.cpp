#include "circuit.h"
#include "gui_handler.h"

void Circuit::AddElement(ElementModel_T& element, olc::vi2d mousePos)
{
	using std::pow;
	element.position = mousePos;

	ElementModel_T* AddedElementPtr = &element;

	if (element.type == DC_SOURCE)
	{
		if (!sources.empty())
			element.ID = (*sources.back()).ID + 1;
		sources.push_back(AddedElementPtr);
	}
	else
	{
		if (!elements.empty())
			element.ID = (*elements.back()).ID + 1;
		elements.push_back(AddedElementPtr);
	}

	//check placement of four limit points of element on the screen matrice
	if (AddedElementPtr->type != WIRE)
	{
		for (int n = 0; n < 4; n++)
		{
			int m = n;
			int x, y;
			if (n >= 2)
				m = n - 1;
			x = (AddedElementPtr->position.x - pow(-1, n) * AddedElementPtr->size.x / 2) * 8 / WINDOW_WIDTH;
			y = (AddedElementPtr->position.y + pow(-1, m) * AddedElementPtr->size.y / 2) * 4 / WINDOW_HEIGHT;
			if (nullptr == screenMap[x][y])
			{
				screenMap[x][y] = new std::vector<ElementModel_T*>;
				screenMap[x][y]->push_back(AddedElementPtr);
			}
			else if (screenMap[x][y]->back() != AddedElementPtr)
			{
				screenMap[x][y]->push_back(AddedElementPtr);
			}

		}
	}
	else
	{
		// TBD updated for WIRE element!
	}
}

void Circuit::DeleteElement(ElementModel_T& element)
{
	// TO BE UPDATED - screenMap should be aligned as elements disappear

	if (element.type == DC_SOURCE)
	{
		auto it = std::remove_if(sources.begin(), sources.end(), [element](ElementModel_T* src)
			{
				if (src->ID == element.ID)
					return true;
				return false;
			});

	}
	else
	{
		auto it = std::remove_if(elements.begin(), elements.end(), [element](ElementModel_T* src)
			{
				if (src->ID == element.ID)
					return true;
				return false;
			});
	}

}

void Circuit::MakeNode(ElementModel_T& first, ElementModel_T& second)
{

}

ElementModel_T* Circuit::CircuitTouched(olc::vi2d mousePos)
{

	if (nullptr != screenMap[mousePos.x * 8 / WINDOW_WIDTH][mousePos.y * 4 / WINDOW_HEIGHT])
	{
		for (auto& a : *screenMap[mousePos.x * 8 / WINDOW_WIDTH][mousePos.y * 4 / WINDOW_HEIGHT])
		{
			if (mousePos.x >= (a->position.x - a->size.x / 2) && mousePos.x <= (a->position.x + a->size.x / 2))
			{
				if (mousePos.y >= (a->position.y - a->size.y / 2) && mousePos.y <= (a->position.y + a->size.y / 2))
					return a;
			}
		}
	}

	return nullptr;
}