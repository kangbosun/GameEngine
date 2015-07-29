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

	class GAMEENGINE_API GameObject final : public Object, public std::enable_shared_from_this<GameObject>
	{
	private:
		std::unordered_map<std::string, std::shared_ptr<Component>> components;
		void UpdateComponents();
		std::weak_ptr<Transform> _transform;
		std::weak_ptr<Renderer> _renderer;
		bool isRegistered = false;
	public:
		std::string name;
		std::list<std::shared_ptr<GameObject>> children;

		std::shared_ptr<GameObject> parent;

		bool active = true;
		bool destroy = false;
		bool enableCollisionTest = false;

		static std::list<std::shared_ptr<GameObject>> allGameObjects;

	private:
		std::shared_ptr<GameObject> CopyEmpty();
		void CopyComponents(std::shared_ptr<GameObject>& src, std::shared_ptr<GameObject>& dst);
		GameObject() = default;

	public:
		GameObject(const GameObject& gameObject);
		std::shared_ptr<Transform> transform();
		std::shared_ptr<Renderer> renderer();

		static void Register(const std::shared_ptr<GameObject>& gameObject);
		static void Destroy(std::shared_ptr<GameObject>& object);

	public:
		~GameObject() { std::cout << name << " destroyed(GameObject)" << std::endl; }
		virtual const std::string ToString() const { return name; }

		void Update();
		void Render();

		virtual std::shared_ptr<Object> Clone();

		void AddChild(const std::shared_ptr<GameObject>& child);
		void RemoveChild(const std::shared_ptr<GameObject>& child) { children.remove(child); }

		std::shared_ptr<GameObject> FindGameObject(const std::string& name);

		static std::shared_ptr<GameObject> Instantiate(const std::string& name = "");

#pragma region Component management methods
		template <typename T>
		std::shared_ptr<T> AddComponent();

		template <typename T>
		std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& com);

		template <typename T>
		std::shared_ptr<T> GetComponent();

		std::shared_ptr<Component> GetComponent(const std::string& name);

		template <typename T>
		std::shared_ptr<T> GetComponentInChildren();

		template <typename T>
		void RemoveComponent();

		void RemoveComponent(const std::string& name) { components.erase(name); }
#pragma endregion
	};

	template <typename T>
	std::shared_ptr<T> GameObject::AddComponent()
	{
		auto temp = std::make_shared<T>();
		return AddComponent<T>(temp);
	}

	template <typename T>
	std::shared_ptr<T> GameObject::AddComponent(const std::shared_ptr<T>& _com)
	{
		std::shared_ptr<Component> com = std::dynamic_pointer_cast<Component>(_com);
		if(com) {
			components[com->ToString()] = com;
			com->gameObject = shared_from_this();
			if(isRegistered)
				Component::Register(com);
		}
		return _com;
	}

	template <typename T>
	std::shared_ptr<T> GameObject::GetComponent()
	{
		for(auto& iter : components) {
			auto& target = std::dynamic_pointer_cast<T>(iter.second);
			if(target)
				return target;
		}
		return std::shared_ptr<T>();
	}

	template <typename T>
	std::shared_ptr<T> GameObject::GetComponentInChildren()
	{
		for(auto& iter : components) {
			auto& target = std::dynamic_pointer_cast<T>(iter.second);
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
		std::string typeName = typeid(T).name();
		components.erase(typeName);
	}

	//////////////////////////////////////////////////////////////

	//class DXSprite2D : public GameObject
	//{
	//public:
	//	bool IsBillBoard;
	//	XMFLOAT2 Size;
	//	DXSprite2D(const string& name = "DXSprite");
	//	void Initialize(const XMFLOAT3& pos, const XMFLOAT2& size, const std::shared_ptr<Texture2D>& Texture2D, const XMFLOAT2& srcBeginPos = { 0, 0 }, const XMFLOAT2& srcEndPos = { 1, 1 });
	//};
}

namespace std
{
	GAMEENGINE_API bool operator==(const weak_ptr<GameEngine::GameObject>& lhs, const weak_ptr<GameEngine::GameObject>& rhs);
}

#pragma warning(pop)
#endif