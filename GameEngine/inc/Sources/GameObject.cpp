
#include "enginepch.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "GraphicDevice.h"


namespace GameEngine
{
	using namespace std;

	std::list<std::shared_ptr<GameObject>> GameObject::allGameObjects;

	GameObject::GameObject(const GameObject& gameObject)
	{
		active = gameObject.active;
		enableCollisionTest = gameObject.enableCollisionTest;
	}


	shared_ptr<Transform> GameObject::transform()
	{
		if(_transform.expired())
			_transform = GetComponent<Transform>();
		return _transform.lock();
	}

	shared_ptr<Renderer> GameObject::renderer()
	{
		if(_renderer.expired())
			_renderer = GetComponent<Renderer>();
		return _renderer.lock();
	}

	void GameObject::Register(const shared_ptr<GameObject>& gameObject)
	{
		gameObject->isRegistered = true;
		allGameObjects.push_back(gameObject);
		for(auto& com : gameObject->components) {
			Component::Register(com.second);
		}
		for(auto& child : gameObject->children)
			Register(child);		
	}

	void GameObject::Destroy(std::shared_ptr<GameObject>& object)
	{
		if(object) {
			for(auto& child : object->children)
				Destroy(child);

			allGameObjects.remove(object);
			for(auto& com : object->components)
				com.second->enabled = false;
			object->components.clear();
			object.reset();
		}
	}

	std::shared_ptr<GameObject> GameObject::Instantiate(const string& name)
	{
		std::shared_ptr<GameObject> go(new GameObject());
		go->name = name;
		go->AddComponent<Transform>();
		return go;
	}

	std::shared_ptr<GameObject> GameObject::FindGameObject(const string& name)
	{
		if(this->name == name)
			return shared_from_this();
		std::shared_ptr<GameObject> target;
		for(auto& child : children) {
			if(child->name == name)
				target = child;
			else 
				target = child->FindGameObject(name);
			if(target)
				return target;
		}
		return target;
	}

	std::shared_ptr<GameObject> GameObject::CopyEmpty()
	{
		std::shared_ptr<GameObject> node = Instantiate(name);

		for(auto& child : children) {
			node->AddChild(child->CopyEmpty());
		}
		return node;
	}

	void GameObject::CopyComponents(shared_ptr<GameObject>& src, shared_ptr<GameObject>& dst)
	{			
		dst->components.clear();

		for(auto& com : this->components) {
			auto& c = static_pointer_cast<Component>(com.second->Clone());
			dst->AddComponent(c);
		}

		auto& srcIter = src->children.begin();
		auto& dstIter = dst->children.begin();
		for(; srcIter != src->children.cend();) {
			(*srcIter)->CopyComponents(*srcIter, *dstIter);
			++srcIter; ++dstIter;
		}
	}

	std::shared_ptr<Object> GameObject::Clone()
	{
		auto node = CopyEmpty();
		CopyComponents(shared_from_this(), node);
		return node;
	}

	std::shared_ptr<Component> GameObject::GetComponent(const string& name)
	{
		std::shared_ptr<Component> ret;
		auto& pair = components.find(name);
		if(pair != components.cend())
			ret = pair->second;
		return ret;
	}

	void GameObject::UpdateComponents()
	{
		for(auto& pair : components) {
			auto& com = pair.second;
			if(com->enabled)
				com->Update();
		}
	}

	void GameObject::Update()
	{
		UpdateComponents();
	}

	void GameObject::AddChild(const std::shared_ptr<GameObject>& child)
	{
		if(child) {
			if(child->parent)
				child->parent->RemoveChild(child);
			children.push_back(child);	
			child->parent = shared_from_this();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////


	//DXSprite2D::DXSprite2D(const string& name) : GameObject(name)
	//{
	//	IsBillBoard = false;
	//	Size = { 0, 0 };
	//}

	//void DXSprite2D::Initialize(const XMFLOAT3& pos, const XMFLOAT2& size, const std::shared_ptr<Texture2D>& Texture2D, const XMFLOAT2& srcBeginPos, const XMFLOAT2& srcEndPos)
	//{
	//	transform()->position = pos;
	//	Size = size;

	//	Vertex vertices[4] = {
	//		{ XMFLOAT3(-size.x / 2, size.y / 2, 0.0f), XMFLOAT2(srcBeginPos.x, srcBeginPos.y) },
	//		{ XMFLOAT3(size.x / 2, size.y / 2, 0.0f), XMFLOAT2(srcEndPos.x, srcBeginPos.y) },
	//		{ XMFLOAT3(size.x / 2, -size.y / 2, 0.0f), XMFLOAT2(srcEndPos.x, srcEndPos.y) },
	//		{ XMFLOAT3(-size.x / 2, -size.y / 2, 0.0f), XMFLOAT2(srcBeginPos.x, srcEndPos.y) }

	//	};
	//	unsigned long indices[] = {
	//		0, 1, 2, 2, 3, 0
	//	};

	//	auto mesh = make_shared<Mesh>();
	//	mesh->Initialize(vertices, indices, 4, 6, dx->device);
	//	renderer()->Mesh.push_back(mesh);
	//	auto mat = Resource
	//	mat->DiffuseMap = Texture2D;
	//	renderer()->Materials.push_back(mat);
	//}
}

namespace std
{
	bool operator==(const weak_ptr<GameEngine::GameObject>& lhs, const weak_ptr<GameEngine::GameObject>& rhs)
	{
		if(lhs.lock() && rhs.lock())
			return lhs.lock() == rhs.lock();
		else
			return false;
	}
}