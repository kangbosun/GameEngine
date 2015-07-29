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
#include "DXUtil.h"
#include "Texture2D.h"

namespace GameEngine
{
	using namespace Math;
	using namespace std;

	unordered_map<wstring, std::shared_ptr<Shader>> Resource::shaders;
	unordered_map<wstring, std::shared_ptr<Texture2D>> Resource::Texture2Ds;
	unordered_map<wstring, std::shared_ptr<Font>> Resource::fonts;
	unordered_map<wstring, std::shared_ptr<GameObject>> Resource::models;
	unordered_map<string, std::shared_ptr<Material>> Resource::materials;

	void Resource::LoadDefaultResource()
	{
		auto s = make_shared<Shader>();
		s->InitShader(L"resources\\shaders\\Tex.fx");
		Resource::AddShader(L"Tex", s);

		auto s2 = make_shared<Shader>();
		s2->InitShader(L"resources\\shaders\\Standard.fx");
		Resource::AddShader(L"Standard", s2);

		AddTexture2D(L"white", L"resources\\textures\\white.png");
		Resource::AddTexture2D(L"skymap", L"resources\\skybox\\Above_The_Sea.dds");

		AddFont(L"NanumGothic", L"resources\\Fonts", L"NanumGothic.ttf");

		auto defaultMaterial = make_shared<Material>();
		defaultMaterial->shader = Resource::GetShader(L"Standard");
		Resource::AddMaterial("default", (defaultMaterial));

		Resource::AddModel(L"sphere", "resources\\models\\primitive", "sphere.fbx");
		Resource::AddModel(L"plane", "resources\\models\\primitive", "plane.fbx");
	}

	std::shared_ptr<Font> Resource::GetFont(const wstring& fontname)
	{
		//Debug(L"#GetFont(" + fontname + L")", false);
		auto val = fonts[fontname];
		if(!val.get())
			Debug("	-Failed");
		//Debug("	-Success");
		return val;
	}

	void Resource::AddShader(const wstring& name, const shared_ptr<Shader>& shader)
	{
		shaders[name] = shader;
	}

	void Resource::AddFont(const wstring& name, const wstring& folder, const wstring& filename)
	{
		wstring path = folder + L"\\" + filename;
		Debug(L"#AddFont(" + path + L")", true);
		auto dx = GraphicDevice::Instance();
		string _path = string(path.begin(), path.end());
		auto& font = Font::LoadFont(string(name.begin(), name.end()), _path);

		fonts[name] = font;
	}

	std::shared_ptr<Texture2D>& Resource::AddTexture2D(const wstring& name, const wstring& path, D3DX11_IMAGE_LOAD_INFO* loadInfo)
	{
		if(Texture2Ds[name].get())
			return Texture2Ds[name];

		Debug(L"#AddTexture2D(" + path + L")", false);
		auto temp = make_shared<Texture2D>();
		temp->LoadFromFile(path);
		if(!temp->IsValid())
			Debug("	-Failed");

		else {
			Debug("	-Success");
			temp->name = string(name.begin(), name.end());
			Texture2Ds[name] = temp;
		}
		return Texture2Ds[name];
	}

	std::shared_ptr<Shader> Resource::GetShader(const wstring& shaderName)
	{
		auto val = shaders[shaderName];
		return val;
	}

	std::shared_ptr<Texture2D> Resource::GetTexture2D(const wstring& Texture2DName)
	{
		//Debug(L"#GetTexture2D(" + Texture2DName + L")", false);
		auto val = Texture2Ds[Texture2DName];
		//if(!val.get())
		//	Debug("	-Failed");
		//else
		//	Debug("	-Success");
		return val;
	}

	void Resource::AddModel(const wstring& name, const string& folder, const string& filename)
	{
		Debug("#AddModel(" + filename + ")", false);
		std::shared_ptr<GameObject> obj = GameObject::Instantiate(string(name.begin(), name.end()));
		FbxLoader::FbxLoader loader(FbxLoader::FbxLoader::eDirectX);

		loader.LoadFromFile(folder, filename);

		auto root = loader.rootNode;
		if(root->vertIndices.size() == 0 && root->mChildNodes.size() == 0) {
			Debug("	-Failed");
		}
		if(root->vertIndices.size() == 0 && root->mChildNodes.size() == 1)
			root = root->mChildNodes[0];

		ProcessMaterial(loader, name);

		std::shared_ptr<GameObject> empty;

		FbxToObject(name, obj, root, loader);

		obj->name = string(name.begin(), name.end());
		models[name] = obj;
		Debug(" -Success");
	}

