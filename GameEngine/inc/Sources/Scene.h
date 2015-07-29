#pragma once

namespace GameEngine
{
	class GAMEENGINE_API Scene 
	{
	protected:
		Transform rootTransform;
	public :
		void Register(GameObject* go);

	public:
		virtual void OnLoad() = 0;
		virtual void OnUnload() {}
		void Update();
		void Render();
	};
}