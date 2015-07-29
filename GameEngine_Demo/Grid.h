
#pragma once



class Grid : public ClonableObject<Component, Grid>
{
public:
	void Start()
	{
		Vertex vertices[404];
		unsigned long indices[404];

		for(int i = 0; i < 202; i += 2) {
			float offset = (i / 200.0f) - 0.5f;
			vertices[i].pos = Vector3(-0.5f, 0, offset);
			vertices[i + 1].pos = Vector3(0.5f, 0, offset);
			indices[i] = i;
			indices[i + 1] = i + 1;
		}

		for(int i = 202; i < 404; i += 2) {
			float offset = ((i - 202) / 200.0f) - 0.5f;
			vertices[i].pos = Vector3(offset, 0, -0.5f);
			vertices[i + 1].pos = Vector3(offset, 0, 0.5f);
			indices[i] = i;
			indices[i + 1] = i + 1;
		}
		auto dx = GraphicDevice::Instance();
		auto mesh = make_shared<Mesh>();
		int n[1] = { 404 };
		mesh->Initialize(vertices, indices, 404, 404, n, 1, dx->device, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		auto& _renderer = gameObject->AddComponent<MeshRenderer>();
		_renderer->mesh = mesh;

		auto mat = Resource::GetMaterial("default")->Clone();
		_renderer->materials.push_back(mat);
		_renderer->materials[0]->shader = Resource::GetShader(L"Uber");
		_renderer->materials[0]->data.color = Color::Black;
		transform()->scale = Vector3(100, 100, 100);
		_renderer->castShadow = false;
		_renderer->receiveShadow = false;
	}
};