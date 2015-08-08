#pragma once

namespace GameEngine
{
	class GAMEENGINE_API Scene 
	{
	public:
		virtual void OnLoad() = 0;
		virtual void OnUnload() {}
		void Update();
		void Render();
	};
}