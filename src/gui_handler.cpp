#include "gui_handler.h"
#include "solver.h"
#include "circuit.h"

void GUI_handler::DrawScheme(Circuit& circuit, Solver* app_ptr)
{

	for (int n = 0; n < (*circuit.GetSources()).size(); n++)
	{
		DrawElement((*circuit.GetSources())[n]->position, app_ptr, (*circuit.GetSources())[n]->type, (*circuit.GetSources())[n]->rotation);
	}

	for (int n = 0; n < (*circuit.GetElements()).size(); n++)
	{
		DrawElement((*circuit.GetElements())[n]->position, app_ptr, (*circuit.GetElements())[n]->type, (*circuit.GetElements())[n]->rotation);
	}

	for (int n = 0; n < (*circuit.GetWires()).size(); n++)
	{
		DrawWire(*(*circuit.GetWires())[n] , app_ptr, circuit);
		// TBD
	}
}

void GUI_handler::DrawMenu(Solver* app_ptr)
{
	app_ptr->Clear(olc::VERY_DARK_GREY);
	//app_ptr->FillRect(Menu.drawingArea.position, Menu.drawingArea.size, olc::DARK_BLUE);

	olc::vi2d offset;
	//resistor
	app_ptr->DrawRect(Menu.resistor.position, Menu.resistor.size, olc::WHITE);
	offset = Menu.resistor.position + Menu.resistor.size / 2;
	DrawElement(offset, app_ptr, RESISTOR);
	//capacitor
	app_ptr->DrawRect(Menu.capacitor.position, Menu.capacitor.size, olc::WHITE);
	offset = Menu.capacitor.position + Menu.capacitor.size / 2;
	DrawElement(offset, app_ptr, CAPACITOR);

	//inductor
	app_ptr->DrawRect(Menu.inductor.position, Menu.inductor.size, olc::WHITE);
	offset = Menu.inductor.position + Menu.inductor.size / 2;
	DrawElement(offset, app_ptr, INDUCTOR);

	// dc source

	app_ptr->DrawRect(Menu.dcSource.position, Menu.dcSource.size, olc::WHITE);
	offset = Menu.dcSource.position + Menu.dcSource.size / 2;
	DrawElement(offset, app_ptr, DC_SOURCE);

	// wire 
	app_ptr->DrawRect(Menu.wire.position, Menu.wire.size, olc::WHITE);
	offset = Menu.wire.position + Menu.wire.size / 2;
	for (int i = 0; i < Menu.wire.points.size() - 1; i++)
	{
		int j = i + 1;
		app_ptr->DrawLine(Menu.wire.points[i] + offset, Menu.wire.points[j] + offset);
	}
}

bool GUI_handler::IsAreaPermited(olc::vi2d& mousePos)
{
	return isInRect(mousePos, Menu.drawingArea.position, Menu.drawingArea.size);
}

