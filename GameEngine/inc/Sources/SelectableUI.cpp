#include "enginepch.h"
#include "UI.h"

namespace GameEngine
{
	using namespace std;
	using namespace std::placeholders;

	SelectableUI::SelectableUI()
	{
		stateMachine = bind(&SelectableUI::state_Up, this, _1);
	}

	SelectableUI::SelectableUI(const SelectableUI& o)
	{
		colorUp = o.colorUp;
		colorDown = o.colorDown;
		colorHover = o.colorHover;
		stateMachine = bind(&SelectableUI::state_Up, this, _1);
	}

	void SelectableUI::SetState(const UIStateParam& param)
	{
		if(stateMachine)
			stateMachine(param);
	}

	void SelectableUI::state_Up(const UIStateParam& param)
	{
		prevState = state;
		SetColor(colorUp);
		if(param.instersects) {
			state = UIState::eHover;
			if(onHoverEnter) onHoverEnter();
			stateMachine = bind(&SelectableUI::state_Hover, this, _1);
		}
	}
	void SelectableUI::state_Down(const UIStateParam& param)
	{
		prevState = state;
		SetColor(colorDown);
		if(param.instersects) {
			if(!param.lbuttonDown) { // click!
				state = UIState::eUp;
				if(onClick) onClick();
				stateMachine = bind(&SelectableUI::state_Up, this, _1);
			}
		}
		else {
			if(!param.lbuttonDown) {
				state = UIState::eUp;
				if(onUp) onUp();
				stateMachine = bind(&SelectableUI::state_Up, this, _1);
			}
		}
	}
	void SelectableUI::state_Hover(const UIStateParam& param)
	{
		prevState = state;
		SetColor(colorHover);
		if(param.instersects) {
			if(param.lbuttonDown) {
				state = UIState::eDown;
				if(onDown) onDown();
				stateMachine = bind(&SelectableUI::state_Down, this, _1);
			}
		}
		else {
			state = UIState::eUp;
			if(onHoverExit) onHoverExit();
			stateMachine = bind(&SelectableUI::state_Up, this, _1);
		}
	}
}