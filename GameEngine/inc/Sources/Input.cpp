#include "enginepch.h"
#include "Input.h"

namespace GameEngine
{
	

	Vector2 Input::screenSize = { 0, 0 };
	Vector2 Input::mousePos = { 0, 0 };
	Vector2 Input::prevMousePos = { 0, 0 };
	Byte Input::keyboardState[256];
	Byte Input::prevKeyboardState[256];

	void Input::UpdateKeyboard()
	{
		memcpy_s(prevKeyboardState, 256, keyboardState, 256);
		GetKeyboardState(keyboardState);
	}

	void Input::UpdateMouse(HWND hWnd)
	{
		prevMousePos = mousePos;
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);
		mousePos = { (float)p.x, (float)p.y };
	}

	void Input::Process()
	{
		if(mousePos.x < 0) mousePos.x = 0;
		if(mousePos.y < 0) mousePos.y = 0;

		if(mousePos.x > screenSize.x) mousePos.x = screenSize.x;
		if(mousePos.y > screenSize.y) mousePos.y = screenSize.y;
	}

	Input::Input()
	{
	}

	Input::~Input()
	{
	}

	void Input::Update(int width, int height, HWND hWnd)
	{
		screenSize = { (float)width, (float)height };
		UpdateKeyboard();
		UpdateMouse(hWnd);
		Process();
	}

	Vector2 Input::GetMousePos()
	{
		return{ (float)mousePos.x, (float)mousePos.y };
	}

	bool Input::GetKeyDown(BYTE keycode)
	{
		return (keyboardState[keycode] & 0x80) > 0;
	}

	bool Input::GetKeyUp(BYTE keycode)
	{
		return !(keyboardState[keycode] & 0x80);
	}

	bool Input::GetKeyPressed(BYTE keycode)
	{
		return ((keyboardState[keycode] & 0x80) && !(prevKeyboardState[keycode] & 0x80));
	}
}