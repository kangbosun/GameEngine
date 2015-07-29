#pragma once
#include <functional>

#define prop_get [&]()
#define prop_set(type) [&](const type& value)

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	template <class T>
	class GAMEENGINE_API readonly
	{
	protected:
		std::function<T()> getter;
	public:
		readonly(T& var)
		{
			getter = [&]() { return var; };
		}

		operator T()
		{
			return getter();
		}

		T* operator->()
		{
			return &getter();
		}

		readonly(std::function<T()> _getter)
		{
			getter = _getter;
		}

		T Get()
		{
			return getter();
		}
	};

	template <class T>
	class GAMEENGINE_API property : public readonly < T >
	{
	protected:
		std::function<void(const T&)> setter;
	public:
		property<T>& operator =(const T& value)
		{
			setter(value);
			return *this;
		}

		property(T& var) : readonly(var)
		{
			setter = [&](const T& value) { var = value; };
		}

		property(std::function<T()> _getter, std::function<void(const T&)> _setter) : readonly(_getter)
		{
			setter = _setter;
		}
	};
}

#pragma warning(pop)
