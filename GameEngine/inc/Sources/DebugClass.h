#pragma once

#include <Windows.h>
#include <string>
#include <iostream>

struct Debug
{
#ifdef _WIN64
	enum DebugColor
	{
		Black = 0, Blue = 1, Green = 2, Aqua = 3, Red = 4, Purple = 5, 
		Yellow = 6, White = 7, Gray = 8, LightBlue = 9, LightGreen = 10,
		LightAqua = 11, LightRed = 12, LightPurple= 13, LightYellow = 14, LightWhite = 15
	};

	template <typename T>
	static void Log(const T& msg, DebugColor color = White)
	{
		auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, (WORD)color);
		std::cout << msg << std::endl;
		if(color != White)
			SetConsoleTextAttribute(handle, (WORD)White);
	}

	template <>
	static void Log(const std::wstring& msg, DebugColor color)
	{
		auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, (WORD)color);
		std::wcout << msg << std::endl;
		if(color != White)
			SetConsoleTextAttribute(handle, (WORD)White);
	}
#else
#error There is no implement of 'Debug' class
#endif
};