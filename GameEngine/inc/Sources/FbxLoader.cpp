#include "enginepch.h"

#include "FbxLoader.h"

namespace GameEngine
{
	namespace FbxLoader
	{
		using std::make_shared;

		FbxLoader::~FbxLoader()
		{
			scene->Clear();
		}

		void FbxLoader::Release()
		{
			materials.clear();
		}

		FbxLoader::FbxLoader(FbxLoader::AxisMode mode)
		{
			axismode = mode;
		}

		void FbxLoader::LoadFromFile(const std::string& folder, const std::string& name)
		{
			FbxManager* fbxManager = FbxManager::Create();
			FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
			fbxManager->SetIOSettings(ios);

			std::string path = folder + "\\" + name;
			relativeFolder = folder;
			FbxImporter* importer = (FbxImporter::Create(fbxManager, ""));
			bool b = importer->Initialize(path.c_str(), -1, fbxManager->GetIOSettings());
			if(!b) {
			}
			scene = FbxScene::Create(fbxManager, path.c_str());
			importer->Import(scene);
			importer->Destroy();

			int fileMajor, fileMinor, fileRevision;
			importer->GetFileVersion(fileMajor, fileMinor, fileRevision);

			FbxNode* root = scene->GetRootNode();

			FbxAxisSystem axisSystem = scene->GetGlobalSettings().GetAxisSystem();
			if(axisSystem != FbxAxisSystem::OpenGL)
				FbxAxisSystem::OpenGL.ConvertScene(scene);

			FbxGeometryConverter geometryConverter(fbxManager);

			geometryConverter.Triangulate(scene, false);
			rootNode = make_shared<Node>(root->GetName(), "null");

			factor = (float)scene->GetGlobalSettings().GetSystemUnit().GetConversionFactorTo(FbxSystemUnit::m);

			ProcessMaterial(scene);

			LoadAnimationClipData();

			ProcessMeshNode(root, rootNode);
		}

		void FbxLoader::ProcessMeshNode(FbxNode* node, const std::shared_ptr<Node>& mnode)
		{
			if(!node)
				return;

			if(node->GetNodeAttribute()) {
				auto type = node->GetNodeAttribute()->GetAttributeType();
				if(type == FbxNodeAttribute::eMesh) {
					mnode->type = Node::eMesh;
					ProcessControlPoints(node, *mnode.get());

					ProcessBoneAndAnimation(node, *mnode);

					ProcessMesh(node, *mnode.get());
					BindMaterial(node, *mnode.get());
				}
				else if(type == FbxNodeAttribute::eSkeleton) {
					mnode->type = Node::eBone;
				}
			}

			ComputeNodeMatrix(node, *mnode.get(), true);
			const int count = node->GetChildCount();
			mnode->mChildNodes.reserve(count);

			for(int i = 0; i < count; i++) {
				auto child = node->GetChild(i);
				auto childNode = make_shared<Node>(child->GetName(), mnode->name);
				ProcessMeshNode(node->GetChild(i), childNode);
				mnode->mChildNodes.push_back(childNode);
			}
		}

		void FbxLoader::ProcessControlPoints(FbxNode* node, Node& meshNode)
		{
			FbxMesh* currMesh = node->GetMesh();
			if(!currMesh)
				return;

			const int ctrlPointCount = currMesh->GetControlPointsCount();
			meshNode.mControlPoints.assign(ctrlPointCount, CtrlPoint());

			for(int i = 0; i < ctrlPointCount; i++) {
				FbxVector4 p = currMesh->GetControlPointAt(i) * factor;
				auto& cp = meshNode.mControlPoints[i];
				cp.position = { (float)p[0], (float)p[1], (float)p[2] };
				if(axismode = AxisMode::eDirectX)
					cp.position.x *= -1;
			}
		}

