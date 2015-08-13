#include "enginepch.h"

#include "Component.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Transform.h"

namespace GameEngine
{
	using namespace std;

	template<typename T>
	vector<T> make_reserved_vector(size_t n)
	{
		vector<T> ret;
		ret.reserve(n);
		return ret;
	}

	vector<weak_ptr<Component>> Component::allComponents = make_reserved_vector<weak_ptr<Component>>(1000);
	queue<weak_ptr<Component>> Component::watingForRegister;

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
			watingForRegister.push(com);
			//allComponents.push_back(com);
			com->registered = true;
			com->Start();
		}
	}

	void Component::UnRegister(const std::shared_ptr<Component>& com)
	{
		if(com && com->registered) {
			com->registered = false;
			//do something
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
				if(strong->registered) {
					if(strong->enabled)
						strong->Update();
					forSwap.emplace_back(move(weak));
				}
			}
		}
		allComponents.swap(forSwap);
		forSwap.clear();
		for(int i = 0; i < watingForRegister.size(); ++i) {
			allComponents.push_back(watingForRegister.front());
			watingForRegister.pop();
		}
	}

	Transform* const Component::GetTransform()
	{
		if(_gameObject)
			return &_gameObject->transform;
		else
			return nullptr;
	}

	const std::shared_ptr<Renderer> Component::renderer()
	{
		if(_gameObject)
			return _gameObject->renderer();
		else
			return std::shared_ptr<Renderer>();
	}
}


