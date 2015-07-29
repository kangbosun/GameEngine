
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
	void Scene::Update()
	{
		auto& componentsMap = Component::allComponents;

		auto& mapIter = componentsMap.begin();
		while(mapIter != componentsMap.cend()) {
			auto& comList = mapIter->second;
			auto& comIter = comList.begin();
			while(comIter != comList.cend()) {
				auto& com = comIter->lock();
				if(!com) 
					comList.erase(comIter++);
				else {
					if(com->enabled) {
						com->Update();
						++comIter;
					}
				}
			}
			if(comList.size() == 0) 
				componentsMap.erase(mapIter++);
			else
				++mapIter;
		}
	}

	void Scene::Render()
	{
		Light::BuildShadowMaps();
		//meshRenderer
		MeshRenderer::RenderAll(Camera::main->GetCameraData());

		//ui Renderer
		GraphicDevice::Instance()->SetDepthEnable(false);
		GraphicDevice::Instance()->ClearDepthStencil();
		UIRenderer::RenderAll(Camera::ui->GetCameraData());
		GraphicDevice::Instance()->SetDepthEnable(true);
	}
}