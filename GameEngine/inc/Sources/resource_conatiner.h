#pragma once
#include <string>

namespace GameEngine
{
	template <class T>
	struct resource_container
	{	
		std::unordered_map<std::string, T*> map;

		T* Add(const std::string& name, T* resource)
		{
			map.insert({ name, resource });
			return resource;
		}

		T* Find(const std::string& name)
		{
			T* ret = nullptr;
			auto iter = map.find(name);
			if(iter != map.cend())
				ret = iter->second;
			return ret;
		}

		bool Remove(const std::string& name)
		{
			auto iter = map.find(name);
			if(iter == map.cend())
				return false;
			map.erase(iter);
			return true;
		}
	};

	template <class T>
	struct resource_container_shared
	{
		std::unordered_map<std::string, std::shared_ptr<T>> map;

		std::shared_ptr<T> Add(const std::string& name, const std::shared_ptr<T>& resource)
		{
			map.insert({ name, resource });
			return resource;
		}

		std::shared_ptr<T> Find(const std::string& name)
		{
			std::shared_ptr<T> ret = nullptr;
			auto iter = map.find(name);
			if(iter != map.cend())
				ret = iter->second;
			return ret;
		}

		bool Remove(const std::string& name)
		{
			auto iter = map.find(name);
			if(iter == map.cend())
				return false;

			map.erase(iter);
			return true;
		}
	};
}