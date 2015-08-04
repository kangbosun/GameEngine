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
		virtual std::shared_ptr<Object> CloneShared()
		{
			return std::shared_ptr<Object>(Clone());
		}
	};

	template <class Base, class Derived>
	class Cloneable : public Base
	{
	public:
		virtual Base* Clone()
		{
			return new Derived((*(Derived*)this));
		}
		std::shared_ptr<Object> CloneShared()
		{
			return std::shared_ptr<Derived>((Derived*)Clone());
		}
	};

	template <class Content>
	class GAMEENGINE_API Node 
	{
	protected:
		static Content root;
		Content* parent = nullptr;
		std::vector<Content*> children;

	public:
		Node();

		bool RemoveChild(Content* child);
		bool AddChild(Content* child);

		void SetParent(Content* _parent);

		Content* GetParent() { return parent; }
		size_t GetChildCount() { return children.size(); }
		Content* GetChild(int n) { if(n < children.size()) return children[n]; else return nullptr; }
	};

	template<class Content>
	Node<Content>::Node()
	{
		root.AddChild((Content*)this);
	}

	template<class Content>
	bool Node<Content>::RemoveChild(Content* child)
	{
		if(child) {
			for(int i = 0; i < children.size(); ++i) {
				auto& c = children[i];
				if(c == child) {
					std::swap(c, children.back());
					children.pop_back();
					return true;
				}
			}
		}
		return false;
	}

	template<class Content>
	bool Node<Content>::AddChild(Content* child)
	{
		if(child) {
			if(child->parent)
				(child->parent)->RemoveChild(child);
			children.push_back(child);
			return true;
		}
		return false;
	}

	template<class Content>
	void Node<Content>::SetParent(Content * _parent)
	{
		if(parent) {
			_parent->RemoveChild((Content*)this);
		}
		parent = _parent;
		parent->AddChild((Content*)this);
	}
}

#pragma warning(pop)
