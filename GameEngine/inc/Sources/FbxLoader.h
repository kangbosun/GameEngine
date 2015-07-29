#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	namespace FbxLoader
	{
		using std::shared_ptr;
		using std::unordered_map;

		using namespace Math;

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
			std::string mName;
			Color mAmbient;
			Color mDiffuse;
			Color mEmissive;
			Color mSpecular;
			Color mReflection;
			float mSpecularPower;
			float mShininess;
			float mReflectionFactor;
			float mTransperencyFactor;
			std::string mDiffuseMapName;
			std::string mEmissiveMapName;
			std::string mNormalMapName;
			std::string mSpecularMapName;
		};

		struct Node
		{
		public:
			enum NodeType { eMesh, eBone };

			std::string name;
			std::string parentName;

			NodeType type;
			std::vector<CtrlPoint> mControlPoints;
			std::vector<std::string> mMaterialNames;
			std::vector<unsigned long> vertIndices;
			std::vector<int> vertexCountOfSubMesh;
			std::vector<Vertex> meshes;
			std::vector<std::shared_ptr<Node>> mChildNodes;
			std::vector<Bone*> bones;
			Matrix mMatrix;

			bool useNormalMap;
			bool useSkinnedMesh;

			Node(const std::string& name, const std::string& parent)
			{
				this->name = name;
				parentName = parent;
				useNormalMap = false;
				useSkinnedMesh = false;
			}

			~Node() { Release(); }
			void Release()
			{
			}
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
			std::shared_ptr<Node> rootNode;
			std::string relativeFolder;
			unordered_map<std::string, std::shared_ptr<FMaterial>> materials;
			unordered_map<std::string, std::shared_ptr<AnimClip>> animationClips;
			bool useNormalMap = false;
			AxisMode axismode = FbxLoader::eOpenGL;

		public:
			~FbxLoader();

			FbxLoader(AxisMode mode = FbxLoader::eOpenGL);
			void ConvertMatrix(Matrix& dst, FbxAMatrix& src);

			void LoadFromFile(const std::string& folder, const std::string& filename);

			void Release();
			int GetBoneIndexByName(const std::string& name);
		private:
			void ProcessMeshNode(FbxNode* node, const std::shared_ptr<Node>& parent);

			void ProcessControlPoints(FbxNode* node, Node& meshNode);

			void ProcessMesh(FbxNode* node, Node& meshNode);

			void ProcessMaterial(FbxScene* scene);
			void BindMaterial(FbxNode* node, Node& meshNode);

			void ReadTangent(FbxGeometryElementTangent* tangents, int ctrlIndex, int vertexCount, FbxVector4& vector);
			void ReadBinormal(FbxGeometryElementBinormal* binormals, int ctrlIndex, int vertexCount, FbxVector4& vector);

			void ProcessMaterialAttribute(FbxSurfaceMaterial* mat, std::shared_ptr<FMaterial>& out);

			void ProcessMaterialTexture2D(FbxSurfaceMaterial* mat, const std::shared_ptr<FMaterial>& material);

			void ProcessSkeletonHierarchy(FbxNode* node, Bone* parent);

			void ComputeNodeMatrix(FbxNode* node, Node& meshNode, bool local = true);

			void ProcessBoneAndAnimation(FbxNode* node, Node& meshNode);

			void ProcessAnimCurve(FbxAnimCurve* curve[], AnimTransformCurve* animCurve, AnimCurve::AnimCurveType type, FbxAMatrix& preRotation);

			void LoadAnimationClipData();
		};
	}
}

#pragma warning(pop)