		void FbxLoader::ProcessMesh(FbxNode* node, Node& meshNode)
		{
			FbxMesh* currMesh = node->GetMesh();

			if(!currMesh)
				return;

			FbxLayerElementTangent* tangents = nullptr;
			FbxLayerElementBinormal* binormals = nullptr;

			if(useNormalMap) {
				if(currMesh->GetElementTangentCount() < 1 ||
				   currMesh->GetElementBinormalCount() < 1) {
					currMesh->GenerateTangentsDataForAllUVSets(true);
					tangents = currMesh->GetElementTangent();
					binormals = currMesh->GetElementBinormal();
				}
			}

			std::map<int, std::vector<Vertex>> subMeshes;

			int vertCounter = 0;
			const int polygonCount = currMesh->GetPolygonCount();

			for(int i = 0; i < polygonCount; i++) {
				const int polySize = currMesh->GetPolygonSize(i);

				int nMaterials = node->GetMaterialCount();
				auto elementMaterial = currMesh->GetElementMaterial();
				int mi = 0;
				if(elementMaterial)
					mi = currMesh->GetElementMaterial()->GetIndexArray()[i];

				for(int j = polySize - 1; j >= 0; j--) {
					int ctrlIndex = currMesh->GetPolygonVertex(i, j);

					auto& currCtrlPoint = meshNode.mControlPoints[ctrlIndex];
					FbxVector4 v4;

					auto& pos = currCtrlPoint.position;
					currMesh->GetPolygonVertexNormal(i, j, v4);
					Vector3 normal = { (float)v4[0], (float)v4[1], (float)v4[2] };
					Vector3 tangent = { 0, 0, 0 };
					//Vector3 binormal = { 0, 0, 0 };
					if(useNormalMap) {
						ReadTangent(tangents, ctrlIndex, vertCounter, v4);
						tangent = { (float)v4[0], (float)v4[1], (float)v4[2] };

						//ReadBinormal(binormals, ctrlIndex, vertCounter, v4);
						//binormal = { (float)v4[0], (float)v4[1], (float)v4[2] };
					}

					Vector2 uv;
					FbxStringList uvSetNames;
					currMesh->GetUVSetNames(uvSetNames);
					int uvCounts = uvSetNames.GetCount();
					bool unmapped = false;
					for(int k = 0; k < uvCounts; k++) {
						FbxVector2 UV;
						currMesh->GetPolygonVertexUV(i, j, uvSetNames[k], UV, unmapped);
						uv = { (float)UV[0], (float)UV[1] };
					}

					if(axismode == AxisMode::eDirectX) {
						normal.x *= -1;
						//binormal.x *= -1;
						tangent.x *= -1;
						uv.y = 1 - uv.y;
						uv.x = uv.x;
					}
					meshNode.useSkinnedMesh = true;
					int blendCount = (int)min(currCtrlPoint.blendWeigths.size(), 4);
					if(currCtrlPoint.blendWeigths.size() > 4)
						sort(currCtrlPoint.blendWeigths.begin(), currCtrlPoint.blendWeigths.end());

					Vector4 weights = { 0, 0, 0, 0 };
					Byte4 indices = { 0, 0, 0, 0 };
					for(int i = 0; i < blendCount; i++) {
						weights[i] = currCtrlPoint.blendWeigths[i].weight;
						int index = currCtrlPoint.blendWeigths[i].boneIndex;
						if(index == -1) index = 0;
						indices.m[i] = index;
					}
					Vertex temp = { pos, uv, normal, tangent, indices, weights };
					subMeshes[mi].push_back(temp);
				}
				vertCounter++;
			}

			if(subMeshes.size() > 0) {
				int index = 0;
				meshNode.meshes.reserve(vertCounter * 3);
				meshNode.vertIndices.reserve(vertCounter * 3);
				meshNode.vertexCountOfSubMesh.reserve(subMeshes.size());
				for(auto& pair : subMeshes) {
					auto& m = pair.second;
					for(int i = 0; i < m.size(); ++i)
						meshNode.vertIndices.emplace_back(index++);
					meshNode.vertexCountOfSubMesh.push_back((int)m.size());
					meshNode.meshes.insert(meshNode.meshes.end(), m.begin(), m.end());
				}
			}
			subMeshes.clear();
		}

		void FbxLoader::BindMaterial(FbxNode* node, Node& meshNode)
		{
			const int materialCount = node->GetMaterialCount();

			for(int i = 0; i < materialCount; i++) {
				FbxSurfaceMaterial* surfaceMaterial = node->GetMaterial(i);
				auto name = surfaceMaterial->GetName();
				meshNode.mMaterialNames.push_back(name);
				if(materials[name]->mNormalMapName != "")
					meshNode.useNormalMap = true;
			}
		}

