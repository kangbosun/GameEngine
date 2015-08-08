#pragma once

#include "Singleton.h"
#include "Camera.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class RenderTarget;
	class DepthStencil;
	class GraphicDevice;

	class GAMEENGINE_API ShadowMap
	{
	private:
		int size = 0;
		std::shared_ptr<RenderTarget> renderTarget;
		std::shared_ptr<DepthStencil> depthStencil;
		Viewport viewport;
	public:
		const std::shared_ptr<RenderTarget>& GetRenderTarget() const { return renderTarget; }
		void Bind(std::shared_ptr<GraphicDevice>& graphicDevice);
		void Resize(int mapSize);
		void Initialize(int mapSize);
		const CComPtr<ID3D11ShaderResourceView>& GetSRV() const;
		const std::shared_ptr<Texture2D> GetTexture();
	};

	class Camera;
	class Shader;

	struct GAMEENGINE_API LightData
	{
		Color color;
		Vector3 dir;
		int lightType;
		Vector3 position;
	};

	class GAMEENGINE_API Light final : public Cloneable<Light, Component>
	{
		enum LightType
		{
			eDirectional = 0,
			ePoint,
			eSpot,
		};
	private:
		LightType type;
		LightData data;

		// for shadow map
		float fov = 60.0f;
		float nearplane = 0.1f;
		float farplane = 100.0f;

		std::shared_ptr<ShadowMap> shadowMap;
		int shadowMapSize;
		std::shared_ptr<Shader> shadowShader;
		Matrix shadowTransform;

		void BuildShadowTransform();
		void BuildShadowMap();

	public:
		CameraData cameraData;
		Color lightColor = Color::White;

		bool renderShadow = true;

		Light();
		void Start();
		LightType GetType() { return type; }
		LightData& GetLightData();

		std::shared_ptr<ShadowMap>& GetShadowMap() { return shadowMap; }
		const Matrix& GetShadowTransform() { return shadowTransform; }

		static std::shared_ptr<Light> CreateDirectionalLight();
		static std::shared_ptr<Light> CreatePointLight();
		static std::shared_ptr<Light> CreateSpotLight();

		static std::list<std::weak_ptr<Light>> allLights;
		static void BuildShadowMaps();
	};
}

#pragma warning(pop)