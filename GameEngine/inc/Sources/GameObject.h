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


	class GAMEENGINE_API GameObject final : public Cloneable<Object, GameObject>, public std::enable_shared_from_this<GameObject>
	{
	private :
		static std::unordered_multimap<std::string, std::shared_ptr<GameObject>> allGameObjects;
		bool isRegistered = false;
	public :
		static void Register(const std::shared_ptr<GameObject>& go);
		static std::shared_ptr<GameObject> FindGameObject(const std::string& name);
		static void FindGameObjects(const std::string& name, std::vector<std::shared_ptr<GameObject>>& result);

		std::shared_ptr<GameObject> FindGameObjectInChildren(const std::string& name);
		void FindGameObjectsInChildren(const std::string& name, std::vector<std::shared_ptr<GameObject>>& result);
		
	public:
		std::string name;
		Transform transform;

		bool active = true;
		bool destroy = false;

	public:
		GameObject(const GameObject& gameObject);


		// access renderer directly
	private :
		std::weak_ptr<Renderer> _renderer;
	public :
		std::shared_ptr<Renderer> renderer();

	private :
		GameObject* CopyRecursive(Transform* parent);
		GameObject();
		/* don't try implicit copy */
		GameObject& operator=(const GameObject& rhs);
		~GameObject() {}
	public :
		GameObject* Clone() override;

	public :
		static std::shared_ptr<GameObject> Instantiate(const std::string& name = "");
		static std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& go);

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
		T* target = nullptr;
		for(auto& com : components) {
			target = dynamic_cast<T*>(com.get());
			if(target)
				break;
		}
		return std::shared_ptr<T>(target);
	}
	
	template <typename T>
	std::shared_ptr<T> GameObject::GetComponentInChildren()
	{
		T* target = nullptr;
		for(auto com : components) {
			auto target = dynamic_cast<T*>(com.get());
			if(target)
				break;
		}

		if(!target) {
			for(int i = 0; i < transform.GetChildCount(); ++i) {
				auto child = transform.GetChild(i);
				target = child->gameObject->GetComponentInChildren<T>().get();
				if(target) break;
			}
		}
		return std::shared_ptr<T>(target);
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