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
		static void Register(GameObject* go);
	public :
		static GameObject* FindGameObject(const std::string& name);
		static void FindGameObjects(const std::string& name, std::vector<GameObject*>& result);

		GameObject* FindGameObjectInChildren(const std::string& name);
		void FindGameObjectsInChildren(const std::string& name, std::vector<GameObject*>& result);

		bool isRegistered = false;
	public:
		std::string name;
		Transform transform;

		bool active = true;
		bool destroy = false;

	private:
		GameObject* CopyRecursive(Transform* parent);
		GameObject();

	public:
		GameObject(const GameObject& gameObject);


		// access renderer directly
	private :
		std::weak_ptr<Renderer> _renderer;
	public :
		std::shared_ptr<Renderer> renderer();

	public:
		~GameObject() { }
	private :
		/* don't try implicit copy */
		GameObject& operator=(const GameObject& rhs);
		GameObject* Clone();

	public :
		static GameObject* Instantiate(const std::string& name = "");
		static GameObject* Instantiate(GameObject* go);

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
		std::shared_ptr<Component> GetComponentInChildren();

		template <typename T>
		void RemoveComponent();

		void RemoveComponent(const std::string& name);
#pragma endregion
	};


#pragma region Template Methods Definitions
	template <typename T>
	std::shared_ptr<T> GameObject::AddComponent()
	{
		auto temp = make_shared<T>();
		return AddComponent<T>(temp);
	}

	template <typename T>
	std::shared_ptr<T> GameObject::GetComponent()
	{
		for(auto com : components) {
			auto target = std::dynamic_pointer_cast<T>(com);
			if(target)
				return target;
		}
		return std::shared_ptr<T>();
	}
	
	template <typename T>
	std::shared_ptr<T> GameObject::GetComponentInChildren()
	{
		for(auto com : components) {
			auto target = std::dynamic_pointer_cast<T>(com);
			if(target)
				return target;
		}

		std::shared_ptr<T> com;
		for(auto& child : children) {
			com = child->GetComponentInChildren<T>();
			if(com) break;
		}
		return com;
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

namespace std
{
	GAMEENGINE_API bool operator==(const weak_ptr<GameEngine::GameObject>& lhs, const weak_ptr<GameEngine::GameObject>& rhs);
}

#pragma warning(pop)
#endif