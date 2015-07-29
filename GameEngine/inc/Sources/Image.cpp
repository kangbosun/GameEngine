#include "enginepch.h"
#include "Transform.h"
#include "Renderer.h"
#include "Mesh.h"
#include "GraphicDevice.h"
#include "Resource.h"
#include "UI.h"

namespace GameEngine
{
	void Image::SetSize(int _width, int _height)
	{
		transform()->SetSize(_width, _height);
		std::vector<Math::Vertex> vertices(4);
		std::vector<unsigned long> indices = { 0, 1, 3, 1, 2, 3 };

		float left = -_width * 0.5f;
		float right = -left;
		float top = _height * 0.5f;
		float bottom = -top;

		vertices[0].pos = Math::Vector3(left, top, 0);
		vertices[0].tex = Math::Vector2(0, 0);
		vertices[1].pos = Math::Vector3(right, top, 0);
		vertices[1].tex = Math::Vector2(1, 0);
		vertices[2].pos = Math::Vector3(right, bottom, 0);
		vertices[2].tex = Math::Vector2(1, 1);
		vertices[3].pos = Math::Vector3(left, bottom, 0);
		vertices[3].tex = Math::Vector2(0, 1);

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->Initialize(&vertices[0], &indices[0], 4, 6, 0, 0, GraphicDevice::Instance()->device);
		renderer()->mesh = mesh;
	}

	void Image::SetTexture(const std::shared_ptr<Texture2D>& texture)
	{
		renderer()->material.diffuseMap = texture;
	}

	const std::shared_ptr<Texture2D>& Image::GetTexture()
	{
		return renderer()->material.diffuseMap;
	}

	std::shared_ptr<Image> Image::CreateImage(const Math::Vector3& pos, const Math::Vector2& size, const std::shared_ptr<Texture2D>& texture)
	{
		auto g = GameObject::Instantiate("Image");
		auto i = g->AddComponent<Image>();
		if(texture)
			i->SetTexture(texture);
		else
			i->SetTexture(Resource::GetTexture2D(L"white"));
		i->SetSize((int)size.x, (int)size.y);
		g->transform()->position = pos;

		return i;
	}
}