		void FbxLoader::ReadTangent(FbxLayerElementTangent* tangents, int ctrlIndex, int vertexCount, __out FbxVector4& vector)
		{
			auto mappingMode = tangents->GetMappingMode();
			auto refMode = tangents->GetReferenceMode();

			switch(mappingMode) {
				case FbxGeometryElement::eByControlPoint:
					switch(refMode) {
						case FbxGeometryElement::eDirect:
						{
							vector = tangents->GetDirectArray().GetAt(ctrlIndex);
							break;
						}
						case FbxGeometryElement::eIndexToDirect:
						{
							int index = tangents->GetIndexArray().GetAt(ctrlIndex);
							vector = tangents->GetDirectArray().GetAt(index);
							break;
						}
						default:
							break;
					}
				case FbxGeometryElement::eByPolygonVertex:
					switch(refMode) {
						case FbxGeometryElement::eDirect:
						{
							vector = tangents->GetDirectArray().GetAt(vertexCount);
							break;
						}
						case FbxGeometryElement::eIndexToDirect:
						{
							int index = tangents->GetIndexArray().GetAt(vertexCount);
							vector = tangents->GetDirectArray().GetAt(index);
							break;
						}
						default:
							break;
					}
			}
		}

		void FbxLoader::ReadBinormal(FbxLayerElementBinormal* binormals, int ctrlIndex, int vertexCount, __out FbxVector4& vector)
		{
			auto mappingMode = binormals->GetMappingMode();
			auto refMode = binormals->GetReferenceMode();

			switch(mappingMode) {
				case FbxGeometryElement::eByControlPoint:
					switch(refMode) {
						case FbxGeometryElement::eDirect:
						{
							vector = binormals->GetDirectArray().GetAt(ctrlIndex);
							break;
						}
						case FbxGeometryElement::eIndexToDirect:
						{
							int index = binormals->GetIndexArray().GetAt(ctrlIndex);
							vector = binormals->GetDirectArray().GetAt(index);
							break;
						}
						default:
							break;
					}
				case FbxGeometryElement::eByPolygonVertex:
					switch(refMode) {
						case FbxGeometryElement::eDirect:
						{
							vector = binormals->GetDirectArray().GetAt(vertexCount);
							break;
						}
						case FbxGeometryElement::eIndexToDirect:
						{
							int index = binormals->GetIndexArray().GetAt(vertexCount);
							vector = binormals->GetDirectArray().GetAt(index);
							break;
						}
						default:
							break;
					}
			}
		}

		void FbxLoader::ProcessMaterial(FbxScene* scene)
		{
			const int materialCount = scene->GetMaterialCount();
			for(int i = 0; i < materialCount; i++) {
				FbxSurfaceMaterial* surfaceMaterial = scene->GetMaterial(i);
				auto material = make_shared<FMaterial>();
				ProcessMaterialAttribute(surfaceMaterial, material);
				ProcessMaterialTexture2D(surfaceMaterial, material);
				material->mName = surfaceMaterial->GetName();
				materials[material->mName] = material;
			}
		}

		void FbxLoader::ProcessMaterialAttribute(FbxSurfaceMaterial* mat, __out std::shared_ptr<FMaterial>& material)
		{
			FbxDouble3 d3;
			FbxDouble d1;

			std::string name = mat->GetName();
			material->type = FMaterial::MT_INVALID;
			//material->mName = mat->GetName();

			std::string className = mat->GetClassId().GetName();

			if(className == "FbxSurfaceLambert") {
				material->type = FMaterial::MT_LAMBERT;

				d3 = reinterpret_cast<FbxSurfaceLambert*>(mat)->Ambient;
				material->mAmbient = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d3 = reinterpret_cast<FbxSurfaceLambert*>(mat)->Diffuse;
				material->mDiffuse = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d3 = reinterpret_cast<FbxSurfaceLambert*>(mat)->Emissive;
				material->mEmissive = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d1 = reinterpret_cast<FbxSurfaceLambert*>(mat)->TransparencyFactor;
				material->mTransperencyFactor = (float)d1;
			}
			// phong model
			else if(className == "FbxSurfacePhong") {
				material->type = FMaterial::MT_PHONG;
				d3 = reinterpret_cast<FbxSurfacePhong*>(mat)->Ambient;
				material->mAmbient = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d3 = reinterpret_cast<FbxSurfacePhong*>(mat)->Diffuse;
				material->mDiffuse = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d3 = reinterpret_cast<FbxSurfacePhong*>(mat)->Emissive;
				material->mEmissive = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d1 = reinterpret_cast<FbxSurfacePhong*>(mat)->TransparencyFactor;
				material->mTransperencyFactor = (float)d1;

				d3 = reinterpret_cast<FbxSurfacePhong*>(mat)->Specular;
				material->mSpecular = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d3 = reinterpret_cast<FbxSurfacePhong*>(mat)->Reflection;
				material->mReflection = Color((float)d3[0], (float)d3[1], (float)d3[2], 1);

				d1 = reinterpret_cast<FbxSurfacePhong*>(mat)->Shininess;
				material->mShininess = (float)d1;

				d1 = reinterpret_cast<FbxSurfacePhong*>(mat)->ReflectionFactor;
				material->mReflectionFactor = (float)d1;

				d1 = reinterpret_cast<FbxSurfacePhong*>(mat)->SpecularFactor;
				material->mSpecularPower = (float)d1;
			}
		}

