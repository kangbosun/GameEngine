#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

#include "Material.h"

namespace GameEngine
{
	class Mesh;
	class Texture2D;
	class Shader;
	class GameObject;
	class Transform;
	struct CameraData;

	struct GAMEENGINE_API RenderOption
	{
		int useAlbedoMap = false;
		int useSpecularMap = false;
		int useNormalMap = false;
		int useShadowMap = false;
		int useSkinning = false;
	};

	class GAMEENGINE_API Renderer  : public Cloneable<Renderer, Component>
	{
	public:
		enum RendererType
		{
			eMesh = 0,
			eSkinnedMesh,
			eUI
		};
		RendererType type;
		bool useAlphaBlending = true;
		std::vector<std::shared_ptr<Material>> materials;
		virtual void Render(const CameraData& cam, const std::shared_ptr<Shader>& shader) {}
	};

	///////////////////////
	class GAMEENGINE_API MeshRenderer : public Cloneable<MeshRenderer, Renderer>
	{
	protected:
		static std::list<std::weak_ptr<MeshRenderer>> allMeshRenderers;
	public:
		std::shared_ptr<Mesh> mesh;
		bool castShadow = true;
		bool receiveShadow = true;

	public:
		//for skinning
		std::string rootBoneName = "";
		std::vector<Transform*> bones;

		std::vector<Matrix> matrices;

	protected:
		void SetBone(Transform* rootBone);
		virtual void Render(const CameraData& cam, const std::shared_ptr<Shader>& shader);
	public:
		virtual void Start();
		virtual void Update();
		MeshRenderer();
		static void RenderAll(const CameraData& cam);
		static void RenderAllToShadowMap(const CameraData& cam, const std::shared_ptr<Shader>& shadowShader);
	};

	///////////////////////////

	class UIRenderer;

	bool operator <(const std::weak_ptr<UIRenderer>& lhs, const std::weak_ptr<UIRenderer>& rhs);

	class GAMEENGINE_API UIRenderer : public Cloneable<UIRenderer, Renderer>
	{
	protected:
		static std::list<std::weak_ptr<UIRenderer>> allUIRenderers;
		virtual void Render(const CameraData& cam, const std::shared_ptr<Shader>& forcedShader);

	public:
		Material material;
		std::shared_ptr<Mesh> mesh;

		UIRenderer();

		virtual void Start();
		static void RenderAll(const CameraData& cam, const std::shared_ptr<Shader>& forcedShader = nullptr);
	};
}

#pragma warning(pop)