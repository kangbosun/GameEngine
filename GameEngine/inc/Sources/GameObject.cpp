#include "enginepch.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "GraphicDevice.h"
#include "FbxLoader.h"

namespace GameEngine
{
	using namespace std;

	unordered_multimap<string, GameObject*> GameObject::allGameObjects;

	GameObject::GameObject()
	{
		components.reserve(5);
		transform.gameObject = this;
	}
	
	GameObject::GameObject(const GameObject& gameObject) :
		transform(gameObject.transform), name(gameObject.name), active(gameObject.active)
	{
		components.reserve(5);
		transform.gameObject = this;
	}

	shared_ptr<Renderer> GameObject::renderer()
	{
		if(_renderer.expired())
			_renderer = GetComponent<Renderer>();
		return _renderer.lock();
	}

	GameObject* GameObject::Instantiate(const string& name)
	{
		auto go = new GameObject();
		go->name = name;
		return go;
	}

	GameObject* GameObject::Instantiate(const GameObject* go)
	{
		auto clone = new GameObject(*go);
		for(int i = 0; i < go->components.size(); ++i) {
			clone->AddComponent(go->components[i]->CloneShared());
		}
		for(auto child : go->transform.children) {
			Instantiate(child->gameObject)->transform.SetParent(&clone->transform);
		}
		return clone;
	}

	void GameObject::Destroy(GameObject*& object)
	{
		if(object) {
			UnRegister(object);
			for(auto child : object->transform.children) {
				Destroy(child->gameObject);
			}
			delete object;
			object = nullptr;
		}
	}

	void GameObject::Register(GameObject* go)
	{
		if(go && !go->isRegistered) {
			allGameObjects.insert({ go->name, go });
			go->isRegistered = true;
			if(go->transform.parent == nullptr) {
				go->transform.SetParent(&Transform::root);
			}

			for(int i = 0; i < go->components.size(); ++i) {
				Component::Register(go->components[i]);
			}

			for(auto child : go->transform.children)
				Register(child->gameObject);
		}
	}

	void GameObject::UnRegister(GameObject * go)
	{
		if(go && go->isRegistered) {
			auto pair = allGameObjects.equal_range(go->name);
			for(auto i = pair.first; i != pair.second; ++i) {
				if(i->second == go) {
					allGameObjects.erase(i);
					break;
				}
			}
			// component
			for(auto& com : go->components)
				Component::UnRegister(com);
			// transform

			if(go->transform.parent == nullptr)
				Transform::root.removeChild(&go->transform);

			for(auto child : go->transform.children) {
				UnRegister(child->gameObject);
			}
			go->isRegistered = false;
		}
	}

	GameObject* GameObject::FindGameObject(const std::string & name)
	{
		GameObject* ret = nullptr;
		auto pair = allGameObjects.equal_range(name);
		if(pair.first != allGameObjects.cend())
			ret = pair.first->second;
		return ret;
	}

	void GameObject::FindGameObjects(const std::string& name, std::vector<GameObject*>& result)
	{
		auto pair = allGameObjects.equal_range(name);
		for(auto i = pair.first; i != pair.second; ++i) {
			result.push_back(i->second);
		}
	}

	GameObject* GameObject::FindGameObjectInChildren(const std::string & name)
	{
		GameObject* result = nullptr;
		if(this->name == name)
			result = this;
		else {
			for(int i = 0; i < transform.GetChildCount(); ++i) {
				auto child = transform.GetChild(i);
				result = child->gameObject->FindGameObjectInChildren(name);
				if(result) break;
			}
		}
		return result;
	}

	void GameObject::FindGameObjectsInChildren(const std::string & name, std::vector<GameObject*>& result)
	{
		if(this->name == name)
			result.push_back(this);
		for(int i = 0; i < transform.GetChildCount(); ++i) {
			auto child = transform.GetChild(i);
			child->gameObject->FindGameObjectsInChildren(name, result);
		}
	}

	GameObject * GameObject::CopyComponentsRecursive(GameObject* dst)
	{
		if(dst) {
			for(auto& com : components) {
				auto& clone = com->CloneShared();
				dst->AddComponent(clone);
			}
			for(int i = 0; i < transform.GetChildCount(); ++i) 
				transform.GetChild(i)->gameObject->CopyComponentsRecursive(dst->transform.GetChild(i)->gameObject);
		}
		return dst;
	}


	GameObject* GameObject::Clone()
	{
		return Instantiate(this);
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
		return shared_ptr<Component>();
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
