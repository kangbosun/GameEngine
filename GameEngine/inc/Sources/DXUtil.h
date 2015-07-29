#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	template <typename T>
	 inline void Debug(T s, bool endline = true)
	{
#ifdef _DEBUG
		if(endline)
			std::cout << s << std::endl;
		else
			std::cout << s;
#endif
	}


	template <>
	 inline void Debug(std::string s, bool endline)
	{
#ifdef _DEBUG
		if(endline)
			std::cout << s.c_str() << std::endl;
		else
			std::cout << s.c_str();
#endif
	}

	template <>
	 inline void Debug(std::wstring s, bool endline)
	{
#ifdef _DEBUG
		if(endline)
			std::wcout << s << std::endl;
		else
			std::wcout << s;
#endif
	}

	 void Tokenize(std::vector<std::string>& tokens, std::string& line, std::string delimeter);

	template <typename T>
	struct  ArrayDeleter
	{
		void operator()(T* p)
		{
			if(p)
				delete[] p;
		}
	};

	template <typename T>
	 inline float clamp(T value, T min, T max)
	{
		if(value > max)
			value = max;
		else if(value < min)
			value = min;
		return value;
	}
}

#pragma warning(pop)