
#pragma once

namespace GameEngine
{
	class GAMEENGINE_API Scene abstract
	{
	public :
		virtual void OnLoad() {}
		virtual void OnUnload() {}
		void Update();
		void Render();
	};
}