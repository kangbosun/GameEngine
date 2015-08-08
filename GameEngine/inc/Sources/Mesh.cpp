#include "enginepch.h"
#include "GraphicDevice.h"
#include "Mesh.h"
#include "Debug.h"

namespace GameEngine
{
	

	bool Mesh::Initialize(std::vector<Vertex>& vertices, std::vector<unsigned long>& indices, std::vector<UINT>* vertsOfSub,
							D3D11_PRIMITIVE_TOPOLOGY primitive)
	{
		primitiveType = primitive;
		
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;

		nVertex = (UINT)vertices.size();
		nIndex = (UINT)indices.size();

		if(vertsOfSub)
			vertCountOfSubMesh.swap(*vertsOfSub);
		else
			vertCountOfSubMesh.push_back(nVertex);

		stride = sizeof(Vertex);

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * nVertex;;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		vertexData.pSysMem = &vertices[0];
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		auto graphicDevice = GraphicDevice::Instance();

		result = graphicDevice->device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
		if(FAILED(result)) {
			Debug::Log("Failed to create Vertex Buffer");
			return false;
		}

		indexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * nIndex;
		indexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		indexData.pSysMem = &indices[0];
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = graphicDevice->device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
		if(FAILED(result)) {
			Debug::Log("Failed to create to Index Buffer");
			return false;
		}
		valid = true;
		type = Mesh::eMesh;
		return true;
	}
	inline UINT Mesh::GetSubMeshCount(UINT index)
	{
		return vertCountOfSubMesh[index];
	}
}