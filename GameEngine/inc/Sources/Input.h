
#pragma once

namespace GameEngine
{
	struct GAMEENGINE_API Input
	{
	private :
		static Math::Byte keyboardState[256];
		static Math::Byte prevKeyboardState[256];
		
		static Math::Vector2 screenSize;
		static Math::Vector2 mousePos;
		static Math::Vector2 prevMousePos;

	private:
		static void UpdateKeyboard();
		static void UpdateMouse(HWND hWnd);
		static void Process();

		Input();
		~Input();
	public :
		static void Update(int width, int height, HWND hWnd);
		static Math::Vector2 GetMousePos();
		static bool GetKeyDown(BYTE keycode);
		static bool GetKeyUp(BYTE keycode);
		static bool GetKeyPressed(BYTE keycode);	};
}
