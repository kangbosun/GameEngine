#pragma once

#include "resource_conatiner.h"

#pragma warning(push)
#pragma warning(disable:4251)

struct D3DX11_IMAGE_LOAD_INFO;

namespace GameEngine
{
	class Shader;
	class Texture2D;
	class Font;
	class GameObject;
	class AudioMusic;
	class AudioSFX;
	class Material;

	namespace FbxLoader
	{
		class FbxLoader;
		struct Node;
	}

	class GAMEENGINE_API Resource
	{
	private:
		static std::wstring resourceFolder;

		Resource() {}
		
	public :
		static void LoadDefaultResource();

	public :		
		static resource_container_shared<Texture2D>		textures;
		static resource_container_shared<Shader>		shaders;
		static resource_container_shared<Font>			fonts;
		static resource_container<GameObject>			models;
		static resource_container_shared<AudioMusic>	audios;
		static resource_container_shared<Material>		materials;
		static resource_container_shared<AnimClip>		animClips;
		
	public :
		static void ClearAll();
		static GameObject* LoadFromFbx(const std::string& name, const std::string& folder, const std::string& filename);
	private:
		static std::shared_ptr<Texture2D> ProcessTexture(const std::string& name, FbxLoader::FbxLoader& loader);
		static void ProcessMaterial(FbxLoader::FbxLoader& loader, std::string modelname);
		static void FbxToObject(const std::string& modelName, GameObject* object, FbxLoader::Node& meshNode, FbxLoader::FbxLoader& loader);
	};
}

#pragma warning(pop)