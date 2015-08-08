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

	/*cloneable */
	template <class derived, class base>
	class Cloneable : public base
	{
	public:
		virtual base* Clone()
		{
			return new derived((*(derived*)this));
		}
		std::shared_ptr<derived> CloneShared()
		{
			return std::shared_ptr<derived>((derived*)Clone());
		}
	};
}

#pragma warning(pop)
