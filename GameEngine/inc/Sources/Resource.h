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

	class GAMEENGINE_API Resource
	{
	private:
		static std::wstring resourceFolder;

		static std::unordered_map<std::wstring, std::shared_ptr<Shader>> shaders;
		static std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> Texture2Ds;
		static std::unordered_map<std::wstring, std::shared_ptr<Font>> fonts;
		static std::unordered_map<std::wstring, std::shared_ptr<GameObject>> models;
		static std::unordered_map<std::wstring, std::shared_ptr<AudioMusic>> musics;
		static std::unordered_map<std::wstring, std::shared_ptr<AudioSFX>> soundEffects;
		static std::unordered_map<std::string, std::shared_ptr<Material>> materials;

		Resource() {}
		static void ProcessMaterial(FbxLoader::FbxLoader& loader, std::wstring modelname);
	public:
		static void LoadDefaultResource();
		static void AddShader(const std::wstring& name, const std::shared_ptr<Shader>& shader);
		static void AddFont(const std::wstring& name, const std::wstring& folder, const std::wstring& filename);
		static std::shared_ptr<Texture2D>& AddTexture2D(const std::wstring& name, const std::wstring& ptr, D3DX11_IMAGE_LOAD_INFO* loadInfo = 0);
		static void AddModel(const std::wstring& name, const std::string& folder, const std::string& filename);
		static void AddMusic(const std::wstring& name, const std::wstring& filename);
		static void AddSFX(const std::wstring& name, const std::wstring& filename);
		static void AddMaterial(const std::string& name, std::shared_ptr<Material>& material);

		static std::shared_ptr<Font> GetFont(const std::wstring& fontname);
		static std::shared_ptr<Shader> GetShader(const std::wstring& shaderName);
		static std::shared_ptr<Texture2D> GetTexture2D(const std::wstring& Texture2DName);
		static std::shared_ptr<GameObject> GetModel(const std::wstring& modelName);
		static std::shared_ptr<AudioMusic> GetMusic(const std::wstring& musicName);
		static std::shared_ptr<AudioSFX> GetSFX(const std::wstring& sfxName);
		static std::shared_ptr<Material> GetMaterial(const std::string& materialName);

		static bool RemoveShader(const std::wstring& name);
		static bool RemoveFont(const std::wstring& name);
		static bool RemoveTexture2D(const std::wstring& name);
		static bool RemoveModel(const std::wstring& name);
		static bool RemoveMusic(const std::wstring& name);
		static bool RemoveSFX(const std::wstring& name);
		static bool RemoveMaterial(const std::string& name);
		static void RemoveAll();

	private:
		static void FbxToObject(const std::wstring& modelName, const std::shared_ptr<GameObject>& object, const std::shared_ptr<FbxLoader::Node>& meshNode, FbxLoader::FbxLoader& loader);
	};
}

#pragma warning(pop)