		void FbxLoader::ProcessMaterialTexture2D(FbxSurfaceMaterial* mat, const std::shared_ptr<FMaterial>& material)
		{
			int Texture2DIndex = 0;
			FbxProperty prop;

			FBXSDK_FOR_EACH_TEXTURE(Texture2DIndex)
			{
				prop = mat->FindProperty(FbxLayerElement::sTextureChannelNames[Texture2DIndex]);
				if(prop.IsValid()) {
					const int Texture2DCount = prop.GetSrcObjectCount<FbxTexture>();
					for(int i = 0; i < Texture2DCount; i++) {
						FbxTexture* texture = prop.GetSrcObject<FbxTexture>(i);
						if(texture) {
							std::string textureType = prop.GetNameAsCStr();
							FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);
							if(fileTexture) {
								if(textureType == "DiffuseColor")
									material->mDiffuseMapName = fileTexture->GetRelativeFileName();
								else if(textureType == "SpecularColor")
									material->mSpecularMapName = fileTexture->GetRelativeFileName();
								else if(textureType == "NormalMap") {
									material->mNormalMapName = fileTexture->GetRelativeFileName();
									useNormalMap = true;
								}
							}
						}
					}

					const int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
					for(int i = 0; i < layeredTextureCount; i++) {
						auto layeredTexture = prop.GetSrcObject<FbxLayeredTexture>(i);
						const int fileCount = layeredTexture->GetSrcObjectCount<FbxFileTexture>();
						for(int j = 0; j < fileCount; j++) {
							auto fileTexture = layeredTexture->GetSrcObject<FbxFileTexture>(j);
							std::string Texture2DType = prop.GetNameAsCStr();
							if(fileTexture) {
								if(Texture2DType == "DiffuseColor")
									material->mDiffuseMapName = fileTexture->GetRelativeFileName();
								else if(Texture2DType == "SpecularColor")
									material->mSpecularMapName = fileTexture->GetRelativeFileName();
								else if(Texture2DType == "NormalMap") {
									material->mNormalMapName = fileTexture->GetRelativeFileName();
									useNormalMap = true;
								}
							}
						}
					}
				}
			}

			if(material->mDiffuseMapName != "") {
				std::string n = FbxPathUtils::GetFileName(material->mDiffuseMapName.c_str(), false).Buffer();
				mat->SetName(n.c_str());
			}
		}

		void FbxLoader::ProcessSkeletonHierarchy(FbxNode* node, Bone* parent)
		{
			//if(node == nullptr) return;
			//auto attrib = node->GetNodeAttribute();
			//if(attrib) {
			//	auto type = attrib->GetAttributeType();
			//	std::string name = node->GetName();
			//	if(type == FbxNodeAttribute::eSkeleton) {
			//		Bone* bone = new Bone();
			//		bone->name = name;
			//		//bone->index = this->boneCounter++;

			//
			//		if(parent) {
			//			//bone->parentName = parent->name;
			//			parent->children.push_back(bone);
			//		}
			//		else {
			//			rootBones[name] = bone;
			//		}

			//		const size_t childCount = node->GetChildCount();
			//		for(size_t i = 0; i < childCount; i++) {
			//			ProcessSkeletonHierarchy(node->GetChild((int)i), bone);
			//		}
			//		return;
			//	}
			//}
			//const size_t childCount = node->GetChildCount();
			//for(size_t i = 0; i < childCount; i++) {
			//	ProcessSkeletonHierarchy(node->GetChild((int)i), parent);
			//}
		}

