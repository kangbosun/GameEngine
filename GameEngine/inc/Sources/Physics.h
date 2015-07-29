
#pragma once

using namespace std;

namespace GameEngine
{
	class GameObject;
	class Object;

	class GAMEENGINE_API Physics
	{
	public :
		static bool	CollisionTest(GameObject* o1, GameObject* o2);
		static void ProcessCollision(list<std::shared_ptr<Object>>& gameObjects);
		static void ProcessCollisionRecursive(GameObject* o1, GameObject* o2);
	};
}