	void Resource::FbxToObject(const wstring& modelName, const std::shared_ptr<GameObject>& object, const std::shared_ptr<FbxLoader::Node>& meshNode, FbxLoader::FbxLoader& loader)
	{
		auto device = GraphicDevice::Instance()->device;

		const size_t materialCount = meshNode->mMaterialNames.size();

		//skinned mesh
		const size_t vertCount = meshNode->meshes.size();
		if(vertCount > 0) {
			auto& renderer = object->AddComponent<MeshRenderer>();
			Vertex* verts = &meshNode->meshes[0];
			unsigned long* indices = &meshNode->vertIndices[0];
			int nIndex = (int)meshNode->vertIndices.size();
			int nSub = (int)meshNode->vertexCountOfSubMesh.size();
			int* vertOfSub = &meshNode->vertexCountOfSubMesh[0];
			auto mesh = make_shared<Mesh>();
			mesh->Initialize(verts, indices, (int)vertCount, nIndex, vertOfSub, nSub, device);
			renderer->mesh = mesh;

			// bones
			const int nBones = (int)meshNode->bones.size();
			if(meshNode->useSkinnedMesh && nBones > 0) {
				renderer->rootBoneName = meshNode->bones[0]->name;
				mesh->bindPoseInv.reserve(nBones);
				for(int i = 0; i < nBones; ++i)
					mesh->bindPoseInv.push_back(meshNode->bones[i]->bindPoseInverse);
			}
			// animations
			if(loader.animationClips.size() > 0) {
				auto& aniCom = object->AddComponent<Animation>();
				aniCom->clips = loader.animationClips;
			}
		}

		// materials
		if(materialCount > 0) {
			auto& renderer = object->renderer();
			renderer->materials.reserve(materialCount);
			for(int j = 0; j < materialCount; j++) {
				std::shared_ptr<Texture2D> diffuse;
				string materialName = meshNode->mMaterialNames[j];
				auto material = materials[materialName];

				if(material) renderer->materials.push_back(material);
			}
		}

		// initial transform
		object->transform()->SetLocalTransform(meshNode->mMatrix.m[0]);

		// children
		for(auto& childNode : meshNode->mChildNodes) {
			auto child = GameObject::Instantiate(childNode->name);
			FbxToObject(modelName, child, childNode, loader);
			child->parent = object;
			object->AddChild(child);
		}
	}

	void Resource::ProcessMaterial(FbxLoader::FbxLoader& loader, wstring modelname)
	{
		const int materialCount = (int)loader.materials.size();
		auto iter = loader.materials.begin();
		for(int i = 0; i < materialCount; i++, iter++) {
			auto matname = iter->first;
			auto mat = iter->second;
			int n = 0;

			std::shared_ptr<Texture2D> diffuse;
			std::shared_ptr<Texture2D> normalMap;
			if(mat->mDiffuseMapName != "") {
				string str = mat->mDiffuseMapName;
				str = FbxPathUtils::GetFileName(str.c_str());
				str = loader.relativeFolder + "\\" + str;
				wstring wstr(str.begin(), str.end());
				diffuse = (Resource::AddTexture2D(wstr, wstr));
			}
			if(mat->mNormalMapName != "") {
				string str = mat->mNormalMapName;
				str = FbxPathUtils::GetFileName(str.c_str());
				str = loader.relativeFolder + "\\" + str;
				wstring wstr(str.begin(), str.end());
				normalMap = (Resource::AddTexture2D(wstr, wstr));
			}

			if(mat->type == FbxLoader::FMaterial::MT_INVALID) {
				Debug(L"FbxLoader::ToObject() => No Materials!(" + modelname + L")");
				auto lam = Resource::GetMaterial("default")->Clone();
				materials[matname] = lam;
				return;
			}

			auto tempMat = make_shared<Material>();
			tempMat->diffuseMap = diffuse;
			tempMat->normalMap = normalMap;
			tempMat->name = mat->mName;
			//tempMat->data.Ambient = mat->mAmbient;
			//tempMat->data.Diffuse = mat->mDiffuse;
			//tempMat->data.Specular = mat->mSpecular;
			//tempMat->data.SpecularFactor = mat->mSpecularPower;
			//tempMat->data.Emissive = mat->mEmissive;
			//tempMat->data.Shininess = mat->mShininess;
			//tempMat->data.Reflection = mat->mReflection;
			//tempMat->data.ReflectionFactor = mat->mReflectionFactor;
			//tempMat->data.TransparencyFactor = mat->mTransperencyFactor;
			if(mat->type == FbxLoader::FMaterial::MT_PHONG) {
				tempMat->shader = Resource::GetShader(L"Standard");
			}
			else if(mat->type == FbxLoader::FMaterial::MT_LAMBERT) {
				tempMat->shader = Resource::GetShader(L"Standard");
			}
			materials[matname] = tempMat;
		}
	}