		void FbxLoader::ProcessBoneAndAnimation(FbxNode* node, Node& meshNode)
		{
			auto currMesh = node->GetMesh();
			if(!currMesh) return;

			FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 lR = node->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 lS = node->GetGeometricScaling(FbxNode::eSourcePivot);

			FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);

			FbxSkin* skin = nullptr;
			const int deformerCnt = currMesh->GetDeformerCount();
			for(int deformerIndex = 0; deformerIndex < deformerCnt; ++deformerIndex) {
				skin = (FbxSkin*)(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
				if(skin) break;
			}
			if(!skin) return;

			meshNode.useSkinnedMesh = true;
			const size_t nClusters = skin->GetClusterCount();
			if(nClusters < 1) return;

			for(auto& clip : animationClips)
				clip.second->transformCurves.assign(nClusters, nullptr);

			meshNode.bones.assign(nClusters, nullptr);
			const int animCount = scene->GetSrcObjectCount<FbxAnimStack>();

			float time = 0;
			for(int ci = 0; ci < nClusters; ++ci) {
				FbxCluster* cluster = skin->GetCluster(ci);
				auto elink = cluster->GetLinkMode();
				std::string boneName = cluster->GetLink()->GetName();

				FbxAMatrix transformMatrix, transformLinkMatrix, globalBindposeInverse;
				cluster->GetTransformMatrix(transformMatrix);
				cluster->GetTransformLinkMatrix(transformLinkMatrix);
				globalBindposeInverse = transformLinkMatrix.Inverse() * geometryTransform * transformMatrix;
				globalBindposeInverse.SetT(globalBindposeInverse.GetT() * factor);

				FbxNode* boneNode = cluster->GetLink();
				Bone* bone = new Bone();
				bone->name = boneName;
				bone->index = ci;

				if(axismode == AxisMode::eDirectX) {
					auto& bT = globalBindposeInverse.GetT();
					auto& bR = globalBindposeInverse.GetR();
					bT[0] *= -1;
					bR[1] *= -1; bR[2] *= -1;
					globalBindposeInverse.SetT(bT);
					globalBindposeInverse.SetR(bR);
				}
				ConvertMatrix(bone->bindPoseInverse, globalBindposeInverse);
				meshNode.bones[ci] = bone;

				const int nCtrl = cluster->GetControlPointIndicesCount();

				for(int ctrlIndex = 0; ctrlIndex < nCtrl; ++ctrlIndex) {
					BlendWeightPair pair;
					pair.boneIndex = ci;
					pair.weight = (float)cluster->GetControlPointWeights()[ctrlIndex];
					meshNode.mControlPoints[cluster->GetControlPointIndices()[ctrlIndex]].blendWeigths.push_back(pair);
				}

				FbxAMatrix preRot;
				auto preR = boneNode->GetPreRotation(FbxNode::eSourcePivot);
				preRot.SetR(preR);

				for(int ai = 0; ai < animCount; ++ai) {
					auto animStack = scene->GetSrcObject<FbxAnimStack>(ai);
					scene->SetCurrentAnimationStack(animStack);
					std::string animName = animStack->GetName();
					auto& clip = animationClips[animName];
					auto transformCurve = new AnimTransformCurve();
					transformCurve->boneName = boneName;

					auto animLayer = animStack->GetMember<FbxAnimLayer>(0);
					FbxAnimCurve* fbxTCurves[3];
					FbxAnimCurve* fbxRCurves[3];
					FbxAnimCurve* fbxSCurves[3];

					fbxTCurves[0] = boneNode->LclTranslation.GetCurve(animLayer, "X");

					if(!fbxTCurves[0])
						continue;

					fbxTCurves[1] = boneNode->LclTranslation.GetCurve(animLayer, "Y");
					fbxTCurves[2] = boneNode->LclTranslation.GetCurve(animLayer, "Z");

					fbxRCurves[0] = boneNode->LclRotation.GetCurve(animLayer, "X");
					fbxRCurves[1] = boneNode->LclRotation.GetCurve(animLayer, "Y");
					fbxRCurves[2] = boneNode->LclRotation.GetCurve(animLayer, "Z");

					fbxSCurves[0] = boneNode->LclScaling.GetCurve(animLayer, "X");
					fbxSCurves[1] = boneNode->LclScaling.GetCurve(animLayer, "Y");
					fbxSCurves[2] = boneNode->LclScaling.GetCurve(animLayer, "Z");

					transformCurve->boneName = boneName;

					FbxAnimCurveFilterKeyReducer keyReducer;
					keyReducer.SetKeySync(true);
					keyReducer.Apply(fbxTCurves, 3);
					keyReducer.Apply(fbxRCurves, 3);
					keyReducer.Apply(fbxSCurves, 3);

					FbxAnimCurveFilterUnroll unroll;
					unroll.SetForceAutoTangents(true);
					unroll.Apply(fbxRCurves, 3);

					FbxAnimCurveFilterTSS tss;
					FbxTime tt;
					tt.SetSecondDouble(-fbxTCurves[0]->KeyGetTime(0).GetSecondDouble());
					tss.SetShift(tt);
					tss.Apply(fbxTCurves, 3);
					tss.Apply(fbxRCurves, 3);
					tss.Apply(fbxSCurves, 3);

					if(fbxTCurves[0]->KeyGetCount() > 0) {
						ProcessAnimCurve(fbxTCurves, transformCurve, AnimCurve::eTranslation, preRot);
						ProcessAnimCurve(fbxRCurves, transformCurve, AnimCurve::eRotation, preRot);
						ProcessAnimCurve(fbxSCurves, transformCurve, AnimCurve::eScaling, preRot);
					}
					clip->transformCurves[ci] = transformCurve;
				} // animations loop
			} // cluster loop

			// clamp animation clips
			int ti = 0;
			for(int i = 0; i < animCount; ++i) {
				auto animStack = scene->GetSrcObject<FbxAnimStack>(i);
				std::string animName = animStack->GetName();
				auto& clip = animationClips[animName];
				auto& curve = clip->transformCurves[ti];
				while(curve == nullptr && ti < nClusters)
					curve = clip->transformCurves[ti++];
				float endTime = curve->translation->curves[0]->keyframes.back()->time;
				clip->startTime = 0;
				clip->lengthInSeconds = endTime;
				clip->lengthInFrames = (int)(1.5f + (endTime / (1.0f / 30.0f)));
			}
		}

