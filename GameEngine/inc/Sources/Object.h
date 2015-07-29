#pragma once

#include <memory>
#include <string>

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class Object;

	class GAMEENGINE_API Object
	{
	public:

		const std::string ToString() const
		{
			std::string name = typeid(*this).name();
			name = name.substr(name.find_last_of(' ') + 1);
			return name.substr(name.find_last_of(':') + 1);
		}

		virtual Object* Clone()
		{
			return new Object(*this);
		}
	};

	template <class Base, class Derived>
	class ClonableObject : public Base
	{
	public:
		virtual Base* Clone()
		{
			return new Derived((Derived)*this);
		}
	};

	template <class Content>
	class GAMEENGINE_API Node 
	{
	protected:
		Content* parent = nullptr;
		std::vector<Content*> children;

	public:
		bool RemoveChild(Content* const child)
		{
			if(child) {
				for(int i = 0; i < children.size(); ++i) {
					auto& c = children[i];
					if(c == child) {
						std::move(c, children.back());
						children.pop_back();
						return true;
					}
				}
			}
			return false;
		}
		bool AddChild(Content* const child)
		{
			if(child) {
				Node* g = (Node*)child;
				if(g->parent)
					((Node*)g->parent)->RemoveChild(child);
				children.push_back(child);
				return true;
			}
			return false;
		}

		void SetParent(Content* _parent)
		{
			if(parent) {
				Node p = (Node*)parent;
				p->RemoveChild(this);
			}
			parent = _parent;
			((Node*)parent)->AddChild(this);
		}

		Content* GetParent()
		{ 
			return parent;
		}
	};
}

#pragma warning(pop)
