
#include "enginepch.h"
#include "Renderer.h"
#include "Resource.h"
#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "GraphicDevice.h"
#include "Transform.h"
#include "GlobalSetting.h"
#include "Camera.h"
#include "Texture2D.h"

namespace GameEngine
{
	using namespace Math;
	using namespace std;
	// MeshRenderer

	list<weak_ptr<MeshRenderer>> MeshRenderer::allMeshRenderers;


	MeshRenderer::MeshRenderer()
	{
		type = Renderer::eMesh;
	}

	void MeshRenderer::Start()
	{
		if(rootBoneName != "") {
			auto node = gameObject;
			while(node->parent)
				node = node->parent;
			auto& rootBone = node->FindGameObject(rootBoneName);
			SetBone(rootBone);
		}
		allMeshRenderers.push_back(gameObject->GetComponent<MeshRenderer>());
	}

	

	void MeshRenderer::SetBone(const std::shared_ptr<GameObject>& rootBone)
	{
		if(rootBone) {
			bones.push_back(rootBone->transform());
			auto& children = rootBone->children;
			for(auto& child : children)
				SetBone(child);
		}
	}

	void MeshRenderer::Update()
	{
		int boneSize = (int)bones.size();
		int bindposeInvSize = (int)mesh->bindPoseInv.size();
		if(boneSize > 0 && boneSize == bindposeInvSize) {
			matrices.clear();
			matrices.reserve(bones.size());
			Matrix offsetInv;
			if(bones[0]->gameObject->parent)
				bones[0]->gameObject->parent->transform()->WorldMatrix().Inverse(offsetInv);
			else offsetInv = Matrix::Identity;

			for(int bi = 0; bi < boneSize; ++bi) {
				Matrix& bindposeInv = mesh->bindPoseInv[bi];
				Matrix& world = bones[bi]->WorldMatrix();
				Matrix& final = bindposeInv * world * offsetInv;
				matrices.emplace_back(final);
			}
		}
	}


	void MeshRenderer::Render(const CameraData& cam, const shared_ptr<Shader>& forcedShader)
	{
		auto dx = GraphicDevice::Instance();
		auto& lights = Light::allLights;
		string name = gameObject->name;
		if(!dx)
			return;
		if(!mesh || !mesh->IsValid()) return;

		bool renderShadow = false;
		bool useSkinning = false;

		int offset = 0;
		mesh->Bind(dx->context);

		for(int i = 0; i < materials.size(); ++i) {
			auto& material = materials[i];
			if(!material) continue;
			
			auto& shader = forcedShader ? forcedShader : material->shader;
			if(!shader) continue;
	
			shader->SetGlobalSetting(&GlobalSetting::graphicSetting);
			shader->SetCamera(&cam);
			shader->SetWorldMatrix(&transform()->WorldMatrix());
			
			if(matrices.size() > 0)
				shader->SetBoneMatrices(&(*matrices.begin()), matrices.size());
			else
				shader->SetBoneMatrices(0, 0);

			auto& light = lights.begin()->lock();
			shader->SetLight(&light->GetLightData());
			shader->SetMaterial(&material->data);
			shader->SetDiffuseMap(material->diffuseMap.get());
			shader->SetNormalMap(material->normalMap.get());

			if(light->renderShadow && receiveShadow)
				shader->SetShadowMap(light->GetShadowMap()->GetTexture().get(), &light->GetShadowTransform());
			else
				shader->SetShadowMap(0, 0);
		
			shader->Render(dx->context, mesh->vertCountOfSubMesh[i], offset);
			offset += mesh->vertCountOfSubMesh[i];
		}
	}


	void MeshRenderer::RenderAll(const CameraData& cam)
	{
		for(auto iter = allMeshRenderers.begin(); iter != allMeshRenderers.end();) {
			if(iter->expired())
				iter = allMeshRenderers.erase(iter);
			else {
				iter->lock()->Render(cam, nullptr);
				iter++;
			}
		}
	}

	void MeshRenderer::RenderAllToShadowMap(const CameraData& cam, const std::shared_ptr<Shader>& shadowShader)
	{
		for(auto iter = allMeshRenderers.begin(); iter != allMeshRenderers.end();) {
			if(iter->expired())
				iter = allMeshRenderers.erase(iter);
			else {
				auto _renderer = iter->lock();
				if(_renderer->castShadow) {
					iter->lock()->Render(cam, shadowShader);
				}
				iter++;
			}
		}
	}


	//////////////////////////////////////////////////////////



	std::list<std::weak_ptr<UIRenderer>> UIRenderer::allUIRenderers;

	UIRenderer::UIRenderer()
	{
		material.shader = Resource::GetShader(L"Standard");
		material.data.color = Color::White;
		type = RendererType::eUI;
	}

	void UIRenderer::Start()
	{
		allUIRenderers.push_back(gameObject->GetComponent<UIRenderer>());
	}

	void UIRenderer::Render(const CameraData& cam, const std::shared_ptr<Shader>& _shader)
	{
		string name = gameObject->name;
		auto dx = GraphicDevice::Instance();
		if(!dx)
			return;
		if(!mesh.get() || mesh->IsValid() == false)
			return;
		auto& uishader = _shader ? _shader : material.shader;
		if(!uishader)
			return;

		Vector3 s, t;
		Quaternion q;

		Matrix world = transform()->WorldMatrix();
		world.Decompose(t, q, s);

		float sx = GlobalSetting::UIScaleFactorX;
		float sy = GlobalSetting::UIScaleFactorY;
		t.x *= sx; t.y *= sy;
		s.x *= sx; s.y *= sy;
		Matrix::CreateTransform(world, t, q, s);

		uishader->SetBoneMatrices(0, 0);
		uishader->SetNormalMap(0);
		uishader->SetLight(0);
		uishader->SetShadowMap(0, 0);
		uishader->SetGlobalSetting(0);

		uishader->SetWorldMatrix(&world);
		uishader->SetMaterial(&material.data);
		uishader->SetCamera(&cam);
		uishader->SetDiffuseMap(material.diffuseMap.get());

		//GraphicDevice::Instance()->SetAlphaBlend(useAlphaBlending);
		mesh->Bind(dx->context);
		uishader->Render(dx->context, mesh->nIndices());
	}

	void UIRenderer::RenderAll(const CameraData& cam, const std::shared_ptr<Shader>& shader)
	{
		for(auto iter = allUIRenderers.begin(); iter != allUIRenderers.end();) {
			if(iter->expired())
				iter = allUIRenderers.erase(iter);
			else {
				auto _renderer = iter->lock();
				//int d = _renderer->transform()->worldPosition->z;
				auto& name = _renderer->gameObject->name;
				iter->lock()->Render(cam, shader);
				iter++;
			}
		}
		allUIRenderers.sort();
	}

	bool operator <(const std::weak_ptr<UIRenderer>& lhs, const std::weak_ptr<UIRenderer>& rhs)
	{
		float a = lhs.lock()->transform()->worldPosition().z;
		float b = rhs.lock()->transform()->worldPosition().z;

		return a > b;
	}

	///////////////////////////////////////

}