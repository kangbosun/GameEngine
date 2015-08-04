#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

struct D3DX11_IMAGE_LOAD_INFO;

namespace GameEngine
{
	namespace FbxLoader
	{
		class FbxLoader;
		struct Node;
	};
	class Shader;
	class Texture2D;
	class Font;
	class GameObject;
	class AudioMusic;
	class AudioSFX;
	class Material;
	struct ShaderFlag;

	template <class T>
	struct resource_container_shared
	{
		std::unordered_map<std::string, std::shared_ptr<T>> map;
		std::shared_ptr<T> Add(const std::string& name, const std::shared_ptr<T>& resource)
		{
			map.insert({ name, resource });
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



	class GAMEENGINE_API Resource
	{
	private:
		static std::wstring resourceFolder;

		Resource() {}
		

	public :
		static void LoadDefaultResource();
		//Textures

	public :
		static resource_container_shared<Texture2D>		textures;
		static resource_container_shared<Shader>		shaders;
		static resource_container_shared<Font>			fonts;
		static resource_container_shared<GameObject>	models;
		static resource_container_shared<AudioMusic>	audios;
		static resource_container_shared<Material>		materials;
		static resource_container_shared<AnimClip>		animClips;
		
	public :
		static void ClearAll();
	private:
		static std::shared_ptr<Texture2D> ProcessTexture(const std::string& name, FbxLoader::FbxLoader& loader);
		static void ProcessMaterial(FbxLoader::FbxLoader& loader, std::wstring modelname);
		static void FbxToObject(const std::wstring& modelName, const std::shared_ptr<GameObject>& object, FbxLoader::Node& meshNode, FbxLoader::FbxLoader& loader);
	};
	}

#pragma warning(pop)