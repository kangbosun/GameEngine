#include "enginepch.h"

#include "Component.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"

namespace GameEngine
{
	using namespace std;

	vector<weak_ptr<Component>> Component::allComponents;

	Component::Component() 
	{
	}

	Component::~Component()
	{
		OnDestroy();
	}

	void Component::Register(const shared_ptr<Component>& com)
	{
		if(!com->registered && com->gameObject) {
			allComponents.push_back(com);
			com->registered = true;
			com->Start();
		}
	}

	void Component::UpdateAll()
	{
		vector<std::weak_ptr<Component>> forSwap;
		forSwap.reserve(allComponents.size());
		for(int i = 0; i < allComponents.size(); ++i) {
			auto& weak = allComponents[i];
			if(!weak.expired()) {
				auto& strong = weak.lock();
				if(strong->enabled)
					strong->Update();
				forSwap.emplace_back(move(weak));
			}
		}
		allComponents.swap(forSwap);
	}

	Transform* const Component::transform()
	{
		if(gameObject)
			return &gameObject->transform;
		else
			return nullptr;
	}

	const std::shared_ptr<Renderer> Component::renderer()
	{
		if(gameObject)
			return gameObject->renderer();
		else
			return std::shared_ptr<Renderer>();
	}
}