#pragma once

namespace GameEngine
{
	struct GAMEENGINE_API Input
	{
	private:
		static Byte keyboardState[256];
		static Byte prevKeyboardState[256];

		static Vector2 screenSize;
		static Vector2 mousePos;
		static Vector2 prevMousePos;

	private:
		static void UpdateKeyboard();
		static void UpdateMouse(HWND hWnd);
		static void Process();

		Input();
		~Input();
	public:
		static void Update(int width, int height, HWND hWnd);
		static Vector2 GetMousePos();
		static bool GetKeyDown(BYTE keycode);
		static bool GetKeyUp(BYTE keycode);
		static bool GetKeyPressed(BYTE keycode);
	};
}