void GUI_handler::StateMachine(olc::vi2d& mousePos, olc::HWButton& buttonState, Circuit& myCircuit, Solver* appPtr)
{
	//ptr to currently element object being drawn
	static ElementModel_T* modelPtr;

	switch (GUI_state)
	{

	case IDLE:
		//check for click on menu or circuit or other user action
	{ static ElementModel_T* touch_ptr = nullptr;
	if (buttonState.bPressed)
	{
		if ((Menu.drawThis = CheckMenuClick(mousePos)) != NONE)
		{
			if (WIRE != Menu.drawThis)
				switchState(DRAWING_ELEMENT, &modelPtr);
			else
				switchState(DRAWING_NODE, &modelPtr);
		}
		touch_ptr = myCircuit.CircuitTouched(mousePos);
		if (touch_ptr)
		{
			touch_ptr->isTouched = true;
		}
	}
	if (touch_ptr != nullptr && touch_ptr->isTouched)
		GlowUpElement(touch_ptr,appPtr,myCircuit);

	//if (appPtr->GetKey(olc::DEL).bPressed && touch_ptr != nullptr)
		//myCircuit.DeleteElement(*touch_ptr);

	break;
	}
	case DRAWING_ELEMENT:
		DrawOnline(mousePos, appPtr, *modelPtr, myCircuit);
		if (true == appPtr->GetMouse(1).bPressed)
		{
			delete modelPtr;
			switchState(IDLE, &modelPtr);
		}
		else if (buttonState.bPressed && IsAreaPermited(mousePos))
		{
			ElementModel_T* element_touched_ptr = myCircuit.CircuitTouched(mousePos);
			if (nullptr == element_touched_ptr)
			{
				myCircuit.AddElement(*modelPtr, mousePos);
				switchState(IDLE, &modelPtr);
			}
			else if (Menu.drawThis == WIRE)
			{
				PortNode(*element_touched_ptr, *modelPtr, mousePos);
				//Makes node if Menu.wire.elementAnchorPtr != nullptr and mousePos anchores another (permited) element
				 if(Menu.wire.elementAnchorPtr != nullptr)
					myCircuit.MakeNode( modelPtr, *element_touched_ptr);

				myCircuit.AddElement(*modelPtr, mousePos);

				switchState(IDLE, &modelPtr);
			}
		}
		if(modelPtr->type == WIRE)	
			static_cast<WireModel_T*>(modelPtr)->position_prev = modelPtr->position;
		break;

	case DRAWING_NODE:
		if (buttonState.bPressed)
		{
			if (IsAreaPermited(mousePos))
			{
				ElementModel_T* element_touched_ptr;
				if (element_touched_ptr = myCircuit.CircuitTouched(mousePos))
					AnchorNode(*element_touched_ptr, mousePos);
				else
					AnchorNode(mousePos);

				switchState(DRAWING_ELEMENT, &modelPtr);
			}
		}


		break;
	default:
		break;
	}
}

void GUI_handler::switchState(GUI_State state, ElementModel_T** ElementPtr)
{
	//on exit actions, tb updated if needed
	switch (GUI_state)
	{
	case IDLE:
		break;
	case DRAWING_ELEMENT:
		break;
	case  DRAWING_NODE:
		break;
	case SIMULATION_RUN:
		break;
	}

	GUI_state = state;
	switch (GUI_state)
	{
	case IDLE:
		break;
	case DRAWING_ELEMENT:
		if (Menu.drawThis == WIRE)
			*ElementPtr = new WireModel_T();
		else
			*ElementPtr = new ElementModel_T(Menu.drawThis);
		break;
	case  DRAWING_NODE:
		break;
	case SIMULATION_RUN:
		break;
	default:
		break;
	}
}

ElementType GUI_handler::CheckMenuClick(olc::vi2d& mousePos)
{
	if (isInRect(mousePos, Menu.resistor.position, Menu.resistor.size))
		return Menu.resistor.type;
	else if (isInRect(mousePos, Menu.capacitor.position, Menu.capacitor.size))
		return Menu.capacitor.type;
	else if (isInRect(mousePos, Menu.inductor.position, Menu.inductor.size))
		return Menu.inductor.type;
	else if (isInRect(mousePos, Menu.dcSource.position, Menu.dcSource.size))
		return Menu.dcSource.type;
	else if (isInRect(mousePos, Menu.wire.position, Menu.wire.size))
		return Menu.wire.type;
	else
		//Check for capacitor, inductor and other action (sim run for ex)
		return ElementType::NONE;
}

bool GUI_handler::isInRect(olc::vi2d& point, olc::vi2d& pos, olc::vi2d& size)
{
	if (point.x >= pos.x && point.x <= (pos.x + size.x))
	{
		if (point.y >= pos.y && point.y <= (pos.y + size.y))
			return true;
	}
	return false;
}

void GUI_handler::DrawOnline(olc::vi2d& mousePos, Solver* appPtr, ElementModel_T& elementToDraw, Circuit& myCircuit)
{
	elementToDraw.position = mousePos;

	if (true == appPtr->GetKey(olc::R).bPressed)
	{
		elementToDraw.rotation++;
		int32_t temp = elementToDraw.size.x;
		elementToDraw.size.x = elementToDraw.size.y;
		elementToDraw.size.y = temp;
	}

	if (elementToDraw.type != WIRE)
	{
		DrawElement(mousePos, appPtr, elementToDraw.type, elementToDraw.rotation);
	}
	else
	{
		WireModel_T* wirePtr = static_cast<WireModel_T*>(&elementToDraw);
		wirePtr->anchor = Menu.wire.anchor;
		wirePtr->rotation = Menu.wire.rotation;
		wirePtr->elementAnchorPtr = Menu.wire.elementAnchorPtr;
		DrawWire(*wirePtr, appPtr, myCircuit);
	}

}

