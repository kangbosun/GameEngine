#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class GameObject;
	class Renderer;
	class Transform;

	class GAMEENGINE_API Component abstract : public Object, public std::enable_shared_from_this<Component>
	{
	public:
		enum EventType
		{
			eOnCollisionEnter, eOnCollisionExit
		};
		friend GameObject;
	public:
		static std::unordered_map<std::string, std::list<std::weak_ptr<Component>>> allComponents;

	protected:
		typedef std::unordered_map<EventType, std::function<void()>> EventReceiverMap;
		bool _registered = false;

	public:
		std::shared_ptr<GameObject> gameObject;
		const std::shared_ptr<Transform> transform();
		const std::shared_ptr<Renderer> renderer();
		readonly<bool> registered = { _registered };
		bool enabled = true;
		bool destroy = false;

	protected:
		virtual void OnDestroy() {}
		virtual void Start() {}
	public:
		Component();
		virtual ~Component();
		virtual void Update() {}
		virtual void OnCollisionEnter() {}
		virtual void OnCollisionExit() {}

		Component(const Component& c) { enabled = c.enabled; _registered = false; }

	public:
		static void Register(const std::shared_ptr<Component>& com);
		static void UnRegister(const std::shared_ptr<Component>& com);
	};
}

#pragma warning(pop)