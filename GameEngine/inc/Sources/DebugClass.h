#pragma once


struct Debug
{
	static std::string blank;
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
		std::cout << blank << msg << std::endl;
		if(msg[0] == '#')
			blank += "\t";
	}

	template <>
	static void Log(const std::wstring& msg, DebugColor color)
	{		
		auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(handle, (WORD)color);
		std::wcout << std::wstring(blank.begin(), blank.end()) << msg << std::endl;
		if(msg[0] == L'#')
			blank += "\t";
	}

	static void Success()
	{
		if(blank.size() > 0)
			blank.pop_back();
		Log("Success", Debug::Green);		
	}

	static void Failed(const std::string& msg)
	{
		if(blank.size() > 0)
			blank.pop_back();
		Log("Failed - " + msg, Debug::Red);	
	}
#else
#error There is no implement of 'Debug' class
#endif
};