void GUI_handler::DrawElement(olc::vi2d offset, Solver* appPtr, ElementType element, int rotation, bool contour, olc::Pixel color)
{
	if (contour)
	{
		if(!rotation)
			appPtr->DrawRect(offset - Menu.resistor.size / 2, Menu.resistor.size, color);
		else
			appPtr->DrawRect(offset - olc::vi2d({Menu.resistor.size.y , Menu.resistor.size.x}) / 2, { Menu.resistor.size.y , Menu.resistor.size.x }, color);
	}
	switch (element)
	{
	case RESISTOR:

		for (int i = 0; i < Menu.resistor.points[rotation % 2].size() - 1; i++)
		{
			int j = i + 1;
			appPtr->DrawLine(Menu.resistor.points[rotation % 2][i] + offset, Menu.resistor.points[rotation % 2][j] + offset, color);
		}
		break;

	case CAPACITOR:

		for (int i = 0; i < Menu.capacitor.points_l[rotation % 2].size(); i++)
		{
			int j = i + 1;
			appPtr->DrawLine(Menu.capacitor.points_l[rotation % 2][i % Menu.capacitor.points_l[rotation % 2].size()] + offset, Menu.capacitor.points_l[rotation % 2][j % Menu.capacitor.points_l[rotation % 2].size()] + offset, color);
		}
		for (int i = 0; i < Menu.capacitor.points_r[rotation % 2].size(); i++)
		{
			int j = i + 1;
			appPtr->DrawLine(Menu.capacitor.points_r[rotation % 2][i % Menu.capacitor.points_r[rotation % 2].size()] + offset, Menu.capacitor.points_r[rotation % 2][j % Menu.capacitor.points_r[rotation % 2].size()] + offset, color);
		}
		break;

	case INDUCTOR:

		for (int i = 0; i < Menu.inductor.points[rotation % 2].size() - 1; i++)
		{
			int j = i + 1;
			if (i == 0 || i == Menu.inductor.points[rotation % 2].size() - 2)
			{
				appPtr->DrawLine(Menu.inductor.points[rotation % 2][i] + offset, Menu.inductor.points[rotation % 2][j] + offset, color);
			}
			else
			{
				if (rotation % 2 == 0)
				{
					for (int n = 0; n < Menu.inductor.points[rotation % 2][j].x - Menu.inductor.points[rotation % 2][i].x; n++)
						appPtr->Draw({ Menu.inductor.points[rotation % 2][i].x + offset.x + n, Menu.inductor.points[rotation % 2][i].y + offset.y - static_cast<int>(sqrt(16 - (-4 + n) * (-4 + n)))
							}, color);
				}
				else
				{
					for (int n = 0; n < Menu.inductor.points[rotation % 2][j].y - Menu.inductor.points[rotation % 2][i].y; n++)
						appPtr->Draw({ Menu.inductor.points[rotation % 2][i].x + offset.x - static_cast<int>(sqrt(16 - (-4 + n) * (-4 + n))), Menu.inductor.points[rotation % 2][i].y + offset.y + n
							}, color);
				}
			}
		}
		break;

	case DC_SOURCE:

		for (int i = 0; i < Menu.dcSource.points_l[rotation % 4].size(); i++)
		{
			int j = i + 1;
			appPtr->DrawLine(Menu.dcSource.points_l[rotation % 4][i % Menu.dcSource.points_l[rotation % 4].size()] + offset, Menu.dcSource.points_l[rotation % 4][j % Menu.dcSource.points_l[rotation % 4].size()] + offset, color);
		}
		for (int i = 0; i < Menu.dcSource.points_r[rotation % 4].size(); i++)
		{
			int j = i + 1;
			appPtr->DrawLine(Menu.dcSource.points_r[rotation % 4][i % Menu.dcSource.points_r[rotation % 4].size()] + offset, Menu.dcSource.points_r[rotation % 4][j % Menu.dcSource.points_r[rotation % 4].size()] + offset, color);
		}
		break;
	}
}

