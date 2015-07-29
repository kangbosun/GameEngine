#pragma once

namespace GameEngine
{
	struct GAMEENGINE_API GraphicSetting
	{
		Math::Color ambientColor = Math::Color(0.1f, 0.1f, 0.1f, 1.0f);
		int shadowMapSize = 2048;
	};

	struct GAMEENGINE_API GlobalSetting
	{
		static float aspectRatio;
		static int resolutionX;
		static int resolutionY;
		static float UIScaleFactorX;
		static float UIScaleFactorY;
		static const int maxUIWidth = 1920;
		static const int maxUIHeight = 1080;
		static GraphicSetting graphicSetting;
	};
}