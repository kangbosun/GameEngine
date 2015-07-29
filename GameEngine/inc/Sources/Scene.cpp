#include "enginepch.h"
#include "Scene.h"
#include "GameTime.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Light.h"
#include "Camera.h"
#include "GraphicDevice.h"

namespace GameEngine
{
	void Scene::Register(GameObject * go)
	{
		if(go) {
			rootTransform.AddChild(&go->transform);
		}
	}
	void Scene::Update()
	{
		Component::UpdateAll();
	}

	void Scene::Render()
	{
		Light::BuildShadowMaps();
		//meshRenderer
		MeshRenderer::RenderAll(Camera::main->GetCameraData());

		//ui Renderer
		GraphicDevice::Instance()->SetDepthEnable(false);
		UIRenderer::RenderAll(Camera::ui->GetCameraData());
		GraphicDevice::Instance()->SetDepthEnable(true);
	}
}