	std::shared_ptr<GameObject> Resource::GetModel(const wstring& modelName)
	{
		//Debug(L"#GetModel(" + modelName + L")", false);
		auto val = models[modelName];
		if(!val.get()) {
			//Debug("	-Failed");
			return val;
		}
		//Debug(" -Success");
		return static_pointer_cast<GameObject, Object>(val->Clone());
	}

	void Resource::RemoveAll()
	{
		shaders.clear();
		fonts.clear();
		musics.clear();
		soundEffects.clear();
		Texture2Ds.clear();
		models.clear();
	}

	unordered_map<wstring, std::shared_ptr<AudioMusic>> Resource::musics;
	unordered_map<wstring, std::shared_ptr<AudioSFX>> Resource::soundEffects;

	void Resource::AddMusic(const wstring& name, const wstring& filename)
	{
		if(musics[name])
			return;

		Debug(L"#AddMusic(" + filename + L")", false);
		auto clip = AudioMusic::CreateAudioClipFromFile(filename);
		if(clip) {
			musics[name] = clip;
			Debug("	-Success");
		}
		else {
			Debug("	-Failed");
		}
	}

	void Resource::AddSFX(const wstring& name, const wstring& filename)
	{
		if(soundEffects[name])
			return;

		Debug(L"#AddSFX(" + filename + L")", false);
		auto clip = AudioSFX::CreateAudioClipFromFile(filename);
		if(clip) {
			soundEffects[name] = clip;
			Debug("	-Success");
		}
		else {
			Debug("	-Failed");
		}
	}

	std::shared_ptr<AudioMusic> Resource::GetMusic(const wstring& musicName)
	{
		//Debug(L"#GetMusic(" + musicName + L")", false);
		auto ret = musics[musicName];
		if(ret) {
			//Debug("	-Success");
		}
		//else
		//Debug("	-Failed");
		return ret;
	}
	std::shared_ptr<AudioSFX> Resource::GetSFX(const wstring& sfxName)
	{
		//Debug(L"#GetSFX(" + sfxName + L")", false);
		auto ret = soundEffects[sfxName];
		if(ret) {
			//Debug("	-Success");
		}
		//else
		//Debug("	-Failed");
		return ret->Clone();
	}

	bool Resource::RemoveShader(const wstring& name)
	{
		auto c = shaders[name];
		if(!c)
			return false;
		if(c.use_count() > 2)
			return false;
		shaders[name] = nullptr;
		return true;
	}
	bool Resource::RemoveFont(const wstring& name)
	{
		auto c = fonts[name];
		if(!c)
			return false;
		if(c.use_count() > 2)
			return false;
		shaders[name] = nullptr;
		return true;
	}
	bool Resource::RemoveTexture2D(const wstring& name)
	{
		auto c = Texture2Ds[name];
		if(!c)
			return false;
		if(c.use_count() > 2)
			return false;
		shaders[name] = nullptr;
		return true;
	}
	bool Resource::RemoveModel(const wstring& name)
	{
		auto c = models[name];
		if(!c)
			return false;
		if(c.use_count() > 2)
			return false;
		shaders[name] = nullptr;
		return true;
	}

	bool Resource::RemoveMusic(const wstring& name)
	{
		auto c = musics[name];
		if(!c)
			return false;
		if(c.use_count() > 2)
			return false;
		shaders[name] = nullptr;
		return true;
	}

	bool Resource::RemoveSFX(const wstring& name)
	{
		auto c = soundEffects[name];
		if(!c)
			return false;
		if(c.use_count() > 2)
			return false;
		shaders[name] = nullptr;
		return true;
	}

	void Resource::AddMaterial(const string& name, std::shared_ptr<Material>& material)
	{
		if(material.get()) {
			materials[name] = material;
		}
	}

	std::shared_ptr<Material> Resource::GetMaterial(const string& name)
	{
		return materials[name];
	}

	bool Resource::RemoveMaterial(const string& name)
	{
		auto mat = materials[name];
		if(mat.get()) {
			materials[name] = nullptr;
			return true;
		}
		else
			return false;
	}
}