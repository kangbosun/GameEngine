
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
	public :

		const std::string ToString() const
		{
			std::string name = typeid(*this).name();
			name = name.substr(name.find_last_of(' ') + 1);
			return name.substr(name.find_last_of(':') + 1);
		}

		virtual std::shared_ptr<Object> Clone()
		{
			return std::make_shared<Object>(*this);
		}
	};

	template <class Base, class Derived>
	class ClonableObject abstract : public Base
	{
	public:
		virtual std::shared_ptr<Object> Clone()
		{
			return std::make_shared<Derived>((Derived&)*this);
		}
	};
}

#pragma warning(pop)
