#include "circuit.h"
#include "gui_handler.h"

void Circuit::AddElement(ElementModel_T& element, olc::vi2d mousePos)
{
	using std::pow;
	//element.position = mousePos;

	ElementModel_T* AddedElementPtr = &element;

	if (element.type == DC_SOURCE)
	{
		if (!sources.empty())
			element.ID = (*sources.back()).ID + 1;
		sources.push_back(AddedElementPtr);
	}
	else if (element.type == WIRE)
	{
		if (!wires.empty())
			element.ID = (*wires.back()).ID + 1;
		wires.push_back(static_cast<WireModel_T*>(AddedElementPtr));
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
			x = (AddedElementPtr->position.x - pow(-1, n) * AddedElementPtr->size.x / 2) / (WINDOW_WIDTH/ 8);
			y = (AddedElementPtr->position.y + pow(-1, m) * AddedElementPtr->size.y / 2) / (WINDOW_HEIGHT/ 4);
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
		int x, y;
		WireModel_T* wirePtr = static_cast<WireModel_T*>(AddedElementPtr);

		for (int n = 0; n < 6; n++)		// for every drawn wire there is 6 'square' points surrouding area
		{
			int xypos;
			if (element.rotation % 2 == 1)
			{
				if (n < 2)
				{
					xypos = wirePtr->anchor.y;

					x = (wirePtr->anchor.x - pow(-1, n) * wirePtr->size.x / 2) / (WINDOW_WIDTH/8);
					y = (xypos) * 4 / WINDOW_HEIGHT;
				}
				else if (n < 4) {
					xypos = wirePtr->position.x;

					x = (xypos) * 8 / WINDOW_WIDTH;
					y = (wirePtr->position.y - pow(-1, n) * wirePtr->size.y / 2) / (WINDOW_HEIGHT/4);
				}
				else
				{
					x = (wirePtr->anchor.x + pow(-1, n) * wirePtr->size.x / 2) / (WINDOW_WIDTH/8);
					y = (wirePtr->position.y + pow(-1, n) * wirePtr->size.y / 2) / (WINDOW_HEIGHT/4);

				}
			}
			else
			{
				if (n < 2)
				{
					xypos = wirePtr->anchor.x;

					x = xypos * 8 / WINDOW_WIDTH;
					y = (wirePtr->anchor.y - pow(-1, n) * wirePtr->size.y / 2)  / (WINDOW_HEIGHT/4);
				}
				else if (n < 4) {
					xypos = wirePtr->position.y;

					x = (wirePtr->position.x - pow(-1, n) * wirePtr->size.x / 2)  / (WINDOW_WIDTH/8);
					y = (xypos) * 4 / WINDOW_HEIGHT;
				}
				else
				{
					x = (wirePtr->position.x + pow(-1, n) * wirePtr->size.x / 2) / (WINDOW_WIDTH/8);
					y = (wirePtr->anchor.y + pow(-1, n) * wirePtr->size.y / 2)  / (WINDOW_HEIGHT/4);

				}
			}
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

		if (abs(wirePtr->position.x - wirePtr->anchor.x) > 2 * (WINDOW_WIDTH / 8))
		{
			x = std::min<int>(wirePtr->position.x, wirePtr->anchor.x) / (WINDOW_WIDTH/8);
			if (element.rotation % 2 == 0)
				y = wirePtr->anchor.y / (WINDOW_HEIGHT/4);
			else
				y = wirePtr->position.y / (WINDOW_HEIGHT/4);

			for (int n = 0; n < (abs(wirePtr->position.x - wirePtr->anchor.x) / (WINDOW_WIDTH/8)); n++)
			{
				if (nullptr == screenMap[x + n][y])
				{
					screenMap[x + n][y] = new std::vector<ElementModel_T*>;
					screenMap[x+ n][y]->push_back(AddedElementPtr);
				}
				else if (screenMap[x+n][y]->back() != AddedElementPtr)
				{
					screenMap[x+n][y]->push_back(AddedElementPtr);
				}
			}
		}
		if (abs(wirePtr->position.y - wirePtr->anchor.y) > 2 * (WINDOW_HEIGHT / 8))
		{
			y = std::min<int>(wirePtr->position.y, wirePtr->anchor.y) / (WINDOW_HEIGHT/4);

			if (element.rotation % 2 == 0)
				x = wirePtr->position.x  / (WINDOW_WIDTH/8);
			else
				x = wirePtr->anchor.x  / (WINDOW_WIDTH/8);

			for (int n = 0; n < abs(wirePtr->position.y - wirePtr->anchor.y) / (WINDOW_HEIGHT/4); n++)
			{
				if (nullptr == screenMap[x][y+n])
				{
					screenMap[x][y+n] = new std::vector<ElementModel_T*>;
					screenMap[x][y+n]->push_back(AddedElementPtr);
				}
				else if (screenMap[x][y+n]->back() != AddedElementPtr)
				{
					screenMap[x][y+n]->push_back(AddedElementPtr);
				}
			}
		}

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
				{
					delete src;
					return true;
				}
				return false;
			});

	}
	else if (element.type == WIRE)
	{
		auto it = std::remove_if(wires.begin(), wires.end(), [element](WireModel_T* src)
			{
				if (src->ID == element.ID)
				{
					delete src;
					return true;
				}
				return false;
			});
	}
	else
	{
		auto it = std::remove_if(elements.begin(), elements.end(), [element](ElementModel_T* src)
			{
				if (src->ID == element.ID)
				{
					delete src;
					return true;
				}
				return false;
			});
	}

}

