#include "enginepch.h"

#include "Component.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"

namespace GameEngine
{
	using namespace std;

	unordered_map<string, list<weak_ptr<Component>>> Component::allComponents;

	Component::Component()
	{
	}

	Component::~Component()
	{
		OnDestroy();
		/*if(gameObject)
			std::cout << gameObject->name << " - " << ToString() << " destroyed(Component)" << std::endl;*/
	}

	void Component::Register(const shared_ptr<Component>& com)
	{
		if(!com->registered && com->gameObject) {
			allComponents[com->ToString()].push_back(com);
			com->_registered = true;
			com->Start();
		}
	}

	void Component::UnRegister(const shared_ptr<Component>& com)
	{
		//if(com->isRegistered) {
		//	auto& list = allComponents[com->ToString()];
		//	list.remove(com);
		//	if(list.size() == 0)
		//		allComponents.erase(com->ToString());

		//	com->isRegistered = false;
		//}
	}

	const std::shared_ptr<Transform> Component::transform()
	{
		if(gameObject)
			return gameObject->transform();
		else
			return std::shared_ptr<GameEngine::Transform>();
	}

	const std::shared_ptr<Renderer> Component::renderer()
	{
		if(gameObject)
			return gameObject->renderer();
		else
			return std::shared_ptr<Renderer>();
	}
}