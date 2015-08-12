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
			auto root = GetTransform();
			while(root->GetParent())
				root = root->GetParent();
			auto rootBone = root->GetGameObject()->FindGameObjectInChildren(rootBoneName);
			SetBone(&rootBone->transform);
		}
		allMeshRenderers.push_back(gameObject->GetComponent<MeshRenderer>());
	}

	void MeshRenderer::SetBone(Transform* bone)
	{
		if(bone) {
			bones.push_back(bone);
			auto size = bone->GetChildCount();
			for(int i = 0; i < size; ++i)
				SetBone(bone->GetChild(i));
		}
	}

	void MeshRenderer::Update()
	{
		int boneSize = (int)bones.size();
		int bindposeInvSize = (int)mesh->bindPoseInverse.size();
		if(boneSize > 0 && boneSize == bindposeInvSize) {
			matrices.clear();
			matrices.reserve(bones.size());
			Matrix offsetInv;
			if(bones[0]->GetParent())
				bones[0]->GetParent()->WorldMatrix().Inverse(offsetInv);
			else offsetInv = Matrix::Identity;

			for(int bi = 0; bi < boneSize; ++bi) {
				Matrix& bindposeInv = mesh->bindPoseInverse[bi];
				Matrix& world = bones[bi]->WorldMatrix();
				Matrix& final = bindposeInv * world * offsetInv;
				matrices.emplace_back(final);
			}
		}
	}

	void MeshRenderer::Render(const CameraData& cam, const shared_ptr<Shader>& forcedShader)
	{
		auto graphicDevice = GraphicDevice::Instance();
		if(!graphicDevice)
			return;	
		if(!mesh || !mesh->IsValid()) return;

		bool renderShadow = false;
		bool useSkinning = false;

		auto& lights = Light::allLights;
		string name = gameObject->name;

		int offset = 0;
		graphicDevice->SetMeshBuffer(mesh);

		for(int i = 0; i < materials.size(); ++i) {
			auto& material = materials[i];
			if(!material) continue;

			auto& shader = forcedShader ? forcedShader : material->shader;
			if(!shader) continue;

			shader->SetGlobalSetting(&GlobalSetting::graphicSetting);
			shader->SetCamera(&cam);
			shader->SetWorldMatrix(&GetTransform()->WorldMatrix());

			if(matrices.size() > 0)
				shader->SetBoneMatrices(&(*matrices.begin()), (uint32_t)matrices.size());
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

			UINT subMeshCount = mesh->GetSubMeshCount(i);
			shader->Render(graphicDevice->context, subMeshCount, offset);
			offset += subMeshCount;
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
		material.shader = Resource::shaders.Find("Texture");
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
		auto graphicDevice = GraphicDevice::Instance();
		
		if(!graphicDevice)	return;
		
		if(!mesh.get() || mesh->IsValid() == false)
			return;
		auto& uishader = _shader ? _shader : material.shader;
		
		if(!uishader) return;

		Vector3 s, t;
		Quaternion q;

		Matrix world = GetTransform()->WorldMatrix();
		world.Decompose(t, q, s);

		float sx = GlobalSetting::UIScaleFactorX;
		float sy = GlobalSetting::UIScaleFactorY;
		t.x *= sx; t.y *= sy;
		s.x *= sx; s.y *= sy;
		Matrix::CreateTransform(world, t, q, s);
	
		uishader->SetWorldMatrix(&world);
		uishader->SetMaterial(&material.data);
		uishader->SetCamera(&cam);
		uishader->SetDiffuseMap(material.diffuseMap.get());

		//GraphicDevice::Instance()->SetAlphaBlend(useAlphaBlending);
		graphicDevice->SetMeshBuffer(mesh);
		uishader->Render(graphicDevice->context, mesh->nIndices());
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
		float a = lhs.lock()->GetTransform()->worldPosition().z;
		float b = rhs.lock()->GetTransform()->worldPosition().z;

		return a > b;
	}

	///////////////////////////////////////
}