void Circuit::MakeNode(ElementModel_T* first, ElementModel_T& second)
{

}

ElementModel_T* Circuit::CircuitTouched(olc::vi2d mousePos)
{

	if (nullptr != screenMap[mousePos.x * 8 / WINDOW_WIDTH][mousePos.y * 4 / WINDOW_HEIGHT])
	{
		for (auto& a : *screenMap[mousePos.x * 8 / WINDOW_WIDTH][mousePos.y * 4 / WINDOW_HEIGHT])
		{
			if (a->type != WIRE)
			{
				if (mousePos.x >= (a->position.x - a->size.x / 2) && mousePos.x <= (a->position.x + a->size.x / 2))
				{
					if (mousePos.y >= (a->position.y - a->size.y / 2) && mousePos.y <= (a->position.y + a->size.y / 2))
						return a;
				}
			}
			else
			{
				WireModel_T* wirePtr = static_cast<WireModel_T*>(a);
				if (wirePtr->rotation % 2 == 0)
				{
					if (mousePos.x >= std::min<int>(wirePtr->anchor.x, wirePtr->position.x) && mousePos.x <= std::max<int>(wirePtr->anchor.x, wirePtr->position.x))
					{
						if (mousePos.y >= (wirePtr->anchor.y - a->size.y / 2) && mousePos.y <= (wirePtr->anchor.y + a->size.y / 2))
							return a;
					}
					if (mousePos.y >= std::min<int>(wirePtr->anchor.y, wirePtr->position.y) && mousePos.y <= std::max<int>(wirePtr->anchor.y, wirePtr->position.y))
					{
						if(mousePos.x >= (wirePtr->position.x - a->size.x / 2) && mousePos.y <= (wirePtr->position.x + a->size.x / 2))
							return a;
					}
				}
				else
				{
					if (mousePos.x >= std::min<int>(wirePtr->anchor.x, wirePtr->position.x) && mousePos.x <= std::max<int>(wirePtr->anchor.x, wirePtr->position.x))
					{
						if (mousePos.y >= (wirePtr->position.y - a->size.y / 2) && mousePos.y <= (wirePtr->position.y + a->size.y / 2))
							return a;
					}
					if (mousePos.y >= std::min<int>(wirePtr->anchor.y, wirePtr->position.y) && mousePos.y <= std::max<int>(wirePtr->anchor.y, wirePtr->position.y))
					{
						if (mousePos.x >= (wirePtr->anchor.x - a->size.x / 2) && mousePos.y <= (wirePtr->anchor.x + a->size.x / 2))
							return a;
					}
						
				}
				
			}
		}
	}

	return nullptr;
}