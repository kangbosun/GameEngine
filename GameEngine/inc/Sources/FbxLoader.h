#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	namespace FbxLoader
	{
		using std::shared_ptr;
		using std::unordered_map;

		

		struct BlendWeightPair
		{
			int boneIndex = 0;
			float weight = 0;

			bool operator < (const BlendWeightPair& rhs)
			{
				return (weight > rhs.weight);
			}
		};

		struct CtrlPoint
		{
			Vector3 position;
			std::vector<BlendWeightPair> blendWeigths;
		};

		struct FMaterial
		{
			enum MaterialType
			{
				MT_LAMBERT = 0,
				MT_PHONG,
				MT_INVALID
			};

			MaterialType type;
			std::string name;
			Color ambient;
			Color diffuse;
			Color emissive;
			Color specular;
			Color reflection;
			float specularPower;
			float shininess;
			float reflectionFactor;
			float transperencyFactor;
			std::string diffuseMapName;
			std::string emissiveMapName;
			std::string normalMapName;
			std::string specularMapName;
		};

		struct Node
		{
		public:
			enum NodeType { eMesh, eBone };

			std::string name;
			std::string parentName;

			NodeType type;
			std::vector<CtrlPoint> controlPoints;
			std::vector<std::string> materialNames;
			std::vector<unsigned long> vertIndices;
			std::vector<UINT> vertexCountOfSubMesh;
			std::vector<Vertex> meshes;
			std::vector<Node> childNodes;
			std::vector<Bone> bones;
			Matrix matrix;

			bool useNormalMap = false;
			bool useSkinnedMesh = false;

			Node() = default;
			Node(const std::string& name, const std::string& parent);
			Node(Node&& node);

			~Node() { }
		};

		class FbxLoader
		{
		public:
			enum AxisMode
			{
				eOpenGL,
				eDirectX
			};
		public:
			float factor = 1;
			FbxScene* scene = nullptr;
			Node rootNode = { "root", "" };
			std::string relativeFolder;
			unordered_map<std::string, FMaterial> materials;
			unordered_map<std::string, std::shared_ptr<AnimClip>> animationClips;
			bool useNormalMap = false;
			AxisMode axismode = FbxLoader::eOpenGL;

		public:
			~FbxLoader();

			FbxLoader(AxisMode mode = FbxLoader::eOpenGL);
			void ConvertMatrix(Matrix& dst, FbxAMatrix& src);

			void LoadFromFile(const std::string& folder, const std::string& filename);

			void Release();

		private:
			void ProcessMeshNode(FbxNode* node, Node& parent);

			void ProcessControlPoints(FbxNode* node, Node& meshNode);

			void ProcessMesh(FbxNode* node, Node& meshNode);

			void ProcessMaterial(FbxScene* scene);
			void BindMaterial(FbxNode* node, Node& meshNode);

			void ReadTangent(FbxGeometryElementTangent* tangents, int ctrlIndex, int vertexCount, FbxVector4& vector);
			
			void ProcessMaterialAttribute(FbxSurfaceMaterial* mat, FMaterial& out);

			void ProcessMaterialTexture2D(FbxSurfaceMaterial* mat, FMaterial& material);

			void ComputeNodeMatrix(FbxNode* node, Node& meshNode, bool local = true);

			void ProcessBoneAndAnimation(FbxNode* node, Node& meshNode);
		
			void ProcessAnimCurve(FbxAnimCurve* curve, AnimCurve& animCurve);
			
			void ProcessAnimCurveS(FbxAnimCurve* curve[3], AnimTransformCurve& animCurve);
			void ProcessAnimCurveT(FbxAnimCurve* curve[3], AnimTransformCurve& animCurve);
			void ProcessAnimCurveR(FbxAnimCurve* curve[3], AnimTransformCurve& animCurve, FbxAMatrix& preRotation);

			void LoadAnimationClipData();
		};
	}
}

#pragma warning(pop)