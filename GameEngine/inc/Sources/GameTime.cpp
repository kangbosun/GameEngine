#include "enginepch.h"
#include "GameTime.h"

namespace GameEngine
{
	using namespace std;
	using namespace std::chrono;

	system_clock::time_point GameTime::start = system_clock::now();
	system_clock::time_point GameTime::current = GameTime::start;
	system_clock::time_point GameTime::prev = GameTime::start;
	unordered_map<int, system_clock::time_point> GameTime::stopWatch;

	float GameTime::_deltaTime = 0;
	float GameTime::_unscaledDeltaTime = 0;
	float GameTime::_timeScale = 1.0f;
	double GameTime::_totalTime = 0.0f;

	readonly<float> GameTime::deltaTime = { GameTime::_deltaTime };
	readonly<float> GameTime::unscaledDeltaTime = { GameTime::_unscaledDeltaTime };
	property<float> GameTime::timeScale = { GameTime::_timeScale };
	readonly<double> GameTime::totalTime = { GameTime::_totalTime };

	void GameTime::Update()
	{
		prev = current;
		current = system_clock::now();

		duration<float> duration = current - prev;
		_unscaledDeltaTime = duration.count();

		_deltaTime = _unscaledDeltaTime * _timeScale;
		_totalTime = _totalTime + _unscaledDeltaTime;
	}

	void GameTime::StopwatchStart(int num)
	{
		stopWatch[num] = system_clock::now();
	}

	float GameTime::StopwatchStop(int num)
	{
		auto now = system_clock::now();
		auto& f = stopWatch.find(num);
		if(f == stopWatch.end())
			return 0;
		duration<float> duration = now - stopWatch[num];
		stopWatch.erase(num);
		return duration.count();
	}
}