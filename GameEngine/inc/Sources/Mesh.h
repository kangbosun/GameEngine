#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{

	struct Vertex;

	class GAMEENGINE_API Mesh final
	{
		friend class GraphicDevice;
		enum MeshType
		{
			eMesh, eSkinnedMesh
		};
	private:
		CComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		CComPtr<ID3D11Buffer> indexBuffer = nullptr;
		UINT nVertex = 0;
		UINT nIndex = 0;
		UINT stride = 0;
		UINT offset = 0;
		D3D11_PRIMITIVE_TOPOLOGY primitiveType;
		bool valid = false;
		
		std::vector<UINT> vertCountOfSubMesh;

	public:
		std::vector<Matrix> bindPoseInverse;
		MeshType type;

	public:
		Mesh() {}

		virtual bool Initialize(std::vector<Vertex>& vertices, std::vector<unsigned long>& indices, std::vector<UINT>* vertsOfSub = nullptr,
								D3D11_PRIMITIVE_TOPOLOGY primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT nIndices() { return nIndex; }
		bool IsValid() { return valid; }
		UINT GetSubMeshCount(UINT index);
	};
}

#pragma warning(pop)