void GUI_handler::DrawWire(WireModel_T& wire, Solver* appPtr, Circuit& myCircuit, olc::Pixel colour)
{

	olc::vi2d temp = wire.position - wire.anchor;
	//if (!Menu.wire.isPathLegit)
	//	Menu.wire.isPathLegit = true;

	for (int n = 0; n < std::abs(temp.x) / Menu.resistor.size.y; n++)
	{

		if (myCircuit.CircuitTouched({ wire.anchor.x + (abs(temp.x) / temp.x) * Menu.resistor.size.y * (n + 1), wire.anchor.y }))
			Menu.wire.isPathLegit = false;
	}
	for (int n = 0; n < abs(temp.y) / Menu.resistor.size.y; n++)
	{
		if (myCircuit.CircuitTouched({ wire.position.x, wire.anchor.y + (abs(temp.y) / temp.y) * Menu.resistor.size.y * (n + 1) }))
			Menu.wire.isPathLegit = false;
	}

	if (wire.rotation == 0)
	{
		appPtr->DrawLine({ wire.position.x, wire.anchor.y }, wire.position, colour);
		appPtr->DrawLine(wire.anchor, { wire.position.x, wire.anchor.y }, colour);
	}
	else
	{
		appPtr->DrawLine({ wire.anchor.x, wire.position.y }, wire.position, colour);
		appPtr->DrawLine(wire.anchor, { wire.anchor.x, wire.position.y }, colour);
	}

}

void GUI_handler::AnchorNode(ElementModel_T& element, olc::vi2d& mousePos)
{

	if (element.rotation % 2 == 0)
	{
		if (mousePos.x > element.position.x)
			Menu.wire.anchor = { element.position.x + element.size.x / 2, element.position.y };
		else
			Menu.wire.anchor = { element.position.x - element.size.x / 2, element.position.y };

		Menu.wire.rotation = 0;
	}
	else
	{
		if (mousePos.y > element.position.y)
			Menu.wire.anchor = { element.position.x, element.position.y + element.size.y / 2 };
		else
			Menu.wire.anchor = { element.position.x, element.position.y - element.size.y / 2 };

		Menu.wire.rotation = 1;
	}

	Menu.wire.elementAnchorPtr = &element;
}

void GUI_handler::AnchorNode(olc::vi2d& mousePos)
{
	Menu.wire.anchor = mousePos;
	Menu.wire.elementAnchorPtr = nullptr;
	Menu.wire.rotation = 0;
}

void GUI_handler::PortNode(ElementModel_T& element, ElementModel_T& wire, olc::vi2d& mousePos)
{
	WireModel_T* wirePtr = static_cast<WireModel_T*>(&wire);
	if (element.type == WIRE)
	{
		if (wirePtr->position.x - wirePtr->position_prev.x != 0 ) // check direction of wire
		{
			wirePtr->position.y = element.position.y;
		}
		else
		{
			wirePtr->position.x = element.position.x;
		}
	}
	else if (element.rotation % 2 == 0)
	{
		if (mousePos.x > element.position.x)
			wirePtr->position = { element.position.x + element.size.x / 2, element.position.y };
		else
			wirePtr->position = { element.position.x - element.size.x / 2, element.position.y };

	}
	else
	{
		if (mousePos.y > element.position.y)
			wirePtr->position = { element.position.x, element.position.y + element.size.y / 2 };
		else
			wirePtr->position = { element.position.x, element.position.y - element.size.y / 2 };

	}

}

void GUI_handler::GlowUpElement(ElementModel_T* touch_ptr, Solver* appPtr, Circuit& myCircuit)
{
	if (touch_ptr->type != WIRE)
	{
		DrawElement(touch_ptr->position, appPtr, touch_ptr->type, touch_ptr->rotation, true, olc::RED);
	}
	else
	{
		WireModel_T* wirePtr = static_cast<WireModel_T*>(touch_ptr);
		DrawWire(*wirePtr, appPtr, myCircuit, olc::RED);
	}

}

