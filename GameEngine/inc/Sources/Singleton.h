#pragma once

namespace GameEngine
{
	template <typename T>
	class Singleton : public Object
	{
	protected:
		static std::shared_ptr<T> instance;
		Singleton() {}

	public:
		static std::shared_ptr<T> GetInstance()
		{
			if(!instance)
				instance = std::make_shared<T>();
			return instance;
		}

		static void Destroy()
		{
			if(instance)
				instance.reset();
		}
	};

	template <class T>
	std::shared_ptr<T> Singleton<T>::instance;
}