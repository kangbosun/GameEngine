
#pragma once


#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class Texture2D;
	class Shader;
	class Material;

	//struct GAMEENGINE_API MaterialData
	//{
	//	Math::Color Ambient = { 1, 1, 1, 1 };
	//	Math::Color Diffuse = { 1, 1, 1, 1 };
	//	Math::Color Emissive = { 0, 0, 0, 0 };
	//	Math::Color Specular = { 1, 1, 1, 1 };
	//	Math::Color Reflection = { 0, 0, 0, 0 };
	//	float SpecularFactor = 1.0f;
	//	float Shininess = 20.0f;
	//	float ReflectionFactor = 1.0f;
	//	float TransparencyFactor = 1.0f;
	//};

	struct GAMEENGINE_API MaterialData
	{
		Math::Color color = Math::Color::White;
		float metalic = 0.5f;
		float roughness = 0.3f;
	};

	class GAMEENGINE_API Material final
	{
	public:		
		std::string name;
		MaterialData data;

		std::shared_ptr<Texture2D> diffuseMap;
		std::shared_ptr<Texture2D> specularMap;
		std::shared_ptr<Texture2D> normalMap;

		std::shared_ptr<Shader> shader;

	public :
		std::shared_ptr<Material> Clone();
	};
}

#pragma warning(pop)