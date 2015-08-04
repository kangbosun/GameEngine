#include "enginepch.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "GraphicDevice.h"

namespace GameEngine
{
	using namespace std;

	GameObject::GameObject()
	{
		transform.gameObject = this;
	}
	
	GameObject::GameObject(const GameObject& gameObject)
	{
		active = gameObject.active;
		transform = gameObject.transform;
		transform.gameObject = this;

		for(auto& com : gameObject.components)
			AddComponent(shared_ptr<Component>(com->Clone()));
	}

	shared_ptr<Renderer> GameObject::renderer()
	{
		if(_renderer.expired())
			_renderer = GetComponent<Renderer>();
		return _renderer.lock();
	}

	shared_ptr<GameObject> GameObject::Instantiate(const string& name)
	{
		auto go = new GameObject();
		go->name = name;
		return shared_ptr<GameObject>(go);
	}

	shared_ptr<GameObject> GameObject::Instantiate(const shared_ptr<GameObject>& go)
	{
		return shared_ptr<GameObject>(new GameObject(*go.get()));
	}

	void GameObject::Register(const shared_ptr<GameObject>& go)
	{
		if(go && !go->isRegistered) {
			allGameObjects.insert({ go->name, go });
			go->isRegistered = true;

			for(const auto& com : go->components) {
				Component::Register(com);
			}
		}
	}

	shared_ptr<GameObject> GameObject::FindGameObject(const std::string & name)
	{
		auto results = allGameObjects.equal_range(name);
		if(results.first != allGameObjects.cend())
			return results.first->second;
	}

	void GameObject::FindGameObjects(const std::string& name, std::vector<shared_ptr<GameObject>>& result)
	{
		auto results = allGameObjects.equal_range(name);
		for(auto iter = results.first; iter != results.second; ++iter) {
			result.push_back(iter->second);
		}
	}

	shared_ptr<GameObject> GameObject::FindGameObjectInChildren(const std::string & name)
	{
		shared_ptr<GameObject> result = nullptr;
		if(this->name == name)
			result = shared_from_this();
		else {
			for(int i = 0; i < transform.GetChildCount(); ++i) {
				auto child = transform.GetChild(i);
				result = child->gameObject->FindGameObjectInChildren(name);
				if(result) break;
			}
		}
		return result;
	}

	void GameObject::FindGameObjectsInChildren(const std::string & name, std::vector<shared_ptr<GameObject>>& result)
	{
		if(this->name == name)
			result.push_back(shared_from_this());
		for(int i = 0; i < transform.GetChildCount(); ++i) {
			auto child = transform.GetChild(i);
			child->gameObject->FindGameObjectsInChildren(name, result);
		}
	}

	GameObject* GameObject::CopyRecursive(Transform* parent)
	{
		auto node = new GameObject(*this);
		if(parent)
			node->transform.SetParent(parent);

		for(int i = 0; i < transform.GetChildCount(); ++i) {
			auto pc = transform.GetChild(i)->gameObject->CopyRecursive(&node->transform);
		}
		return node;
	}

	GameObject* GameObject::Clone()
	{
		return CopyRecursive(nullptr);
	}


	std::shared_ptr<Component> GameObject::AddComponent(const std::shared_ptr<Component>& _com)
	{
		if(_com) {
			components.push_back(_com);
			_com->_gameObject = this;
			if(isRegistered)
				Component::Register(_com);
		}
		return _com;
	}

	shared_ptr<Component> GameObject::GetComponent(const string& name)
	{
		for(auto com : components) {
			if(name.compare(com->ToString()) == 0)
				return com;
		}
	}

	void GameObject::RemoveComponent(const std::string & name)
	{
		for(auto& com : components) {
			if(com->ToString() == name) {
				swap(com, components.back());
				components.pop_back();
				return;
			}
		}
	}
}

namespace std
{
	bool operator==(const weak_ptr<GameEngine::GameObject>& lhs, const weak_ptr<GameEngine::GameObject>& rhs)
	{
		if(lhs.lock() && rhs.lock())
			return lhs.lock() == rhs.lock();
		else
			return false;
	}
}