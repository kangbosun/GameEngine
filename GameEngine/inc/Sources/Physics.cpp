#include "enginepch.h"
#include "Physics.h"
#include "GameObject.h"
#include "DXUtil.h"
#include "Transform.h"

namespace GameEngine
{
	bool Physics::CollisionTest(GameObject* o1, GameObject* o2)
	{
		//if(!(o1->enableCollisionTest && o2->enableCollisionTest))
		//	return false;
		//auto c1 = o1->collider;
		//auto c2 = o2->collider;

		//c1.Center = XMFLOAT3(0, 0, 0);
		//c2.Center = XMFLOAT3(0, 0, 0);

		//c1.Transform(c1, o1->transform()->World());
		//c2.Transform(c2, o2->transform()->World());

		//c1.Radius = 1;
		//c2.Radius = 1;

		//if(c1.Intersects(c2)) {
		//	return true;
		//}
		//return false;
		return false;
	}

	void Physics::ProcessCollision(list<std::shared_ptr<Object>>& gameObjects)
	{
		/*auto iter = (gameObjects.begin());
		const int size = (int)gameObjects.size();
		for(int i = 0; i < size - 1; i++) {
			auto o1 = *iter++;
			if(o1->type != Object::eGameObject)
				continue;
			auto go1 = ((GameObject*)o1.get());
			for(int j = i + 1; j < size; j++) {
				auto o2 = *iter++;
				if(o2->type != Object::eGameObject)
					continue;
				auto go2 = ((GameObject*)o2.get());

				ProcessCollisionRecursive(go1, go2);
				iter--;
			}
		}*/
	}

	void Physics::ProcessCollisionRecursive(GameObject* o1, GameObject* o2)
	{
		//if(CollisionTest(o1, o2))
		//	Debug("collision detect(" + o1->name + ", " + o2->name + ")");

		//auto& childs = o2->children;
		//const size_t childCount = childs.size();

		//if(childCount == 0)
		//	return;

		//auto iter = childs.begin();
		//for(int i = 0; i < childCount; i++) {
		//	auto c = *iter++;
		//	if(c->type != Object::eGameObject)
		//		continue;
		//	auto gc = ((GameObject*)c.get());
		//	if(!gc->enableCollisionTest)
		//		continue;
		//	ProcessCollisionRecursive(o1, gc);
		//}
	}
}