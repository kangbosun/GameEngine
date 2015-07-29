#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	namespace Math
	{
		struct Vertex;
	};

	class GAMEENGINE_API Mesh
	{
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

	public:
		std::vector<Math::Matrix> bindPoseInv;
		std::vector<int> vertCountOfSubMesh;
		MeshType type;

	public:
		Mesh() {}

		virtual bool Initialize(Math::Vertex vertices[], unsigned long indices[], int nVertices, int nIndices, int vertsOfSub[],
								int subMeshCount, ID3D11Device* device,
								D3D11_PRIMITIVE_TOPOLOGY primitive = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void Bind(const CComPtr<ID3D11DeviceContext>& context);
		UINT nIndices() { return nIndex; }
		bool IsValid() { return valid; }
	};
}

#pragma warning(pop)