
#include "enginepch.h"

#include "Mesh.h"
#include "DXUtil.h"

namespace GameEngine
{
	using namespace Math;

	bool Mesh::Initialize(Vertex vertices[], unsigned long indices[], int nVertices, int nIndices, int vertsOfSub[],
					int subMeshCount, ID3D11Device* device, D3D11_PRIMITIVE_TOPOLOGY primitive)
	{
		primitiveType = primitive;
		vertCountOfSubMesh.assign(subMeshCount, 0);
		for(int i = 0; i < subMeshCount; ++i) 
			vertCountOfSubMesh[i] = vertsOfSub[i];
		
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;

		nVertex = nVertices;
		nIndex = nIndices;
		stride = sizeof(Vertex);

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * nVertex;;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
		if(FAILED(result)) {
			Debug("Failed to create Vertex Buffer");
			return false;
		}

		indexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * nIndex;
		indexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
		if(FAILED(result)) {
			Debug("Failed to create to Index Buffer");
			return false;
		}
		valid = true;
		type = Mesh::eMesh;
		return true;
	}

	void Mesh::Bind(const CComPtr<ID3D11DeviceContext>& context)
	{
		context->IASetVertexBuffers(0, 1, (&vertexBuffer.p), &stride, &offset);
		context->IASetIndexBuffer(indexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(primitiveType);
	}
}