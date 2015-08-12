#include "enginepch.h"
#include "Light.h"
#include "Transform.h"
#include "GraphicDevice.h"
#include "Shader.h"
#include "Resource.h"
#include "GameObject.h"
#include "GlobalSetting.h"
#include "DebugClass.h"
#include "Renderer.h"
#include "Texture2D.h"

namespace GameEngine
{
	using namespace std;
	

	void ShadowMap::Bind(std::shared_ptr<GraphicDevice>& graphicDevice)
	{
		graphicDevice->SetRenderTarget(renderTarget, depthStencil);
		graphicDevice->SetViewport(viewport);
		graphicDevice->SetDepthEnable(true);
		graphicDevice->ClearRenderTarget(Color::White);
		graphicDevice->ClearDepthStencil();
	}

	void ShadowMap::Resize(int mapSize)
	{
		if(renderTarget)
			renderTarget->Initialize(mapSize, mapSize, R32G32_FLOAT);
		if(depthStencil)
			depthStencil->Initialize(mapSize, mapSize);
	}

	void ShadowMap::Initialize(int mapSize)
	{
		size = mapSize;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = (float)size;
		viewport.Height = (float)size;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		renderTarget = make_shared<RenderTarget>();
		depthStencil = make_shared<DepthStencil>();

		Resize(mapSize);
	}

	const CComPtr<ID3D11ShaderResourceView>& ShadowMap::GetSRV() const
	{
		return renderTarget->srv;
	}

	const std::shared_ptr<Texture2D> ShadowMap::GetTexture()
	{
		return std::static_pointer_cast<Texture2D>(renderTarget);
	}

	///////////////////////////////////////////////////////////

	list<weak_ptr<Light>> Light::allLights;

	Light::Light()
	{
		shadowMapSize = GlobalSetting::graphicSetting.shadowMapSize;
		shadowShader = Resource::shaders.Find("VSMBuild");
		shadowTransform = Matrix::Identity;
	}

	void Light::Start()
	{
		auto dx = GraphicDevice::Instance();
		shadowMap = make_shared<ShadowMap>();
		shadowMap->Initialize(shadowMapSize);
	}

	std::shared_ptr<Light> Light::CreateDirectionalLight()
	{
		auto go = GameObject::Instantiate("DirectionalLight");
		std::shared_ptr<Light> ptr = go->AddComponent<Light>();
		ptr->type = LightType::eDirectional;
		allLights.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<Light> Light::CreatePointLight()
	{
		auto go = GameObject::Instantiate("DirectionalLight");
		std::shared_ptr<Light> ptr = go->AddComponent<Light>();
		ptr->type = LightType::ePoint;
		return ptr;
	}

	std::shared_ptr<Light> Light::CreateSpotLight()
	{
		auto go = GameObject::Instantiate("DirectionalLight");
		std::shared_ptr<Light> ptr = go->AddComponent<Light>();
		ptr->type = LightType::eSpot;

		return ptr;
	}

	LightData& Light::GetLightData()
	{
		data.position = GetTransform()->GetPosition();
		data.dir = GetTransform()->forward();
		data.lightType = type;
		data.color = lightColor;
		return data;
	}

	void Light::BuildShadowMap()
	{
		auto graphicDevice = GraphicDevice::Instance();
		shadowMap->Bind(graphicDevice);
		MeshRenderer::RenderAllToShadowMap(cameraData, shadowShader);
		graphicDevice->SetBackBufferRenderTarget();
	}

	void Light::BuildShadowTransform()
	{
		cameraData.position = GetTransform()->GetPosition();
		Matrix::LookAtLH(cameraData.position, Vector3::Zero, GetTransform()->up(), cameraData.viewMatrix);
		Matrix::OrthographicOffCenterLH(-2, 2, -2, 2, nearplane, farplane, cameraData.projMatrix);
		//Matrix::PerspectiveFovLH(MathUtil::ToRadians(45), 1, nearplane, farplane, cameraData.projMatrix);

		Matrix t = {
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f };

		shadowTransform = cameraData.viewMatrix * cameraData.projMatrix * t;
	}

	void Light::BuildShadowMaps()
	{
		for(auto& i = allLights.begin(); i != allLights.end(); i++) {
			if(i->expired())
				allLights.erase(i);
			else if(i->lock()->renderShadow) {
				i->lock()->BuildShadowTransform();
				i->lock()->BuildShadowMap();
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
}