		void FbxLoader::ProcessAnimCurve(FbxAnimCurve* curve[], AnimTransformCurve* transformCurve, AnimCurve::AnimCurveType type, FbxAMatrix& preRotation)
		{
			if(!curve) return;

			const int nKeys = curve[0]->KeyGetCount();
			AnimCurve3* curve3;

			if(type == AnimCurve::eTranslation)
				curve3 = transformCurve->translation;
			else if(type == AnimCurve::eRotation)
				curve3 = transformCurve->rotation;
			else if(type == AnimCurve::eScaling)
				curve3 = transformCurve->scaling;
			else
				return;

			(*curve3)[0]->keyframes.assign(nKeys, nullptr);
			(*curve3)[1]->keyframes.assign(nKeys, nullptr);
			(*curve3)[2]->keyframes.assign(nKeys, nullptr);

			for(int ki = 0; ki < nKeys; ++ki) {
				auto xkey = new Keyframe();
				auto ykey = new Keyframe();
				auto zkey = new Keyframe();

				curve[0]->KeySetTangentMode(ki, FbxAnimCurveDef::eTangentAuto);
				curve[1]->KeySetTangentMode(ki, FbxAnimCurveDef::eTangentAuto);
				curve[2]->KeySetTangentMode(ki, FbxAnimCurveDef::eTangentAuto);
				auto xvalue = curve[0]->KeyGetValue(ki);
				auto yvalue = curve[1]->KeyGetValue(ki);
				auto zvalue = curve[2]->KeyGetValue(ki);

				auto xlt = curve[0]->KeyGetLeftAuto(ki);
				auto xrt = curve[0]->KeyGetRightAuto(ki);
				auto ylt = curve[1]->KeyGetLeftAuto(ki);
				auto yrt = curve[1]->KeyGetRightAuto(ki);
				auto zlt = curve[2]->KeyGetLeftAuto(ki);
				auto zrt = curve[2]->KeyGetRightAuto(ki);

				if(type == AnimCurve::eTranslation) {
					if(axismode == AxisMode::eDirectX) {
						xvalue = -xvalue;
						xlt = -xlt; xrt = -xrt;
					}
					xvalue *= factor; yvalue *= factor; zvalue *= factor;
				}
				if(type == AnimCurve::eRotation) {
					FbxAMatrix temp;
					temp.SetR(FbxVector4(xvalue, yvalue, zvalue));
					auto R = (preRotation* temp).GetR();
					xvalue = (float)R[0]; yvalue = (float)R[1]; zvalue = (float)R[2];
					if(axismode == AxisMode::eDirectX) {
						yvalue = -yvalue; zvalue = -zvalue;
						ylt = -ylt; yrt = -yrt; zlt = -zlt; zrt = -zrt;
					}
				}
				float t = (float)curve[0]->KeyGetTime(ki).GetSecondDouble();
				xkey->time = t; ykey->time = t; zkey->time = t;
				xkey->value = xvalue; ykey->value = yvalue; zkey->value = zvalue;
				xkey->leftTangent = xlt; xkey->rightTangent = xrt;
				ykey->leftTangent = ylt; ykey->rightTangent = yrt;
				zkey->leftTangent = zlt; zkey->rightTangent = zrt;

				curve3->curves[0]->keyframes[ki] = (xkey);
				curve3->curves[1]->keyframes[ki] = (ykey);
				curve3->curves[2]->keyframes[ki] = (zkey);
			}
		}

