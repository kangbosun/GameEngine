#include "enginepch.h"
#include "Resource.h"
#include "Shader.h"
#include "Font.h"
#include "GraphicDevice.h"
#include "Audio.h"
#include "GameObject.h"
#include "Material.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Transform.h"
#include "Animation.h"
#include "FbxLoader.h"
#include "Debug.h"
#include "Texture2D.h"

namespace GameEngine
{
	using namespace std;

	resource_container_shared<Texture2D>		Resource::textures;
	resource_container_shared<Shader>			Resource::shaders;
	resource_container_shared<Font>				Resource::fonts;
	resource_container<GameObject>				Resource::models;
	resource_container_shared<AudioMusic>		Resource::audios;
	resource_container_shared<Material>			Resource::materials;
	resource_container_shared<AnimClip>			Resource::animClips;

	void Resource::LoadDefaultResource()
	{
		auto s = make_shared<Shader>();
		s->InitShader(L"resources\\shaders\\Texture.fx");
		Resource::shaders.Add("Texture", s);

		auto s2 = make_shared<Shader>();
		s2->InitShader(L"resources\\shaders\\Standard.fx");
		Resource::shaders.Add("Standard", s2);

		auto white = make_shared<Texture2D>();
		white->LoadFromFile(L"resources\\textures\\white.png");
		Resource::textures.Add("white", white);

		auto nanumGothic = Font::LoadFont("NanumGothic", "resources\\Fonts\\NanumGothic.ttf");
		Resource::fonts.Add("NanumGothic", nanumGothic);
		
		auto defaultMaterial = make_shared<Material>();
		defaultMaterial->shader = Resource::shaders.Find("Standard");
		Resource::materials.Add("default", defaultMaterial);

		Resource::LoadFromFbx("sphere", "resources\\models\\primitive", "sphere.fbx");
		Resource::LoadFromFbx("plane", "resources\\models\\primitive", "plane.fbx");
	}

	GameObject* Resource::LoadFromFbx(const std::string& name, const std::string & folder, const std::string & filename)
	{
		FbxLoader::FbxLoader loader = { FbxLoader::FbxLoader::eDirectX };
		loader.LoadFromFile(folder, filename);

		auto* pRoot = &loader.rootNode;
		//check meshdata 
		Debug::Log("#Load : " + folder + "\\" + filename);
		if(pRoot->vertIndices.size() == 0 && pRoot->childNodes.size() == 0) {
			Debug::Log("Fail : No Mesh Datas", Debug::Red);
			return nullptr;
		}
		//check meaningless root node
		if(pRoot->vertIndices.size() == 0 && pRoot->childNodes.size() == 1)
			pRoot = &pRoot->childNodes[0];

		ProcessMaterial(loader, name);

		GameObject* obj = GameObject::Instantiate(name);
		FbxToObject(name, obj, *pRoot, loader);

		models.Add(name, obj);
		Debug::Log("Success", Debug::Green);
		return obj->Clone();
	}


	void Resource::FbxToObject(const string& modelName, GameObject* object, FbxLoader::Node& meshNode, FbxLoader::FbxLoader& loader)
	{
		auto device = GraphicDevice::Instance()->device;

		const size_t materialCount = meshNode.materialNames.size();

		const size_t vertCount = meshNode.meshes.size();
		if(vertCount > 0) {
			auto& renderer = object->AddComponent<MeshRenderer>();
			auto mesh = make_shared<Mesh>();
			mesh->Initialize(meshNode.meshes, meshNode.vertIndices, &meshNode.vertexCountOfSubMesh);
			renderer->mesh = mesh;

			// bones
			const int nBones = (int)meshNode.bones.size();
			if(meshNode.useSkinnedMesh && nBones > 0) {
				renderer->rootBoneName = meshNode.bones[0].name;
				mesh->bindPoseInverse.reserve(nBones);
				for(int i = 0; i < nBones; ++i)
					mesh->bindPoseInverse.push_back(meshNode.bones[i].bindPoseInverse);
			}
			// animations
			if(loader.animationClips.size() > 0) {
				auto& aniCom = object->AddComponent<Animation>();
				aniCom->clips.swap(loader.animationClips);
				for(auto& clip : aniCom->clips)
					Resource::animClips.Add(clip.first, clip.second);
			}
		}

		// materials
		auto& renderer = object->renderer();
		renderer->materials.reserve(materialCount);
		for(int j = 0; j < materialCount; j++) {
			std::shared_ptr<Texture2D> diffuse;
			string materialName = meshNode.materialNames[j];
			auto material = materials.Find(materialName);
			if(!material)
				material = materials.Find("default");

			if(material) renderer->materials.push_back(material);
		}

		// initial transform
		object->transform.SetLocalTransform(meshNode.matrix.m[0]);

		// children
		for(auto& childNode : meshNode.childNodes) {
			auto child = GameObject::Instantiate(childNode.name);
			FbxToObject(modelName, child, childNode, loader);
			child->transform.SetParent(&object->transform);
		}
	}

	shared_ptr<Texture2D> Resource::ProcessTexture(const std::string& name, FbxLoader::FbxLoader& loader)
	{
		shared_ptr<Texture2D> ret;
		if(name != "") {
			string str = name;
			str = FbxPathUtils::GetFileName(str.c_str());
			str = loader.relativeFolder + "\\" + str;
			wstring wstr(str.begin(), str.end());
			ret = make_shared<Texture2D>();
			ret->LoadFromFile(wstr);
			Resource::textures.Add(str, ret);
		}
		return ret;
	}

	void Resource::ProcessMaterial(FbxLoader::FbxLoader& loader, string modelname)
	{
		const int materialCount = (int)loader.materials.size();
		auto iter = loader.materials.begin();
		for(int i = 0; i < materialCount; i++, iter++) {
			auto matname = iter->first;
			auto& mat = iter->second;

			auto diffuseMap = ProcessTexture(mat.diffuseMapName, loader);
			auto normalMap = ProcessTexture(mat.normalMapName, loader);
			auto specularMap = ProcessTexture(mat.specularMapName, loader);

			auto tempMat = make_shared<Material>();
			tempMat->diffuseMap = diffuseMap;
			tempMat->normalMap = normalMap;
			tempMat->name = mat.name;
			tempMat->shader = Resource::shaders.Find("Standard");

			Resource::materials.Add(tempMat->name, tempMat);
		}
	}
}