#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class GAMEENGINE_API GameTime
	{
	private:
		GameTime() = default;
		static std::chrono::system_clock::time_point prev;
		static std::chrono::system_clock::time_point current;
		static std::chrono::system_clock::time_point start;
		static std::unordered_map<int, std::chrono::system_clock::time_point> stopWatch;

		static float _deltaTime;
		static float _unscaledDeltaTime;
		static float _timeScale;
		
		static double _totalTime;
	public:
		static readonly<float> deltaTime;
		static readonly<float> unscaledDeltaTime;
		static property<float> timeScale;
		static readonly<double> totalTime;
		static float updateTime;
		static float frameTime;

		static void Update();
		static void StopwatchStart(int num);
		static float StopwatchStop(int num);
	};
}

#pragma warning(pop)