		void FbxLoader::ComputeNodeMatrix(FbxNode* node, Node& meshNode, bool local)
		{
			if(!node)
				return;
			FbxAnimEvaluator* evaluator = scene->GetAnimationEvaluator();
			FbxAMatrix global;
			global.SetIdentity();

			FbxTime time;
			time.SetSecondDouble(0.0);

			if(node != scene->GetRootNode()) {
				if(local) {
					global = evaluator->GetNodeLocalTransform(node, time);
				}
				else {
					global = evaluator->GetNodeGlobalTransform(node, time);
				}
			}

			if(axismode == AxisMode::eDirectX) {
				auto& T = global.GetT() * factor;
				auto& R = global.GetR();
				T[0] *= -1;
				R[1] *= -1;
				R[2] *= -1;
				global.SetT(T);
				global.SetR(R);

				R = global.GetR();
			}

			meshNode.mMatrix = Matrix(
				(float)global[0][0], (float)global[0][1], (float)global[0][2], (float)global[0][3],
				(float)global[1][0], (float)global[1][1], (float)global[1][2], (float)global[1][3],
				(float)global[2][0], (float)global[2][1], (float)global[2][2], (float)global[2][3],
				(float)global[3][0], (float)global[3][1], (float)global[3][2], (float)global[3][3]);
		}

		int FbxLoader::GetBoneIndexByName(const std::string& name)
		{
			//const size_t BoneCount = Bones.size();
			//for(int i = 0; i < BoneCount; i++) {
			//	if(Bones[i]->name == name)
			//		return i;
			//}
			return -1;
		}

		void FbxLoader::LoadAnimationClipData()
		{
			// load animation data
			const int animCount = scene->GetSrcObjectCount<FbxAnimStack>();

			for(int ac = 0; ac < animCount; ++ac) {
				FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(ac);
				if(!currAnimStack) return;

				FbxString animStackName = currAnimStack->GetName();
				FbxTakeInfo* takeInfo = scene->GetTakeInfo(animStackName);

				if(!takeInfo) return;
				FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				FbxTime stop = takeInfo->mLocalTimeSpan.GetStop();
				FbxTime::EMode mode = FbxTime::eDefaultMode;
				int startFrame = (int)start.GetFrameCount(mode);
				int stopFrame = (int)stop.GetFrameCount(mode);
				int animLength = stopFrame - startFrame + 1;

				std::shared_ptr<AnimClip> clip = make_shared<AnimClip>();
				clip->name = animStackName;
				animationClips[clip->name] = clip;
			}
		}
		void FbxLoader::ConvertMatrix(Matrix& dest, FbxAMatrix& src)
		{
#pragma omp parallel for
			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++)
					dest.m[i][j] = (float)src[i][j];
			}
		}
	}
}