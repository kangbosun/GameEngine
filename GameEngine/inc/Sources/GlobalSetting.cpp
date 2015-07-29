#include "enginepch.h"
#include "GlobalSetting.h"

namespace GameEngine
{
	float GlobalSetting::aspectRatio = 16.0f / 9.0f;
	GraphicSetting GlobalSetting::graphicSetting;	

	int GlobalSetting::resolutionX = 0;
	int GlobalSetting::resolutionY = 0;

	float GlobalSetting::UIScaleFactorX = 1;
	float GlobalSetting::UIScaleFactorY = 1;
}