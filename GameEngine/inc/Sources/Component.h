#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class GameObject;
	class Renderer;
	class Transform;

	class GAMEENGINE_API Component  : public Object
	{
		friend class Scene;
		friend class GameObject;

	protected :
		bool registered = false;
		bool destroy = false;
		GameObject* _gameObject = nullptr;

	public:
		readonly<GameObject*> gameObject{ prop_get { return _gameObject; } };
		Transform* const transform();
		const std::shared_ptr<Renderer> renderer();
		bool enabled = true;
		
	protected:
		virtual void OnDestroy() {}
		virtual void Start() {}
	public:	
		virtual void Update() {}
		virtual void OnCollisionEnter() {}
		virtual void OnCollisionExit() {}

		Component();
		virtual ~Component();
		Component(const Component& c) { enabled = c.enabled; }


	private :
		static std::vector<std::weak_ptr<Component>> allComponents;
		static void Register(const std::shared_ptr<Component>& com);
		static void UpdateAll();
		static void Clear() { allComponents.clear(); }
	};
}

#pragma warning(pop)