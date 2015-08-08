#pragma once
#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class Texture2D;
	class Transform;
	class Renderer;

	class GAMEENGINE_API GameObject final : public Object
	{
	private :
		static std::unordered_multimap<std::string, GameObject*> allGameObjects;
		bool isRegistered = false;
	public :
		static void Register(GameObject* go);
		static GameObject* FindGameObject(const std::string& name);
		static void FindGameObjects(const std::string& name, std::vector<GameObject*>& result);

		GameObject* FindGameObjectInChildren(const std::string& name);
		void FindGameObjectsInChildren(const std::string& name, std::vector<GameObject*>& result);
		
	public:
		std::string name;
		Transform transform;

		bool active = true;
		bool destroy = false;

	public:
		GameObject(const GameObject& gameObject);
		~GameObject() {}

		// access renderer directly
	private :
		std::weak_ptr<Renderer> _renderer;
	public :
		std::shared_ptr<Renderer> renderer();

	private :
		GameObject* CopyComponentsRecursive(GameObject* dst);
		GameObject();
		/* don't try implicit copy */
		GameObject& operator=(const GameObject& rhs);
		
	public :
		GameObject* Clone();

	public :
		static GameObject* Instantiate(const std::string& name = "");
		static GameObject* Instantiate(const GameObject* go);

#pragma region Component management methods
	private:
		std::vector<std::shared_ptr<Component>> components;

	public :
		template <typename T>
		std::shared_ptr<T> AddComponent();
		
		std::shared_ptr<Component> AddComponent(const std::shared_ptr<Component>& _com);

		template <typename T>
		std::shared_ptr<T> GetComponent();

		std::shared_ptr<Component> GetComponent(const std::string& name);

		template <typename T>
		std::shared_ptr<T> GetComponentInChildren();

		template <typename T>
		void RemoveComponent();

		void RemoveComponent(const std::string& name);
#pragma endregion
	};


#pragma region Template Methods Definitions
	template <typename T>
	std::shared_ptr<T> GameObject::AddComponent()
	{
		auto temp = std::make_shared<T>();
		AddComponent(std::static_pointer_cast<Component>(temp));
		return temp;
	}

	template <typename T>
	std::shared_ptr<T> GameObject::GetComponent()
	{
		std::shared_ptr<T> target;
		for(auto& com : components) {
			target = std::dynamic_pointer_cast<T>(com);
			if(target)
				break;
		}
		return target;
	}
	
	template <typename T>
	std::shared_ptr<T> GameObject::GetComponentInChildren()
	{
		std::shared_ptr<T> target = GetComponent<T>();

		if(!target) {
			for(int i = 0; i < transform.GetChildCount(); ++i) {
				auto child = transform.GetChild(i);
				target = child->gameObject->GetComponentInChildren<T>();
				if(target) break;
			}
		}
		return target;
	}

	template <typename T>
	void GameObject::RemoveComponent()
	{
		for(int i = 0; i < components.size(); ++i) {
			auto target = std::dynamic_cast<T*>(components[i]);
			if(target) {
				swap(components[i], components.back());
				components.pop_back();
				return;
			}
		}
	}
#pragma endregion

}

#pragma warning(pop)
#endif